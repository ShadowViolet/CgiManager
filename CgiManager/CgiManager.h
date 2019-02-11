#pragma once
/* CGI Manager
 * 作者: 紫辉(紫影龙)
 * 日期: 2018-6-22
 * 版权所有 2016 - 2018 紫影龙工作室
 * https://www.shadowviolet.com
 */

#pragma warning(disable:4996)

#include "cgic.h"
#include "SqliteManager.h"

using namespace std;
#include <vector>
#include <string>

#include <map>
#include <fstream>
#include <sstream>

/*#ifdef  __unix or #ifdef  __unix__
  #ifdef  __linux or #ifdef  __linux__
  #ifdef  __FreeBSD__
  32bit #ifdef  _WIN32（或者WIN32）
  64bit #ifdef  _WIN64
 */

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#include <windows.h>
#else
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <typeinfo>
#ifndef BYTE
#define BYTE unsigned char
#endif
#endif

//配置文件读取 *********************************************************************************************************************************************************************** 

class CConfig 
{  
    // Data  
protected:  
    std::string m_Delimiter;  //!< separator between key and value  
    std::string m_Comment;    //!< separator between value and comments  
    std::map<std::string,std::string> m_Contents;  //!< extracted keys and values  
  
    typedef std::map<std::string,std::string>::iterator mapi;  
    typedef std::map<std::string,std::string>::const_iterator mapci;  
    // Methods
public:
    CConfig( std::string filename,std::string delimiter = "=",std::string comment = "#" );
    CConfig();

	// linux 下专用
	void linux_trim(char *src)
	{
		char *begin = src;
		char *end = src;

		while (*end++)
		{
		}

		if(begin == end)
			return;

		while(*begin == ' ' || *begin == '\t')
			++begin;
		while((*end) == '\0' || *end == ' ' || *end == '\t')
			--end;

		if(begin > end)
		{
			*src = '\0';
			return;
		}

		while(begin != end)
		{
			*src++ = *begin++;
		}

		*src++ = *end;
		*src = '\0';
		return;
	}

    template<class T> T Read( const std::string& in_key ) const;  //!<Search for key and read value or optional default value, call as read<T>  
    template<class T> T Read( const std::string& in_key, const T& in_value ) const;  
    template<class T> bool ReadInto( T& out_var, const std::string& in_key ) const;  
    template<class T>  
    bool ReadInto( T& out_var, const std::string& in_key, const T& in_value ) const;  
    static bool FileExist(std::string filename);  
    void ReadFile(std::string filename,std::string delimiter = "=",std::string comment = "#" );  
  
    // Check whether key exists in CConfiguration  
    bool KeyExists( const std::string& in_key ) const;  
  
    // Modify keys and values  
    template<class T> void Add( const std::string& in_key, const T& in_value );  
    void Remove( const std::string& in_key );  
  
    // Check or change CConfiguration syntax  
    std::string GetDelimiter() const { return m_Delimiter; }  
    std::string GetComment() const { return m_Comment; }  
    std::string SetDelimiter( const std::string& in_s )  
    { std::string old = m_Delimiter;  m_Delimiter = in_s;  return old; }    
    std::string SetComment( const std::string& in_s )  
    { std::string old = m_Comment;  m_Comment =  in_s;  return old; }  
  
    // Write or read CConfiguration  
    friend std::ostream& operator<<( std::ostream& os, const CConfig& cf );  
    friend std::istream& operator>>( std::istream& is, CConfig& cf );  
  
protected:  
    template<class T> static std::string T_as_string( const T& t );  
    template<class T> static T string_as_T( const std::string& s );  
    static void Trim( std::string& inout_s );  
  
  
    // Exception types  
public:  
    struct File_not_found {  
        std::string filename;  
        File_not_found( const std::string& filename_ = std::string() )  
            : filename(filename_) {} };  
        struct Key_not_found {  // thrown only by T read(key) variant of read()  
            std::string key;  
            Key_not_found( const std::string& key_ = std::string() )  
                : key(key_) {} };  
};  


typedef void (* PtrFun) ();

class FunctionEntry
{
public:
    PtrFun pFun;    
    std::string strFun;
};

/***************************************************************************************************************************************************************************************/


