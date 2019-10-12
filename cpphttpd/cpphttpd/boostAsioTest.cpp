//boost需要
#ifdef _MSC_VER
#define _WIN32_WINNT 0x0601   
#endif
#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING

#define _CRT_SECURE_NO_WARNINGS

//库
#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

const unsigned short PORT = 8080;

//int main()
//{
//	typedef boost::asio::io_service ioservice;
//	typedef boost::asio::ip::tcp tcp;
//
//	try
//	{
//		ioservice ios;
//		/*
//			*以下构造函数相当于以下步骤
//			* basic_socket_acceptor<tcp> acceptor(io_service);
//			* acceptor.open(tcp::v4());
//			* acceptor.bind(tcp::endpoint(13));
//			* acceptor.listen(0);//default
//		*/
//		/*
//			tcp::endpoint(端点)由以下三个部分组成
//			1. ip地址(address, address_v4, address_v6)
//			2. 端口号
//			3. 协议版本
//		*/
//		tcp::acceptor acceptor(ios, tcp::endpoint(tcp::v4(), 13));
//		for (;;)
//		{
//			tcp::socket socket(ios);
//			acceptor.accept(socket);
//
//			for (;;)
//			{
//				std::string temp;
//				std::cout << "input a string to sent to the client: , if input exit, close the connect";
//				std::cin >> temp;
//
//				if (temp == "exit")
//				{
//					boost::system::error_code ignored_error;
//					boost::asio::write(socket,
//						boost::asio::buffer(temp),
//						boost::asio::transfer_all(),
//						ignored_error);
//					std::cout << "bye" << std::endl;
//					break;
//				}
//				else
//				{
//					boost::system::error_code ignored_error;
//					boost::asio::write(socket,
//						boost::asio::buffer(temp),
//						boost::asio::transfer_all(),
//						ignored_error);
//				}
//			}
//			/*
//			std::string message = make_daytime_string();
//			boost::system::error_code ignored_error;
//			boost::asio::write(socket,
//				boost::asio::buffer(message),
//				boost::asio::transfer_all(),
//				ignored_error);*/
//		}
//	}
//	catch (std::exception& e)
//	{
//		std::cout << e.what() << std::endl;
//	}
//
//	return 0;
//}
