#pragma once
//HTTP��������

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
	const u_short DEFAULTPORT = 8090;				    //Ĭ�϶˿�
	const int SEND_TIMEOUT = 1000;                      //���ͳ�ʱ1s
	const int RECV_TIMEOUT = 1000;                      //���ܳ�ʱ1s
	u_short port;                                       //��ǰ�˿�

	SOCKET StartServer(u_short port);

	const bool useParentDir = true;  //���true, ʹ�õ�ǰĿ¼�ĸ�Ŀ¼��Ϊ��ĿĿ¼, ����ֱ�ӽ���ǰĿ¼��Ϊ��ĿĿ¼
public:
	HttpServer();
	void InitServer();
	void RunServer();

	~HttpServer();
};
