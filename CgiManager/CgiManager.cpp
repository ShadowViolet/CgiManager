#include "StdAfx.h"
#include "CgiManager.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <cstring>
using namespace std;


// ��̬��Ա��ʼ��
bool CCgiManager::IsSetHead = false;
int CModel::Type = 0;


/* �����ļ���***********************************************************************************************************************************************************************************
 * ��;����д�����ļ�
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


/* CGI����***********************************************************************************************************************************************************************************
 * ��;�������������CGI����
 */

CCgiManager::CCgiManager()
{
	// Ĭ�Ϸ���HTML
	SetHead("text/html");
}


CCgiManager::CCgiManager(char * Type)
{
	// ����type
	SetHead(Type);
}


CCgiManager::~CCgiManager()
{
}


/*���ַ�������߽�ȡn���ַ�*/  
char * left(char *dst,char *src, int n)  
{  
    char *p = src;  
    char *q = dst;  
    int len = strlen(src);  
    if(n>len) n = len;  
    while(n--) *(q++) = *(p++);  
    *(q++)='\0'; /*�б�Ҫ�𣿺��б�Ҫ*/  
    return dst;  
}  
  
/*���ַ������м��ȡn���ַ�*/  
char * mid(char *dst,char *src, int n,int m) /*nΪ���ȣ�mΪλ��*/  
{  
    char *p = src;  
    char *q = dst;  
    int len = strlen(src);  
    if(n>len) n = len-m;    /*�ӵ�m�������*/  
    if(m<0) m=0;    /*�ӵ�һ����ʼ*/  
    if(m>len) return NULL;  
    p += m;  
    while(n--) *(q++) = *(p++);  
    *(q++)='\0'; /*�б�Ҫ�𣿺��б�Ҫ*/  
    return dst;  
}  
  
/*���ַ������ұ߽�ȡn���ַ�*/  
char * right(char *dst,char *src, int n)  
{  
    char *p = src;  
    char *q = dst;  
    int len = strlen(src);  
    if(n>len) n = len;  
    p += (len-n);   /*���ұߵ�n���ַ���ʼ����0���������ɰ�*/  
    while(*(q++) = *(p++));  
    return dst;  
}  

// �ַ����ָ�
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


// ��source�ַ���������s1�ַ�ȫ���滻���ַ�s2
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


// ��ת�����󷽷�
void CCgiManager::Jump()
{
	char* Method = cgiPathInfo;
	char* Directory, *Controller, *Function;

	// Ŀ������
	vector<string> v;

	// Ĭ�ϲ���(û�в���)
	if( strlen(Method) == strlen(cgiScriptName) )
	{
		// Ĭ��ֵ
		Directory  = "index";
		Controller = "index";
		Function   = "index";
	}
	else
	{
		// �����ַ���
		right(Method, cgiPathInfo, strlen(cgiPathInfo) - strlen(cgiScriptName));

		// �ָ��
		SplitString(Method, v, "/");

		// �������
		/*for(vector<string>::size_type i = 0; i != v.size(); ++i)
		{
		OutPut((char*)v[i].c_str());
		}*/

		// ����Ŀ¼����
		Directory  = (char*)v[1].c_str();

		// ��������������
		Controller = (char*)v[2].c_str();

		// ������������
		Function   = (char*)v[3].c_str();

		// Ĭ��Ŀ¼
		if(Directory == "" || strlen(Directory) == 0)
			Directory = "index";

		// Ĭ�Ͽ�����
		if(Controller == "" || strlen(Controller) == 0)
			Controller = "index";

		// Ĭ�Ϸ���
		if(Function == "" || strlen(Function) == 0)
			Function = "index";
	}

	// �ض���Ŀ��
	string Target = "/application/" + (string)Directory + "/controller/" + (string)Controller + ".cgi/" + (string)Function;
	Redirect((char*)Target.c_str());
}


// ����ͷ
void CCgiManager::SetHead(char * Type)
{
	cgiHeaderContentType(Type);
	IsSetHead = true;
}


// ����Location
void CCgiManager::Location(char* Url)
{
	cgiHeaderLocation(Url);
}


// �ض���Url
void CCgiManager::Redirect(char* Url, bool Visible)
{
	// Url ��ַ������ʾ�ı�
	if(Visible)
		OutPut("<script>location.href = '%s'</script>", true, Url);

	// Url ��ַ������(���򽫳��ִ���)
	else
	{
		string Target = "<script language=javascript> function createXMLHttpRequest(){if(window.XMLHttpRequest){XMLHttpR = new XMLHttpRequest();}else if(window.ActiveXObject){try{XMLHttpR = new ActiveXObject(\"Msxml2.XMLHTTP\");}catch(e){try{XMLHttpR = new ActiveXObject(\"Microsoft.XMLHTTP\");}catch(e){}}}} function sendRequest(url){createXMLHttpRequest();XMLHttpR.open(\"GET\",url,true);XMLHttpR.setRequestHeader(\"Content-Type\",\"text/html;charset=gb2312\");XMLHttpR.onreadystatechange = processResponse;XMLHttpR.send(null);} function processResponse(){if(XMLHttpR.readyState ==4 && XMLHttpR.status == 200){document.write(XMLHttpR.responseText);}}";
		Target += "sendRequest(\"%s\");</script>";

		OutPut((char*)Target.c_str(), true, Url);
	}
}


// ���HTTP����״̬����
void CCgiManager::SetStatus(int Status, char* Message)
{
	cgiHeaderStatus(Status, Message);
}


// ת�벢���Html
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


// ת�벢���Value
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


// �������
void CCgiManager::OutPut(char * String, ...)
{
	// ����Ƿ���Ĭ�ϵ�ͷ
	if(!IsSetHead)
		SetHead();

	//�õ���ε���ʼ��ַ
	va_list  pArgList;
	va_start(pArgList, String);

	// ȡֵ���������
	char s[65535];
	
	// ��ʽ�������
	vsnprintf(s, 65535, String, pArgList);
	vfprintf(cgiOut, s, pArgList);

	//��β
	va_end(pArgList);
}


// ��ȡ�ַ�������
char* CCgiManager::InPutString(char * String, bool OutPut, bool Newlines, int Max)
{
	// ���Ŀ���ֶ��Ƿ����
	if (cgiFormSubmitClicked(String) == cgiFormSuccess)
	{
		char name[65535];

		if(Max == 0)
			cgiFormString(String, name, sizeof(String) * 2);
		else
			cgiFormString(String, name, Max);

		// �ж��Ƿ���Ҫ���
		if(OutPut)
		{
			// ת�벢���
			HtmlEscape(name);

			if(Newlines)
				fprintf(cgiOut, "\n");
		}

		// ��ֵ
		char* result = name;

		// ����
		return result;
	}

	return NULL;
}


// ��ȡ�����س����з����ַ�������
char* CCgiManager::InPutStringNoNewlines(char * String, bool OutPut, bool Newlines, int Max)
{
	// ���Ŀ���ֶ��Ƿ����
	if (cgiFormSubmitClicked(String) == cgiFormSuccess)
	{
		char name[65535];
		if(Max == 0)
			cgiFormStringNoNewlines(String, name, sizeof(String) * 2);
		else
			cgiFormStringNoNewlines(String, name, Max);

		// �ж��Ƿ���Ҫ���
		if(OutPut)
		{
			// ת�벢���
			HtmlEscape(name);

			if(Newlines)
				fprintf(cgiOut, "\n");
		}

		// ��ֵ
		char* result = name;

		// ����
		return result;
	}

	return NULL;
}


// �����ַ����Ĵ���ռ䣬���InPutString��InPutStringNoNewlines���ʹ��
bool CCgiManager::InPutStringSpaceNeeded(char * String, int Length)
{
	if(cgiFormStringSpaceNeeded(String, &Length) ==  cgiFormSuccess)
		return true;
	else 
		return false;
}


// ��ȡ����������
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


// ��ȡ��������������(���ֵ����СֵΪ�������)
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


// ��ȡ˫��������
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


// ��ȡ˫������������(���ֵ����СֵΪ�������)
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


// ��ȡ����Checkbox����(�����Ƿ�ѡ��)
bool CCgiManager::InPutCheckboxSingle(char * String)
{
	if (cgiFormCheckboxSingle(String) == cgiFormSuccess)
		return true;
	else
		return false;
}


// ��ȡһ��Checkbox����(��������ѡ�������ѡ�����򷵻ؿ�)
char * CCgiManager::InPutCheckboxMultiple(char * String)
{
	char **responses;
	if (cgiFormStringMultiple(String, &responses) == cgiFormNotFound) 
		return NULL;
	else
	{
		// ��ֵ
		char** result = responses;

		// �ͷŶ���
		cgiStringArrayFree(responses);

		// ����
		return *result;
	}
}


// ��ȡһ�鵥ѡSelect����(����ѡ�е���)
char* CCgiManager::InPutSelectSingle(char * String, char *texts[], int total, int Default)
{
	int Choice;
	cgiFormSelectSingle(String, texts, total, &Choice, Default);
	
	char* result = texts[Choice];
	return result;
}


// ��ȡһ���ѡSelect����(��û��ѡ���κ����򷵻ؼ٣����򷵻���)
bool CCgiManager::InPutSelectMultiple(char * String, vector<char> &pResult, char *texts[], int total, int Invalid)
{
	int Choices[65535];

	//û��ѡ���κ���
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


// ��ȡһ��Radio����(����ѡ�е���)
char* CCgiManager::InPutRadio(char * String, char *Texts[], int Total, int Default)
{
	int Choice;
	cgiFormRadio(String, Texts, Total, &Choice, Default);
	char* result = Texts[Choice];

	return result;
}


// ��ȡSubmit����(�ύ�ɹ���������򷵻ؼ�)
bool CCgiManager::SubmitClicked(char * String)
{
	if ( cgiFormSubmitClicked(String) == cgiFormSuccess )
		return true;
	else
		return false;
}


// Ϊվ������Cookie����
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


// ��ȡվ��Cookie����
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


//��ȡ����Cookie����
char* CCgiManager::GetCookies(bool OutPut)
{
	char **arrays, **arrayStep;
	if (cgiCookies(&arrays) != cgiFormSuccess) 
	{
		return NULL;
	}
	
	// ��ֵ
	arrayStep = arrays;
	if(OutPut)
	{
		while (*arrayStep) 
		{
			HtmlEscape(*arrayStep);
		    arrayStep++;
		}
	}

	// �ͷŶ���
	cgiStringArrayFree(arrays);

	return *arrayStep;
}


//��ȡ���б�����(Name)
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


// �������ݴ����ڴ�����(ʵ�ֶ�ȡsession����) 
bool CCgiManager::LoadEnvironment(char * FileName)
{
	if (cgiReadEnvironment(FileName) != cgiEnvironmentSuccess) 
	{
		return false;
	}
	else
		return true;
}


// �Ӵ������ȡ�����ݣ�ʵ�ַ���session���ݣ�
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


// ��ȡ�ļ�����
bool CCgiManager::InPutFile(char* String, char* &FileName, int &FileSize, char* &contentType)
{
	// �õ��ļ���
	if (cgiFormFileName(String, FileName, sizeof(FileName)) != cgiFormSuccess)
	{
		// û�н��ܵ��ļ�����
		return false;
	}

	// �õ��ļ���С
	cgiFormFileSize(String, &FileSize);

	// �õ��ļ�����
	cgiFormFileContentType(String, contentType, sizeof(contentType));

	// ����
	return true;
}


// ��ȡ�ļ�����
char* CCgiManager::ReadFileData(char* String, bool OutPut)
{
	cgiFilePtr File;
	char buffer[1024];
	int got;

	// ��Ŀ���ļ�
	if (cgiFormFileOpen(String, &File) != cgiFormSuccess)
		return NULL;

	char* result = "";
	while (cgiFormFileRead(File, buffer, sizeof(buffer), &got) == cgiFormSuccess)
	{
		// ��ֵ
		result = result + (char)buffer;
		if(OutPut)
			cgiHtmlEscapeData(buffer, got);
	}

	// �ر�Ŀ���ļ�
	cgiFormFileClose(File);

	// ����
	return result;
}


// �����ļ�����
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
		// д������
		// ��Ŀ���ļ�
		if (cgiFormFileOpen(String, &File) != cgiFormSuccess)
			return false;

		while (cgiFormFileRead(File, buffer, sizeof(buffer), &got) == cgiFormSuccess)
		{
			// д������
			fputs(buffer, fp);
		}

		// �ر�Ŀ���ļ�
		cgiFormFileClose(File);
	}

	// �ر��ļ�
	fclose(fp);

	// ����
	return true;
}



// ��ȡ��������������ƣ����δ֪����ָ����ַ�����
char* CCgiManager::GetServerSoftware()
{
	return cgiServerSoftware;
}

// ��ȡ�����������ƣ����δ֪����ָ����ַ�����
char* CCgiManager::GetServerName()
{
	return cgiServerName;
}

// ��ȡ���ؽӿڣ�ͨ��ΪCGI / 1.1�������ƣ����δ֪����ָ����ַ�����
char* CCgiManager::GetGatewayInterface()
{
	return cgiGatewayInterface;
}

// ��ȡʹ�õ�Э�飨ͨ��ΪHTTP / 1.0�������δ֪����ָ����ַ����� 
char* CCgiManager::GetServerProtocol()
{
	return cgiServerProtocol;
}

// ��ȡ���������ڼ���HTTP���ӣ�ͨ��Ϊ80���Ķ˿ںţ���δ֪�Ŀ��ַ����� 
char* CCgiManager::GetServerPort()
{
	return cgiServerPort;
}

// ��ȡ������ʹ�õķ�����ͨ��ΪGET��POST�������δ֪���ⲻӦ�÷���������Ϊ���ַ������� 
char* CCgiManager::GetRequestMethod()
{
	return cgiRequestMethod;
}

// �����Web�������������URL��ʶ�������CGI��������κθ���·����Ϣ����������Ϣ���ݸ�����cgiPathInfoָ����������·����Ϣ�� 
char* CCgiManager::GetPathInfo()
{
	return cgiPathInfo;
}

// �����Web�������������URL��ʶ�������CGI��������κθ���·����Ϣ����������Ϣ���ݸ�����cgiPathTranslatedָ��˸���·����Ϣ���ɷ�����ת��Ϊ���ط������ϵ��ļ�ϵͳ·���� 
char* CCgiManager::GetPathTranslated()
{
	return cgiPathTranslated;
}

// ��ȡ���ó�������ơ� 
char* CCgiManager::GetScriptName()
{
	return cgiScriptName;
}

// ��ȡ������GET��������<ISINDEX>��ǩ�����û��ύ���κβ�ѯ��Ϣ����ע�⣬����ʹ��<ISINDEX>��ǣ�������Ҫֱ�ӽ�������Ϣ; ͨ�����Զ�������ʹ��cgiForm����ϵ�м�����������ֶ���ص�ֵ��
char* CCgiManager::GetQueryString()
{
	return cgiQueryString;
}

// ��ȡ���������ȫ�������������������֪������ַ��������δ֪���� 
char* CCgiManager::GetRemoteHost()
{
	return cgiRemoteHost;
}

// ��ȡ������ĵ��ʮ����IP��ַ�������֪������ַ��������δ֪���� 
char* CCgiManager::GetRemoteAddr()
{
	return cgiRemoteAddr;
}

// ��ȡ�����������Ȩ���ͣ�����еĻ��������û�л�δ֪����ָ����ַ����� 
char* CCgiManager::GetAuthType()
{
	return cgiAuthType;
}

// ��ȡ�û��Ѿ���֤���û���; ���û�з��������֤����Ϊ���ַ�������Щ��Ϣ��ȷ����ȡ����ʹ����Ȩ������; 
char* CCgiManager::GetRemoteUser()
{
	return cgiRemoteUser;
}

// ���û�ͨ���û�ʶ��Э����Ըָ���û���; һ�����ַ��������δ֪������Ϣ����ȫ���������û���װ�ڲ���ȫ��ϵͳ����Windows�����ϡ� 
char* CCgiManager::GetRemoteIdent()
{
	return cgiRemoteIdent;
}

// ��ȡ�û��ύ����Ϣ��MIME�������ͣ�����У�; ���û���ύ��Ϣ����Ϊ���ַ�����������ַ������� application/x-www-form-urlencoded�� multipart/form-data�����Զ�����ύ�ı����ݡ�������ַ��������κ������ǿ�ֵ������ύ��ͬ���͵����ݡ����Ƿǳ������ģ���Ϊ����������ֻ��ֱ���ύ�����ļ��ϴ��� 
char* CCgiManager::GetContentType()
{
	return cgiContentType;
}

// ��ȡWeb������ύ��ԭʼCookie������������ô洢�����ݡ�Ӧ��ʹ�ú���GetCookies�� GetCookieString�� GetCookieInteger��������ֱ�Ӽ������ַ����� 
char* CCgiManager::GetCookie()
{
	return cgiCookie;
}

// ��ȡ��������Խ��ܵ�MIME�������͵Ŀո�ָ��б������ cgiHeaderContentType����������ַ��������ҵ��ǣ��������ǰ���������������һ�����õ���ʽ�ṩ���������
char* CCgiManager::GetAccept()
{
	return cgiAccept;
}

// ��ȡ����ʹ�õ�����������ƣ��������Ϣ�����ã���Ϊ���ַ����� 
char* CCgiManager::GetUserAgent()
{
	return cgiUserAgent;
}

// ��ȡ�û����ʵ���һҳ��URL����ͨ���ǽ��û��������ĳ���ı���URL����ע�⣬�������Ϣ��ȫȡ���������������ѡ�������������ǣ��ñ���ͨ����׼ȷ�ġ�
char* CCgiManager::GetReferrer()
{
	return cgiReferrer;
}

// ��ȡ�յ��ı����ѯ���ݵ��ֽ�������ע�⣬����ύ���ύ�����ѯ���⽫ֱ�Ӵ�cgiIn��/��cgiQueryString��ȡ�ͽ���������Ϣ������������£�����Ա��Ӧ����������
int CCgiManager::GetContentLength()
{
	return cgiContentLength;
}

/************************************************************************************************************************************************************************************************/


/* ��������***********************************************************************************************************************************************************************************
 *  ��;������ģ������ͼ�Ľ���
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


// �õ� HTML ����
char* CController::HtmlRead(char* FileName)
{
	// ��ȡ�ļ�
	FILE *fp;
	fopen_s(&fp, FileName, "r");

	// �������
	char str[1024], buff[65535];
	if ( fp == NULL )
	{
		printf("open file error\n" );
		return "";
	}

	// ѭ����ȡHTML
	while( fgets(str, sizeof(str), fp ) )
	{
		// ������buff��
		strcat(buff, str);
	}

	// �ر��ļ�
	fclose(fp);

	// ��������
	char * result = buff;
	return result;
}


// ��Ⱦ�����HTML
void CController::HtmlView(char* FileName, ...)
{
	//�õ���ε���ʼ��ַ
	va_list  pArgList;
	va_start(pArgList, FileName);

	// �õ� HTML ����
	char s[65535], *source = HtmlRead(FileName);
	
	// ��ʽ�������
	vsnprintf(s, 65535, source, pArgList);
	OutPut(s);

	//��β
	va_end(pArgList);
}


// Json����,����json����
char* CController::Json_Encode()
{

}


// Json ����
void CController::Json_Decode()
{

}


// Xml����,����xml����
char* CController::Xml_Encode()
{

}


// Xml ����
void CController::Xml_Decode()
{
}


/***************************************************************************************************************************************************************************************************/


/* ģ����***********************************************************************************************************************************************************************************
 *  ��;�����и������ݽ���
 */
CModel::CModel(): CSqliteManager(true)
{
	// �Զ��������ݿ�
	DatabaseConfig();
}


CModel::~CModel()
{

}


// �������ݿ�
void CModel::DatabaseConfig(char* ConfigFile)
{
	// ��ȡ�����ļ�
    CConfig configSettings(ConfigFile);

	// ������������
	int    hostport = configSettings.Read("hostport", 0);
	string type     = configSettings.Read("type",     type);
    string hostname = configSettings.Read("hostname", hostname);
	string database = configSettings.Read("database", database);
    string username = configSettings.Read("username", username);
    string password = configSettings.Read("password", password);
	string charset  = configSettings.Read("charset",  charset);

	// �������ݿ����ͽ�������(�����ִ�Сд)
	if(_stricmp((char*)type.c_str(), "sqlite") == 0)
	{
		// ��ֵ
		Type = 1;

		// �õ����ݿ�·��
		CString Path(hostname.c_str()), DataBase_Name, DataBase_Path;

		// �ж�·������(����ȫ·��)
		if(Path.Replace(_T("\\"), _T("\\")))
		{
			DataBase_Name = Path.Right(Path.GetLength() - Path.ReverseFind('\\') -1);
			DataBase_Path = Path.Left(Path.GetLength()  - DataBase_Name.GetLength());
		}

		// �ж�·������(���ȫ·��)
		if(Path.ReverseFind('/'))
		{
			DataBase_Name = Path.Right(Path.GetLength() - Path.ReverseFind('/') -1);
			DataBase_Path = Path.Left(Path.GetLength()  - DataBase_Name.GetLength());
		}

		// �������Ŀ�����ݿ�
		if (m_Sql.CheckDataBase( DataBase_Name, DataBase_Path, true ))
		{
			// �������ݿ�
			if(!m_Sql.OpenDataBase( DataBase_Name, DataBase_Path, true ))
				return;
		}
		else
		{
			// �������ݿ�
			if(!m_Sql.CreateDataBase( DataBase_Name, DataBase_Path, true ))
				return;

			// �������ݿ�
			if(!m_Sql.OpenDataBase(  DataBase_Name, DataBase_Path, true ))
				return;
		}
	}
	else if(_stricmp((char*)type.c_str(), "mysql") == 0)
	{
		// ��ֵ
		Type = 2;

		// ����MySQL���ݿ�
		std::cout<<"mysql:"<<std::endl;
	}
	else if(_stricmp((char*)type.c_str(), "sqlserver") == 0)
	{
		// ��ֵ
		Type = 3;

		// ����SqlServer���ݿ�
		std::cout<<"SqlServer:"<<std::endl;
	}


	// �������
    /*std::cout<<"port:"<<hostport<<std::endl;
	std::cout<<"type:"<<type<<std::endl;
    std::cout<<"hostname:"<<hostname<<std::endl;
	std::cout<<"database:"<<database<<std::endl;
    std::cout<<"username:"<<username<<std::endl;
    std::cout<<"password:"<<password<<std::endl;
	std::cout<<"charset:"<<charset<<std::endl;*/


}


// д������
BOOL CModel::Save(char* TableName, char* Params)
{
	switch(Type)
	{
	case 1: // Sqlite
		{
			// ת��ΪCString
			CString Table(TableName), Param(Params);

			// ���Ŀ���
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("���ݱ�%s������!", TableName);
			else
				// д������
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

	// Ĭ�Ϸ���ʧ��
	return FALSE;
}


// ɾ������
BOOL CModel::Delete(char* TableName, char* Params)
{
	switch(Type)
	{
	case 1: // Sqlite
		{
			// ת��ΪCString
			CString Table(TableName), Param(Params);

			// ���Ŀ���
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("���ݱ�%s������!", TableName);
			else
				// ɾ������
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

	// Ĭ�Ϸ���ʧ��
	return FALSE;
}


// �������� (����)
BOOL CModel::UpDate(char* TableName, char* Columns, char* NewData, char* Params)
{
	switch(Type)
	{
	case 1: // Sqlite
		{
			// ת��ΪCString
			CString Table(TableName), Column(Columns), Data(NewData), Param(Params);

			// ���Ŀ���
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("���ݱ�%s������!", TableName);
			else
				// ��������
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

	// Ĭ�Ϸ���ʧ��
	return FALSE;
}


// �������� (����)
BOOL CModel::UpDate(char* TableName, char* Columns, char* Params)
{
	switch(Type)
	{
	case 1: // Sqlite
		{
			// ת��ΪCString
			CString Table(TableName), Column(Columns), Param(Params);

			// ���Ŀ���
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("���ݱ�%s������!", TableName);
			else
				// ��������
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

	// Ĭ�Ϸ���ʧ��
	return FALSE;
}


// ��ȡ���� (����)
char* CModel::Get(char* TableName, int Col, char* Params, char* Order, char* Limit, int SortMode, BOOL DISTINCT, char* COUNT, char* COLUMN, char* GROUP,  char* HAVING)
{
	// ���巵��ֵ
	char* result = "";

	switch(Type)
	{
	case 1: // Sqlite
		{
			// ת��ΪCString
			CString Table(TableName), Result, Param(Params), pOrder(Order), pLimit(Limit), pCOUNT(COUNT), pCOLUMN(COLUMN), pGROUP(GROUP), pHAVING(HAVING);

			// ���Ŀ���
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("���ݱ�%s������!", TableName);
			else
			{
				// ��ȡ����
				if( m_Sql.SelectData(Table, Result, Col, Param, true, pOrder, pLimit, SortMode, DISTINCT, pCOUNT, pCOLUMN, pGROUP, pHAVING) )
				{
					//�����￪ʼ����ת��������һ���궨��
					USES_CONVERSION;

					//����ת��
					char* pResult = T2A(Result.GetBuffer(0));
					Result.ReleaseBuffer();

					// Ϊ�����ֵ
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

	// ���ؽ��
	return result;
}


// ��ȡ���� (����)
vector<char*> CModel::GetAll(char* TableName, int Col, char* Params, char* Order, char* Limit, int SortMode, BOOL DISTINCT, char* COUNT, char* COLUMN, char* GROUP,  char* HAVING)
{
	// �������
	vector<char*>   Result;
	vector<CString> Temp;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// ת��ΪCString
			CString Table(TableName), Param(Params), pOrder(Order), pLimit(Limit), pCOUNT(COUNT), pCOLUMN(COLUMN), pGROUP(GROUP), pHAVING(HAVING);

			// ���Ŀ���
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("���ݱ�%s������!", TableName);
			else
			{
				// ��ȡ����
				if( m_Sql.SelectData(Table, Temp, Param, true, pOrder, pLimit, SortMode, DISTINCT, pCOUNT, pCOLUMN, pGROUP, pHAVING) )
				{
					for(int i = 0; i < (int)Temp.size(); i += Col)
					{
						//�����￪ʼ����ת��������һ���궨��
						USES_CONVERSION;

						//����ת��
						for(int j = 0; j < Col; j++)
						{
							char* temp = T2A(Temp.at(i + j).GetBuffer(0));
							Temp.at(i + j).ReleaseBuffer();

							// �Ž�Ŀ��������
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

	// ���ز�ѯ���
	return Result;
}


// ͳ������
int CModel::Count(char* TableName, char* Params)
{
	switch(Type)
	{
	case 1: // Sqlite
		{
			// ת��ΪCString
			CString Table(TableName), Param(Params);
			
			// ���Ŀ���
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("���ݱ�%s������!", TableName);
			else
			{
				// ���巵�ض���
				int Count;

				// ͳ������
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

	// Ĭ�Ϸ���ʧ��
	return FALSE;
}


// �������
int CModel::Sum(char* TableName, char* Column, char* Params)
{
	// ���巵�ض���
	int Data = 0;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// ת��ΪCString
			CString Table(TableName), pColumn(Column), Param(Params);

			// ���Ŀ���
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("���ݱ�%s������!", TableName);
			else
			{
				// �������
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

	// Ĭ�Ϸ���ʧ��
	return -1;
}


// �������
int CModel::Product(char* TableName, char* Column, char* Params)
{
	// ���巵�ض���
	int Data = 1;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// ת��ΪCString
			CString Table(TableName), pColumn(Column), Param(Params);

			// ���Ŀ���
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("���ݱ�%s������!", TableName);
			else
			{
				// �������
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

	// Ĭ�Ϸ���ʧ��
	return -1;
}


// ��ƽ����
int CModel::Avg(char* TableName, char* Column, char* Params)
{
	// ���巵�ض���
	int Data = 0;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// ת��ΪCString
			CString Table(TableName), pColumn(Column), Param(Params);

			// ���Ŀ���
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("���ݱ�%s������!", TableName);
			else
			{
				// ͳ������
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

	// Ĭ�Ϸ���ʧ��
	return -1;
}


// ����λ��
int CModel::Mid(char* TableName, char* Column, char* Params)
{
	// ���巵�ض���
	int Data = 0;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// ת��ΪCString
			CString Table(TableName), pColumn(Column), Param(Params);

			// ���Ŀ���
			if(!m_Sql.CheckDataTable(Table, true))
				CController::OutPut("���ݱ�%s������!", TableName);
			else
			{
				// ͳ������
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

	// Ĭ�Ϸ���ʧ��
	return -1;
}

/***************************************************************************************************************************************************************************************************/

