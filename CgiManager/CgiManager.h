#pragma once
/* CGI Manager
 * 作者: 紫辉(紫影龙)
 * 日期: 2018-6-22
 * 版权所有 2016 - 2018 紫影龙工作室
 * https://www.shadowviolet.com
 */

#include "cgic.h"

#include <afx.h>
#include <vector>
using std::vector;

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

#include "SqliteManager.h"

//配置文件读取 *********************************************************************************************************************************************************************** 
class CConfig {  
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
    template<class T> T Read( const std::string& in_key ) const;  //!<Search for key and read value or optional default value, call as read<T>  
    template<class T> T Read( const std::string& in_key, const T& in_value ) const;  
    template<class T> bool ReadInto( T& out_var, const std::string& in_key ) const;  
    template<class T>  
    bool ReadInto( T& out_var, const std::string& in_key, const T& in_value ) const;  
    bool FileExist(std::string filename);  
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
  
  
/* static */  
template<class T>  
std::string CConfig::T_as_string( const T& t )  
{  
    // Convert from a T to a string  
    // Type T must support << operator  
    std::ostringstream ost;  
    ost << t;  
    return ost.str();  
}  
  
  
/* static */  
template<class T>  
T CConfig::string_as_T( const std::string& s )  
{  
    // Convert from a string to a T  
    // Type T must support >> operator  
    T t;  
    std::istringstream ist(s);  
    ist >> t;  
    return t;  
}  
  
  
/* static */  
template<>  
inline std::string CConfig::string_as_T<std::string>( const std::string& s )  
{  
    // Convert from a string to a string  
    // In other words, do nothing  
    return s;  
}  
  
  
/* static */  
template<>  
inline bool CConfig::string_as_T<bool>( const std::string& s )  
{  
    // Convert from a string to a bool  
    // Interpret "false", "F", "no", "n", "0" as false  
    // Interpret "true", "T", "yes", "y", "1", "-1", or anything else as true  
    bool b = true;  
    std::string sup = s;  
    for( std::string::iterator p = sup.begin(); p != sup.end(); ++p )  
        *p = toupper(*p);  // make string all caps  
    if( sup==std::string("FALSE") || sup==std::string("F") ||  
        sup==std::string("NO") || sup==std::string("N") ||  
        sup==std::string("0") || sup==std::string("NONE") )  
        b = false;  
    return b;  
}  
  
  
template<class T>  
T CConfig::Read( const std::string& key ) const  
{  
    // Read the value corresponding to key  
    mapci p = m_Contents.find(key);  
    if( p == m_Contents.end() ) throw Key_not_found(key);  
    return string_as_T<T>( p->second );  
}  
  
  
template<class T>  
T CConfig::Read( const std::string& key, const T& value ) const  
{  
    // Return the value corresponding to key or given default value  
    // if key is not found  
    mapci p = m_Contents.find(key);  
    if( p == m_Contents.end() ) return value;  
    return string_as_T<T>( p->second );  
}  
  
  
template<class T>  
bool CConfig::ReadInto( T& var, const std::string& key ) const  
{  
    // Get the value corresponding to key and store in var  
    // Return true if key is found  
    // Otherwise leave var untouched  
    mapci p = m_Contents.find(key);  
    bool found = ( p != m_Contents.end() );  
    if( found ) var = string_as_T<T>( p->second );  
    return found;  
}  
  
  
template<class T>  
bool CConfig::ReadInto( T& var, const std::string& key, const T& value ) const  
{  
    // Get the value corresponding to key and store in var  
    // Return true if key is found  
    // Otherwise set var to given default  
    mapci p = m_Contents.find(key);  
    bool found = ( p != m_Contents.end() );  
    if( found )  
        var = string_as_T<T>( p->second );  
    else  
        var = value;  
    return found;  
}  
  
  
template<class T>  
void CConfig::Add( const std::string& in_key, const T& value )  
{  
    // Add a key with given value  
    std::string v = T_as_string( value );  
    std::string key=in_key;  
    trim(key);  
    trim(v);  
    m_Contents[key] = v;  
    return;  
}  


/***************************************************************************************************************************************************************************************/


// 使用说明: 本程序将自动添加入口点:cgiMain ,请把主程序里的入口点删除，并加上 int cgiMain(){}
class CCgiManager
{
private:
	static bool IsSetHead;

public:
	// 构造与折构函数
	CCgiManager();
	CCgiManager(char * Type);
	~CCgiManager();


	// 跳转到请求方法
	static void Jump();

	// 设置头
	static void SetHead(char* Type = "text/html");

	// 设置 Location
	static void Location(char* Url);

	// 重定向Url
	static void Redirect(char* Url, bool Visible = false);

	// 输出HTTP错误状态代码
	static void SetStatus(int Status, char* Message);

	// 输出内容
	static void OutPut(char * String, ...);

