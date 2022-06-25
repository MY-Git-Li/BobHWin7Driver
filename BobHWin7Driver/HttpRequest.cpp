/*! @file
**********************************************************************
<PRE>
ģ����       :
�ļ���       : HttpRequest.cpp
����ļ�     : HttpRequest.h
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
#include "HttpRequest.h"
#include <WinSock.h>
#include <iostream>
#pragma warning(disable : 4996)
#pragma comment(lib, "ws2_32.lib")

HttpRequest::HttpRequest(const std::string& ip, int port) : m_ip(ip), m_port(port)
{
}


HttpRequest::~HttpRequest(void)
{
}

// Http GET����
std::string HttpRequest::HttpGet(std::string req)
{
	std::string ret = ""; // ����Http Response
	try
	{
		// ��ʼ����socket��ʼ��
		WSADATA wData;
		::WSAStartup(MAKEWORD(2, 2), &wData);

		SOCKET clientSocket = socket(AF_INET, 1, 0);
		struct sockaddr_in ServerAddr = { 0 };
		ServerAddr.sin_addr.s_addr = inet_addr(m_ip.c_str());
		ServerAddr.sin_port = htons(m_port);
		ServerAddr.sin_family = AF_INET;
		int errNo = connect(clientSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
		if (errNo == 0)
		{
			//  "GET /[req] HTTP/1.1\r\n"  
			//  "Connection:Keep-Alive\r\n"
			//  "Accept-Encoding:gzip, deflate\r\n"  
			//  "Accept-Language:zh-CN,en,*\r\n"
			//  "User-Agent:Mozilla/5.0\r\n\r\n";
			std::string strSend = " HTTP/1.1\r\n";
			strSend = "GET " + req + strSend;
			strSend += "Connection:Keep-Alive\r\n";
			strSend += "Accept-Encoding:gzip, deflate\r\n";
			strSend += "Accept-Language:zh-CN,en,*\r\n";
			strSend += "host:admin.zhongxia5211.cn:8080\r\n";
			strSend += "User-Agent:Mozilla/5.0\r\n\r\n";
			// ����
			errNo = send(clientSocket, strSend.c_str(), strSend.length(), 0);
			if (errNo > 0)
			{
				//cout << "���ͳɹ�" << endl;
			}
			else
			{
				std::cout << "errNo:" << errNo << std::endl;
				return ret;
			}

			// ����
			char bufRecv[3069] = { 0 };
			errNo = recv(clientSocket, bufRecv, 3069, 0);
			if (errNo > 0)
			{
				ret = bufRecv;// ������ճɹ����򷵻ؽ��յ���������  
			}
			else
			{
				std::cout << "errNo:" << errNo << std::endl;
				return ret;
			}
		}
		else
		{
			errNo = WSAGetLastError();
			std::cout << "errNo:" << errNo << std::endl;
		}
		// socket��������
		::WSACleanup();
	}
	catch (...)
	{
		return "";
	}
	return ret;
}

// Http POST����
std::string HttpRequest::HttpPost(std::string req, std::string data)
{
	std::string ret = ""; // ����Http Response
	try
	{
		// ��ʼ����socket��ʼ��;
		WSADATA wData;
		::WSAStartup(MAKEWORD(2, 2), &wData);

		SOCKET clientSocket = socket(AF_INET, 1, 0);
		struct sockaddr_in ServerAddr = { 0 };
		ServerAddr.sin_addr.s_addr = inet_addr(m_ip.c_str());
		ServerAddr.sin_port = htons(m_port);
		ServerAddr.sin_family = AF_INET;
		int errNo = connect(clientSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
		if (errNo == 0)
		{
			// ��ʽ��data����
			char len[10] = { 0 };
			sprintf(len, "%d", data.length());
			std::string strLen = len;

			//  "POST /[req] HTTP/1.1\r\n"
			//  "Connection:Keep-Alive\r\n"
			//  "Accept-Encoding:gzip, deflate\r\n"
			//  "Accept-Language:zh-CN,en,*\r\n"
			//  "Content-Length:[len]\r\n"
			//  "Content-Type:application/x-www-form-urlencoded; charset=UTF-8\r\n"
			//  "User-Agent:Mozilla/5.0\r\n\r\n"
			//  "[data]\r\n\r\n";
			std::string strSend = " HTTP/1.1\r\n"
				"Cookie:16888\r\n"
				"Content-Type:application/x-www-form-urlencoded\r\n"
				"Charset:utf-8\r\n"
				"Content-Length:";
			strSend = "POST " + req + strSend + strLen + "\r\n\r\n" + data;

			// ����
			errNo = send(clientSocket, strSend.c_str(), strSend.length(), 0);
			if (errNo > 0)
			{
				//cout<<"���ͳɹ�\n";
			}
			else
			{
				std::cout << "errNo:" << errNo << std::endl;
				return ret;
			}

			// ����
			char bufRecv[3069] = { 0 };
			errNo = recv(clientSocket, bufRecv, 3069, 0);
			if (errNo > 0)
			{
				ret = bufRecv;// ������ճɹ����򷵻ؽ��յ���������
			}
			else
			{
				std::cout << "errNo:" << errNo << std::endl;
				return ret;
			}
		}
		else
		{
			errNo = WSAGetLastError();
		}
		// socket��������
		::WSACleanup();
	}
	catch (...)
	{
		return "";
	}
	return ret;
}

// �ϳ�JSON�ַ���
std::string HttpRequest::genJsonString(std::string key, int value)
{
	char buf[128] = { 0 };
	sprintf(buf, "{\"%s\":%d}", key.c_str(), value);
	std::string ret = buf;
	return ret;
}

// �ָ��ַ���
std::vector<std::string> HttpRequest::split(const std::string &s, const std::string &seperator)
{
	std::vector<std::string> result;
	typedef std::string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		// �ҵ��ַ������׸������ڷָ�������ĸ
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		// �ҵ���һ���ָ������������ָ���֮����ַ���ȡ��
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}

// ��Response�в���key��Ӧ��Header������
std::string HttpRequest::getHeader(std::string respose, std::string key)
{
	std::vector<std::string> lines = split(respose, "\r\n");
	for (int i = 0; i < lines.size(); i++)
	{
		std::vector<std::string> line = split(lines[i], ": ");// ע��ո�
		if (line.size() >= 2 && line[0] == key)
		{
			return line[1];
		}
	}
	return "";
}
