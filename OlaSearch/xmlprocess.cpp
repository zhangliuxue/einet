/*
 * *
 * */
#include "xmlprocess.h"
#include "boost/format.hpp"
using namespace boost;
mainsturct m_mainstie;
xmlprocess::xmlprocess(void)
{
}


xmlprocess::~xmlprocess(void)
{
}

void xmlprocess::load(const std::string &filename) {
	using boost::property_tree::ptree;
	ptree pt;

	read_xml(filename, pt);
	ptree web = pt.get_child("main.web");
	m_mainstie.pool = web.get<int>("pool");
	m_mainstie.port = web.get<string>("port");
	m_mainstie.ipaddr = web.get<string>("ipaddr");
	m_mainstie.path = web.get<string>("path");

	ptree childtt = pt.get_child("main.mongodb");
	m_mainstie.mongo_dblink.ipaddr = childtt.get<string>("ipaddr");
	m_mainstie.mongo_dblink.db = childtt.get<string>("db");
	m_mainstie.mongo_dblink.port = childtt.get<string>("port");
	m_mainstie.mongo_dblink.usrname =childtt.get<string>("usr");
	m_mainstie.mongo_dblink.pwd =childtt.get<string>("pwd");
	m_mainstie.mongo_dblink.timeout = childtt.get<int>("timeout");

	ptree childtt1 = pt.get_child("main.mcast");
	m_mainstie.mcast.ipaddr = childtt1.get<string>("ipaddr");
	m_mainstie.mcast.port = childtt1.get<string>("port");
}

void xmlprocess::save(const std::string &filename)
{

}
