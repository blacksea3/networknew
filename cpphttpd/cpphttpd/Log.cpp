#include "Log.h"

Mlog::Mlog(const std::string &name)
{
	_name = name;
	//std::cout << "This class Mlog, Mlog: object name:" << _name.c_str() << std::endl;
}

Mlog::~Mlog()
{
	//std::cout << "This class Mlog, ~Mlog: object name:" << _name.c_str() << std::endl;
}

//�Ҳ����ҵ�Instance�ĺ�������,�²�Ϊ
//������
//����Mlog������
Mlog* Mlog::Instance()
{
	if (pMlog == NULL)
	{
		pMlog = new Mlog(std::string("111"));
	}
	return pMlog;
}

/*
 * ��ӡ��Ϣ:
 * s���Զ����ַ���
 * Ŀ���ӡһ����Ϣ, ��s�͵�ǰʱ��
 */
void Mlog::print(std::string filename, std::string content)
{
	//���content�л��з�, ��ɾ����
	if (!content.empty() && content.back() == '\n') content.pop_back();

	std::ofstream out(filename.c_str(), std::ios::app);
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);

	char timestr[26];
	ctime_s(timestr, sizeof timestr, &end_time);
	out << content << " time: "<< timestr;  //ע:timestr�Դ����з�
	out.close();
}

//����
void Mlog::destroy()
{
	if (pMlog != NULL)
	{
		delete pMlog;
		pMlog = NULL;
	}
}

Mlog *Mlog::pMlog = NULL;
