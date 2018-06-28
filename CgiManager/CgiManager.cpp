#include "StdAfx.h"
#include "CgiManager.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <cstring>
using namespace std;


// 静态成员初始化
bool CCgiManager::IsSetHead = false;
int CModel::Type = 0;


/* 配置文件类***********************************************************************************************************************************************************************************
 * 用途：读写配置文件
 */

CConfig::CConfig( string filename, string delimiter,  
               string comment )  
               : m_Delimiter(delimiter), m_Comment(comment)  
{  
    // Construct a CConfig, getting keys and values from given file  
  
    std::ifstream in( filename.c_str() );  
  
    if( !in ) throw File_not_found( filename );   
  
    in >> (*this);  
}  
  
  
CConfig::CConfig()  
: m_Delimiter( string(1,'=') ), m_Comment( string(1,'#') )  
{  
    // Construct a CConfig without a file; empty  
}  
  
bool CConfig::KeyExists( const string& key ) const  
{  
    // Indicate whether key is found  
    mapci p = m_Contents.find( key );  
    return ( p != m_Contents.end() );  
}  
  
/* static */  
void CConfig::Trim( string& inout_s )  
{  
    // Remove leading and trailing whitespace  
    static const char whitespace[] = " \n\t\v\r\f";  
    inout_s.erase( 0, inout_s.find_first_not_of(whitespace) );  
    inout_s.erase( inout_s.find_last_not_of(whitespace) + 1U );  
}  
  
std::ostream& operator<<( std::ostream& os, const CConfig& cf )  
{  
    // Save a CConfig to os  
    for( CConfig::mapci p = cf.m_Contents.begin();  
        p != cf.m_Contents.end();  
        ++p )  
    {  
        os << p->first << " " << cf.m_Delimiter << " ";  
        os << p->second << std::endl;  
    }  
    return os;  
}  
  
void CConfig::Remove( const string& key )  
{  
    // Remove key and its value  
    m_Contents.erase( m_Contents.find( key ) );  
    return;  
}  
  
std::istream& operator>>( std::istream& is, CConfig& cf )  
{  
    // Load a CConfig from is  
    // Read in keys and values, keeping internal whitespace  
    typedef string::size_type pos;  
    const string& delim  = cf.m_Delimiter;  // separator  
    const string& comm   = cf.m_Comment;    // comment  
    const pos skip = delim.length();        // length of separator  
  
    string nextline = "";  // might need to read ahead to see where value ends  
  
    while( is || nextline.length() > 0 )  
    {  
        // Read an entire line at a time  
        string line;  
        if( nextline.length() > 0 )  
        {  
            line = nextline;  // we read ahead; use it now  
            nextline = "";  
        }  
        else  
        {  
            std::getline( is, line );  
        }  
  
        // Ignore comments  
        line = line.substr( 0, line.find(comm) );  
  
        // Parse the line if it contains a delimiter  
        pos delimPos = line.find( delim );  
        if( delimPos < string::npos )  
        {  
            // Extract the key  
            string key = line.substr( 0, delimPos );  
            line.replace( 0, delimPos+skip, "" );  
  
            // See if value continues on the next line  
            // Stop at blank line, next line with a key, end of stream,  
            // or end of file sentry  
            bool terminate = false;  
            while( !terminate && is )  
            {  
                std::getline( is, nextline );  
                terminate = true;  
  
                string nlcopy = nextline;  
                CConfig::Trim(nlcopy);  
                if( nlcopy == "" ) continue;  
  
                nextline = nextline.substr( 0, nextline.find(comm) );  
                if( nextline.find(delim) != string::npos )  
                    continue;  
  
                nlcopy = nextline;  
                CConfig::Trim(nlcopy);  
                if( nlcopy != "" ) line += "\n";  
                line += nextline;  
                terminate = false;  
            }  
  
            // Store key and value  
            CConfig::Trim(key);  
            CConfig::Trim(line);  
            cf.m_Contents[key] = line;  // overwrites if key is repeated  
        }  
    }  
  
    return is;  
}  

bool CConfig::FileExist(std::string filename)  
{  
    bool exist= false;  
    std::ifstream in( filename.c_str() );  
    if( in )   
        exist = true;  
    return exist;  
}  
  
void CConfig::ReadFile( string filename, string delimiter,  
                      string comment )  
{  
    m_Delimiter = delimiter;  
    m_Comment = comment;  
    std::ifstream in( filename.c_str() );  
  
    if( !in ) throw File_not_found( filename );   
  
    in >> (*this);  
}  


/************************************************************************************************************************************************************************************************/


/* CGI父类***********************************************************************************************************************************************************************************
 * 用途：各种最基础的CGI操作
 */

CCgiManager::CCgiManager()
{
	// 默认返回HTML
	SetHead("text/html");
}


CCgiManager::CCgiManager(char * Type)
{
	// 返回type
	SetHead(Type);
}


CCgiManager::~CCgiManager()
{
}


