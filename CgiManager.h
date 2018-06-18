#pragma once

#include "cgic.h"

#include <afx.h>
#include <vector>
using std::vector;

// 使用说明: 本程序将自动添加入口点:cgiMain ,请把主程序里的入口点删除，并加上 int cgiMain(){}
class CCgiManager
{
public:
	CCgiManager();
	~CCgiManager();

	// 跳转到请求方法
	void Jump(char* Method);

	// 设置头
	void SetHead(char* type = "text/html");

	// 重定向Url
	void Redirect(char* Url);

	// 输出HTTP错误状态代码
	void SetStatus(int Status, char* Message);

	// 输出内容
	void OutPut(char * string, bool Newlines = true);

	// 转码输出Html
	void HtmlEscape(char * name, bool Newlines = true);

	// 转码输出Html数据流
	void HtmlEscapeData(char * name, int len, bool Newlines = true);

	// 转码输出Value
	void ValueEscape(char * value, bool Newlines = true);

	// 转码输出Value数据流
	void ValueEscapeData(char * value, int len, bool Newlines = true);

	// 获取字符串数据
	char* InPutString(char * String, bool OutPut = false, bool Newlines = false, int Max = 0);

	// 获取不带回车换行符的字符串数据
	char* InPutStringNoNewlines(char * String, bool OutPut = false, bool Newlines = false, int Max = 0);

	// 设置字符串的储存空间，需和InPutString或InPutStringNoNewlines配合使用
	bool InPutStringSpaceNeeded(char * String, int Length);

	// 获取短整型数据
	int InPutInteger(char * String,  int Default = 0, bool OutPut = false, bool Newlines = false);

	// 获取短整型区间数据(最大值与最小值为必须参数)
	int InPutIntegerBound(char* String, int Min, int Max, int Default = 0, bool OutPut = false, bool Newlines = false);

	// 获取双精度数据
	double InPutDouble(char * String, int Default = 0, bool OutPut = false, bool Newlines = false);

	// 获取双精度区间数据(最大值与最小值为必须参数)
	double InPutDoubleBound(char* String, double Min, double Max, double Default = 0.00, bool OutPut = false, bool Newlines = false);

	// 获取单个Checkbox数据(返回是否选中)
	bool InPutCheckboxSingle(char * String);

	// 获取一组Checkbox数据(返回所有选中项，若无选中项则返回空)
    char * InPutCheckboxMultiple(char * String);

	// 获取一组单选Select数据(返回选中的项)
	char* InPutSelectSingle(char * String , char *texts[], int total, int Default = 0);

	// 获取一组多选Select数据(返回选中的项)
	bool InPutSelectMultiple(char * String, vector<char> &pResult, char *texts[], int total, int Invalid = 0);

	// 获取一组Radio数据(返回选中的项)
	char* InPutRadio(char * String, char *Texts[], int Total, int Default = 0);

	// 获取Submit数据(提交成功返回真否则返回假)
	bool SubmitClicked(char * String);

	// 为站点设置Cookie数据
	void SetCookieString(char * Name, char * Value, char* Domain = "DefaultDomain");
	void SetCoolieInteger(char * Name,  int  Value, char* Domain = "DefaultDomain");

	// 获取站点Cookie数据
	char* GetCookieString(char * Name);
	int  GetCookieInteger(char * Name, int Default);

	//获取所有Cookie数据
	char * GetCookies(bool OutPut = false);

	//获取所有表单名称(Name)
	char* Entries(bool OutPut = false);

	// 将表单数据储存在磁盘里 
	bool LoadEnvironment(char * FileName);

	// 从磁盘里读取表单数据
	bool SaveEnvironment(char * FileName);

	// 获取文件数据
	bool InPutFile(char* String, char* &FileName, int &FileSize, char* &contentType);

	// 读取文件数据
	char* ReadFileData(char* String, bool OutPut = false);

	// 保存文件数据
	bool SaveFileData(char* String, char* FilePath);


