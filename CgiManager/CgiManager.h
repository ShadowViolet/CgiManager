#pragma once
/* CGI Manager
 * ����: �ϻ�(��Ӱ��)
 * ����: 2018-6-22
 * ��Ȩ���� 2016 - 2018 ��Ӱ��������
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

//�����ļ���ȡ *********************************************************************************************************************************************************************** 
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


// ʹ��˵��: �������Զ������ڵ�:cgiMain ,��������������ڵ�ɾ���������� int cgiMain(){}
class CCgiManager
{
private:
	static bool IsSetHead;

public:
	// �������۹�����
	CCgiManager();
	CCgiManager(char * Type);
	~CCgiManager();


	// ��ת�����󷽷�
	static void Jump();

	// ����ͷ
	static void SetHead(char* Type = "text/html");

	// ���� Location
	static void Location(char* Url);

	// �ض���Url
	static void Redirect(char* Url, bool Visible = false);

	// ���HTTP����״̬����
	static void SetStatus(int Status, char* Message);

	// �������
	static void OutPut(char * String, ...);

	// ת�����Html
	static void HtmlEscape(char * Name, bool Newlines = true);

	// ת�����Html������
	static void HtmlEscapeData(char * Name, int len, bool Newlines = true);

	// ת�����Value
	static void ValueEscape(char * Value, bool Newlines = true);

	// ת�����Value������
	static void ValueEscapeData(char * Value, int len, bool Newlines = true);

	// ��ȡ�ַ�������
	static char* InPutString(char * String, bool OutPut = false, bool Newlines = false, int Max = 0);

	// ��ȡ�����س����з����ַ�������
	static char* InPutStringNoNewlines(char * String, bool OutPut = false, bool Newlines = false, int Max = 0);

	// �����ַ����Ĵ���ռ䣬���InPutString��InPutStringNoNewlines���ʹ��
	static bool InPutStringSpaceNeeded(char * String, int Length);

	// ��ȡ����������
	static int InPutInteger(char * String,  int Default = 0, bool OutPut = false, bool Newlines = false);

	// ��ȡ��������������(���ֵ����СֵΪ�������)
	static int InPutIntegerBound(char* String, int Min, int Max, int Default = 0, bool OutPut = false, bool Newlines = false);

	// ��ȡ˫��������
	static double InPutDouble(char * String, int Default = 0, bool OutPut = false, bool Newlines = false);

	// ��ȡ˫������������(���ֵ����СֵΪ�������)
	static double InPutDoubleBound(char* String, double Min, double Max, double Default = 0.00, bool OutPut = false, bool Newlines = false);

	// ��ȡ����Checkbox����(�����Ƿ�ѡ��)
	static bool InPutCheckboxSingle(char * String);

	// ��ȡһ��Checkbox����(��������ѡ�������ѡ�����򷵻ؿ�)
    static char * InPutCheckboxMultiple(char * String);

	// ��ȡһ�鵥ѡSelect����(����ѡ�е���)
	static char* InPutSelectSingle(char * String , char *texts[], int total, int Default = 0);

	// ��ȡһ���ѡSelect����(����ѡ�е���)
	static bool InPutSelectMultiple(char * String, vector<char> &pResult, char *texts[], int total, int Invalid = 0);

	// ��ȡһ��Radio����(����ѡ�е���)
	static char* InPutRadio(char * String, char *Texts[], int Total, int Default = 0);

	// ��ȡSubmit����(�ύ�ɹ���������򷵻ؼ�)
	static bool SubmitClicked(char * String);

	// Ϊվ������Cookie����
	static void SetCookieString(char * Name, char * Value, char* Domain = "DefaultDomain");
	static void SetCoolieInteger(char * Name,  int  Value, char* Domain = "DefaultDomain");

	// ��ȡվ��Cookie����
	static char* GetCookieString(char * Name);
	static int  GetCookieInteger(char * Name, int Default);

	//��ȡ����Cookie����
	static char * GetCookies(bool OutPut = false);

	//��ȡ���б�����(Name)
	static char* Entries(bool OutPut = false);

	// �������ݴ����ڴ����� 
	static bool LoadEnvironment(char * FileName);

	// �Ӵ������ȡ������
	static bool SaveEnvironment(char * FileName);

	// ��ȡ�ļ�����
	static bool InPutFile(char* String, char* &FileName, int &FileSize, char* &contentType);

	// ��ȡ�ļ�����
	static char* ReadFileData(char* String, bool OutPut = false);

	// �����ļ�����
	static bool SaveFileData(char* String, char* FilePath);


	// ��ȡ��������������ƣ����δ֪����ָ����ַ�����
	static char* GetServerSoftware();

	// ��ȡ�����������ƣ����δ֪����ָ����ַ�����
	static char* GetServerName();

	// ��ȡ���ؽӿڣ�ͨ��ΪCGI / 1.1�������ƣ����δ֪����ָ����ַ�����
	static char* GetGatewayInterface();

	// ��ȡʹ�õ�Э�飨ͨ��ΪHTTP / 1.0�������δ֪����ָ����ַ����� 
	static char* GetServerProtocol();

	// ��ȡ���������ڼ���HTTP���ӣ�ͨ��Ϊ80���Ķ˿ںţ���δ֪�Ŀ��ַ����� 
	static char* GetServerPort();

	// ��ȡ������ʹ�õķ�����ͨ��ΪGET��POST�������δ֪���ⲻӦ�÷���������Ϊ���ַ������� 
	static char* GetRequestMethod();

	// �����Web�������������URL��ʶ�������CGI��������κθ���·����Ϣ����������Ϣ���ݸ�����cgiPathInfoָ����������·����Ϣ�� 
	static char* GetPathInfo();

	// �����Web�������������URL��ʶ�������CGI��������κθ���·����Ϣ����������Ϣ���ݸ�����cgiPathTranslatedָ��˸���·����Ϣ���ɷ�����ת��Ϊ���ط������ϵ��ļ�ϵͳ·���� 
	static char* GetPathTranslated();

	// ��ȡ���ó�������ơ� 
	static char* GetScriptName();

	// ��ȡ������GET��������<ISINDEX>��ǩ�����û��ύ���κβ�ѯ��Ϣ����ע�⣬����ʹ��<ISINDEX>��ǣ�������Ҫֱ�ӽ�������Ϣ; ͨ�����Զ�������ʹ��cgiForm����ϵ�м�����������ֶ���ص�ֵ��
	static char* GetQueryString();

	// ��ȡ���������ȫ�������������������֪������ַ��������δ֪���� 
	static char* GetRemoteHost();

	// ��ȡ������ĵ��ʮ����IP��ַ�������֪������ַ��������δ֪���� 
	static char* GetRemoteAddr();

	// ��ȡ�����������Ȩ���ͣ�����еĻ��������û�л�δ֪����ָ����ַ����� 
	static char* GetAuthType();

	// ��ȡ�û��Ѿ���֤���û���; ���û�з��������֤����Ϊ���ַ�������Щ��Ϣ��ȷ����ȡ����ʹ����Ȩ������; 
	static char* GetRemoteUser();

	// ���û�ͨ���û�ʶ��Э����Ըָ���û���; һ�����ַ��������δ֪������Ϣ����ȫ���������û���װ�ڲ���ȫ��ϵͳ����Windows�����ϡ� 
	static char* GetRemoteIdent();

	// ��ȡ�û��ύ����Ϣ��MIME�������ͣ�����У�; ���û���ύ��Ϣ����Ϊ���ַ�����������ַ������� application/x-www-form-urlencoded�� multipart/form-data�����Զ�����ύ�ı����ݡ�������ַ��������κ������ǿ�ֵ������ύ��ͬ���͵����ݡ����Ƿǳ������ģ���Ϊ����������ֻ��ֱ���ύ�����ļ��ϴ��� 
	static char* GetContentType();

	// ��ȡWeb������ύ��ԭʼCookie������������ô洢�����ݡ�Ӧ��ʹ�ú���GetCookies�� GetCookieString�� GetCookieInteger��������ֱ�Ӽ������ַ����� 
	static char* GetCookie();

	// ��ȡ��������Խ��ܵ�MIME�������͵Ŀո�ָ��б������ cgiHeaderContentType����������ַ��������ҵ��ǣ��������ǰ���������������һ�����õ���ʽ�ṩ���������
	static char* GetAccept();

	// ��ȡ����ʹ�õ�����������ƣ��������Ϣ�����ã���Ϊ���ַ����� 
	static char* GetUserAgent();

	// ��ȡ�û����ʵ���һҳ��URL����ͨ���ǽ��û��������ĳ���ı���URL����ע�⣬�������Ϣ��ȫȡ���������������ѡ�������������ǣ��ñ���ͨ����׼ȷ�ġ�
	static char* GetReferrer();

	// ��ȡ�յ��ı����ѯ���ݵ��ֽ�������ע�⣬����ύ���ύ�����ѯ���⽫ֱ�Ӵ�cgiIn��/��cgiQueryString��ȡ�ͽ���������Ϣ������������£�����Ա��Ӧ����������
	static int GetContentLength();
};


/***************************************************************************************************************************************************************************************************/


