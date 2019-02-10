#include "CgiManager.h"

// 静态成员初始化
bool   CCgiManager::debug              = false;
string CCgiManager::error_message      = "页面错误！请稍后再试～";
string CCgiManager::log_path           = "log";
string CCgiManager::charset            = "";
string CCgiManager::return_type        = "text/html";
string CCgiManager::ajax_return        = "json";
string CCgiManager::controller_prefix  = "C";
string CCgiManager::controller_suffix  = "";
string CCgiManager::default_controller = "CIndex";
string CCgiManager::default_action     = "index";
string CCgiManager::action_suffix      = "";
string CCgiManager::pathinfo_depr      = "/";
string CCgiManager::html_suffix        = "html";
bool   CCgiManager::url_route          = true;
int    CCgiManager::cookie_expire      = 86400;
string CCgiManager::cookie_path        = "/";
string CCgiManager::cookie_domain      = "localhost";

bool CCgiManager::Initialized  = false;
bool CCgiManager::IsSetHead    = false;

string CCgiManager::Controller = "";
string CCgiManager::Function   = "";

vector<string> CCgiManager::routes;
map <string, map<string, PtrFun> > CCgiManager::FunTab;

map <string, int>    CController::IntegerMap;
map <string, string> CController::StringMap;
map <string, double> CController::DoubleMap;
map <string, vector<string> > CController::ListMap;

int  CModel::Type       = 0;
bool CModel::Initialize = false;

/* 配置文件类***********************************************************************************************************************************************************************************
 * 用途：读写配置文件
 */

CConfig::CConfig( string filename, string delimiter, string comment )  
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
  
void CConfig::ReadFile( string filename, string delimiter, string comment )  
{  
    m_Delimiter = delimiter;  
    m_Comment = comment;  
    std::ifstream in( filename.c_str() );  
  
    if( !in ) throw File_not_found( filename );   
  
    in >> (*this);  
}  


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
    std::string v   = T_as_string( value );
    std::string key = in_key;

	#ifdef _WIN32
    trim(key);
    trim(v);
	#endif

	#ifdef __linux
	linux_trim((char*)key.c_str());
	linux_trim((char*)v.c_str());
    #endif

    m_Contents[key] = v;  
    return;
}


/************************************************************************************************************************************************************************************************/


/* CGI父类***********************************************************************************************************************************************************************************
 * 用途：各种最基础的CGI操作
 */

CCgiManager::CCgiManager()
{
	if(!Initialized)
	{
		// CGI 配置
		Config();

		// 路由配置
		Route();

		// 解析Url
		ParsingUrl();

		// 初始化完成
		Initialized = true;
	}
}


CCgiManager::~CCgiManager()
{
}


///////////////////////////////////字符串操作代码///////////////////////////////////


/*从字符串的左边截取n个字符*/  
char* CCgiManager::Left(char* dst, char* src, int n)  
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
char* CCgiManager::Mid(char* dst, char* src, int n, int m) /*n为长度，m为位置*/  
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
char* CCgiManager::Right(char* dst, char* src, int n)
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
void CCgiManager::Split(const std::string& s, std::vector<std::string>& v, const std::string& c)
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


// 字符串替换 s1里替换s2与s3
char* CCgiManager::Replace(char* s1, char* s2, char* s3)
{
    char *p,*from,*to,*begin=s1;
    int c1,c2,c3,c;         //串长度及计数
    c2=strlen(s2);
    c3=(s3!=NULL)?strlen(s3):0;
    if(c2==0)return s1;     //注意要退出
    while(true)             //替换所有出现的串
    {
        c1=strlen(begin);
        p=strstr(begin,s2); //出现位置
        if(p==NULL)         //没找到
            return s1;
        if(c2>c3)           //串往前移
        {
            from=p+c2;
            to=p+c3;
            c=c1-c2+begin-p+1;
            while(c--)
                *to++=*from++;
        }
        else if(c2<c3)      //串往后移
        {
            from=begin+c1;
            to=from-c2+c3;
            c=from-p-c2+1;
            while(c--)
                *to--=*from--;
        }
        if(c3)              //完成替换
        {
            from=s3,to=p,c=c3;
            while(c--)
                *to++=*from++;
        }
        begin=p+c3;         //新的查找位置
    }
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


// 转大写
char* CCgiManager::strupr(char *str)
{
	#ifdef _WIN32
    char *orign=str;
    for (; *str!='\0'; str++)
        *str = toupper(*str);
    return orign;
	#else
	string target(str);
	transform(target.begin(), target.end(), target.begin(), (int (*)(int))toupper);
	return (char*)target.c_str();
	 #endif
}


// 转小写
char* CCgiManager::strlowr(char *str)
{
    /*char *orign=str;
    for (; *str!='\0'; str++)
        *str = tolower(*str);
    return orign;*/

	#ifdef _WIN32
	for (unsigned int i = 0; i < strlen(str); i++)
    {
        str[i] = tolower(str[i]);
    }
	return str;
    #else
	string target(str);
	transform(target.begin(), target.end(), target.begin(), (int (*)(int))tolower);
	return (char*)target.c_str();
    #endif
}


void string_replace( std::string &strBig, const std::string &strsrc, const std::string &strdst)
{
    std::string::size_type pos = 0;
    std::string::size_type srclen = strsrc.size();
    std::string::size_type dstlen = strdst.size();

    while( (pos=strBig.find(strsrc, pos)) != std::string::npos )
    {
        strBig.replace( pos, srclen, strdst );
        pos += dstlen;
    }
} 


///////////////////////////////////编码类型转换代码///////////////////////////////////


void CCgiManager::Gb2312ToUnicode(wchar_t* pOut,char *gbBuffer)
{
    //::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);
	mbstowcs(pOut, gbBuffer, strlen(gbBuffer)*2);
    return;
}


void CCgiManager::UTF_8ToUnicode(wchar_t* pOut,char *pText)
{
    char* uchar = (char *)pOut;
    
    uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
    uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);
 
    return;
}


void CCgiManager::UnicodeToUTF_8(char* pOut,wchar_t* pText)
{
    // 注意 WCHAR高低字的顺序,低字节在前，高字节在后
    char* pchar = (char *)pText;
 
    pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
    pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
    pOut[2] = (0x80 | (pchar[0] & 0x3F));
 
    return;
}


void CCgiManager::UnicodeToGB2312(char* pOut,wchar_t* uData)
{
    //WideCharToMultiByte(CP_ACP,NULL,&uData,1,pOut,sizeof(WCHAR),NULL,NULL);
	wcstombs(pOut, uData, wcslen(uData));
    return;
}


//做为解Url使用
char CCgiManager::CharToInt(char ch){
        if(ch>='0' && ch<='9')return (char)(ch-'0');
        if(ch>='a' && ch<='f')return (char)(ch-'a'+10);
        if(ch>='A' && ch<='F')return (char)(ch-'A'+10);
        return -1;
}


char CCgiManager::StrToBin(char *str){
        char tempWord[2];
        char chn;
 
        tempWord[0] = CharToInt(str[0]);                         //make the B to 11 -- 00001011
        tempWord[1] = CharToInt(str[1]);                         //make the 0 to 0 -- 00000000
 
        chn = (tempWord[0] << 4) | tempWord[1];                //to change the BO to 10110000
 
        return chn;
}
 

//UTF_8 转gb2312
void CCgiManager::UTF_8ToGB2312(string &pOut, char *pText, int pLen)
{
     char buf[4];
     char* rst = new char[pLen + (pLen >> 2) + 2];
    memset(buf,0,4);
    memset(rst,0,pLen + (pLen >> 2) + 2);
 
    int i =0;
    int j = 0;
     
    while(i < pLen)
    {
        if(*(pText + i) >= 0)
        {
           
            rst[j++] = pText[i++];
        }
        else                
        {
            wchar_t Wtemp;
           
            UTF_8ToUnicode(&Wtemp,pText + i);
             
            UnicodeToGB2312(buf,&Wtemp);
           
            unsigned short int tmp = 0;
            tmp = rst[j] = buf[0];
            tmp = rst[j+1] = buf[1];
            tmp = rst[j+2] = buf[2];
 
            //newBuf[j] = Ctemp[0];
            //newBuf[j + 1] = Ctemp[1];
 
            i += 3;   
            j += 2;  
        }
       
}
    rst[j]='\0';
   pOut = rst;
    delete []rst;
}
 