	// 获取服务器软件的名称，如果未知，则指向空字符串。
	char* GetServerSoftware();

	// 获取服务器的名称，如果未知，则指向空字符串。
	char* GetServerName();

	// 获取网关接口（通常为CGI / 1.1）的名称，如果未知，则指向空字符串。
	char* GetGatewayInterface();

	// 获取使用的协议（通常为HTTP / 1.0），如果未知，则指向空字符串。 
	char* GetServerProtocol();

	// 获取服务器正在监听HTTP连接（通常为80）的端口号，或未知的空字符串。 
	char* GetServerPort();

	// 获取请求中使用的方法（通常为GET或POST），如果未知（这不应该发生），则为空字符串）。 
	char* GetRequestMethod();

	// 大多数Web服务器在请求的URL中识别出超出CGI程序本身的任何附加路径信息，并将该信息传递给程序。cgiPathInfo指向这个额外的路径信息。 
	char* GetPathInfo();

	// 大多数Web服务器在请求的URL中识别出超出CGI程序本身的任何附加路径信息，并将该信息传递给程序。cgiPathTranslated指向此附加路径信息，由服务器转换为本地服务器上的文件系统路径。 
	char* GetPathTranslated();

	// 获取调用程序的名称。 
	char* GetScriptName();

	// 获取包含由GET方法表单或<ISINDEX>标签导致用户提交的任何查询信息。请注意，除非使用<ISINDEX>标记，否则不需要直接解析此信息; 通常它自动解析。使用cgiForm函数系列检索与表单输入字段相关的值。
	char* GetQueryString();

	// 获取浏览器的完全解析的主机名（如果已知）或空字符串（如果未知）。 
	char* GetRemoteHost();

	// 获取浏览器的点分十进制IP地址（如果已知）或空字符串（如果未知）。 
	char* GetRemoteAddr();

	// 获取用于请求的授权类型（如果有的话），如果没有或未知，则指向空字符串。 
	char* GetAuthType();

	// 获取用户已经认证的用户名; 如果没有发生身份验证，则为空字符串。这些信息的确定性取决于使用授权的类型; 
	char* GetRemoteUser();

	// 由用户通过用户识别协议自愿指定用户名; 一个空字符串，如果未知。此信息不安全。可以由用户安装在不安全的系统，如Windows机器上。 
	char* GetRemoteIdent();

	// 获取用户提交的信息的MIME内容类型（如果有）; 如果没有提交信息，则为空字符串。如果此字符串等于 application/x-www-form-urlencoded或 multipart/form-data，则自动检查提交的表单数据。如果此字符串具有任何其他非空值，则会提交不同类型的数据。这是非常罕见的，因为大多数浏览器只能直接提交表单和文件上传。 
	char* GetContentType();

	// 获取Web浏览器提交的原始Cookie（浏览器端永久存储）数据。应该使用函数GetCookies， GetCookieString和 GetCookieInteger，而不是直接检查这个字符串。 
	char* GetCookie();

	// 获取浏览器可以接受的MIME内容类型的空格分隔列表（请参阅 cgiHeaderContentType（））或空字符串。不幸的是，大多数当前的浏览器并不是以一种有用的形式提供这个变量。
	char* GetAccept();

	// 获取正在使用的浏览器的名称，如果此信息不可用，则为空字符串。 
	char* GetUserAgent();

	// 获取用户访问的上一页的URL。这通常是将用户带到您的程序的表单的URL。请注意，报告此信息完全取决于浏览器，可能选择不这样做。但是，该变量通常是准确的。
	char* GetReferrer();

	// 获取收到的表单或查询数据的字节数。请注意，如果提交是提交表单或查询，库将直接从cgiIn和/或cgiQueryString读取和解析所有信息。在这种情况下，程序员不应该这样做。
	int GetContentLength();
};


// 控制器父类
class CController: public CCgiManager
{
public:
	// 构造与折构函数
	CController();
	~CController();

	void HtmlView(char* FileName = "index.html", char Parameter[] = "");
};
