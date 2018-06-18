#include "StdAfx.h"
#include "CgiManager.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <cstring>
using namespace std;

CCgiManager::CCgiManager()
{
	// 默认返回HTML
	SetHead("text/html");

	// 得到请求路径
	CString Path(cgiPathInfo), Script(cgiScriptName);
	Path.Replace(Script, _T(""));
	Jump((char*)Path.GetBuffer());
}


CCgiManager::~CCgiManager()
{
}


// 跳转到请求方法
void CCgiManager::Jump(char* Method)
{
	OutPut("path:");
	OutPut(Method);
}


// 设置头
void CCgiManager::SetHead(char * type)
{
	cgiHeaderContentType(type);
}


// 重定向Url
void CCgiManager::Redirect(char* Url)
{
	cgiHeaderLocation(Url);
}


// 输出HTTP错误状态代码
void CCgiManager::SetStatus(int Status, char* Message)
{
	cgiHeaderStatus(Status, Message);
}


// 转码并输出
void CCgiManager::HtmlEscape(char * name, bool Newlines)
{
	cgiHtmlEscape(name);
	if(Newlines)
	    fprintf(cgiOut, "\n");
}


void CCgiManager::HtmlEscapeData(char * name, int len, bool Newlines)
{
	cgiHtmlEscapeData(name, len);
	if(Newlines)
	    fprintf(cgiOut, "\n");
}


// 转码并输出
void CCgiManager::ValueEscape(char * value, bool Newlines)
{
	cgiValueEscape(value);
	if(Newlines)
	    fprintf(cgiOut, "\n");
}


void CCgiManager::ValueEscapeData(char * value, int len, bool Newlines)
{
	cgiValueEscapeData(value, len);
	if(Newlines)
	    fprintf(cgiOut, "\n");
}


// 输出数据
void CCgiManager::OutPut(char * string, bool Newlines)
{
	fprintf(cgiOut, string);

	if(Newlines)
	    fprintf(cgiOut, "\n");
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

	char* result;
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






/* 控制器类***********************************************************************************************************************************************************************************
*  用途：控制模型与视图的交互
*/
CController::CController()
{
}


CController::~CController()
{

}