//GB2312 转为 UTF-8
void CCgiManager::GB2312ToUTF_8(string& pOut,char *pText, int pLen)
{
    char buf[4];
    memset(buf,0,4);
 
    pOut.clear();
 
    int i = 0;
    while(i < pLen)
    {
        //如果是英文直接复制就可以
        if( pText[i] >= 0)
        {
            char asciistr[2]={0};
            asciistr[0] = (pText[i++]);
            pOut.append(asciistr);
        }
        else
        {
            wchar_t pbuffer;
            Gb2312ToUnicode(&pbuffer,pText+i);
 
            UnicodeToUTF_8(buf,&pbuffer);
 
            pOut.append(buf);
 
            i += 2;
        }
    }
 
    return;
}


//把str编码为网页中的 GB2312 url encode ,英文不变，汉字双字节 如%3D%AE%88
string CCgiManager::UrlGB2312(char * str)
{
    string dd;
    size_t len = strlen(str);
    for (size_t i=0;i<len;i++)
    {
        if(isalnum((BYTE)str[i]))
        {
            char tempbuff[2];
            sprintf(tempbuff,"%c",str[i]);
            dd.append(tempbuff);
        }
        else if (isspace((BYTE)str[i]))
        {
            dd.append("+");
        }
        else
        {
            char tempbuff[4];
            sprintf(tempbuff,"%%%X%X",((BYTE*)str)[i] >>4,((BYTE*)str)[i] %16);
            dd.append(tempbuff);
        }
 
    }
    return dd;
}
 

//把str编码为网页中的 UTF-8 url encode ,英文不变，汉字三字节 如%3D%AE%88
 

string CCgiManager::UrlUTF8(char * str)
{
    string tt;
    string dd;
    GB2312ToUTF_8(tt,str,(int)strlen(str));
 
    size_t len=tt.length();
    for (size_t i=0;i<len;i++)
    {
        if(isalnum((BYTE)tt.at(i)))
        {
            char tempbuff[2]={0};
            sprintf(tempbuff,"%c",(BYTE)tt.at(i));
            dd.append(tempbuff);
        }
        else if (isspace((BYTE)tt.at(i)))
        {
            dd.append("+");
        }
        else
        {
            char tempbuff[4];
            sprintf(tempbuff,"%%%X%X",((BYTE)tt.at(i)) >>4,((BYTE)tt.at(i)) %16);
            dd.append(tempbuff);
        }
 
    }
    return dd;
}


//把url GB2312解码
string CCgiManager::UrlGB2312Decode(string str)
{
   string output="";
        char tmp[2];
        int i=0,idx=0,len=str.length();
       
        while(i<len){
                if(str[i]=='%'){
                        tmp[0]=str[i+1];
                        tmp[1]=str[i+2];
                        output += StrToBin(tmp);
                        i=i+3;
                }
                else if(str[i]=='+'){
                        output+=' ';
                        i++;
                }
                else{
                        output+=str[i];
                        i++;
                }
        }
       
        return output;
}


//把url utf8解码
string CCgiManager::UrlUTF8Decode(string str)
{
     string output="";
 
    string temp =UrlGB2312Decode(str);//
 
    UTF_8ToGB2312(output,(char *)temp.data(),strlen(temp.data()));
 
    return output;
 
}


//// ACSII   编码转 Unicode 编码  
//wstring CCgiManager::AcsiiToUnicode(const string  &acsii_string)
//{
//	wstring unicode_string;
//
//	//CP_ACP - default to ANSI code page  
//	int len = MultiByteToWideChar(CP_ACP, 0, acsii_string.c_str(), -1, NULL, 0);
//	if (ERROR_NO_UNICODE_TRANSLATION == len || 0 == len)
//	{
//		//return empty wstring
//		return unicode_string;
//	}
//
//	vector<wchar_t> vec_result(len);
//	int result_len = MultiByteToWideChar(CP_ACP, 0, acsii_string.c_str(), -1, &vec_result[0], len);
//	if (result_len != len)
//	{
//		//return empty wstring  
//		return unicode_string;
//	}
//
//	unicode_string = wstring(&vec_result[0]);
//	return unicode_string;
//}
//
//
//// ACSII   编码转 UTF8    编码
//string  CCgiManager::AcsiiToUtf8(   const string  &acsii_string)
//{
//	wstring unicode_string = AcsiiToUnicode(acsii_string);  //将 ACSII   编码转换为 Unicode  编码
//	string  utf8_string = UnicodeToUtf8(unicode_string);    //将 Unicode 编码转换为 UTF8     编码
//
//	// 返回编码转换结果
//	return  utf8_string;
//}
//
//
//// Unicode 编码转 ACSII   编码
//string  CCgiManager::UnicodeToAcsii(const wstring &unicode_string)
//{
//	string acsii_string;
//
//	//CP_OEMCP - default to OEM  code page  
//	int len = WideCharToMultiByte(CP_OEMCP, 0, unicode_string.c_str(), -1, NULL, 0, NULL, NULL);
//	if (ERROR_NO_UNICODE_TRANSLATION == len || 0 == len)
//	{
//		//return empty wstring  
//		return acsii_string;
//	}
//
//	vector<char> vec_result(len);
//	int result_len = WideCharToMultiByte(CP_OEMCP, 0, unicode_string.c_str(), -1, &vec_result[0], len, NULL, NULL);;
//	if (result_len != len)
//	{
//		//return empty wstring  
//		return acsii_string;
//	}
//
//	acsii_string = string(&vec_result[0]);
//	return acsii_string;
//}
//
//
//// Unicode 编码转 UTF8    编码
//string  CCgiManager::UnicodeToUtf8( const wstring &unicode_string)
//{
//	string utf8_string;
//
//	//CP_UTF8 - UTF-8 translation  
//	int len = WideCharToMultiByte(CP_UTF8, 0, unicode_string.c_str(), -1, NULL, 0, NULL, NULL);
//	if (0 == len)
//	{
//		//return empty wstring  
//		return utf8_string;
//	}
//
//	vector<char> vec_result(len);
//	int result_len = WideCharToMultiByte(CP_UTF8, 0, unicode_string.c_str(), -1, &vec_result[0], len, NULL, NULL);;
//	if (result_len != len)
//	{
//		//return empty wstring  
//		return utf8_string;
//	}
//
//	utf8_string = string(&vec_result[0]);
//	return utf8_string;
//}
//
//
//// UTF8    编码转 ACSII   编码
//string  CCgiManager::Utf8ToAcsii(   const string  &utf8_string)
//{
//	wstring unicode_string = Utf8ToUnicode(utf8_string);        //将UTF8转换为Unicode  
//	string acsii_string = UnicodeToAcsii(unicode_string);   //将Unicode转换为ACSII  
//	return acsii_string;
//}
//
//
//// UTF8    编码转 Unicode 编码
//wstring CCgiManager::Utf8ToUnicode( const string  &utf8_string)
//{
//	wstring unicode_string;
//
//	//CP_UTF8 - UTF-8 translation  
//	int len = MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, NULL, 0);
//	if (ERROR_NO_UNICODE_TRANSLATION == len || 0 == len)
//	{
//		//return empty wstring  
//		return unicode_string;
//	}
//
//	vector<wchar_t> vec_result(len);
//	int result_len = MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, &vec_result[0], len);
//	if (result_len != len)
//	{
//		//return empty wstring  
//		return unicode_string;
//	}
//
//	unicode_string = wstring(&vec_result[0]);
//	return unicode_string;
//}


///////////////////////////////////类成员方法代码///////////////////////////////////


