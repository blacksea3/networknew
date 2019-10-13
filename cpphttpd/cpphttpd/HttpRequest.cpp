#include "HttpRequest.h"
#include "HttpServer.h"
#include "HttpStringParser.h"

/*
 * 字符串分割函数
 */
static std::vector<std::string> split(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;//扩展字符串以方便操作
	int size = str.size();

	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			result.emplace_back(str.substr(i, pos - i));
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

/*
 * 获取当前程序运行路径
 */
static std::string GetProgramDir()
{
	char exeFullPath[MAX_PATH]; // Full path
	std::string strPath = "";
	GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);
	strPath = (std::string)exeFullPath;    // Get full path of the file
	int pos = strPath.find_last_of('\\', strPath.length());
	return strPath.substr(0, pos);  // Return the directory without the file name
}

/*
 * 获得请求内容, 内部做分行处理
 */
std::vector<std::string> HttpRequest::getRequestContent()
{
	int recv_len = 0;
	char recv_buf[101];
	std::string total;
	while (1)
	{
		memset(recv_buf, 0, 101);
		recv_len = recv(this->clientSocketID, recv_buf, 100, 0);
		if (recv_len < 0) {
			std::cout << "接受失败！" << std::endl;
			break;
		}
		else if (recv_len == 0)
		{
			std::cout << "客户端主动关闭" << std::endl;
			break;
		}
		else
		{
			total += (std::string)recv_buf;
		}

		if (total.size() >= 4)
		{
			int tSize = total.size();
			if (total[tSize - 4] == '\r' && total[tSize - 3] == '\n' &&
				total[tSize - 2] == '\r' && total[tSize - 1] == '\n')
			{
				std::cout << total << std::endl;
				std::cout << "结束!" << std::endl;
				break;
			}
		}
	}
	std::vector<std::string> res = split(total, "/r/n");
	if (res.size() == 1 && res[0].empty()) //客户端主动关闭, 空数据
		res.pop_back();
	return res;
}

/*
 * 通用请求
 * s: 备选字符串, 对于非200响应是默认发送消息, 对于200响应是文件路径
 *  , isRenderFile: 是否用文件内容, (enum)en请求类型
 */
void HttpRequest::CommonResponse(std::string s, bool isRenderFile, enum HttpRequest::HTTPCODE hc)
{
	HttpRequest::CONTENT_TYPE ct = HttpRequest::CONTENT_TYPE::HTML;   //默认文件类型是html, 即Content-Type默认值

	if (isRenderFile && hc == HttpRequest::HTTP200)    //文件路径扩展
	{
		//找文件类型
		if (this->isEndWith(s, this->FILETYPE_HTML))	//html
		{
			s = this->HTML_FILE_DIRECTORY + s;
		}
		if (this->isEndWith(s, this->FILETYPE_CSS))		//css
		{
			s = this->CSS_FILE_DIRECTORY + s;
			ct = HttpRequest::CONTENT_TYPE::CSS;
		}
		if (this->isEndWith(s, this->FILETYPE_JS))		//js
		{
			s = this->JS_FILE_DIRECTORY + s;
			ct = HttpRequest::CONTENT_TYPE::JS;
		}

		//前逻辑: 如果200但是文件不存在那么改成404
		if (!IsFileOccur(s.c_str()))
		{
			s = "NOT FOUNDDDDD";
			hc = HttpRequest::HTTP404;
		}
	}

	send(this->clientSocketID, this->HTTP_CODE.at(hc).c_str() , (int)this->HTTP_CODE.at(hc).size(), 0);
	send(this->clientSocketID, this->HTTP_SERVER.c_str(), (int)this->HTTP_SERVER.size(), 0);
	send(this->clientSocketID, this->HTTP_CONTENT_TYPE.at(ct).c_str(), (int)this->HTTP_CONTENT_TYPE.at(ct).size(), 0);
	send(this->clientSocketID, this->HTTP_EMPTR_LINE.c_str(), (int)this->HTTP_EMPTR_LINE.size(), 0);
	if (isRenderFile)  //使用文件
	{
		if (hc == HttpRequest::HTTP200)
		{
			this->SendFileContent(s.c_str()); //由于前述判断文件存在, 故此处一定可以发送文件
		}
		else
		{
			if (!this->SendFileContent(this->HTTP_FILEPATH.at(hc).c_str()))
				send(this->clientSocketID, s.c_str(), (int)s.size(), 0);
		}
	}
	else
		send(this->clientSocketID, s.c_str(), (int)s.size(), 0);
}

/*
 * 仅判断文件是否存在
 * 返回true说明文件存在, false说明文件不存在
 */
