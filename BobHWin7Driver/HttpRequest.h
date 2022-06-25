#pragma once
/*! @file
**********************************************************************
<PRE>
ģ����       :
�ļ���       : HttpRequest.h
����ļ�     : HttpRequest.cpp
�ļ�ʵ�ֹ��� : Http ����
����         : Song
�汾         : 1.0
----------------------------------------------------------------------
��ע         :
----------------------------------------------------------------------
�޸ļ�¼     :
�� ��        �汾   �޸���                 �޸�����
2018/4/10    1.0    Song                    ����
</PRE>
**********************************************************************

* ��Ȩ����(c) 2018-2019, ��������Ȩ��

*********************************************************************/
#pragma once
#include <string>
#include <vector>

class HttpRequest
{
public:
	HttpRequest(const std::string& ip, int port);
	~HttpRequest(void);

	// Http GET����
	std::string HttpGet(std::string req);

	// Http POST����
	std::string HttpPost(std::string req, std::string data);

	// �ϳ�JSON�ַ���
	static std::string genJsonString(std::string key, int value);

	// �ָ��ַ���
	static std::vector<std::string> split(const std::string &s, const std::string &seperator);

	// ����key��Response��ȡHeader�е�����
	static std::string getHeader(std::string respose, std::string key);

private:
	std::string         m_ip;
	int             m_port;
};