// 记录错误信息
void CCgiManager::Record(string Content)
{
	time_t t;
    struct tm * lt;
    time (&t);//获取Unix时间戳。
    lt = localtime (&t);//转为时间结构。

	// 得到文件名
	char name[1024] = {0}, log[1024] = {0};
	sprintf(name, "%s/%d-%d-%d.log", log_path.c_str(), lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);//输出结果

	// 记录的数据
	sprintf(log, "%d年%d月%d日 %d时%d分%d秒: %s\r\n", lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec, Content.c_str());

	#ifdef _WIN32
	// 检查路径
	if (!log_path.empty() && log_path != "" && 0 != _access(log_path.c_str(), 0))
	{
		// 目标链表
		vector<string> v;

		// 分割处理
		Split(log_path, v, "/");

		// 创建目录
		string dir;
		for(int i = 0; i < (int)v.size(); i++)
		{
			dir += v.at(i) + "/";

			// 目录不存在，创建目录
			_mkdir(dir.c_str());
		}
	}
    #else
	if (!log_path.empty() && log_path != "" && 0 != access(log_path.c_str(), 0))
	{
		// 目标链表
		vector<string> v;

		// 分割处理
		Split(log_path, v, "/");

		// 创建目录
		string dir;
		for(int i = 0; i < (int)v.size(); i++)
		{
			dir += v.at(i) + "/";

			// 目录不存在，创建目录
			mkdir(log_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		}
	}
    #endif;

	// 创建并记录
	FILE *fp = fopen(name, "at");
	fprintf(fp, log);

	// 关闭文件
	fclose(fp);
}


// 配置CGI
void CCgiManager::Config(string ConfigFile)
{
	// 检查配置文件
	if(!CConfig::FileExist(ConfigFile))
	{
		// 记录
		Record("application/config.txt 不存在, 将使用默认配置!");
	}
	else
	{
		// 读取配置文件
		CConfig configSettings(ConfigFile);

		// 解析配置数据
		debug              = configSettings.Read("debug",              debug);
		error_message      = configSettings.Read("error_message",      error_message);
		log_path           = configSettings.Read("log_path",           log_path);
		charset            = configSettings.Read("charset",            charset);
		return_type        = configSettings.Read("return_type",        return_type);
		ajax_return        = configSettings.Read("ajax_return",        ajax_return);
		controller_prefix  = configSettings.Read("controller_prefix",  controller_prefix);
		controller_suffix = configSettings.Read("controller_suffix",   controller_suffix);
		default_controller = configSettings.Read("default_controller", default_controller);
		default_action     = configSettings.Read("default_action",     default_action);
		action_suffix      = configSettings.Read("action_suffix",      action_suffix);
		pathinfo_depr      = configSettings.Read("pathinfo_depr",      pathinfo_depr);
		html_suffix        = configSettings.Read("html_suffix",        html_suffix);
		url_route          = configSettings.Read("url_route",          url_route);
		cookie_expire      = configSettings.Read("cookie_expire",      86400);
		cookie_path        = configSettings.Read("cookie_path",        cookie_path);
		cookie_domain      = configSettings.Read("cookie_domain",      cookie_domain);
	}
}


// 配置路由
void CCgiManager::Route(string ConfigFile)
{
	// 如果开启了路由
	if(url_route)
	{
		if(CConfig::FileExist(ConfigFile))
		{
			// 读取配置文件
			CConfig configSettings(ConfigFile);

			// 解析配置数据
			string route = configSettings.Read("route", route);

			// 去掉回车空格
			Replace((char*)route.c_str(), "\r", "");
			Replace((char*)route.c_str(), "\n", "");

			// 分割数据
			Split(route, routes, ":");
		}
		else
		{
			// 记录
			Record("application/route.txt 不存在, 伪静态将禁用!");
			url_route = false;
		}
	}

	// 遍历数据
	/*for(vector<string>::size_type i = 0; i != routes.size(); i++)
	{
		OutPut("<br>");
		OutPut(routes[i]);
		OutPut("<br>");
	}*/
}


// 解析Url
void CCgiManager::ParsingUrl()
{
	// 定义变量
	char* Method = cgiPathInfo;

	// 目标链表
	vector<string> v;

	// 伪静态判断
	string target = "." + html_suffix;
	if(strstr(Method, (char*)target.c_str()) != NULL)
	{
		if(url_route)
		{
			// 处理字符串
			Replace(Method, (char*)pathinfo_depr.c_str(), "");
			Replace(Method, (char*)target.c_str(), "");

			vector<string>::iterator it = find(routes.begin(), routes.end(), Method);
			if (it != routes.end())
			{
				int num = it - routes.begin();
				string Target = routes.at(num+1);

				// 分割处理
				Split(Target, v, pathinfo_depr);

				// 解析控制器名称
				Controller = v[0];

				// 解析方法名称
				Function   = v[1];

				// 判断是否为空
				if(Controller.empty() || Function.empty())
					goto empty;
			}
			else
			{
				char message[1024] = {0};
				sprintf(message, "找不到伪静态%s的路由地址, 请在route.txt中配置!", Method);

				// 记录日志
				Record(message);

				// 输出错误消息
				DisplayError(message);
			}
		}
		else
		{
			// 记录日志
			Record("没有开启路由, 伪静态无法使用!");

			// 输出错误消息
			DisplayError("没有开启路由, 伪静态无法使用!");
		}
	}

	// 默认参数(没有参数)
	#ifdef _WIN32
	else if( strlen(Method) == strlen(cgiScriptName) || strlen(Method) == 0 && _stricmp(cgiScriptName, "/index.cgi") == 0 )
	#endif

	#ifdef __linux
	else if( strlen(Method) == strlen(cgiScriptName) || strlen(Method) == 0 && strcasecmp(cgiScriptName, "/index.cgi") == 0 )
	#endif
	{
		empty:

		// 处理控制器名称, 转为小写
		char* controller = strlowr((char*)default_controller.c_str());

		// 判断并去掉前缀
		string temp = controller;
		int pos = temp.find(strlowr((char*)controller_prefix.c_str()));
		if(pos == 0 && pos != -1 && controller_prefix.length() > 0)
			strncpy(controller, controller + controller_prefix.length(), strlen(controller));

		// 判断并去掉后缀
		pos = temp.rfind(strlowr((char*)controller_suffix.c_str()));
		if(pos == 0 && pos != -1 && controller_suffix.length() > 0)
			controller[strlen(controller) - controller_suffix.length()] = '\0';

		// 获得默认控制器
		Controller = controller;

		// 处理方法名称, 转为小写
		char* action = strlowr((char*)default_action.c_str());

		// 判断并去掉后缀
		temp = action;
		pos = temp.rfind(strlowr((char*)action_suffix.c_str()));
		if(pos == 0 && pos != -1 && action_suffix.length() > 0)
			action[strlen(action) - action_suffix.length()] = '\0';

		// 获得默认方法
		Function = action;
	}

	// apache 没有PathInfo
	else if( strlen(cgiPathInfo) == 0 )
	{
		Record("无法获取PathInfo, 请修改apache服务器的httpd.conf设置。");
		DisplayError("无法获取PathInfo, 请修改apache服务器的httpd.conf设置。 <br><br>");
		DisplayError("<Directory /> <br>Options +Indexes +FollowSymLinks +ExecCGI <br>AllowOverride All <br>Order allow,deny <br>Allow from all <br>Require all granted <br>AcceptPathInfo On //加入这行代码就OK了 <br> </Directory>");
		return;
	}

	// 解析Url
	else
	{
		string source(Method);
		string target ="index.cgi";
		if (source.find(target) < source.length())
		{
			// 处理字符串
			Right(Method, cgiPathInfo, strlen(cgiPathInfo) - strlen(cgiScriptName));
			//Method[strlen(cgiPathInfo) - strlen(cgiScriptName)] = 0;
		}
		else
		{
			// apache服务器无需处理字符串
		}

		// 分割处理
		Split(Method, v, pathinfo_depr);

		// 解析控制器名称
		char* controller = strlowr((char*)v[1].c_str());

		// 判断并去掉前缀
		string temp = controller;
		int pos = temp.find(strlowr((char*)controller_prefix.c_str()));
		if(pos == 0 && pos != -1 && controller_prefix.length() > 0)
			strncpy(controller, controller + controller_prefix.length(), strlen(controller));

		// 判断并去掉后缀
		pos = temp.rfind(strlowr((char*)controller_suffix.c_str()));
		if(pos == 0 && pos != -1 && controller_suffix.length() > 0)
			controller[strlen(controller) - controller_suffix.length()] = '\0';

		// 获得控制器名称
		Controller = controller;

		// 解析方法名称
		char* action = strlowr((char*)v[2].c_str());

		// 判断并去掉后缀
		temp = action;
		pos = temp.rfind(strlowr((char*)action_suffix.c_str()));
		if(pos == 0 && pos != -1 && action_suffix.length() > 0)
			action[strlen(action) - action_suffix.length()] = '\0';

		// 得到方法名称
		Function = action;

		// 判断是否为空
		if(Controller.empty() || Function.empty())
			goto empty;
	}
}


// 错误显示
void CCgiManager::DisplayError(string String, ...)
{
	// 设置头
	if(!IsSetHead)
		SetHead("text/html;charset=" + charset);

	if(debug)
	{
		//得到变参的起始地址
		va_list  pArgList;
		va_start(pArgList, String);

		// 取值并输出数据
		char s[65535];

		// 格式化并输出
		vsnprintf(s, 65535, String.c_str(), pArgList);
		vfprintf(cgiOut, s, pArgList);

		//收尾
		va_end(pArgList);
	}
	else
		fprintf(cgiOut, error_message.c_str());
}


// 设置头
void CCgiManager::SetHead(string Type)
{
	cgiHeaderContentType((char*)Type.c_str());
	IsSetHead = true;
}


// 设置Location
void CCgiManager::Location(string Url)
{
	cgiHeaderLocation((char*)Url.c_str());
}


// Url跳转
void CCgiManager::Jump(string Url, bool Visible)
{
	// Url 地址栏会显示改变
	if(Visible)
		OutPut("<script>location.href = '%s'</script>", Url.c_str());

	// Url 地址栏不变(跨域将出现错误)
	else
	{
		// document.body.innerHTML 
		string Target = "<script language=javascript> function createXMLHttpRequest(){if(window.XMLHttpRequest){XMLHttpR = new XMLHttpRequest();}else if(window.ActiveXObject){try{XMLHttpR = new ActiveXObject(\"Msxml2.XMLHTTP\");}catch(e){try{XMLHttpR = new ActiveXObject(\"Microsoft.XMLHTTP\");}catch(e){}}}} function sendRequest(url){createXMLHttpRequest();XMLHttpR.open(\"GET\",url,true);XMLHttpR.setRequestHeader(\"Content-Type\",\"text/html;charset=utf-8\");XMLHttpR.onreadystatechange = processResponse;XMLHttpR.send(null);} function processResponse(){if(XMLHttpR.readyState ==4 && XMLHttpR.status == 200){document.body = XMLHttpR.responseText;document.write(XMLHttpR.responseText);}}";
		Target += "sendRequest(\"%s\");</script>";
		OutPut(Target.c_str(), Url.c_str());
	}
}


// 输出HTTP错误状态代码
void CCgiManager::SetStatus(int Status, string Message)
{
	cgiHeaderStatus(Status, (char*)Message.c_str());
}


// 转码并输出Html
void CCgiManager::HtmlEscape(string Name, bool Newlines)
{
	// 设置默认的头
	if(!IsSetHead)
		SetHead(return_type + ";charset=" + charset);

	cgiHtmlEscape(Name.c_str());
	if(Newlines)
	    fprintf(cgiOut, "\n");
}


void CCgiManager::HtmlEscapeData(string Name, int len, bool Newlines)
{
	cgiHtmlEscapeData(Name.c_str(), len);
	if(Newlines)
	    fprintf(cgiOut, "\n");
}


// 转码并输出Value
void CCgiManager::ValueEscape(string Value, bool Newlines)
{
	// 设置默认的头
	if(!IsSetHead)
		SetHead(return_type + ";charset=" + charset);

	cgiValueEscape(Value.c_str());
	if(Newlines)
	    fprintf(cgiOut, "\n");
}


void CCgiManager::ValueEscapeData(string Value, int len, bool Newlines)
{
	cgiValueEscapeData(Value.c_str(), len);
	if(Newlines)
	    fprintf(cgiOut, "\n");
}


// 输出数据
void CCgiManager::OutPut(string String, ...)
{
	// 设置默认的头
	if(!IsSetHead)
		SetHead(return_type + ";charset=" + charset);

	//得到变参的起始地址
	va_list  pArgList;
	va_start(pArgList, String);

	// 取值并输出数据
	char s[65535];
	
	// 格式化并输出
	vsnprintf(s, 65535, String.c_str(), pArgList);
	vfprintf(cgiOut, s, pArgList);

	//收尾
	va_end(pArgList);
}


// 获得输入字符
string CCgiManager::InPut(string String)
{
	return InPutString(String);
}


int    CCgiManager::InPut(string String, int    Default)
{
	return InPutInteger(String, Default);
}


double CCgiManager::InPut(string String, double Default)
{
	return InPutDouble(String, Default);
}


// 获取字符串数据
string CCgiManager::InPutString(string String, bool OutPut, bool Newlines, int Max)
{
	// 检查目标字段是否存在
	char* target = (char*)String.c_str();
	if (cgiFormSubmitClicked(target) == cgiFormSuccess)
	{
		char name[65535];

		if(Max == 0)
			cgiFormString((char*)String.c_str(), name, sizeof(String) * 2);
		else
			cgiFormString((char*)String.c_str(), name, Max);

		// 赋值
		string result(name);

		// 判断是否需要输出
		if(OutPut)
		{
			// 转码并输出
			HtmlEscape(result);

			if(Newlines)
				fprintf(cgiOut, "\n");
		}

		// 返回
		return result;
	}

	return "";
}


// 获取不带回车换行符的字符串数据
string CCgiManager::InPutStringNoNewlines(string String, bool OutPut, bool Newlines, int Max)
{
	// 检查目标字段是否存在
	if (cgiFormSubmitClicked((char*)String.c_str()) == cgiFormSuccess)
	{
		char name[65535];
		if(Max == 0)
			cgiFormStringNoNewlines((char*)String.c_str(), name, sizeof(String) * 2);
		else
			cgiFormStringNoNewlines((char*)String.c_str(), name, Max);

		// 赋值
		string result(name);

		// 判断是否需要输出
		if(OutPut)
		{
			// 转码并输出
			HtmlEscape(result);

			if(Newlines)
				fprintf(cgiOut, "\n");
		}

		// 返回
		return result;
	}

	return NULL;
}


// 设置字符串的储存空间，需和InPutString或InPutStringNoNewlines配合使用
bool CCgiManager::InPutStringSpaceNeeded(string String, int Length)
{
	if(cgiFormStringSpaceNeeded((char*)String.c_str(), &Length) ==  cgiFormSuccess)
		return true;
	else 
		return false;
}


// 获取短整型数据
int CCgiManager::InPutInteger(string String, int Default, bool OutPut, bool Newlines)
{
	int result;
	cgiFormInteger((char*)String.c_str(), &result, Default);

	if(OutPut)
	{
		fprintf(cgiOut, "%d", result);

		if(Newlines)
			fprintf(cgiOut, "\n");
	}

	return result;
}


// 获取短整型区间数据(最大值与最小值为必须参数)
int CCgiManager::InPutIntegerBound(string String, int Min, int Max, int Default, bool OutPut, bool Newlines)
{
	int result;
	cgiFormIntegerBounded((char*)String.c_str(), &result, Min, Max, Default);

	if(OutPut)
	{
		fprintf(cgiOut, "%d", result);

		if(Newlines)
			fprintf(cgiOut, "\n");
	}

	return result;
}


// 获取双精度数据 (保留2位小数)
double CCgiManager::InPutDouble(string String, double Default, bool OutPut, bool Newlines)
{
	double result;
	cgiFormDouble((char*)String.c_str(), &result, Default);

	if(OutPut)
	{
		fprintf(cgiOut, "%.2lf", result);

		if(Newlines)
			fprintf(cgiOut, "\n");
	}

	return result;
}


// 获取双精度区间数据(最大值与最小值为必须参数)
double CCgiManager::InPutDoubleBound(string String, double Min, double Max, double Default, bool OutPut, bool Newlines)
{
	double result;
	cgiFormDoubleBounded((char*)String.c_str(), &result, Min, Max, Default);

	if(OutPut)
	{
		fprintf(cgiOut, "%.2lf", result);

		if(Newlines)
			fprintf(cgiOut, "\n");
	}

	return result;
}


// 获取单个Checkbox数据(返回是否选中)
bool CCgiManager::InPutCheckboxSingle(string String)
{
	if (cgiFormCheckboxSingle((char*)String.c_str()) == cgiFormSuccess)
		return true;
	else
		return false;
}


// 获取一组Checkbox数据(返回所有选中项，若无选中项则返回空)
string CCgiManager::InPutCheckboxMultiple(string String)
{
	char **responses;
	if (cgiFormStringMultiple((char*)String.c_str(), &responses) == cgiFormNotFound) 
		return NULL;
	else
	{
		// 赋值
		//char** result = responses;
		string result((char*)responses);

		// 释放对象
		cgiStringArrayFree(responses);

		// 返回
		//return *result;
		return result;
	}
}


// 获取一组单选Select数据(返回选中的项)
string CCgiManager::InPutSelectSingle(string String, string texts[], int total, int Default)
{
	int Choice;
	cgiFormSelectSingle((char*)String.c_str(), (char**)texts[Choice].c_str(), total, &Choice, Default);
	
	//char* result = texts[Choice];
	string result(texts[Choice]);
	return result;
}


// 获取一组多选Select数据(若没有选中任何项则返回假，否则返回真)
bool CCgiManager::InPutSelectMultiple(string String, vector<string> &pResult, string texts[], int total, int Invalid)
{
	int Choices[65535];

	//没有选择任何项
	if (cgiFormSelectMultiple((char*)String.c_str(), (char**)texts[0].c_str(), total, Choices, &Invalid) == cgiFormNotFound) 
	{
		return false;
	}
	else
	{
		for (int i=0; i < total; i++) 
		{
			if (Choices[i]) 
			{
				string result(texts[i]);
				pResult.push_back(result);
			}
		}
		return true;
	}
}


// 获取一组Radio数据(返回选中的项)
string CCgiManager::InPutRadio(string String, string Texts[], int Total, int Default)
{
	int Choice;
	cgiFormRadio((char*)String.c_str(), (char**)Texts[Choice].c_str(), Total, &Choice, Default);
	string result(Texts[Choice]);

	return result;
}


// 获取Submit数据(提交成功返回真否则返回假)
bool CCgiManager::SubmitClicked(string String)
{
	if ( cgiFormSubmitClicked((char*)String.c_str()) == cgiFormSuccess )
		return true;
	else
		return false;
}


// 为站点设置Cookie数据
void CCgiManager::SetCookieString(string Name, string Value)
{
	if (Name.length()) 
	{
		/* Cookie lives for one day (or until browser chooses
			to get rid of it, which may be immediately),
			and applies only to this script on this site. */

		// 保存
		cgiHeaderCookieSetString((char*)Name.c_str(), (char*)UrlGB2312((char*)Value.c_str()).c_str(), cookie_expire, (char*)cookie_path.c_str(), (char*)cookie_domain.c_str());
	}
}


void CCgiManager::SetCoolieInteger(string Name, int Value)
{
	if (Name.length())
	{
		cgiHeaderCookieSetInteger((char*)Name.c_str(), Value, cookie_expire, (char*)cookie_path.c_str(), (char*)cookie_domain.c_str());
	}
}


// 获取站点Cookie数据
string CCgiManager::GetCookieString(string Name)
{
	/*char **arrays, **arrayStep;
	if (cgiCookies(&arrays) != cgiFormSuccess) 
	{
		return "";
	}

	arrayStep = arrays;
	while (*arrayStep) 
	{
		char value[65535];
		cgiHtmlEscape(*arrayStep);
		cgiCookieString(*arrayStep, value, sizeof(value));

		string TargetName(*arrayStep);
		if(TargetName == Name)
		{
			cgiHtmlEscape(value);
			string Value(value);
			return Value;
		}
		arrayStep++;
	}

	cgiStringArrayFree(arrays);
	return "";*/

	char value[65535];
	if (Name.length()) 
	{
		cgiCookieString((char*)Name.c_str(), value, sizeof(value));
		return UrlGB2312Decode(value);
	}
	else
		return NULL;
}


int CCgiManager::GetCookieInteger(string Name, int Default)
{
	int value;
	if (Name.length()) 
	{
		cgiCookieInteger((char*)Name.c_str(), &value, Default);
		return value;
	}
	else
		return NULL;
}


//获取所有表单名称(Name)
string CCgiManager::Entries(bool OutPut)
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
			string value(*arrayStep);
			HtmlEscape(value);
		    arrayStep++;
		}
	}

	// 赋值
	string result(*arrayStep);

	cgiStringArrayFree(arrays);
	
	//return *arrayStep;
	return result;
}