bool HttpRequest::IsFileOccur(const char * filename)
{
	std::filesystem::path filePath = std::filesystem::absolute(filename);
	std::string windowsTypePath = filePath.string();
	FILE *resource = NULL;
	errno_t err;
	err = fopen_s(&resource, windowsTypePath.c_str(), "r");
	if (err != 0) return false;
	else
	{
		fclose(resource);
		return true;
	}
}

/*
 * 发送文件内容
 * 返回true说明文件存在发送成功, false说明文件不存在, 需要调用者自行处理content内容
 *    注意: 此处用到工作目录, filename 按照如下格式html/404.html
 *    因此: 用filesystem库格式化这个目录, 再用C api进行文件读写
 */
bool HttpRequest::SendFileContent(const char * filename)
{
	std::filesystem::path filePath = std::filesystem::absolute(filename);
	std::string windowsTypePath = filePath.string();

	FILE *resource = NULL;
	errno_t err;
	err = fopen_s(&resource, windowsTypePath.c_str(), "r");
	if (err != 0) return false;
	else
	{
		char buf[1024];
		fgets(buf, sizeof(buf), resource);
		//循环读
		while (!feof(resource))
		{
			send(this->clientSocketID, buf, strlen(buf), 0);
			fgets(buf, sizeof(buf), resource);
		}
		if (strlen(buf) != 0) send(this->clientSocketID, buf, strlen(buf), 0);
		fclose(resource);
		return true;
	}
}

/*
 * HttpRequest类初始化, 初始化客户端SocketID
 * 初始化项目目录
 */
HttpRequest::HttpRequest(unsigned int c) :clientSocketID(c)
{
	pmLog = Mlog::Instance();
}

/*
 * 接受请求总接口
 */
void HttpRequest::acceptRequestInterface()
{
	/*
	//定义长度变量
	int send_len = 0;
	int recv_len = 0;
	int len = 0;
	//定义发送缓冲区和接受缓冲区
	char send_buf[100];
	char recv_buf[100];
	while (1) {
		recv_len = recv(this->clientSocketID, recv_buf, 100, 0);
		if (recv_len < 0) {
			std::cout << "接受失败！" << std::endl;
			break;
		}
		else {
			std::cout << "客户端信息:" << recv_buf << std::endl;
		}
		std::cout << "请输入回复信息:";
		std::cin >> send_buf;
		send_len = send(this->clientSocketID, send_buf, 100, 0);
		if (send_len < 0) {
			std::cout << "发送失败！" << std::endl;
			break;
		}
	}*/
	
	//获取请求数据
	pmLog->print(this->LOGFILENAME, "                                 ");
	pmLog->print(this->LOGFILENAME, "=================================");
	pmLog->print(this->LOGFILENAME, "begin accept_request");
	std::vector<std::string> requestContent =  this->getRequestContent();

	//打印request所有信息
	for (auto& rC : requestContent) this->pmLog->print(this->LOGFILENAME, rC);

	if (requestContent.empty())
		this->pmLog->print(this->LOGFILENAME, "Confusing HTTP data: empty");
	else if (PRINT_ALL_RAW_DATA_DEBUG)
		this->NotFound("NOT FOUNDDDDD", true);
	else
	{
		httpMethodStr hms = parserFirstLine(requestContent[0]);   //解析第一行字符串
		if (hms.httpMethod == HTTPMETHOD::HTTPMETHOD_OTHER)
		{
			this->MethodNotImplemented("NOT IMPLEMENTED!!", true);
		}
		else if (hms.httpMethod == HTTPMETHOD::HTTPMETHOD_GET_PARAERROR)
		{
			this->BadRequest("BAD REQUEST!!", true);
		}
		else if (hms.httpMethod == HTTPMETHOD::HTTPMETHOD_GET_COMMON ||
			hms.httpMethod == HTTPMETHOD::HTTPMETHOD_POST)   //查找文件
		{
			//this->NotFound("NOT FOUNDDDDD", true);
			//如果hms.dir为空, 则使用DEFAULT_FILE
			//否则如果hms.dir[0] == '/'则去除头部(实际上一定是'/')再用hms.dir
			//否则直接用hms.dir
			if (hms.dir.empty()) this->NormalRequest(DEFAULT_FILE, true);
			else if (hms.dir[0] == '/')
			{
				hms.dir.erase(hms.dir.begin());
				this->NormalRequest(hms.dir, true);
			}
			else this->NormalRequest(hms.dir, true);
			//执行cgi文件
			//execute_cgi(client, path, method, query_string);
		}
	}
	this->pmLog->print(this->LOGFILENAME, "end accept_request");
}

void HttpRequest::closeRequestInterface()
{
	this->pmLog->destroy();
}

HttpRequest::~HttpRequest()
{
    
}

/*
 * 子线程函数
 */
void acceptRequestThread(unsigned int client)
{
	HttpRequest h(client);
	h.acceptRequestInterface();
	h.closeRequestInterface();
}