	// 转码输出Html
	static void HtmlEscape(char * Name, bool Newlines = true);

	// 转码输出Html数据流
	static void HtmlEscapeData(char * Name, int len, bool Newlines = true);

	// 转码输出Value
	static void ValueEscape(char * Value, bool Newlines = true);

	// 转码输出Value数据流
	static void ValueEscapeData(char * Value, int len, bool Newlines = true);

	// 获取字符串数据
	static char* InPutString(char * String, bool OutPut = false, bool Newlines = false, int Max = 0);

	// 获取不带回车换行符的字符串数据
	static char* InPutStringNoNewlines(char * String, bool OutPut = false, bool Newlines = false, int Max = 0);

	// 设置字符串的储存空间，需和InPutString或InPutStringNoNewlines配合使用
	static bool InPutStringSpaceNeeded(char * String, int Length);

	// 获取短整型数据
	static int InPutInteger(char * String,  int Default = 0, bool OutPut = false, bool Newlines = false);

	// 获取短整型区间数据(最大值与最小值为必须参数)
	static int InPutIntegerBound(char* String, int Min, int Max, int Default = 0, bool OutPut = false, bool Newlines = false);

	// 获取双精度数据
	static double InPutDouble(char * String, int Default = 0, bool OutPut = false, bool Newlines = false);

	// 获取双精度区间数据(最大值与最小值为必须参数)
	static double InPutDoubleBound(char* String, double Min, double Max, double Default = 0.00, bool OutPut = false, bool Newlines = false);

	// 获取单个Checkbox数据(返回是否选中)
	static bool InPutCheckboxSingle(char * String);

	// 获取一组Checkbox数据(返回所有选中项，若无选中项则返回空)
    static char * InPutCheckboxMultiple(char * String);

	// 获取一组单选Select数据(返回选中的项)
	static char* InPutSelectSingle(char * String , char *texts[], int total, int Default = 0);

	// 获取一组多选Select数据(返回选中的项)
	static bool InPutSelectMultiple(char * String, vector<char> &pResult, char *texts[], int total, int Invalid = 0);

	// 获取一组Radio数据(返回选中的项)
	static char* InPutRadio(char * String, char *Texts[], int Total, int Default = 0);

	// 获取Submit数据(提交成功返回真否则返回假)
	static bool SubmitClicked(char * String);

	// 为站点设置Cookie数据
	static void SetCookieString(char * Name, char * Value, char* Domain = "DefaultDomain");
	static void SetCoolieInteger(char * Name,  int  Value, char* Domain = "DefaultDomain");

	// 获取站点Cookie数据
	static char* GetCookieString(char * Name);
	static int  GetCookieInteger(char * Name, int Default);

	//获取所有Cookie数据
	static char * GetCookies(bool OutPut = false);

	//获取所有表单名称(Name)
	static char* Entries(bool OutPut = false);

	// 将表单数据储存在磁盘里 
	static bool LoadEnvironment(char * FileName);

	// 从磁盘里读取表单数据
	static bool SaveEnvironment(char * FileName);

	// 获取文件数据
	static bool InPutFile(char* String, char* &FileName, int &FileSize, char* &contentType);

	// 读取文件数据
	static char* ReadFileData(char* String, bool OutPut = false);

	// 保存文件数据
	static bool SaveFileData(char* String, char* FilePath);


	// 获取服务器软件的名称，如果未知，则指向空字符串。
	static char* GetServerSoftware();

	// 获取服务器的名称，如果未知，则指向空字符串。
	static char* GetServerName();

	// 获取网关接口（通常为CGI / 1.1）的名称，如果未知，则指向空字符串。
	static char* GetGatewayInterface();

	// 获取使用的协议（通常为HTTP / 1.0），如果未知，则指向空字符串。 
	static char* GetServerProtocol();

	// 获取服务器正在监听HTTP连接（通常为80）的端口号，或未知的空字符串。 
	static char* GetServerPort();

	// 获取请求中使用的方法（通常为GET或POST），如果未知（这不应该发生），则为空字符串）。 
	static char* GetRequestMethod();

	// 大多数Web服务器在请求的URL中识别出超出CGI程序本身的任何附加路径信息，并将该信息传递给程序。cgiPathInfo指向这个额外的路径信息。 
	static char* GetPathInfo();

	// 大多数Web服务器在请求的URL中识别出超出CGI程序本身的任何附加路径信息，并将该信息传递给程序。cgiPathTranslated指向此附加路径信息，由服务器转换为本地服务器上的文件系统路径。 
	static char* GetPathTranslated();

	// 获取调用程序的名称。 
	static char* GetScriptName();

	// 获取包含由GET方法表单或<ISINDEX>标签导致用户提交的任何查询信息。请注意，除非使用<ISINDEX>标记，否则不需要直接解析此信息; 通常它自动解析。使用cgiForm函数系列检索与表单输入字段相关的值。
	static char* GetQueryString();