// 将表单数据储存在磁盘里(实现读取session数据) 
bool CCgiManager::LoadEnvironment(string FileName)
{
	if (cgiReadEnvironment((char*)FileName.c_str()) != cgiEnvironmentSuccess) 
	{
		return false;
	}
	else
		return true;
}


// 从磁盘里读取表单数据（实现放置session数据）
bool CCgiManager::SaveEnvironment(string FileName)
{
	if (cgiWriteEnvironment((char*)FileName.c_str()) != cgiEnvironmentSuccess) 
	{
		return false;
	} 
	else 
	{
		return true;
	}
}


// 获取文件数据
bool CCgiManager::InPutFile(string String, string &FileName, int &FileSize, string &contentType)
{
	// 得到文件名
	if (cgiFormFileName((char*)String.c_str(), (char*)FileName.c_str(), FileName.size()) != cgiFormSuccess)
	{
		// 没有接受到文件数据
		return false;
	}

	// 得到文件大小
	cgiFormFileSize((char*)String.c_str(), &FileSize);

	// 得到文件类型
	cgiFormFileContentType((char*)String.c_str(), (char*)contentType.c_str(), contentType.size());

	// 返回
	return true;
}


// 读取文件数据
string CCgiManager::ReadFileData(string String, bool OutPut)
{
	cgiFilePtr File;
	char buffer[1024];
	int got;

	// 打开目标文件
	if (cgiFormFileOpen((char*)String.c_str(), &File) != cgiFormSuccess)
		return NULL;

	string result = "";
	while (cgiFormFileRead(File, buffer, sizeof(buffer), &got) == cgiFormSuccess)
	{
		// 赋值
		result = result + buffer;
		if(OutPut)
			cgiHtmlEscapeData(buffer, got);
	}

	// 关闭目标文件
	cgiFormFileClose(File);

	// 返回
	return result;
}