/*从字符串的左边截取n个字符*/  
char * left(char *dst,char *src, int n)  
{  
    char *p = src;  
    char *q = dst;  
    int len = strlen(src);  
    if(n>len) n = len;  
    while(n--) *(q++) = *(p++);  
    *(q++)='\0'; /*有必要吗？很有必要*/  
    return dst;  
}  
  
/*从字符串的中间截取n个字符*/  
char * mid(char *dst,char *src, int n,int m) /*n为长度，m为位置*/  
{  
    char *p = src;  
    char *q = dst;  
    int len = strlen(src);  
    if(n>len) n = len-m;    /*从第m个到最后*/  
    if(m<0) m=0;    /*从第一个开始*/  
    if(m>len) return NULL;  
    p += m;  
    while(n--) *(q++) = *(p++);  
    *(q++)='\0'; /*有必要吗？很有必要*/  
    return dst;  
}  
  
/*从字符串的右边截取n个字符*/  
char * right(char *dst,char *src, int n)  
{  
    char *p = src;  
    char *q = dst;  
    int len = strlen(src);  
    if(n>len) n = len;  
    p += (len-n);   /*从右边第n个字符开始，到0结束，很巧啊*/  
    while(*(q++) = *(p++));  
    return dst;  
}  

// 字符串分割
void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
  std::string::size_type pos1, pos2;
  pos2 = s.find(c);
  pos1 = 0;
  while(std::string::npos != pos2)
  {
    v.push_back(s.substr(pos1, pos2-pos1));
 
    pos1 = pos2 + c.size();
    pos2 = s.find(c, pos1);
  }
  if(pos1 != s.length())
    v.push_back(s.substr(pos1));
}


// 把source字符串里所有s1字符全部替换成字符s2
void replace_char(char *result, char *source, char s1, char s2)
{
    unsigned i = 0;
    char *q = NULL;

    q = source;
    for(i=0; i<strlen(q); i++)
    {
        if(q[i] == s1)
        {
            q[i] = s2;
        }
    }
    strcpy(result, q);
}


void replace_string(char *result, char *source, char* s1, char *s2)
{
    char *q=NULL;
    char *p=NULL;
   
    p=source;
    while((q=strstr(p, s1))!=NULL)
    {
        strncpy(result, p, q-p);
        result[q-p]= '\0';//very important, must attention!
        strcat(result, s2);
        strcat(result, q+strlen(s1));
        strcpy(p,result);
    }
    strcpy(result, p);    
}


// 跳转到请求方法
void CCgiManager::Jump()
{
	char* Method = cgiPathInfo;
	char* Directory, *Controller, *Function;

	// 目标链表
	vector<string> v;

	// 默认参数(没有参数)
	if( strlen(Method) == strlen(cgiScriptName) )
	{
		// 默认值
		Directory  = "index";
		Controller = "index";
		Function   = "index";
	}
	else
	{
		// 处理字符串
		right(Method, cgiPathInfo, strlen(cgiPathInfo) - strlen(cgiScriptName));

		// 分割处理
		SplitString(Method, v, "/");

		// 测试输出
		/*for(vector<string>::size_type i = 0; i != v.size(); ++i)
		{
		OutPut((char*)v[i].c_str());
		}*/

		// 解析目录名称
		Directory  = (char*)v[1].c_str();

		// 解析控制器名称
		Controller = (char*)v[2].c_str();

		// 解析方法名称
		Function   = (char*)v[3].c_str();

		// 默认目录
		if(Directory == "" || strlen(Directory) == 0)
			Directory = "index";

		// 默认控制器
		if(Controller == "" || strlen(Controller) == 0)
			Controller = "index";

		// 默认方法
		if(Function == "" || strlen(Function) == 0)
			Function = "index";
	}

	// 重定向到目标
	string Target = "/application/" + (string)Directory + "/controller/" + (string)Controller + ".cgi/" + (string)Function;
	Redirect((char*)Target.c_str());
}


// 设置头
void CCgiManager::SetHead(char * Type)
{
	cgiHeaderContentType(Type);
	IsSetHead = true;
}


// 设置Location
void CCgiManager::Location(char* Url)
{
	cgiHeaderLocation(Url);
}


// 重定向Url
void CCgiManager::Redirect(char* Url, bool Visible)
{
	// Url 地址栏会显示改变
	if(Visible)
		OutPut("<script>location.href = '%s'</script>", true, Url);

	// Url 地址栏不变(跨域将出现错误)
	else
	{
		string Target = "<script language=javascript> function createXMLHttpRequest(){if(window.XMLHttpRequest){XMLHttpR = new XMLHttpRequest();}else if(window.ActiveXObject){try{XMLHttpR = new ActiveXObject(\"Msxml2.XMLHTTP\");}catch(e){try{XMLHttpR = new ActiveXObject(\"Microsoft.XMLHTTP\");}catch(e){}}}} function sendRequest(url){createXMLHttpRequest();XMLHttpR.open(\"GET\",url,true);XMLHttpR.setRequestHeader(\"Content-Type\",\"text/html;charset=gb2312\");XMLHttpR.onreadystatechange = processResponse;XMLHttpR.send(null);} function processResponse(){if(XMLHttpR.readyState ==4 && XMLHttpR.status == 200){document.write(XMLHttpR.responseText);}}";
		Target += "sendRequest(\"%s\");</script>";

		OutPut((char*)Target.c_str(), true, Url);
	}
}


