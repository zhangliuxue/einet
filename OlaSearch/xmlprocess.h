/*
ģ�����ƣ���ȡ��д��XML�ļ�
��ȡվ����������ļ�����Ŀ¼�����ݿ����ӣ��ļ�ϵͳ���ӣ�action��ʽ���ӵ�
���ߣ�����ѧ
���ڣ�2012-08-14
*/
#pragma once
#include <iostream>  
#include <string>  
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/xml_parser.hpp>  
#include <boost/foreach.hpp>  
#include <set>
#include <map>
using namespace std;

//���ݿ����Ӷ���
typedef struct dblink
{
	std::string ipaddr;      //IP
	std::string usrname;    //�û�����
	std::string pwd;        //����
	std::string db;
	string      port;          //���ݿ����˿�
	int timeout;
}DBLINK;
//mcast����
typedef struct mcaststruct
{
	std::string ipaddr;
	std::string port;
}MCASTSTRUCT;
//web������Ҫ�ṹ����
typedef struct mainsturct
{
	int pool;                    //�����̸߳���
	string ipaddr;
	string port;                         //��������˿�
	string path;
	dblink mongo_dblink;
	mcaststruct mcast;
}MAINSTRUCT;


class xmlprocess
{
public:
	xmlprocess(void);
	~xmlprocess(void);
	void load(const std::string &filename);
	void save(const std::string &filename);
private:
	std::string m_file;         

};

extern	mainsturct m_mainstie;