// 保存文件数据
bool CCgiManager::SaveFileData(string String, string FilePath)
{
	char buffer[1024];
	cgiFilePtr File;
	int got;

	FILE *fp;

	#ifdef _WIN32
	fopen_s(&fp, FilePath.c_str(), "a+");
	#endif

	#ifdef __linux
	fp = fopen(FilePath.c_str(), "a+");
	#endif

	if(fp == NULL)
	{
		return false;
	}
	else
	{
		// 写入数据
		// 打开目标文件
		if (cgiFormFileOpen((char*)String.c_str(), &File) != cgiFormSuccess)
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
string CCgiManager::GetServerSoftware()
{
	string result(cgiServerSoftware);
	return result;
}


// 获取服务器的名称，如果未知，则指向空字符串。
string CCgiManager::GetServerName()
{
	string result(cgiServerName);
	return result;
}


// 获取网关接口（通常为CGI / 1.1）的名称，如果未知，则指向空字符串。
string CCgiManager::GetGatewayInterface()
{
	string result(cgiGatewayInterface);
	return result;
}


// 获取使用的协议（通常为HTTP / 1.0），如果未知，则指向空字符串。 
string CCgiManager::GetServerProtocol()
{
	string result(cgiServerProtocol);
	return result;
}


// 获取服务器正在监听HTTP连接（通常为80）的端口号，或未知的空字符串。 
string CCgiManager::GetServerPort()
{
	string result(cgiServerPort);
	return result;
}


// 获取请求中使用的方法（通常为GET或POST），如果未知（这不应该发生），则为空字符串）。 
string CCgiManager::GetRequestMethod()
{
	string result(cgiRequestMethod);
	return result;
}


// 大多数Web服务器在请求的URL中识别出超出CGI程序本身的任何附加路径信息，并将该信息传递给程序。cgiPathInfo指向这个额外的路径信息。 
string CCgiManager::GetPathInfo()
{
	string result(cgiPathInfo);
	return result;
}


// 大多数Web服务器在请求的URL中识别出超出CGI程序本身的任何附加路径信息，并将该信息传递给程序。cgiPathTranslated指向此附加路径信息，由服务器转换为本地服务器上的文件系统路径。 
string CCgiManager::GetPathTranslated()
{
	string result(cgiPathTranslated);
	return result;
}


// 获取调用程序的名称。 
string CCgiManager::GetScriptName()
{
	string result(cgiScriptName);
	return result;
}


// 获取包含由GET方法表单或<ISINDEX>标签导致用户提交的任何查询信息。请注意，除非使用<ISINDEX>标记，否则不需要直接解析此信息; 通常它自动解析。使用cgiForm函数系列检索与表单输入字段相关的值。
string CCgiManager::GetQueryString()
{
	string result(cgiQueryString);
	return result;
}


// 获取浏览器的完全解析的主机名（如果已知）或空字符串（如果未知）。 
string CCgiManager::GetRemoteHost()
{
	string result(cgiRemoteHost);
	return result;
}


// 获取浏览器的点分十进制IP地址（如果已知）或空字符串（如果未知）。 
string CCgiManager::GetRemoteAddr()
{
	string result(cgiRemoteAddr);
	return result;
}


// 获取用于请求的授权类型（如果有的话），如果没有或未知，则指向空字符串。 
string CCgiManager::GetAuthType()
{
	string result(cgiAuthType);
	return result;
}


// 获取用户已经认证的用户名; 如果没有发生身份验证，则为空字符串。这些信息的确定性取决于使用授权的类型; 
string CCgiManager::GetRemoteUser()
{
	string result(cgiRemoteUser);
	return result;
}


// 由用户通过用户识别协议自愿指定用户名; 一个空字符串，如果未知。此信息不安全。可以由用户安装在不安全的系统，如Windows机器上。 
string CCgiManager::GetRemoteIdent()
{
	string result(cgiRemoteIdent);
	return result;
}


// 获取用户提交的信息的MIME内容类型（如果有）; 如果没有提交信息，则为空字符串。如果此字符串等于 application/x-www-form-urlencoded或 multipart/form-data，则自动检查提交的表单数据。如果此字符串具有任何其他非空值，则会提交不同类型的数据。这是非常罕见的，因为大多数浏览器只能直接提交表单和文件上传。 
string CCgiManager::GetContentType()
{
	string result(cgiContentType);
	return result;
}


// 获取Web浏览器提交的原始Cookie（浏览器端永久存储）数据。应该使用函数GetCookies， GetCookieString和 GetCookieInteger，而不是直接检查这个字符串。 
string CCgiManager::GetCookie()
{
	string result(cgiCookie);
	return result;
}


// 获取浏览器可以接受的MIME内容类型的空格分隔列表（请参阅 cgiHeaderContentType（））或空字符串。不幸的是，大多数当前的浏览器并不是以一种有用的形式提供这个变量。
string CCgiManager::GetAccept()
{
	string result(cgiAccept);
	return result;
}


// 获取正在使用的浏览器的名称，如果此信息不可用，则为空字符串。 
string CCgiManager::GetUserAgent()
{
	string result(cgiUserAgent);
	return result;
}


// 获取用户访问的上一页的URL。这通常是将用户带到您的程序的表单的URL。请注意，报告此信息完全取决于浏览器，可能选择不这样做。但是，该变量通常是准确的。
string CCgiManager::GetReferrer()
{
	string result(cgiReferrer);
	return result;
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


CController::~CController()
{

}


// 控制器初始化
void CController::Initialize()
{
	bool ControllerExist = false;
	bool FunctionExist   = false;

	// 判断解析结果
	if(Controller.empty() || Controller == "" || Function.empty() || Function == "")
		return;

	// 查找目标控制器
	for(map<string, map<string, PtrFun> >::iterator i = FunTab.begin(); i != FunTab.end(); i++)
	{
		string Name = i->first;
		if(Name == Controller)
			ControllerExist = true;

		for(map<string, PtrFun>::iterator j = (i->second).begin(); j != (i->second).end(); j++)
		{
			string Name = j->first;
			if(Name == Function)
				FunctionExist = true;
        }
    }

	// 控制器不存在
	if(!ControllerExist)
	{
		char message[1024] = {0};
		sprintf(message, "控制器:%s 不存在!", Controller.c_str());
		
		// 记录日志
		Record(message);

		// 输出错误
		DisplayError(message);
	}
	else if(!FunctionExist)
	{
		char message[1024] = {0};
		sprintf(message, "方法:%s 不存在!", Function.c_str());

		// 记录日志
		Record(message);

		// 输出错误
		DisplayError(message);
	}
	else
	{
		// 执行方法
		FunTab[Controller][Function]();
	}
}


// 得到 HTML 内容
string CController::HtmlRead(string FileName)
{
	// 读取文件
	FILE *fp;

	#ifdef _WIN32
	fopen_s(&fp, FileName.c_str(), "r");
	#endif

	#ifdef __linux
	fp = fopen(FileName.c_str(), "r");
	#endif

	// 定义变量
	char str[1024], buff[65535];
	if ( fp == NULL )
	{
		char message[1024] = {0};
		sprintf(message, "网页文件:%s 不存在!", FileName.c_str());

		// 记录日志
		CController::Record(message);

		// 输出错误
		CController::DisplayError(message);
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
	string result(buff);
	return result;
}


// 渲染并输出HTML
void CController::HtmlView(string FileName, ...)
{
	//得到变参的起始地址
	va_list  pArgList;
	va_start(pArgList, FileName);

	// 连接字符串, 得到目标
	string Target = "application/view/" + Controller + "/"+ FileName + "." + html_suffix;

	// 得到 HTML 内容
	char s[65535];
	
	// 格式化并输出
	vsnprintf(s, 65535, HtmlRead(Target).c_str(), pArgList);

	// 替换绑定数据
	if(!IntegerMap.empty())
	{
		map<string, int>::iterator iter;
		for(iter = IntegerMap.begin(); iter != IntegerMap.end(); iter++)
		{
			char* name = (char*)iter->first.c_str(), value[65535] = "";
			sprintf(value, "%d", iter->second);
			Replace(s, name, value);
		}
		IntegerMap.clear();
	}

	if(!StringMap.empty())
	{
		map<string, string>::iterator iter;
		for(iter = StringMap.begin(); iter != StringMap.end(); iter++)
		{
			char* name  = (char*)iter->first.c_str();
			char* value = (char*)iter->second.c_str();
			Replace(s, name, value);
		}
		StringMap.clear();
	}

	if(!DoubleMap.empty())
	{
		map<string, double>::iterator iter;
		for(iter = DoubleMap.begin(); iter != DoubleMap.end(); iter++)
		{
			char* name = (char*)iter->first.c_str(), value[65535] = "";
			sprintf(value, "%.2lf", iter->second);
			Replace(s, name, value);
		}
		DoubleMap.clear();
	}

	if(!ListMap.empty())
	{
		map<string, vector<string> >::iterator iter;
		for(iter = ListMap.begin(); iter != ListMap.end(); iter++)
		{
			//
		}
		ListMap.clear();
	}

	// 默认替换css与js资源
	Replace(s, "__PUBLIC__", "/public");
	Replace(s, "__CSS__",    "/public/css");
	Replace(s, "__JS__",     "/public/js");
	Replace(s, "__IMG__",    "/public/img");

	// 渲染输出HTML
	OutPut(s, pArgList);

	//收尾
	va_end(pArgList);
}


// 绑定视图数据
void CController::BindValue(string Name, int Value)
{
	IntegerMap[Name] = Value;
}


void CController::BindValue(string Name, string Value)
{
	StringMap[Name] = Value;
}


void CController::BindValue(string Name, double Value)
{
	DoubleMap[Name] = Value;
}


void CController::BindValue(string Name, vector<string> Value)
{
	ListMap[Name] = Value;
}


// Json编码,返回json数据
string CController::Json_Encode()
{
	return "";
}


// Json 解码
void CController::Json_Decode()
{
}


// Xml编码,返回xml数据
string CController::Xml_Encode()
{
	return "";
}


// Xml 解码
void CController::Xml_Decode()
{
}


/***************************************************************************************************************************************************************************************************/


/* 模型类***********************************************************************************************************************************************************************************
 *  用途：进行各种数据交互
 */
CModel::CModel()
{
	// 初始化
	Initialize = false;

	// 数据库初始化
	DatabaseConfig();
}


CModel::~CModel()
{

}


//快速排序，在子函数中，数组已被改变
void quick_sort(int *a, int left, int right) //left和right为索引值
{
	int temp; //存储每次选定的基准数（从最左侧选基准数）
	int t;
	int initial=left;
	int end=right;
	temp=a[left];
 
	//***必须有这一部分***//
	if (left>right)  //因为在递归过程中有减1加1的情况，当其越界时，直接return,不返回任何值，即结束当前程序块
		return;   
 
	while(left!=right)  //此时左右index在移动中，若left==right,则跳出循环，将基数归位
	{
		while(a[right]>=temp && left<right)  //直到找到小于基准数的值为准
			right--;
		while(a[left]<=temp && left<right)
			left++;
		if(left<right)  //交换左右两侧值，当left=right时，跳出外层while循环
		{
			t=a[right];
			a[right]=a[left];
			a[left]=t;
		}	
	}
	a[initial]=a[left];
	a[left]=temp;        //基数归位
 
	//递归处理归位后的基准数的左右两侧
	quick_sort(a,initial,left-1);  //此时left=right
	quick_sort(a,left+1,end);
}


// 配置数据库
void CModel::DatabaseConfig(string ConfigFile)
{
	// 检查配置文件
	if(!CConfig::FileExist(ConfigFile))
	{
		CController::Record("数据库配置文件 database.txt 不存在!");
		CController::DisplayError("数据库配置文件 database.txt 不存在!");
		return;
	}

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
	#ifdef _WIN32
	if(_stricmp((char*)type.c_str(), "sqlite") == 0)
	#endif

	#ifdef __linux
	if(strcasecmp((char*)type.c_str(), "sqlite") == 0)
	#endif
	{
		// 赋值
		Type = 1;

		// 尝试连接数据库
		if(!CModel::m_sqlite.LocalConnect(hostname, password))
		{
			//变量
			string data_name, data_path, temp(hostname);

			//#ifdef _WIN32
			////判断路径类型(绝对全路径)
			//if(hostname.find("\\") != -1)
			//{
			//	CCgiManager::Right(m_name, host, strlen(host) - hostname.rfind('\\') -1);
			//	data_name = m_name;

			//	CCgiManager::Left (m_path, host, strlen(host) - strlen(m_name));
			//	data_path = m_path;
			//}
			//
			////判断路径类型(相对全路径)
			//else if(hostname.rfind('/') != -1)
			//{
			//	CCgiManager::Right(m_name, host, strlen(host) - hostname.rfind('/') -1);
			//	data_name = m_name;

			//	CCgiManager::Left (m_path, host, strlen(host) - strlen(m_name));
			//	data_path = m_path;
			//}
   //         #else
			////判断路径类型(相对全路径)
			//if(hostname.rfind('/') != -1)
			//{
			//	CCgiManager::Right(m_name, host, strlen(host) - hostname.rfind('/') -1);
			//	data_name = m_name;

			//    CController::OutPut("%s %s", data_name.c_str(), data_path.c_str());
			//    return;

			//	CCgiManager::Left (m_path, host, strlen(host) - strlen(m_name));
			//	data_path = m_path;
			//}
   //         #endif


			//判断路径类型(绝对全路径)
			if(hostname.find("\\") != -1)
			{
				data_name = hostname.substr(hostname.find_last_of("\\") +1);

				CCgiManager::Replace((char*)temp.c_str(), (char*)data_name.c_str(), "");
				CCgiManager::Replace((char*)temp.c_str(), "\\", "");

				data_path = temp;
			}

			//判断路径类型(相对全路径)
			else if(hostname.rfind('/') != -1)
			{
				data_name = hostname.substr(hostname.find_last_of("/") +1);

				CCgiManager::Replace((char*)temp.c_str(), (char*)data_name.c_str(), "");
				CCgiManager::Replace((char*)temp.c_str(), "/", "");

				data_path = temp;
			}

			// windows
		    #ifdef _WIN32
			// 检查路径
			if (!data_path.empty() && data_path != "" && 0 != _access(data_path.c_str(), 0))
			{
				// 目录不存在，创建目录
				_mkdir(data_path.c_str());
			}
            #else
			if (!data_path.empty() && data_path != "" && 0 != access(data_path.c_str(), 0))
			{
				// 目录不存在，创建目录
				mkdir(data_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			}
		    #endif;

			// 尝试创建数据库
			if(!m_sqlite.OpenDataBase(hostname))
			{
				// 连接失败,返回错误
				CController::Record("数据库连接失败!");
				CController::DisplayError("数据库连接失败!");
				return;
			}
			else
				Initialize = true;
		}
		else
			Initialize = true;

		// 得到数据库路径
		//CString Path(hostname.c_str()), DataBase_Name, DataBase_Path;

		//// 判断路径类型(绝对全路径)
		//if(Path.Replace(_T("\\"), _T("\\")))
		//{
		//	DataBase_Name = Path.Right(Path.GetLength() - Path.ReverseFind('\\') -1);
		//	DataBase_Path = Path.Left(Path.GetLength()  - DataBase_Name.GetLength());
		//}

		//// 判断路径类型(相对全路径)
		//if(Path.ReverseFind('/'))
		//{
		//	DataBase_Name = Path.Right(Path.GetLength() - Path.ReverseFind('/') -1);
		//	DataBase_Path = Path.Left(Path.GetLength()  - DataBase_Name.GetLength());
		//}

		//// 如果存在目标数据库
		//if (m_sqlite.CheckDataBase( DataBase_Name, DataBase_Path))
		//{
		//	// 连接数据库
		//	if(!m_sqlite.OpenDataBase( DataBase_Name, DataBase_Path))
		//		return;
		//}
		//else
		//{
		//	// 创建数据库
		//	if(!m_sqlite.CreateDataBase( DataBase_Name, DataBase_Path))
		//		return;

		//	// 连接数据库
		//	if(!m_sqlite.OpenDataBase(  DataBase_Name, DataBase_Path))
		//		return;
		//}
	}

	#ifdef _WIN32
	else if(_stricmp((char*)type.c_str(), "mysql") == 0)
	#endif

	#ifdef __linux
	else if(strcasecmp((char*)type.c_str(), "mysql") == 0)
	#endif
	{
		// 赋值
		Type = 2;

		// 配置MySQL数据库
		std::cout<<"mysql:"<<std::endl;
	}

	#ifdef _WIN32
	else if(_stricmp((char*)type.c_str(), "sqlserver") == 0)
	#endif

	#ifdef __linux
	else if(strcasecmp((char*)type.c_str(), "sqlserver") == 0)
	#endif
	{
		// 赋值
		Type = 3;

		// 配置SqlServer数据库
		std::cout<<"SqlServer:"<<std::endl;
	}
}


// 执行Sql (需自行检查sql语句合理性)
bool CModel::Execute(string Sql)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return false;
	}

	switch(Type)
	{
	case 1: // Sqlite
		{
			return m_sqlite.Execute(Sql);
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
	return false;
}


// 查询Sql (需自行检查sql语句合理性)
bool CModel::Query(string Sql, vector<string> &Results)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return false;
	}

	switch(Type)
	{
	case 1: // Sqlite
		{
			Result* query = m_sqlite.Query(m_sqlite.db, Sql);

			if(query)
			{
				int i=0;
				while(query->next())
				{
					//string id = query->value("ID");
					string result(query->pResult[i]);
					Results.push_back(result);
					i++;
				}
				delete query;
				return true;
			}

			return false;
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
	return false;
}


// 创建数据表
bool CModel::Create(string TableName, string Params)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return false;
	}

	switch(Type)
	{
	case 1: // Sqlite
		{
			return m_sqlite.CreateDataTable(TableName, Params);
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
	return false;
}


// 删除数据表
bool CModel::Drop(string TableName)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return false;
	}

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return false;
			}
			else
				// 删除数据表
				return m_sqlite.DeleteDataTable(TableName);
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
	return false;
}


// 修改数据表 (Operation可选: 重命名表:REN_TABLE / 添加列:ADD_COLUMN / 删除列:DEL_COLUMN / 修改列:REN_COLUMN)(NewParams仅用于修改列)
bool CModel::Alter(string TableName, int Operation, string Params, string NewParams)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return false;
	}

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return false;
			}
			else
				// 修改数据表
				return m_sqlite.UpdataDataTable(TableName, Operation, Params, NewParams);
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
	return false;
}


