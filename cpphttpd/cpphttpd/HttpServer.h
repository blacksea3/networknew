#pragma once
//HTTP服务器类

#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ws2tcpip.h>
#include <thread>
#include <filesystem>
#include "HttpRequest.h"

class HttpServer
{
private:
	const u_short DEFAULTPORT = 8090;				    //默认端口
	const int SEND_TIMEOUT = 1000;                      //发送超时1s
	const int RECV_TIMEOUT = 1000;                      //接受超时1s
	u_short port;                                       //当前端口

	SOCKET StartServer(u_short port);

	const bool useParentDir = true;  //如果true, 使用当前目录的父目录作为项目目录, 否则直接将当前目录作为项目目录
public:
	HttpServer();
	void InitServer();
	void RunServer();

	~HttpServer();
};