// 输出HTTP错误状态代码
void CCgiManager::SetStatus(int Status, char* Message)
{
	cgiHeaderStatus(Status, Message);
}


// 转码并输出Html
void CCgiManager::HtmlEscape(char * Name, bool Newlines)
{
	cgiHtmlEscape(Name);
	if(Newlines)
	    fprintf(cgiOut, "\n");
}


void CCgiManager::HtmlEscapeData(char * Name, int len, bool Newlines)
{
	cgiHtmlEscapeData(Name, len);
	if(Newlines)
	    fprintf(cgiOut, "\n");
}


// 转码并输出Value
void CCgiManager::ValueEscape(char * Value, bool Newlines)
{
	cgiValueEscape(Value);
	if(Newlines)
	    fprintf(cgiOut, "\n");
}


void CCgiManager::ValueEscapeData(char * Value, int len, bool Newlines)
{
	cgiValueEscapeData(Value, len);
	if(Newlines)
	    fprintf(cgiOut, "\n");
}


// 输出数据
void CCgiManager::OutPut(char * String, ...)
{
	// 检查是否有默认的头
	if(!IsSetHead)
		SetHead();

	//得到变参的起始地址
	va_list  pArgList;
	va_start(pArgList, String);

	// 取值并输出数据
	char s[65535];
	
	// 格式化并输出
	vsnprintf(s, 65535, String, pArgList);
	vfprintf(cgiOut, s, pArgList);

	//收尾
	va_end(pArgList);
}


// 获取字符串数据
char* CCgiManager::InPutString(char * String, bool OutPut, bool Newlines, int Max)
{
	// 检查目标字段是否存在
	if (cgiFormSubmitClicked(String) == cgiFormSuccess)
	{
		char name[65535];

		if(Max == 0)
			cgiFormString(String, name, sizeof(String) * 2);
		else
			cgiFormString(String, name, Max);

		// 判断是否需要输出
		if(OutPut)
		{
			// 转码并输出
			HtmlEscape(name);

			if(Newlines)
				fprintf(cgiOut, "\n");
		}

		// 赋值
		char* result = name;

		// 返回
		return result;
	}

	return NULL;
}


// 获取不带回车换行符的字符串数据
char* CCgiManager::InPutStringNoNewlines(char * String, bool OutPut, bool Newlines, int Max)
{
	// 检查目标字段是否存在
	if (cgiFormSubmitClicked(String) == cgiFormSuccess)
	{
		char name[65535];
		if(Max == 0)
			cgiFormStringNoNewlines(String, name, sizeof(String) * 2);
		else
			cgiFormStringNoNewlines(String, name, Max);

		// 判断是否需要输出
		if(OutPut)
		{
			// 转码并输出
			HtmlEscape(name);

			if(Newlines)
				fprintf(cgiOut, "\n");
		}

		// 赋值
		char* result = name;

		// 返回
		return result;
	}

	return NULL;
}


// 设置字符串的储存空间，需和InPutString或InPutStringNoNewlines配合使用
bool CCgiManager::InPutStringSpaceNeeded(char * String, int Length)
{
	if(cgiFormStringSpaceNeeded(String, &Length) ==  cgiFormSuccess)
		return true;
	else 
		return false;
}


// 获取短整型数据
int CCgiManager::InPutInteger(char * String, int Default, bool OutPut, bool Newlines)
{
	int result;
	cgiFormInteger(String, &result, Default);

	if(OutPut)
	{
		fprintf(cgiOut, "%d", result);

		if(Newlines)
			fprintf(cgiOut, "\n");
	}

	return result;
}


// 获取短整型区间数据(最大值与最小值为必须参数)
int InPutIntegerBound(char* String, int Min, int Max, int Default, bool OutPut, bool Newlines)
{
	int result;
	cgiFormIntegerBounded(String, &result, Min, Max, Default);

	if(OutPut)
	{
		fprintf(cgiOut, "%d", result);

		if(Newlines)
			fprintf(cgiOut, "\n");
	}

	return result;
}


// 获取双精度数据
double InPutDouble(char * String, int Default, bool OutPut, bool Newlines)
{
	double result;
	cgiFormDouble(String, &result, Default);

	if(OutPut)
	{
		fprintf(cgiOut, "%f", result);

		if(Newlines)
			fprintf(cgiOut, "\n");
	}

	return result;
}