// ����������**************************************************************************************************************************************************************************************
class CController: public CCgiManager
{
public:
	// �������۹�����
	CController();
	CController(char* Type);
	~CController();

	// �õ� HTML ����
	static char* HtmlRead(char* FileName = "index.html");

	// ��Ⱦ�����HTMl
	static void  HtmlView(char* FileName = "index.html", ...);

	// Json����,����json����
	char* Json_Encode();

	// Json ����
	void Json_Decode();

	// Xml����,����xml����
	char* Xml_Encode();

	// Xml ����
	void Xml_Decode();
};


/***************************************************************************************************************************************************************************************************/


// ģ�͸���**************************************************************************************************************************************************************************************
class CModel: public CSqliteManager
{
private:
	// ���ݿ�����
	static int Type;

	// ����Sqlite���ݿ�
	static CSqliteManager m_Sql;

public:
	// �������۹�����
	CModel();
	~CModel();

	// �������ݿ�
	static void DatabaseConfig(char* ConfigFile = "./application/database.config");

	// д������
	static BOOL Save(char* TableName, char* Params);

	// ɾ������
	static BOOL Delete(char* TableName, char* Params);

	// �������� (����)
	static BOOL UpDate(char* TableName, char* Columns, char* NewData, char* Params);

	// �������� (����)
	static BOOL UpDate(char* TableName, char* Columns, char* Params);

	// ��ȡ���� (����)
	static char* Get  (char* TableName, int Col, char* Params, char* Order = "", char* Limit = "", int SortMode = MOD_ASC, BOOL DISTINCT = FALSE, char* COUNT = "", char* COLUMN = "", char* GROUP = "", char* HAVING = "");

	// ��ȡ���� (����)
	static vector<char*> GetAll(char* TableName, int Col, char* Params = "", char* Order = "", char* Limit = "", int SortMode = MOD_ASC, BOOL DISTINCT = FALSE, char* COUNT = "", char* COLUMN = "", char* GROUP = "", char* HAVING = "");

	// ͳ������
	static int Count(char* TableName, char* Param = ""); 

	// �������
	static int Sum  (char* TableName, char* Column, char* Params = "");

	// �������
	static int Product(char* TableName, char* Column, char* Params="");

	// ������ƽ����
	static int Avg  (char* TableName, char* Column, char* Params = "");

	// ��������λ��
	static int Mid  (char* TableName, char* Column, char* Params = "");
};

