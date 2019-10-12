#include "HttpStringParser.h"

//������һ��
//��������"�ϸ�"����:
//���ж��Ƿ���ڿո�, ���������зֿ���һ���ո�ͺ����
//�жϵ�һ����������GET, POST������
//��������ֱ���˳�����, ��ӦhttpMethod = HTTPMETHOD_OTHER, ��������Ϊ��
//����GET������ÿո��з�(��һ��), �Ե�һ�ν��н���, �ڶ��ζ���
//  �����һ����û���ʺ�, ˵�����޲���GET; �����ʱĩβ��/��ȥ��
//     ȥ��/�����Ϊ����ĳ�index.html.  �������dir, contentPairs���, ���غ���
//  ����˵�����в���GET:�õ�һ�����ַ����п�, ��һ�η���dir, �ڶ��ν������²���
//     ȫ����&�п�, ÿһ����=�п�(��һ��), ǰһ�����Ǽ�, ��һ������ֵ, ����contentPairs, ���غ���
//       ���ĳһ��û��=�����޷��ֿ�, ��ô����HTTPMETHOD_GET_PARAERROR����, dir��Ϊ��, contentPairsΪ��
//����POST������ÿո��з�, �ѵ�һ�η���dir, �ڶ��ζ���, contentPairs���, ���غ���

//���� "GET /ddasads HTTP/1.1
//     "GET /dssd/dd?i=1&p=2 HTTP/1.1"

httpMethodStr parserFirstLine(std::string input)
{
	httpMethodStr returnhms;
	size_t loc;
	loc = input.find(' ');
	if (loc != input.npos)
	{
		std::string pre = input.substr(0, loc);
		std::string aft = input.substr(loc + 1);
		std::transform(pre.begin(), pre.end(), pre.begin(), ::toupper);
		if (pre == "GET")
		{
			loc = aft.find(' ');
			pre = aft.substr(0, loc);
			aft = aft.substr(loc + 1);

			loc = pre.find('?');
			if (loc == pre.npos) //�޲���GET
			{
				if (pre.back() == '/') pre.pop_back();
				returnhms.dir = pre;
				returnhms.httpMethod = HTTPMETHOD::HTTPMETHOD_GET_COMMON;
				return returnhms;
			}
			else  //�в���GET
			{
				aft = pre.substr(loc + 1);
				pre = pre.substr(0, loc);
				returnhms.dir = pre;
				int aIndex = aft.find('&');
				while (aIndex != aft.npos)
				{
					std::string innerPre = aft.substr(0, aIndex);
					size_t innerSlice = innerPre.find('=');
					if (innerSlice == innerPre.npos)   //��������, ����HTTPMETHOD_GET_PARAERROR
					{
						returnhms.httpMethod = HTTPMETHOD::HTTPMETHOD_GET_PARAERROR;
						returnhms.contentPairs = {};
						return returnhms;
					}
					returnhms.contentPairs[innerPre.substr(0, innerSlice)] = innerPre.substr(innerSlice + 1);
					aft = aft.substr(aIndex + 1);
					aIndex = aft.find('&');
				}
				if (!aft.empty())
				{
					//���һ��
					std::string innerPre = aft.substr(0, aIndex);
					size_t innerSlice = innerPre.find('=');
					if (innerSlice == innerPre.npos)   //��������, ����HTTPMETHOD_GET_PARAERROR
					{
						returnhms.httpMethod = HTTPMETHOD::HTTPMETHOD_GET_PARAERROR;
						returnhms.contentPairs = {};
						return returnhms;
					}
					returnhms.contentPairs[innerPre.substr(0, innerSlice)] = innerPre.substr(innerSlice + 1);
				}

				returnhms.httpMethod = HTTPMETHOD::HTTPMETHOD_GET_COMMON;
				return returnhms;
			}

		}
		else if (pre == "POST")
		{
			loc = aft.find(' ');
			pre = aft.substr(0, loc);
			aft = aft.substr(loc + 1);

			returnhms.dir = pre;
			returnhms.httpMethod = HTTPMETHOD::HTTPMETHOD_POST;
			return returnhms;
		}
		else
		{
			returnhms.httpMethod = HTTPMETHOD::HTTPMETHOD_OTHER;
			return returnhms;
		}
	}
	else
	{
		returnhms.httpMethod = HTTPMETHOD::HTTPMETHOD_OTHER;
		return returnhms;
	}
}

void parserFirstLineTest(void)
{
	std::vector<std::string> inputs = 
	{
		"GET /foo HTTP/1.1",
		"get / HTTP/1.1",
		"GeT /foo/ HTTP/1.1",
		"GET /foo/bar HTTP/1.1",
		"GEt foo/ HTTP/1.1",
		"GEt foo HTTP/1.1",

		"GET /foo ",
		"GET /foo  ",
		"GET /foo  s",

		"GET ? HTTP/1.1",
		"GET foo/?a=2 HTTP/1.1",
		"GET foo/?a=2&b=3 HTTP/1.1",
		"GET foo/?a=2&b=3HTTP/1.1",
		"GET foo/?a2&b=3HTTP/1.1",

		"GET ?a=2 HTTP/1.1",
		"GET ?a=2&b=3 HTTP/1.1",
		"GET ?a=2&b=3HTTP/1.1",
		"GET ?a2&b=3HTTP/1.1",

		"posT /foo/ HTTP/1.1",
		"POST /foo HTTP/1.1",
		"POST /foodsad!DSP%:>a sda w4 aAA HTTP/1.1",
	
		"GS /foo HTTP/1.1",
		""
	};

	std::vector<httpMethodStr> ress =
	{
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "/foo", {}),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "", {}),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "/foo", {}),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "/foo/bar", {}),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "foo", {}),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "foo", {}),

		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "/foo", {}),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "/foo", {}),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "/foo", {}),

		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "", {}),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "foo/", {{"a", "2"}}),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "foo/", {{"a", "2"}, {"b", "3"} }),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "foo/", {{"a", "2"}, {"b", "3HTTP/1.1"} }),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_PARAERROR, "foo/", {}),

		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "", {{"a", "2"} }),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "", {{"a", "2"}, {"b", "3"} }),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_COMMON, "", {{"a", "2"}, {"b", "3HTTP/1.1"} }),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_GET_PARAERROR, "", {}),

		httpMethodStr(HTTPMETHOD::HTTPMETHOD_POST, "/foo/", {}),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_POST, "/foo", {}),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_POST, "/foodsad!DSP%:>a", {}),

		httpMethodStr(HTTPMETHOD::HTTPMETHOD_OTHER, "", {}),
		httpMethodStr(HTTPMETHOD::HTTPMETHOD_OTHER, "", {})
	};

	_ASSERT(inputs.size() == ress.size());

	for (size_t index = 0; index < inputs.size(); ++index)
	{
		httpMethodStr tempres = parserFirstLine(inputs[index]);

		_ASSERT(ress[index] == tempres);
	}

	std::cout << "parserFirstLineTest ok" << std::endl;
}