// 获取双精度区间数据(最大值与最小值为必须参数)
double CCgiManager::InPutDoubleBound(char * String, double Min, double Max, double Default, bool OutPut, bool Newlines)
{
	double result;
	cgiFormDoubleBounded(String, &result, Min, Max, Default);

	if(OutPut)
	{
		fprintf(cgiOut, "%f", result);

		if(Newlines)
			fprintf(cgiOut, "\n");
	}

	return result;
}


// 获取单个Checkbox数据(返回是否选中)
bool CCgiManager::InPutCheckboxSingle(char * String)
{
	if (cgiFormCheckboxSingle(String) == cgiFormSuccess)
		return true;
	else
		return false;
}


// 获取一组Checkbox数据(返回所有选中项，若无选中项则返回空)
char * CCgiManager::InPutCheckboxMultiple(char * String)
{
	char **responses;
	if (cgiFormStringMultiple(String, &responses) == cgiFormNotFound) 
		return NULL;
	else
	{
		// 赋值
		char** result = responses;

		// 释放对象
		cgiStringArrayFree(responses);

		// 返回
		return *result;
	}
}


// 获取一组单选Select数据(返回选中的项)
char* CCgiManager::InPutSelectSingle(char * String, char *texts[], int total, int Default)
{
	int Choice;
	cgiFormSelectSingle(String, texts, total, &Choice, Default);
	
	char* result = texts[Choice];
	return result;
}


// 获取一组多选Select数据(若没有选中任何项则返回假，否则返回真)
bool CCgiManager::InPutSelectMultiple(char * String, vector<char> &pResult, char *texts[], int total, int Invalid)
{
	int Choices[65535];

	//没有选择任何项
	if (cgiFormSelectMultiple(String, texts, total, Choices, &Invalid) == cgiFormNotFound) 
	{
		return false;
	}
	else
	{
		for (int i=0; i < total; i++) 
		{
			if (Choices[i]) 
			{
				char* result = texts[i];
				pResult.push_back((char)result);
			}
		}
		return true;
	}
}


// 获取一组Radio数据(返回选中的项)
char* CCgiManager::InPutRadio(char * String, char *Texts[], int Total, int Default)
{
	int Choice;
	cgiFormRadio(String, Texts, Total, &Choice, Default);
	char* result = Texts[Choice];

	return result;
}


// 获取Submit数据(提交成功返回真否则返回假)
bool CCgiManager::SubmitClicked(char * String)
{
	if ( cgiFormSubmitClicked(String) == cgiFormSuccess )
		return true;
	else
		return false;
}


// 为站点设置Cookie数据
void CCgiManager::SetCookieString(char * Name, char * Value, char* Domain)
{
	if (strlen(Name)) 
	{
		/* Cookie lives for one day (or until browser chooses
			to get rid of it, which may be immediately),
			and applies only to this script on this site. */	
		cgiHeaderCookieSetString(Name, Value, 86400, cgiScriptName, Domain);
	}
}


void CCgiManager::SetCoolieInteger(char * Name, int Value, char* Domain)
{
	if (strlen(Name)) 
	{
		cgiHeaderCookieSetInteger(Name, Value, 86400, cgiScriptName, Domain);
	}
}


// 获取站点Cookie数据
char* CCgiManager::GetCookieString(char * Name)
{
	char * value;
	if (strlen(Name)) 
	{
		cgiCookieString(Name, value, sizeof(value));
		return value;
	}
	else
		return NULL;
}


int CCgiManager::GetCookieInteger(char * Name, int Default)
{
	int value;
	if (strlen(Name)) 
	{
		cgiCookieInteger(Name, &value, Default);
		return value;
	}
	else
		return NULL;
}


//获取所有Cookie数据
char* CCgiManager::GetCookies(bool OutPut)
{
	char **arrays, **arrayStep;
	if (cgiCookies(&arrays) != cgiFormSuccess) 
	{
		return NULL;
	}
	
	// 赋值
	arrayStep = arrays;
	if(OutPut)
	{
		while (*arrayStep) 
		{
			HtmlEscape(*arrayStep);
		    arrayStep++;
		}
	}

	// 释放对象
	cgiStringArrayFree(arrays);

	return *arrayStep;
}


//获取所有表单名称(Name)
char* CCgiManager::Entries(bool OutPut)
{
	char **arrays, **arrayStep;
	if (cgiFormEntries(&arrays) != cgiFormSuccess) 
	{
		return NULL;
	}

	arrayStep = arrays;
	if(OutPut)
	{
		while (*arrayStep) 
		{
			HtmlEscape(*arrayStep);
		    arrayStep++;
		}
	}

	cgiStringArrayFree(arrays);
	
	return *arrayStep;
}


// 将表单数据储存在磁盘里(实现读取session数据) 
bool CCgiManager::LoadEnvironment(char * FileName)
{
	if (cgiReadEnvironment(FileName) != cgiEnvironmentSuccess) 
	{
		return false;
	}
	else
		return true;
}