// 使用说明: 本程序将自动添加入口点:cgiMain ,请把主程序里的入口点删除，并加上 int cgiMain(){}

class CCgiManager
{
private:
	//Cgi 配置
	static bool   debug;
	static string error_message;
	static string log_path;
	static string charset;
	static string return_type;
	static string ajax_return;
	static string controller_prefix;
	static string controller_suffix;
	static string default_controller;
	static string default_action;
	static string action_suffix;
	static string pathinfo_depr;
	static string html_suffix;
	static bool   url_route;
	static int    cookie_expire;
	static string cookie_path;
	static string cookie_domain;

	// 初始化状态
	static bool Initialized;

	// 是否已设置请求头
	static bool IsSetHead;

	// CGI 配置
	static void Config(string ConfigFile = "application/config.txt");

	// 路由配置
	static void Route(string ConfigFile = "application/route.txt");

	// 解析Url
	static void ParsingUrl();

	// 编码工具
	static void Gb2312ToUnicode(wchar_t* pOut,char *gbBuffer);
    static void UTF_8ToUnicode(wchar_t* pOut,char *pText);
    static void UnicodeToUTF_8(char* pOut,wchar_t* pText);
    static void UnicodeToGB2312(char* pOut,wchar_t* uData);
    static char CharToInt(char ch);
    static char StrToBin(char *str);

public:
	// 构造与折构函数
	CCgiManager();
	~CCgiManager();
	friend class CController;

	// 路由表
	static vector<string> routes;

	// Url参数
	static string Controller, Function;

	// 方法映射
	static map <string, map<string, PtrFun> > FunTab;

	///////////////////////////////////字符串操作代码////////////////////////////////////////////////////////////////////////


	/*从字符串的左边截取n个字符*/  
    static char* Left(char* dst, char* src, int n);

	/*从字符串的中间截取n个字符*/  
    static char* Mid(char* dst, char* src, int n, int m);

	/*从字符串的右边截取n个字符*/  
    static char* Right(char* dst, char* src, int n);

	// 字符串分割
    static void Split(const std::string& s, std::vector<std::string>& v, const std::string& c);

	// 字符串替换 s1里替换s2与s3
    static char* Replace(char* s1, char* s2, char* s3 = NULL);

	// 转大写
	static char* strupr(char *str);

	// 转小写
	static char* strlowr(char *str);

	
	///////////////////////////////////数据编码转换代码////////////////////////////////////////////////////////////////////////

	//utf_8转为gb2312
	static void UTF_8ToGB2312(string &pOut, char *pText, int pLen);

	//gb2312 转utf_8
    static void GB2312ToUTF_8(string& pOut,char *pText, int pLen);

	//urlgb2312编码
    static string UrlGB2312(char * str);

	//urlutf8 编码
    static string UrlUTF8(char * str);

	//urlutf8解码
    static string UrlUTF8Decode(string str);

	//urlgb2312解码
    static string UrlGB2312Decode(string str);


	//ACSII转Unicode
	//static wstring AcsiiToUnicode( const string  & acsii_string);

	//ACSII转UTF8  
	//static string  AcsiiToUtf8(    const string  & acsii_string);

	//Unicode转ACSII  
	//static string  UnicodeToAcsii( const wstring & unicode_string);

	//Unicode转UTF8  
	//static string  UnicodeToUtf8(  const wstring & unicode_string);

	//UTF8转ACSII 
	//static string  Utf8ToAcsii(    const string  & utf8_string);

	//UTF8转Unicode  
	//static wstring Utf8ToUnicode(  const string  & utf8_string);
	

	///////////////////////////////////类成员方法代码////////////////////////////////////////////////////////////////////////


	// 记录错误信息
	static void Record(string Content);

	// 显示错误信息
	static void DisplayError(string String, ...);

	// 设置头
	static void SetHead(string Type);

	// 设置 Location
	static void Location(string Url);

	// Url跳转
	static void Jump(string Url, bool Visible = true);

	// 输出HTTP错误状态代码
	static void SetStatus(int Status, string Message);

	// 转码输出Html
	static void HtmlEscape(string Name, bool Newlines = true);

	// 转码输出Html数据流
	static void HtmlEscapeData(string Name, int len, bool Newlines = true);