	// 获取浏览器的完全解析的主机名（如果已知）或空字符串（如果未知）。 
	static char* GetRemoteHost();

	// 获取浏览器的点分十进制IP地址（如果已知）或空字符串（如果未知）。 
	static char* GetRemoteAddr();

	// 获取用于请求的授权类型（如果有的话），如果没有或未知，则指向空字符串。 
	static char* GetAuthType();

	// 获取用户已经认证的用户名; 如果没有发生身份验证，则为空字符串。这些信息的确定性取决于使用授权的类型; 
	static char* GetRemoteUser();

	// 由用户通过用户识别协议自愿指定用户名; 一个空字符串，如果未知。此信息不安全。可以由用户安装在不安全的系统，如Windows机器上。 
	static char* GetRemoteIdent();

	// 获取用户提交的信息的MIME内容类型（如果有）; 如果没有提交信息，则为空字符串。如果此字符串等于 application/x-www-form-urlencoded或 multipart/form-data，则自动检查提交的表单数据。如果此字符串具有任何其他非空值，则会提交不同类型的数据。这是非常罕见的，因为大多数浏览器只能直接提交表单和文件上传。 
	static char* GetContentType();

	// 获取Web浏览器提交的原始Cookie（浏览器端永久存储）数据。应该使用函数GetCookies， GetCookieString和 GetCookieInteger，而不是直接检查这个字符串。 
	static char* GetCookie();

	// 获取浏览器可以接受的MIME内容类型的空格分隔列表（请参阅 cgiHeaderContentType（））或空字符串。不幸的是，大多数当前的浏览器并不是以一种有用的形式提供这个变量。
	static char* GetAccept();

	// 获取正在使用的浏览器的名称，如果此信息不可用，则为空字符串。 
	static char* GetUserAgent();

	// 获取用户访问的上一页的URL。这通常是将用户带到您的程序的表单的URL。请注意，报告此信息完全取决于浏览器，可能选择不这样做。但是，该变量通常是准确的。
	static char* GetReferrer();

	// 获取收到的表单或查询数据的字节数。请注意，如果提交是提交表单或查询，库将直接从cgiIn和/或cgiQueryString读取和解析所有信息。在这种情况下，程序员不应该这样做。
	static int GetContentLength();
};


/***************************************************************************************************************************************************************************************************/


// 控制器父类**************************************************************************************************************************************************************************************
class CController: public CCgiManager
{
public:
	// 构造与折构函数
	CController();
	CController(char* Type);
	~CController();

	// 得到 HTML 内容
	static char* HtmlRead(char* FileName = "index.html");

	// 渲染并输出HTMl
	static void  HtmlView(char* FileName = "index.html", ...);

	// Json编码,返回json数据
	char* Json_Encode();

	// Json 解码
	void Json_Decode();

	// Xml编码,返回xml数据
	char* Xml_Encode();

	// Xml 解码
	void Xml_Decode();
};


/***************************************************************************************************************************************************************************************************/


// 模型父类**************************************************************************************************************************************************************************************
class CModel: public CSqliteManager
{
private:
	// 数据库类型
	static int Type;

	// 配置Sqlite数据库
	static CSqliteManager m_Sql;

public:
	// 构造与折构函数
	CModel();
	~CModel();

	// 配置数据库
	static void DatabaseConfig(char* ConfigFile = "./application/database.config");

	// 写入数据
	static BOOL Save(char* TableName, char* Params);

	// 删除数据
	static BOOL Delete(char* TableName, char* Params);

	// 更新数据 (单条)
	static BOOL UpDate(char* TableName, char* Columns, char* NewData, char* Params);

	// 更新数据 (批量)
	static BOOL UpDate(char* TableName, char* Columns, char* Params);

	// 获取数据 (单条)
	static char* Get  (char* TableName, int Col, char* Params, char* Order = "", char* Limit = "", int SortMode = MOD_ASC, BOOL DISTINCT = FALSE, char* COUNT = "", char* COLUMN = "", char* GROUP = "", char* HAVING = "");

	// 获取数据 (批量)
	static vector<char*> GetAll(char* TableName, int Col, char* Params = "", char* Order = "", char* Limit = "", int SortMode = MOD_ASC, BOOL DISTINCT = FALSE, char* COUNT = "", char* COLUMN = "", char* GROUP = "", char* HAVING = "");

	// 统计数据
	static int Count(char* TableName, char* Param = ""); 

	// 数据求和
	static int Sum  (char* TableName, char* Column, char* Params = "");

	// 数据求积
	static int Product(char* TableName, char* Column, char* Params="");

	// 数据求平均数
	static int Avg  (char* TableName, char* Column, char* Params = "");

	// 数据求中位数
	static int Mid  (char* TableName, char* Column, char* Params = "");
};