// 从磁盘里读取表单数据（实现放置session数据）
bool CCgiManager::SaveEnvironment(char * FileName)
{
	if (cgiWriteEnvironment(FileName) != cgiEnvironmentSuccess) 
	{
		return false;
	} 
	else 
	{
		return true;
	}
}


// 获取文件数据
bool CCgiManager::InPutFile(char* String, char* &FileName, int &FileSize, char* &contentType)
{
	// 得到文件名
	if (cgiFormFileName(String, FileName, sizeof(FileName)) != cgiFormSuccess)
	{
		// 没有接受到文件数据
		return false;
	}

	// 得到文件大小
	cgiFormFileSize(String, &FileSize);

	// 得到文件类型
	cgiFormFileContentType(String, contentType, sizeof(contentType));

	// 返回
	return true;
}


// 读取文件数据
char* CCgiManager::ReadFileData(char* String, bool OutPut)
{
	cgiFilePtr File;
	char buffer[1024];
	int got;

	// 打开目标文件
	if (cgiFormFileOpen(String, &File) != cgiFormSuccess)
		return NULL;

	char* result = "";
	while (cgiFormFileRead(File, buffer, sizeof(buffer), &got) == cgiFormSuccess)
	{
		// 赋值
		result = result + (char)buffer;
		if(OutPut)
			cgiHtmlEscapeData(buffer, got);
	}

	// 关闭目标文件
	cgiFormFileClose(File);

	// 返回
	return result;
}


// 保存文件数据
bool CCgiManager::SaveFileData(char* String, char* FilePath)
{
	char buffer[1024];
	cgiFilePtr File;
	int got;

	FILE *fp;
	fopen_s(&fp, FilePath, "a+");
	if(fp == NULL)
	{
		return false;
	}
	else
	{
		// 写入数据
		// 打开目标文件
		if (cgiFormFileOpen(String, &File) != cgiFormSuccess)
			return false;

		while (cgiFormFileRead(File, buffer, sizeof(buffer), &got) == cgiFormSuccess)
		{
			// 写入数据
			fputs(buffer, fp);
		}

		// 关闭目标文件
		cgiFormFileClose(File);
	}

	// 关闭文件
	fclose(fp);

	// 返回
	return true;
}



// 获取服务器软件的名称，如果未知，则指向空字符串。
char* CCgiManager::GetServerSoftware()
{
	return cgiServerSoftware;
}

// 获取服务器的名称，如果未知，则指向空字符串。
char* CCgiManager::GetServerName()
{
	return cgiServerName;
}

// 获取网关接口（通常为CGI / 1.1）的名称，如果未知，则指向空字符串。
char* CCgiManager::GetGatewayInterface()
{
	return cgiGatewayInterface;
}

// 获取使用的协议（通常为HTTP / 1.0），如果未知，则指向空字符串。 
char* CCgiManager::GetServerProtocol()
{
	return cgiServerProtocol;
}

// 获取服务器正在监听HTTP连接（通常为80）的端口号，或未知的空字符串。 
char* CCgiManager::GetServerPort()
{
	return cgiServerPort;
}

// 获取请求中使用的方法（通常为GET或POST），如果未知（这不应该发生），则为空字符串）。 
char* CCgiManager::GetRequestMethod()
{
	return cgiRequestMethod;
}

// 大多数Web服务器在请求的URL中识别出超出CGI程序本身的任何附加路径信息，并将该信息传递给程序。cgiPathInfo指向这个额外的路径信息。 
char* CCgiManager::GetPathInfo()
{
	return cgiPathInfo;
}

// 大多数Web服务器在请求的URL中识别出超出CGI程序本身的任何附加路径信息，并将该信息传递给程序。cgiPathTranslated指向此附加路径信息，由服务器转换为本地服务器上的文件系统路径。 
char* CCgiManager::GetPathTranslated()
{
	return cgiPathTranslated;
}

// 获取调用程序的名称。 
char* CCgiManager::GetScriptName()
{
	return cgiScriptName;
}

// 获取包含由GET方法表单或<ISINDEX>标签导致用户提交的任何查询信息。请注意，除非使用<ISINDEX>标记，否则不需要直接解析此信息; 通常它自动解析。使用cgiForm函数系列检索与表单输入字段相关的值。
char* CCgiManager::GetQueryString()
{
	return cgiQueryString;
}

// 获取浏览器的完全解析的主机名（如果已知）或空字符串（如果未知）。 
char* CCgiManager::GetRemoteHost()
{
	return cgiRemoteHost;
}

// 获取浏览器的点分十进制IP地址（如果已知）或空字符串（如果未知）。 
char* CCgiManager::GetRemoteAddr()
{
	return cgiRemoteAddr;
}

// 获取用于请求的授权类型（如果有的话），如果没有或未知，则指向空字符串。 
char* CCgiManager::GetAuthType()
{
	return cgiAuthType;
}

// 获取用户已经认证的用户名; 如果没有发生身份验证，则为空字符串。这些信息的确定性取决于使用授权的类型; 
char* CCgiManager::GetRemoteUser()
{
	return cgiRemoteUser;
}