// 检查数据表是否存在
bool CModel::Exist(string TableName)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return false;
	}

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			return m_sqlite.CheckDataTable(TableName);
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
	return false;
}


// 写入数据
bool CModel::Save(string TableName, string Params)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return false;
	}

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return false;
			}
			else
				// 写入数据
				return m_sqlite.InsertData(TableName, Params);
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
	return false;
}


// 删除数据
bool CModel::Delete(string TableName, string Params)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return false;
	}

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return false;
			}
			else if( !Check(TableName, Params) )
			{
				CController::Record("目标数据不存在, 无法删除!");
				CController::DisplayError("目标数据不存在, 无法删除!");
				return false;
			}
			else
				// 删除数据
				return m_sqlite.DeleteData(TableName, Params);
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
	return false;
}


// 更新数据 (单条)
bool CModel::UpDate(string TableName, string Columns, string NewData, string Params)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return false;
	}

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return false;
			}
			else if( !Check(TableName, Params) )
			{
				CController::Record("目标数据不存在, 无法更新!");
				CController::DisplayError("目标数据不存在, 无法更新!");
				return false;
			}
			else
				// 更新数据
				return m_sqlite.UpdataData(TableName, Columns, NewData, Params);
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
	return false;
}


// 更新数据 (批量)
bool CModel::UpDate(string TableName, string Columns, string Params)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return false;
	}

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return false;
			}
			else
				// 更新数据
				return m_sqlite.UpdataData(TableName, Columns, Params);
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
	return false;
}


// 获取数据 (单条)
string CModel::Get(string TableName, string Column, string Params, string Order, string Limit, int SortMode, bool DISTINCT, string COUNT, string COLUMN, string GROUP, string HAVING)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return "";
	}

	// 定义返回值
	string pResult = "";

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return false;
			}
			else
			{
				// 获取数据
				if( m_sqlite.SelectData(TableName, pResult, Column, Params, Order, Limit, SortMode, DISTINCT, COUNT, COLUMN, GROUP, HAVING) )
				{
					//从这里开始进行转化，这是一个宏定义
					//USES_CONVERSION;

					////进行转换
					//char* pResult = T2A(Result.GetBuffer(0));
					//Result.ReleaseBuffer();

					// 返回结果
					return pResult;
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
	return pResult;
}


// 获取数据 (批量)
vector<string> CModel::GetAll(string TableName, string Params, string Order, string Limit, int SortMode, bool DISTINCT, string COUNT, string COLUMN, string GROUP, string HAVING)
{
	// 链表对象
	vector<string> Result;
	vector<string> Temp;

	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return Result;
	}

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return Result;
			}
			else
			{
				// 得到列数
				int Col = 0;
				m_sqlite.CountColName(TableName, Col);

				// 获取数据
				if( m_sqlite.SelectData(TableName, Temp, Params, Order, Limit, SortMode, DISTINCT, COUNT, COLUMN, GROUP, HAVING) )
				{
					for(int i = 0; i < (int)Temp.size(); i += Col)
					{
						//进行转换
						for(int j = 0; j < Col; j++)
						{
							// 放进目标链表中
							Result.push_back(Temp.at(i + j));
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


// 检查数据项是否存在
bool CModel::Check(string TableName, string Params)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return false;
	}

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return false;
			}
			else
			{
				// 检查数据
				if( m_sqlite.CheckData(TableName, "", Params) )
					return true;
				else
					return false;
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

	// 默认返回值
	return false;
}


// 统计数据
int CModel::Count(string TableName, string Params)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return -1;
	}

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return -1;
			}
			else
			{
				// 定义返回对象
				int Count;

				// 统计数据
				if( m_sqlite.CountData(TableName, Params, Count) )
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

	// 默认返回
	return -1;
}


