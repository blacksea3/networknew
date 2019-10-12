#include "HttpServer.h"
#include "HttpRequest.h"


/*
 * 构造函数: 初始化HttpServer类, 初始化类变量
 */
HttpServer::HttpServer():port(DEFAULTPORT)
{
	;
}

/*
 * 初始化服务器
 * 加载Ws2_32.dll, 生成项目目录
 */
void HttpServer::InitServer()
{
	//初始化套接字库
	WORD w_req = MAKEWORD(2, 2);//版本号
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		std::cout << "初始化套接字库失败！" << std::endl;
	}
	else {
		std::cout << "初始化套接字库成功！" << std::endl;
	}
	//检测版本号
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		std::cout << "套接字库版本号不符！" << std::endl;
		WSACleanup();
	}
	else {
		std::cout << "套接字库版本正确！" << std::endl;
	}
	//填充服务端地址信息

	//更新项目目录
	if (useParentDir)
	{
		std::filesystem::path parentFullPath = std::filesystem::absolute(std::filesystem::path("../"));
		std::filesystem::current_path(parentFullPath);
	}
}

/*
 * 开始服务器
 * 即开始TCP服务端
 */
SOCKET HttpServer::StartServer(u_short port)
{
	//定义服务端套接字和地址
	SOCKET s_server;
	SOCKADDR_IN server_addr;

	//填充服务端信息
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);
	//创建套接字
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		std::cout << "套接字绑定失败！" << std::endl;
		WSACleanup();
	}
	else {
		std::cout << "套接字绑定成功！" << std::endl;
	}
	//设置套接字为监听状态
	if (listen(s_server, SOMAXCONN) < 0) {
		std::cout << "设置监听状态失败！" << std::endl;
		WSACleanup();
	}
	else {
		std::cout << "设置监听状态成功！" << std::endl;
	}

	//设置超时
	setsockopt(s_server, SOL_SOCKET, SO_RCVTIMEO, (char *)&this->RECV_TIMEOUT, sizeof(int));
	setsockopt(s_server, SOL_SOCKET, SO_SNDTIMEO, (char *)&this->SEND_TIMEOUT, sizeof(int));

	std::cout << "服务端正在监听连接，请稍候...." << std::endl;
	return s_server;
}

/*
 * 运行服务器, 此方法内置while(1)循环!
 */
void HttpServer::RunServer()
{
	//定义长度变量
	int send_len = 0;
	int recv_len = 0;
	int len = 0;
	//定义服务端套接字，接受请求套接字
	SOCKET ss_server;
	SOCKET s_accept;
	SOCKADDR_IN accept_addr;

	ss_server = this->StartServer(8868);
	//接受连接请求
	len = sizeof(SOCKADDR);

	while (1)
	{
		s_accept = accept(ss_server, (SOCKADDR *)&accept_addr, &len);
		if (s_accept == SOCKET_ERROR) {
			std::cout << "连接失败！" << std::endl;
			WSACleanup();
			throw new std::exception("FATAL ERROR");
		}
		std::cout << "连接建立，准备接受数据" << std::endl;
		
		//接收数据
		//acceptRequestThread(s_accept);  //单线程
		//每次收到请求，创建一个线程来处理接受到的请求
		//把client_sock转成地址作为参数传入
		std::thread t1(acceptRequestThread, s_accept);
		t1.join();										//开启t1线程
		//关闭套接字
		closesocket(s_accept);

		//std::cout << "请输入回复信息:";
		//std::cin >> send_buf;
		//send_len = send(s_accept, send_buf, 100, 0);
		//if (send_len < 0) {
		//	std::cout << "发送失败！" << std::endl;
		//	break;
		//}
	}
	closesocket(ss_server);
	//释放DLL资源
	WSACleanup();
}

/*
 * 默认析构函数
 */
HttpServer::~HttpServer()
{
	;
}

int main()
{	
	HttpServer hs = HttpServer();
	hs.InitServer();
	hs.RunServer();
	return 0;
}

