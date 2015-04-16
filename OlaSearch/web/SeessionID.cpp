/*
 * SeessionID.cpp
 *
 *  Created on: Apr 28, 2014
 *      Author: zlx
 */

#include "SeessionID.h"
#include <boost/regex.hpp>
#include "../JsonMsg.h"

Lock myLock;
SeessionID m_sessionID;

SeessionID::SeessionID() {
	kki = 0;
}

SeessionID::~SeessionID() {

}
std::string SeessionID::GetIPAddr(const std::string & sid) {
	ReadLock r_lock(myLock);
	return ptrsmap[sid].ipaddr;
}
std::string SeessionID::ReadFunction(std::string sid) {
	ReadLock r_lock(myLock);
	int id = ptrsmap[sid].i;
	std::string msg;
	bool num = false;

	if(mmap.size()>0)
	{
		for (msgMap::const_iterator i = mmap.begin(), e = mmap.end(); i != e; ++i)
		{
			if (i->i <= id)
				continue;
			msg += i->msg;
			msg += ",";
			num = true;
			ptrsmap[sid].i = i->i;
		}
	}
	if (num)
	{
		msg = msg.substr(0, msg.length() - 1);

	}
	msg = "[" + msg;
	msg = msg + "]";

	return msg;
}
void SeessionID::Clear() {
	WriteLock w_lock(myLock);
	mmap.clear();
	ptrsmap.clear();
}
void SeessionID::Clear(const std::string & sid) {
	WriteLock w_lock(myLock);
	ptrsmap[sid].ptr_s.clear();
}
Ptr_strVector SeessionID::GetVect(const std::string & sid) {
	ReadLock r_lock(myLock);
	return ptrsmap[sid].ptr_s;
}
void SeessionID::SetKillFlag(const std::string & sid, bool flag) {
	WriteLock w_lock(myLock);
	ptrsmap[sid].killflag = flag;
}
bool SeessionID::GetKillFlag(const std::string & sid) {
	ReadLock r_lock(myLock);
	return ptrsmap[sid].killflag;
}
void SeessionID::push_back(const std::string & sid, const std::string str) {
	WriteLock w_lock(myLock);
	std::vector<std::string> value;
	boost::regex regex1(" ", boost::regbase::normal | boost::regbase::icase);
	std::string mstr = str;
	boost::regex_split(std::back_inserter(value), mstr, regex1);
	for (int i = 0; i < (int) value.size(); i++) {
		if (!value[i].empty()) {
			if (ptrsmap[sid].ptr_s.empty()) {
				ptrsmap[sid].ptr_s.push_back(value[i]);
			} else {
				bool find = false;
				for (unsigned int j = 0; j < ptrsmap[sid].ptr_s.size(); j++) {
					if (ptrsmap[sid].ptr_s[j] == value[i]) {
						find = true;
						break;
					}
				}
				if (!find) {
					ptrsmap[sid].ptr_s.push_back(value[i]);
				}
			}
		}
	}
}
void SeessionID::WriteFunction(std::string msg) {
	WriteLock w_lock(myLock);
	//保留2000条消息，删除最早的1000条
	if (mmap.size() > 2000) {
		mmap.erase(mmap.begin(),mmap.begin()+1000);
	}

	msgstruct_ptr smsg(new msgstruct());
	smsg->i = kki;
	smsg->msg = msg;
	mmap.push_back(smsg);
	kki = kki + 1;
}
void SeessionID::WriteHostInfo(const std::string& sid,
		const std::string& hostname) {
	WriteLock w_lock(myLock);
	ptrsmap[sid].ptr_s.push_back(hostname);

}
Ptr_strVector SeessionID::GetHostInfo(const std::string& sid) {
	ReadLock r_lock(myLock);

	return ptrsmap[sid].ptr_s;
}