	// 转码输出Value
	static void ValueEscape(string Value, bool Newlines = true);

	// 转码输出Value数据流
	static void ValueEscapeData(string Value, int len, bool Newlines = true);

	// 获取字符串数据
	static string InPutString(string String, bool OutPut = false, bool Newlines = false, int Max = 0);

	// 获取不带回车换行符的字符串数据
	static string InPutStringNoNewlines(string String, bool OutPut = false, bool Newlines = false, int Max = 0);

	// 设置字符串的储存空间，需和InPutString或InPutStringNoNewlines配合使用
	static bool InPutStringSpaceNeeded(string String, int Length);

	// 获取短整型数据
	static int InPutInteger(string String,  int Default = 0, bool OutPut = false, bool Newlines = false);

	// 获取短整型区间数据(最大值与最小值为必须参数)
	static int InPutIntegerBound(string String, int Min, int Max, int Default = 0, bool OutPut = false, bool Newlines = false);

	// 获取双精度数据
	static double InPutDouble(string String, double Default = 0.00, bool OutPut = false, bool Newlines = false);

	// 获取双精度区间数据(最大值与最小值为必须参数)
	static double InPutDoubleBound(string String, double Min, double Max, double Default = 0.00, bool OutPut = false, bool Newlines = false);

	// 获取单个Checkbox数据(返回是否选中)
	static bool InPutCheckboxSingle(string String);

	// 获取一组Checkbox数据(返回所有选中项，若无选中项则返回空)
    static string InPutCheckboxMultiple(string String);

	// 获取一组单选Select数据(返回选中的项)
	static string InPutSelectSingle(string String , string texts[], int total, int Default = 0);

	// 获取一组多选Select数据(返回选中的项)
	static bool InPutSelectMultiple(string String, vector<string> &pResult, string texts[], int total, int Invalid = 0);

	// 获取一组Radio数据(返回选中的项)
	static string InPutRadio(string String, string Texts[], int Total, int Default = 0);

	// 获取Submit数据(提交成功返回真否则返回假)
	static bool SubmitClicked(string String);

	// 为站点设置Cookie数据
	static void SetCookieString(string Name, string Value);
	static void SetCoolieInteger(string Name,  int  Value);

	// 获取站点Cookie数据
	static string GetCookieString(string Name);
	static int   GetCookieInteger(string Name, int Default);

	//获取所有表单名称(Name)
	static string Entries(bool OutPut = false);

	// 将表单数据储存在磁盘里 
	static bool LoadEnvironment(string FileName);

	// 从磁盘里读取表单数据
	static bool SaveEnvironment(string FileName);

	// 获取文件数据
	static bool InPutFile(string String, string &FileName, int &FileSize, string &contentType);

	// 读取文件数据
	static string ReadFileData(string String, bool OutPut = false);

	// 保存文件数据
	static bool SaveFileData(string String, string FilePath);


	// 获取服务器软件的名称，如果未知，则指向空字符串。
	static string GetServerSoftware();

	// 获取服务器的名称，如果未知，则指向空字符串。
	static string GetServerName();

	// 获取网关接口（通常为CGI / 1.1）的名称，如果未知，则指向空字符串。
	static string GetGatewayInterface();

	// 获取使用的协议（通常为HTTP / 1.0），如果未知，则指向空字符串。 
	static string GetServerProtocol();

	// 获取服务器正在监听HTTP连接（通常为80）的端口号，或未知的空字符串。 
	static string GetServerPort();

	// 获取请求中使用的方法（通常为GET或POST），如果未知（这不应该发生），则为空字符串）。 
	static string GetRequestMethod();

	// 大多数Web服务器在请求的URL中识别出超出CGI程序本身的任何附加路径信息，并将该信息传递给程序。cgiPathInfo指向这个额外的路径信息。 
	static string GetPathInfo();

	// 大多数Web服务器在请求的URL中识别出超出CGI程序本身的任何附加路径信息，并将该信息传递给程序。cgiPathTranslated指向此附加路径信息，由服务器转换为本地服务器上的文件系统路径。 
	static string GetPathTranslated();

	// 获取调用程序的名称。 
	static string GetScriptName();

