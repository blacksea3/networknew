#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "Log.h"

//HTTP响应类

class HttpRequest
{
private:
	//类常量-恒定内容
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
	const std::string LOGFILENAME = "log/log.txt";                   //日志文件路径

	const std::string HTTP_SERVER = "Server: jxthttpd/0.0.1\r\n";
	//const std::string HTTP_CHARSET = "Content-Type: text/html; charset=utf-8\r\n";
	const std::string HTTP_EMPTR_LINE = "\r\n";

	//200响应html/css/js前缀路径
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



	//类常量-设置
	const bool PRINT_ALL_RAW_DATA_DEBUG = false;  //如果true: 仅打印所有请求信息, 全部按照404响应(或其他单一响应), 否则正常响应

private:  //私有变量
	unsigned int clientSocketID;   //客户端SocketID  (其实就是SOCKET类型)
	Mlog* pmLog;          //Mlog指针, 这是唯一的, Mlog是单例设计

private:  //对win32 socket api部分函数的封装, 牺牲效率, 提高可读性
	std::vector<std::string> getRequestContent();      //获取多行数据放入vector中, 每个string内都有一个换行符

private:  //上层函数, HTTP相关
	void CommonResponse(std::string s, bool isRenderFile, enum HttpRequest::HTTPCODE hc);  //通用响应函数
	//以下5个方法: s: 备选字符串, isRenderFile: 是否用文件内容
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
	bool IsFileOccur(const char * filename);        //判断文件是否存在
	bool SendFileContent(const char * filename);    //发送文件内容
private: //其他函数
	inline bool isEndWith(std::string total, std::string find) { return (total.rfind(find) == (total.size() - find.size())); };
public:
	HttpRequest(unsigned int c);
	void acceptRequestInterface();                  //接受请求接口
	void closeRequestInterface();                   //关闭请求接口
	~HttpRequest();
};

void acceptRequestThread(unsigned int client);                    //供子进程调用的函数入口