// 由用户通过用户识别协议自愿指定用户名; 一个空字符串，如果未知。此信息不安全。可以由用户安装在不安全的系统，如Windows机器上。 
char* CCgiManager::GetRemoteIdent()
{
	return cgiRemoteIdent;
}

// 获取用户提交的信息的MIME内容类型（如果有）; 如果没有提交信息，则为空字符串。如果此字符串等于 application/x-www-form-urlencoded或 multipart/form-data，则自动检查提交的表单数据。如果此字符串具有任何其他非空值，则会提交不同类型的数据。这是非常罕见的，因为大多数浏览器只能直接提交表单和文件上传。 
char* CCgiManager::GetContentType()
{
	return cgiContentType;
}

// 获取Web浏览器提交的原始Cookie（浏览器端永久存储）数据。应该使用函数GetCookies， GetCookieString和 GetCookieInteger，而不是直接检查这个字符串。 
char* CCgiManager::GetCookie()
{
	return cgiCookie;
}

// 获取浏览器可以接受的MIME内容类型的空格分隔列表（请参阅 cgiHeaderContentType（））或空字符串。不幸的是，大多数当前的浏览器并不是以一种有用的形式提供这个变量。
char* CCgiManager::GetAccept()
{
	return cgiAccept;
}

// 获取正在使用的浏览器的名称，如果此信息不可用，则为空字符串。 
char* CCgiManager::GetUserAgent()
{
	return cgiUserAgent;
}

// 获取用户访问的上一页的URL。这通常是将用户带到您的程序的表单的URL。请注意，报告此信息完全取决于浏览器，可能选择不这样做。但是，该变量通常是准确的。
char* CCgiManager::GetReferrer()
{
	return cgiReferrer;
}

// 获取收到的表单或查询数据的字节数。请注意，如果提交是提交表单或查询，库将直接从cgiIn和/或cgiQueryString读取和解析所有信息。在这种情况下，程序员不应该这样做。
int CCgiManager::GetContentLength()
{
	return cgiContentLength;
}

/************************************************************************************************************************************************************************************************/


/* 控制器类***********************************************************************************************************************************************************************************
 *  用途：控制模型与视图的交互
 */
CController::CController()
{
}

CController::CController(char * Type): CCgiManager(Type)
{
}

CController::~CController()
{

}


// 得到 HTML 内容
char* CController::HtmlRead(char* FileName)
{
	// 读取文件
	FILE *fp;
	fopen_s(&fp, FileName, "r");

	// 定义变量
	char str[1024], buff[65535];
	if ( fp == NULL )
	{
		printf("open file error\n" );
		return "";
	}

	// 循环读取HTML
	while( fgets(str, sizeof(str), fp ) )
	{
		// 储存在buff中
		strcat(buff, str);
	}

	// 关闭文件
	fclose(fp);

	// 返回数据
	char * result = buff;
	return result;
}


// 渲染并输出HTML
void CController::HtmlView(char* FileName, ...)
{
	//得到变参的起始地址
	va_list  pArgList;
	va_start(pArgList, FileName);

	// 得到 HTML 内容
	char s[65535], *source = HtmlRead(FileName);
	
	// 格式化并输出
	vsnprintf(s, 65535, source, pArgList);
	OutPut(s);

	//收尾
	va_end(pArgList);
}


// Json编码,返回json数据
char* CController::Json_Encode()
{

}


// Json 解码
void CController::Json_Decode()
{

}


// Xml编码,返回xml数据
char* CController::Xml_Encode()
{

}


// Xml 解码
void CController::Xml_Decode()
{
}


/***************************************************************************************************************************************************************************************************/


/* 模型类***********************************************************************************************************************************************************************************
 *  用途：进行各种数据交互
 */
CModel::CModel(): CSqliteManager(true)
{
	// 自动配置数据库
	DatabaseConfig();
}


CModel::~CModel()
{

}


