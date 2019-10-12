#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "Log.h"

//HTTP��Ӧ��

class HttpRequest
{
private:
	//�ೣ��-�㶨����
	enum HTTPCODE{HTTP200, HTTP404, HTTP400, HTTP500, HTTP501};
	const std::string LOGFILENAME = "log/log.txt";                   //��־�ļ�·��
	const std::string HTTP_CODE200 = "HTTP/1.1 200 OK\r\n";
	const std::string HTTP_CODE404 = "HTTP/1.1 404 NOT FOUND\r\n";
	const std::string HTTP_CODE400 = "HTTP/1.1 400 BAD REQUEST\r\n";
	const std::string HTTP_CODE500 = "HTTP/1.1 500 Internal Server Error\r\n";
	const std::string HTTP_CODE501 = "HTTP/1.0 501 Method Not Implemented\r\n";
	const std::string HTTP_SERVER = "Server: jxthttpd/0.0.1\r\n";
	const std::string HTTP_CHARSET = "Content-Type: text/html;charset=utf-8\r\n";
	const std::string HTTP_EMPTR_LINE = "\r\n";

	const std::string FILETYPE_HTML = "html";
	const std::string HTML_FILE_DIRECTORY = "html/";
	const std::string FILE_NOTFOUND = "html/404.html";
	const std::string FILE_BAD_REQUEST = "html/400.html";
	const std::string FILE_METHOD_NOT_IMPLEMENTED = "html/501.html";
	const std::string FILE_INTERNAL_SERVER_ERROR = "html/500.html";

	const std::string DEFAULT_FILE = "index.html";



	//�ೣ��-����
	const bool PRINT_ALL_RAW_DATA_DEBUG = false;  //���true: ����ӡ����������Ϣ, ȫ������404��Ӧ(��������һ��Ӧ), ����������Ӧ

private:  //˽�б���
	unsigned int clientSocketID;   //�ͻ���SocketID  (��ʵ����SOCKET����)
	Mlog* pmLog;          //Mlogָ��, ����Ψһ��, Mlog�ǵ������

private:  //��win32 socket api���ֺ����ķ�װ, ����Ч��, ��߿ɶ���
	std::vector<std::string> getRequestContent();      //��ȡ�������ݷ���vector��, ÿ��string�ڶ���һ�����з�

private:  //�ϲ㺯��, HTTP���
	void CommonResponse(std::string s, bool isRenderFile, enum HTTPCODE hc);  //ͨ����Ӧ����
	//����5������: s: ��ѡ�ַ���, isRenderFile: �Ƿ����ļ�����
	inline void NormalRequest(std::string s, bool isRenderFile)
	{
		this->CommonResponse(s, isRenderFile, HTTP200);
	};
	inline void NotFound(std::string s = "not found", bool isRenderFile = true)
	{
		this->CommonResponse(s, isRenderFile, HTTP404);
	};
	void BadRequest(std::string s = "bad request", bool isRenderFile = true)
	{
		this->CommonResponse(s, isRenderFile, HTTP400);
	};
	void MethodNotImplemented(std::string s = "method not implemented", bool isRenderFile = true)
	{
		this->CommonResponse(s, isRenderFile, HTTP501);
	};
	void InternalServerError(std::string s = "internal server error", bool isRenderFile = true)
	{
		this->CommonResponse(s, isRenderFile, HTTP500);
	};
	bool SendFileContent(const char * filename);    //�����ļ�����
private: //��������
	inline bool isEndWith(std::string total, std::string find) { return (total.rfind(find) == (total.size() - find.size())); };
public:
	HttpRequest(unsigned int c);
	void acceptRequestInterface();                  //��������ӿ�
	void closeRequestInterface();                   //�ر�����ӿ�
	~HttpRequest();
};

void acceptRequestThread(unsigned int client);                    //���ӽ��̵��õĺ������
