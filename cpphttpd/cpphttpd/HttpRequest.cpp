#include "HttpRequest.h"
#include "HttpServer.h"
#include "HttpStringParser.h"

/*
 * �ַ����ָ��
 */
static std::vector<std::string> split(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;//��չ�ַ����Է������
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
 * ��ȡ��ǰ��������·��
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
 * �����������, �ڲ������д���
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
			std::cout << "����ʧ�ܣ�" << std::endl;
			break;
		}
		else if (recv_len == 0)
		{
			std::cout << "�ͻ��������ر�" << std::endl;
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
				std::cout << "����!" << std::endl;
				break;
			}
		}
	}
	std::vector<std::string> res = split(total, "/r/n");
	if (res.size() == 1 && res[0].empty()) //�ͻ��������ر�, ������
		res.pop_back();
	return res;
}

/*
 * ͨ������
 * s: ��ѡ�ַ���, ���ڷ�200��Ӧ��Ĭ�Ϸ�����Ϣ, ����200��Ӧ���ļ�·��
 *  , isRenderFile: �Ƿ����ļ�����, (enum)en��������
 */
void HttpRequest::CommonResponse(std::string s, bool isRenderFile, enum HttpRequest::HTTPCODE hc)
{
	HttpRequest::CONTENT_TYPE ct = HttpRequest::CONTENT_TYPE::HTML;   //Ĭ���ļ�������html, ��Content-TypeĬ��ֵ

	if (isRenderFile && hc == HttpRequest::HTTP200)    //�ļ�·����չ
	{
		//���ļ�����
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

		//ǰ�߼�: ���200�����ļ���������ô�ĳ�404
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
	if (isRenderFile)  //ʹ���ļ�
	{
		if (hc == HttpRequest::HTTP200)
		{
			this->SendFileContent(s.c_str()); //����ǰ���ж��ļ�����, �ʴ˴�һ�����Է����ļ�
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
 * ���ж��ļ��Ƿ����
 * ����true˵���ļ�����, false˵���ļ�������
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
 * �����ļ�����
 * ����true˵���ļ����ڷ��ͳɹ�, false˵���ļ�������, ��Ҫ���������д���content����
 *    ע��: �˴��õ�����Ŀ¼, filename �������¸�ʽhtml/404.html
 *    ���: ��filesystem���ʽ�����Ŀ¼, ����C api�����ļ���д
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
		//ѭ����
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
 * HttpRequest���ʼ��, ��ʼ���ͻ���SocketID
 * ��ʼ����ĿĿ¼
 */
HttpRequest::HttpRequest(unsigned int c) :clientSocketID(c)
{
	pmLog = Mlog::Instance();
}

/*
 * ���������ܽӿ�
 */
void HttpRequest::acceptRequestInterface()
{
	/*
	//���峤�ȱ���
	int send_len = 0;
	int recv_len = 0;
	int len = 0;
	//���巢�ͻ������ͽ��ܻ�����
	char send_buf[100];
	char recv_buf[100];
	while (1) {
		recv_len = recv(this->clientSocketID, recv_buf, 100, 0);
		if (recv_len < 0) {
			std::cout << "����ʧ�ܣ�" << std::endl;
			break;
		}
		else {
			std::cout << "�ͻ�����Ϣ:" << recv_buf << std::endl;
		}
		std::cout << "������ظ���Ϣ:";
		std::cin >> send_buf;
		send_len = send(this->clientSocketID, send_buf, 100, 0);
		if (send_len < 0) {
			std::cout << "����ʧ�ܣ�" << std::endl;
			break;
		}
	}*/
	
	//��ȡ��������
	pmLog->print(this->LOGFILENAME, "                                 ");
	pmLog->print(this->LOGFILENAME, "=================================");
	pmLog->print(this->LOGFILENAME, "begin accept_request");
	std::vector<std::string> requestContent =  this->getRequestContent();

	//��ӡrequest������Ϣ
	for (auto& rC : requestContent) this->pmLog->print(this->LOGFILENAME, rC);

	if (requestContent.empty())
		this->pmLog->print(this->LOGFILENAME, "Confusing HTTP data: empty");
	else if (PRINT_ALL_RAW_DATA_DEBUG)
		this->NotFound("NOT FOUNDDDDD", true);
	else
	{
		httpMethodStr hms = parserFirstLine(requestContent[0]);   //������һ���ַ���
		if (hms.httpMethod == HTTPMETHOD::HTTPMETHOD_OTHER)
		{
			this->MethodNotImplemented("NOT IMPLEMENTED!!", true);
		}
		else if (hms.httpMethod == HTTPMETHOD::HTTPMETHOD_GET_PARAERROR)
		{
			this->BadRequest("BAD REQUEST!!", true);
		}
		else if (hms.httpMethod == HTTPMETHOD::HTTPMETHOD_GET_COMMON ||
			hms.httpMethod == HTTPMETHOD::HTTPMETHOD_POST)   //�����ļ�
		{
			//this->NotFound("NOT FOUNDDDDD", true);
			//���hms.dirΪ��, ��ʹ��DEFAULT_FILE
			//�������hms.dir[0] == '/'��ȥ��ͷ��(ʵ����һ����'/')����hms.dir
			//����ֱ����hms.dir
			if (hms.dir.empty()) this->NormalRequest(DEFAULT_FILE, true);
			else if (hms.dir[0] == '/')
			{
				hms.dir.erase(hms.dir.begin());
				this->NormalRequest(hms.dir, true);
			}
			else this->NormalRequest(hms.dir, true);
			//ִ��cgi�ļ�
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
 * ���̺߳���
 */
void acceptRequestThread(unsigned int client)
{
	HttpRequest h(client);
	h.acceptRequestInterface();
	h.closeRequestInterface();
}