// 配置数据库
void CModel::DatabaseConfig(char* ConfigFile)
{
	// 读取配置文件
    CConfig configSettings(ConfigFile);

	// 解析配置数据
	int    hostport = configSettings.Read("hostport", 0);
	string type     = configSettings.Read("type",     type);
    string hostname = configSettings.Read("hostname", hostname);
	string database = configSettings.Read("database", database);
    string username = configSettings.Read("username", username);
    string password = configSettings.Read("password", password);
	string charset  = configSettings.Read("charset",  charset);

	// 根据数据库类型进行配置(不区分大小写)
	if(_stricmp((char*)type.c_str(), "sqlite") == 0)
	{
		// 赋值
		Type = 1;

		// 得到数据库路径
		CString Path(hostname.c_str()), DataBase_Name, DataBase_Path;

		// 判断路径类型(绝对全路径)
		if(Path.Replace(_T("\\"), _T("\\")))
		{
			DataBase_Name = Path.Right(Path.GetLength() - Path.ReverseFind('\\') -1);
			DataBase_Path = Path.Left(Path.GetLength()  - DataBase_Name.GetLength());
		}

		// 判断路径类型(相对全路径)
		if(Path.ReverseFind('/'))
		{
			DataBase_Name = Path.Right(Path.GetLength() - Path.ReverseFind('/') -1);
			DataBase_Path = Path.Left(Path.GetLength()  - DataBase_Name.GetLength());
		}

		// 如果存在目标数据库
		if (m_Sql.CheckDataBase( DataBase_Name, DataBase_Path, true ))
		{
			// 连接数据库
			if(!m_Sql.OpenDataBase( DataBase_Name, DataBase_Path, true ))
				return;
		}
		else
		{
			// 创建数据库
			if(!m_Sql.CreateDataBase( DataBase_Name, DataBase_Path, true ))
				return;

			// 连接数据库
			if(!m_Sql.OpenDataBase(  DataBase_Name, DataBase_Path, true ))
				return;
		}
	}
	else if(_stricmp((char*)type.c_str(), "mysql") == 0)
	{
		// 赋值
		Type = 2;

		// 配置MySQL数据库
		std::cout<<"mysql:"<<std::endl;
	}
	else if(_stricmp((char*)type.c_str(), "sqlserver") == 0)
	{
		// 赋值
		Type = 3;

		// 配置SqlServer数据库
		std::cout<<"SqlServer:"<<std::endl;
	}


	// 测试输出
    /*std::cout<<"port:"<<hostport<<std::endl;
	std::cout<<"type:"<<type<<std::endl;
    std::cout<<"hostname:"<<hostname<<std::endl;
	std::cout<<"database:"<<database<<std::endl;
    std::cout<<"username:"<<username<<std::endl;
    std::cout<<"password:"<<password<<std::endl;
	std::cout<<"charset:"<<charset<<std::endl;*/


}


// 写入数据
BOOL CModel::Save(char* TableName, char* Params)
{
	switch(Type)
	{
	case 1: // Sqlite
		{
			// 转换为CString
			CString Table(TableName), Param(Params);

			// 检查目标表
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("数据表%s不存在!", TableName);
			else
				// 写入数据
				return m_Sql.InsertData(Table, Param, true);
		}break;

	case 2:
		{

		}break;

	case 3:
		{

		}break;

	default:
		break;
	}

	// 默认返回失败
	return FALSE;
}


// 删除数据
BOOL CModel::Delete(char* TableName, char* Params)
{
	switch(Type)
	{
	case 1: // Sqlite
		{
			// 转换为CString
			CString Table(TableName), Param(Params);

			// 检查目标表
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("数据表%s不存在!", TableName);
			else
				// 删除数据
				return m_Sql.DeleteData(Table, Param, true);
		}break;

	case 2:
		{

		}break;

	case 3:
		{

		}break;

	default:
		break;
	}

	// 默认返回失败
	return FALSE;
}


// 更新数据 (单条)
BOOL CModel::UpDate(char* TableName, char* Columns, char* NewData, char* Params)
{
	switch(Type)
	{
	case 1: // Sqlite
		{
			// 转换为CString
			CString Table(TableName), Column(Columns), Data(NewData), Param(Params);

			// 检查目标表
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("数据表%s不存在!", TableName);
			else
				// 更新数据
				return m_Sql.UpdataData(Table, Column, Data, Param, true);
		}break;

	case 2:
		{

		}break;

	case 3:
		{

		}break;

	default:
		break;
	}

	// 默认返回失败
	return FALSE;
}


// 更新数据 (批量)
BOOL CModel::UpDate(char* TableName, char* Columns, char* Params)
{
	switch(Type)
	{
	case 1: // Sqlite
		{
			// 转换为CString
			CString Table(TableName), Column(Columns), Param(Params);

			// 检查目标表
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("数据表%s不存在!", TableName);
			else
				// 更新数据
				return m_Sql.UpdataData(Table, Column, Param, true);
		}break;

	case 2:
		{

		}break;

	case 3:
		{

		}break;

	default:
		break;
	}

	// 默认返回失败
	return FALSE;
}


// 获取数据 (单条)
char* CModel::Get(char* TableName, int Col, char* Params, char* Order, char* Limit, int SortMode, BOOL DISTINCT, char* COUNT, char* COLUMN, char* GROUP,  char* HAVING)
{
	// 定义返回值
	char* result = "";

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 转换为CString
			CString Table(TableName), Result, Param(Params), pOrder(Order), pLimit(Limit), pCOUNT(COUNT), pCOLUMN(COLUMN), pGROUP(GROUP), pHAVING(HAVING);

			// 检查目标表
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("数据表%s不存在!", TableName);
			else
			{
				// 获取数据
				if( m_Sql.SelectData(Table, Result, Col, Param, true, pOrder, pLimit, SortMode, DISTINCT, pCOUNT, pCOLUMN, pGROUP, pHAVING) )
				{
					//从这里开始进行转化，这是一个宏定义
					USES_CONVERSION;

					//进行转换
					char* pResult = T2A(Result.GetBuffer(0));
					Result.ReleaseBuffer();

					// 为结果赋值
					result = pResult;
				}
				else
					return "";
			}
		}break;

	case 2:
		{

		}break;

	case 3:
		{

		}break;

	default:
		break;
	}

	// 返回结果
	return result;
}