	// 获取包含由GET方法表单或<ISINDEX>标签导致用户提交的任何查询信息。请注意，除非使用<ISINDEX>标记，否则不需要直接解析此信息; 通常它自动解析。使用cgiForm函数系列检索与表单输入字段相关的值。
	static string GetQueryString();

	// 获取浏览器的完全解析的主机名（如果已知）或空字符串（如果未知）。 
	static string GetRemoteHost();

	// 获取浏览器的点分十进制IP地址（如果已知）或空字符串（如果未知）。 
	static string GetRemoteAddr();

	// 获取用于请求的授权类型（如果有的话），如果没有或未知，则指向空字符串。 
	static string GetAuthType();

	// 获取用户已经认证的用户名; 如果没有发生身份验证，则为空字符串。这些信息的确定性取决于使用授权的类型; 
	static string GetRemoteUser();

	// 由用户通过用户识别协议自愿指定用户名; 一个空字符串，如果未知。此信息不安全。可以由用户安装在不安全的系统，如Windows机器上。 
	static string GetRemoteIdent();

	// 获取用户提交的信息的MIME内容类型（如果有）; 如果没有提交信息，则为空字符串。如果此字符串等于 application/x-www-form-urlencoded或 multipart/form-data，则自动检查提交的表单数据。如果此字符串具有任何其他非空值，则会提交不同类型的数据。这是非常罕见的，因为大多数浏览器只能直接提交表单和文件上传。 
	static string GetContentType();

	// 获取Web浏览器提交的原始Cookie（浏览器端永久存储）数据。应该使用函数GetCookies， GetCookieString和 GetCookieInteger，而不是直接检查这个字符串。 
	static string GetCookie();

	// 获取浏览器可以接受的MIME内容类型的空格分隔列表（请参阅 cgiHeaderContentType（））或空字符串。不幸的是，大多数当前的浏览器并不是以一种有用的形式提供这个变量。
	static string GetAccept();

	// 获取正在使用的浏览器的名称，如果此信息不可用，则为空字符串。 
	static string GetUserAgent();

	// 获取用户访问的上一页的URL。这通常是将用户带到您的程序的表单的URL。请注意，报告此信息完全取决于浏览器，可能选择不这样做。但是，该变量通常是准确的。
	static string GetReferrer();

	// 获取收到的表单或查询数据的字节数。请注意，如果提交是提交表单或查询，库将直接从cgiIn和/或cgiQueryString读取和解析所有信息。在这种情况下，程序员不应该这样做。
	static int GetContentLength();
};


/***************************************************************************************************************************************************************************************************/

// 控制器父类**************************************************************************************************************************************************************************************
class CController: public CCgiManager
{
private:
	// 成员变量
	static map <string, int>            IntegerMap;
	static map <string, string>         StringMap;
	static map <string, double>         DoubleMap;
	static map <string, vector<string> > ListMap;

	// 得到 HTML 内容
	static string HtmlRead(string FileName);

public:
	// 构造与折构函数
	CController();
	~CController();

	// 输出内容
	static void OutPut(string String, ...);

	// 获得输入字符
	static string InPut(string String);
	static int    InPut(string String, int    Default);
	static double InPut(string String, double Default);

	// 获取文件数据 (返回文件内容)
	static string InPut(string String, string &FileName, int &FileSize, string &contentType);

	// 设置Cookie
	static void SetCookie(string Name, string Value);
	static void SetCookie(string Name, int    Value);

	// 获得Cookie
	static string GetCookie(string Name);
	static int    GetCookie(string Name, int Default);

	// 上传文件到服务器
	static void Upload(string Name, string Path);

	// 从服务器下载文件
	static void Download(string FileName, string FilePath);
	
	// 控制器初始化
	static void Initialize();

