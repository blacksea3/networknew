#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "Log.h"

//HTTP��Ӧ��

class HttpRequest
{
private:
	//�ೣ��-�㶨����
	enum HTTPCODE{HTTP200, HTTP404, HTTP400, HTTP500, HTTP501};
	const std::string HTTP_CODE200 = "HTTP/1.1 200 OK\r\n";
	const std::string HTTP_CODE404 = "HTTP/1.1 404 NOT FOUND\r\n";
	const std::string HTTP_CODE400 = "HTTP/1.1 400 BAD REQUEST\r\n";
	const std::string HTTP_CODE500 = "HTTP/1.1 500 Internal Server Error\r\n";
	const std::string HTTP_CODE501 = "HTTP/1.0 501 Method Not Implemented\r\n";
	const std::unordered_map<int, int> fuck = { {1,11},{2,22},{3,33} };
	const std::unordered_map<HttpRequest::HTTPCODE, std::string> HTTP_CODE =
		{
			{HTTP200, HTTP_CODE200},
			{HTTP404, HTTP_CODE404},
			{HTTP400, HTTP_CODE400},
			{HTTP500, HTTP_CODE500},
			{HTTP501, HTTP_CODE501}
		};
	const std::string FILE_NOTFOUND = "production/404.html";
	const std::string FILE_BAD_REQUEST = "production/400.html";
	const std::string FILE_METHOD_NOT_IMPLEMENTED = "production/501.html";
	const std::string FILE_INTERNAL_SERVER_ERROR = "production/500.html";
	const std::unordered_map<HttpRequest::HTTPCODE, std::string> HTTP_FILEPATH =
	{
		{HTTP404, FILE_NOTFOUND},
		{HTTP400, FILE_BAD_REQUEST},
		{HTTP500, FILE_INTERNAL_SERVER_ERROR},
		{HTTP501, FILE_METHOD_NOT_IMPLEMENTED}
	};
	const std::string LOGFILENAME = "log/log.txt";                   //��־�ļ�·��

	const std::string HTTP_SERVER = "Server: jxthttpd/0.0.1\r\n";
	//const std::string HTTP_CHARSET = "Content-Type: text/html; charset=utf-8\r\n";
	const std::string HTTP_EMPTR_LINE = "\r\n";

	//200��Ӧhtml/css/jsǰ׺·��
	const std::string FILETYPE_HTML = "html";
	const std::string HTML_FILE_DIRECTORY = "";
	const std::string FILETYPE_CSS = "css";
	const std::string CSS_FILE_DIRECTORY = "";
	const std::string FILETYPE_JS = "js";
	const std::string JS_FILE_DIRECTORY = "";
	enum CONTENT_TYPE { HTML, CSS, JS };

	const std::unordered_map<HttpRequest::CONTENT_TYPE, std::string> HTTP_CONTENT_TYPE =
	{
		{HTML, "Content-Type: text/html\r\n"},
		{CSS, "Content-Type: text/css\r\n"},
		{JS, "Content-Type: text/javascript\r\n"}
	};

	const std::string DEFAULT_FILE = "index.html";



	//�ೣ��-����
	const bool PRINT_ALL_RAW_DATA_DEBUG = false;  //���true: ����ӡ����������Ϣ, ȫ������404��Ӧ(��������һ��Ӧ), ����������Ӧ

private:  //˽�б���
	unsigned int clientSocketID;   //�ͻ���SocketID  (��ʵ����SOCKET����)
	Mlog* pmLog;          //Mlogָ��, ����Ψһ��, Mlog�ǵ������

private:  //��win32 socket api���ֺ����ķ�װ, ����Ч��, ��߿ɶ���
	std::vector<std::string> getRequestContent();      //��ȡ�������ݷ���vector��, ÿ��string�ڶ���һ�����з�

private:  //�ϲ㺯��, HTTP���
	void CommonResponse(std::string s, bool isRenderFile, enum HttpRequest::HTTPCODE hc);  //ͨ����Ӧ����
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
	bool IsFileOccur(const char * filename);        //�ж��ļ��Ƿ����
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