// 获取数据 (批量)
vector<char*> CModel::GetAll(char* TableName, int Col, char* Params, char* Order, char* Limit, int SortMode, BOOL DISTINCT, char* COUNT, char* COLUMN, char* GROUP,  char* HAVING)
{
	// 链表对象
	vector<char*>   Result;
	vector<CString> Temp;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 转换为CString
			CString Table(TableName), Param(Params), pOrder(Order), pLimit(Limit), pCOUNT(COUNT), pCOLUMN(COLUMN), pGROUP(GROUP), pHAVING(HAVING);

			// 检查目标表
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("数据表%s不存在!", TableName);
			else
			{
				// 获取数据
				if( m_Sql.SelectData(Table, Temp, Param, true, pOrder, pLimit, SortMode, DISTINCT, pCOUNT, pCOLUMN, pGROUP, pHAVING) )
				{
					for(int i = 0; i < (int)Temp.size(); i += Col)
					{
						//从这里开始进行转化，这是一个宏定义
						USES_CONVERSION;

						//进行转换
						for(int j = 0; j < Col; j++)
						{
							char* temp = T2A(Temp.at(i + j).GetBuffer(0));
							Temp.at(i + j).ReleaseBuffer();

							// 放进目标链表中
							Result.push_back(temp);
						}
					}
				}
			}
		}break;

	case 2:
		{

		}break;

	case 3:
		{

		}break;

	default:
		break;
	}

	// 返回查询结果
	return Result;
}


// 统计数据
int CModel::Count(char* TableName, char* Params)
{
	switch(Type)
	{
	case 1: // Sqlite
		{
			// 转换为CString
			CString Table(TableName), Param(Params);
			
			// 检查目标表
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("数据表%s不存在!", TableName);
			else
			{
				// 定义返回对象
				int Count;

				// 统计数据
				if( m_Sql.CountNumber(Table, Param, Count, true) )
					return Count;
				else
					return -1;
			}
		}break;

	case 2:
		{

		}break;

	case 3:
		{

		}break;

	default:
		break;
	}

	// 默认返回失败
	return FALSE;
}


// 数据求和
int CModel::Sum(char* TableName, char* Column, char* Params)
{
	// 定义返回对象
	int Data = 0;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 转换为CString
			CString Table(TableName), pColumn(Column), Param(Params);

			// 检查目标表
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("数据表%s不存在!", TableName);
			else
			{
				// 数据求和
				if( m_Sql.SumData(Table, pColumn, Param, Data, true) )
					return Data;
				else
					return -1;
			}
		}break;

	case 2:
		{

		}break;

	case 3:
		{

		}break;

	default:
		break;
	}

	// 默认返回失败
	return -1;
}


// 数据求积
int CModel::Product(char* TableName, char* Column, char* Params)
{
	// 定义返回对象
	int Data = 1;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 转换为CString
			CString Table(TableName), pColumn(Column), Param(Params);

			// 检查目标表
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("数据表%s不存在!", TableName);
			else
			{
				// 数据求积
				if( m_Sql.ProductData(Table, pColumn, Param, Data, true) )
					return Data;
				else
					return -1;
			}
		}break;

	case 2:
		{

		}break;

	case 3:
		{

		}break;

	default:
		break;
	}

	// 默认返回失败
	return -1;
}


// 求平均数
int CModel::Avg(char* TableName, char* Column, char* Params)
{
	// 定义返回对象
	int Data = 0;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 转换为CString
			CString Table(TableName), pColumn(Column), Param(Params);

			// 检查目标表
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("数据表%s不存在!", TableName);
			else
			{
				// 统计数据
				if( m_Sql.AvgData(Table, pColumn, Param, Data, true) )
					return Data;
				else
					return -1;
			}
		}break;

	case 2:
		{

		}break;

	case 3:
		{

		}break;

	default:
		break;
	}

	// 默认返回失败
	return -1;
}


// 求中位数
int CModel::Mid(char* TableName, char* Column, char* Params)
{
	// 定义返回对象
	int Data = 0;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 转换为CString
			CString Table(TableName), pColumn(Column), Param(Params);

			// 检查目标表
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("数据表%s不存在!", TableName);
			else
			{
				// 统计数据
				if( m_Sql.MidData(Table, pColumn, Param, Data, true) )
					return Data;
				else
					return -1;
			}
		}break;

	case 2:
		{

		}break;

	case 3:
		{

		}break;

	default:
		break;
	}

	// 默认返回失败
	return -1;
}

/***************************************************************************************************************************************************************************************************/