	// 绑定方法 (只能绑定void类型并且参数为空的静态方法)
	template<typename T>
	static void BindFunction(const T& ControllerClass, string FunctionText, PtrFun FunctionName)
	{
		// 处理控制器名称, 转为小写
		char* Name = strlowr((char*)(typeid( T ).name()));
		
		#ifdef _WIN32
		// 替换固定字符串
		Replace(Name, "class ", "");
		Replace(Name, "controller", "");
		Replace(Name, " *", "");
        #else
		// 替换固定字符串
		Replace(Name, "6", "");
        #endif
		
		// 判断并去掉前缀
		string temp = Name;
		int pos = temp.find(strlowr((char*)controller_prefix.c_str()));
		if(pos == 0 && pos != -1 && controller_prefix.length() > 0)
			strncpy(Name, Name + controller_prefix.length(), strlen(Name));

		// 判断并去掉后缀
		pos = temp.rfind(strlowr((char*)controller_suffix.c_str()));
		if(pos == 0 && pos != -1 && controller_suffix.length() > 0)
			Name[strlen(Name) - controller_suffix.length()] = '\0';

		// 得到控制器名称
		string ControllerName(Name);

		// 绑定方法
		FunctionEntry funEntry = {FunctionName, FunctionText};
		FunTab[ControllerName][FunctionText] = funEntry.pFun;
	}

	// 渲染并输出HTMl
	static void  HtmlView(string FileName = "index", ...);

	// 绑定视图数据
	static void BindValue(string Name, int    Value);
	static void BindValue(string Name, string Value);
	static void BindValue(string Name, double Value);
	static void BindValue(string Name, vector<string> Value);

	// Json编码,返回json数据
	string Json_Encode();

	// Json 解码
	void Json_Decode();

	// Xml编码,返回xml数据
	string Xml_Encode();

	// Xml 解码
	void Xml_Decode();
};


/***************************************************************************************************************************************************************************************************/


// 模型父类**************************************************************************************************************************************************************************************
class CModel
{
private:
	// 数据库类型
	static int Type;

	// Sqlite数据库对象
	static CSqliteManager m_sqlite;

	// 数据库配置
	static void DatabaseConfig(string ConfigFile = "application/database.txt");

public:
	// 构造与折构函数
	CModel();
	~CModel();

	// 初始化状态
	static bool Initialize;

	// 执行Sql (需自行检查sql语句合理性)
	static bool Execute(string Sql);

	// 查询Sql (需自行检查sql语句合理性)
	static bool Query(string Sql, vector<string> &Result);

	// 创建数据表
	static bool Create(string TableName, string Params);

	// 删除数据表
	static bool Drop(string TableName);

	// 修改数据表 (Operation可选: 重命名表:REN_TABLE / 添加列:ADD_COLUMN / 删除列:DEL_COLUMN / 修改列:REN_COLUMN)(NewParams仅用于修改列)
	static bool Alter(string TableName, int Operation, string Params, string NewParams = "");

	// 检查数据表是否存在
	static bool Exist(string TableName);

	// 写入数据
	static bool Save(string TableName, string Params);

	// 删除数据
	static bool Delete(string TableName, string Params);

	// 更新数据 (单条)
	static bool UpDate(string TableName, string Columns, string NewData, string Params);

	// 更新数据 (批量)
	static bool UpDate(string TableName, string Columns, string Params);

	// 获取数据 (单条)
	static string Get (string TableName, string Column,  string Params,  string Order = "", string Limit = "", int SortMode = MOD_ASC, bool DISTINCT = false, string COUNT = "", string COLUMN = "",string GROUP = "", string HAVING = "");

	// 获取数据 (批量)
	static vector<string> GetAll(string TableName, string Params = "", string Order = "", string Limit = "", int SortMode = MOD_ASC, bool DISTINCT = false, string COUNT = "", string COLUMN = "", string GROUP = "", string HAVING = "");

	// 检查数据项是否存在
	static bool Check(string TableName, string Params = "");

	// 统计数据
	static int Count(string TableName, string Param = ""); 

	// 数据求和
	static double Sum  (string TableName, string Column, string Params = "");

	// 数据求积
	static double Product(string TableName, string Column, string Params="");

	// 数据求平均数
	static double Avg  (string TableName, string Column, string Params = "");

	// 数据求出众数
	static double Plu  (string TableName, string Column, string Params = "");

	// 数据求中位数
	static double Mid  (string TableName, string Column, string Params = "");

	// 数据求最大值
	static double Max  (string TableName, string Column, string Params = "");

	// 数据求最小值
	static double Min  (string TableName, string Column, string Params = "");

	// 数据求随机值
	static int   Rand  (string TableName, string Column, string Params = "");
};