// 数据求和
double CModel::Sum(string TableName, string Column, string Params)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return -1;
	}

	// 定义返回对象
	double Data = 0.00;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return -1;
			}
			else
			{
				// 数据求和
				if( m_sqlite.SumData(TableName, Column, Params, Data) )
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
double CModel::Product(string TableName, string Column, string Params)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return -1;
	}

	// 定义返回对象
	double Data = 1.00;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return false;
			}
			else
			{
				// 数据求积
				if( m_sqlite.ProductData(TableName, Column, Params, Data) )
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
double CModel::Avg(string TableName, string Column, string Params)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return -1;
	}

	// 定义返回对象
	double Data = 0.00;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return -1;
			}
			else
			{
				// 统计数据
				if( m_sqlite.AvgData(TableName, Column, Params, Data) )
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


// 求出众数
double CModel::Plu(string TableName, string Column, string Params)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return -1;
	}

	// 定义返回对象
	double Data = 0.00;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return -1;
			}
			else
			{
				// 统计数据
				if( m_sqlite.PluData(TableName, Column, Params, Data) )
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
double CModel::Mid(string TableName, string Column, string Params)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return -1;
	}

	// 定义返回对象
	double Data = 0.00;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return -1;
			}
			else
			{
				// 统计数据
				if( m_sqlite.MidData(TableName, Column, Params, Data) )
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


// 求最大值
double CModel::Max(string TableName, string Column, string Params)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return -1;
	}

	// 定义返回对象
	double Data = 0.00;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return -1;
			}
			else
			{
				// 统计数据
				if( m_sqlite.MaxData(TableName, Column, Params, Data) )
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


// 求最小值
double CModel::Min(string TableName, string Column, string Params)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return -1;
	}

	// 定义返回对象
	double Data = 0.00;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return -1;
			}
			else
			{
				// 统计数据
				if( m_sqlite.MinData(TableName, Column, Params, Data) )
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


// 求随机值
int CModel::Rand(string TableName, string Column, string Params)
{
	if(!Initialize)
	{
		CController::Record("数据库初始化错误!");
		CController::DisplayError("数据库初始化错误!");
		return -1;
	}

	// 定义返回对象
	int Data = 0;

	switch(Type)
	{
	case 1: // Sqlite
		{
			// 检查目标表
			if(!m_sqlite.CheckDataTable(TableName))
			{
				char message[1024] = {0};
				sprintf(message, "数据表%s不存在!", TableName.c_str());

				// 记录日志
				CController::Record(message);

				// 输出错误
				CController::DisplayError(message);
				return -1;
			}
			else
			{
				// 统计数据
				if( m_sqlite.RandData(TableName, Column, Params, Data) )
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

	// 默认返回
	return -1;
}
