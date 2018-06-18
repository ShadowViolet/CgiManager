#pragma once

#include "cgic.h"

#include <afx.h>
#include <vector>
using std::vector;

// ʹ��˵��: �������Զ������ڵ�:cgiMain ,��������������ڵ�ɾ���������� int cgiMain(){}
class CCgiManager
{
public:
	CCgiManager();
	~CCgiManager();

	// ��ת�����󷽷�
	void Jump(char* Method);

	// ����ͷ
	void SetHead(char* type = "text/html");

	// �ض���Url
	void Redirect(char* Url);

	// ���HTTP����״̬����
	void SetStatus(int Status, char* Message);

	// �������
	void OutPut(char * string, bool Newlines = true);

	// ת�����Html
	void HtmlEscape(char * name, bool Newlines = true);

	// ת�����Html������
	void HtmlEscapeData(char * name, int len, bool Newlines = true);

	// ת�����Value
	void ValueEscape(char * value, bool Newlines = true);

	// ת�����Value������
	void ValueEscapeData(char * value, int len, bool Newlines = true);

	// ��ȡ�ַ�������
	char* InPutString(char * String, bool OutPut = false, bool Newlines = false, int Max = 0);

	// ��ȡ�����س����з����ַ�������
	char* InPutStringNoNewlines(char * String, bool OutPut = false, bool Newlines = false, int Max = 0);

	// �����ַ����Ĵ���ռ䣬���InPutString��InPutStringNoNewlines���ʹ��
	bool InPutStringSpaceNeeded(char * String, int Length);

	// ��ȡ����������
	int InPutInteger(char * String,  int Default = 0, bool OutPut = false, bool Newlines = false);

	// ��ȡ��������������(���ֵ����СֵΪ�������)
	int InPutIntegerBound(char* String, int Min, int Max, int Default = 0, bool OutPut = false, bool Newlines = false);

	// ��ȡ˫��������
	double InPutDouble(char * String, int Default = 0, bool OutPut = false, bool Newlines = false);

	// ��ȡ˫������������(���ֵ����СֵΪ�������)
	double InPutDoubleBound(char* String, double Min, double Max, double Default = 0.00, bool OutPut = false, bool Newlines = false);

	// ��ȡ����Checkbox����(�����Ƿ�ѡ��)
	bool InPutCheckboxSingle(char * String);

	// ��ȡһ��Checkbox����(��������ѡ�������ѡ�����򷵻ؿ�)
    char * InPutCheckboxMultiple(char * String);

	// ��ȡһ�鵥ѡSelect����(����ѡ�е���)
	char* InPutSelectSingle(char * String , char *texts[], int total, int Default = 0);

	// ��ȡһ���ѡSelect����(����ѡ�е���)
	bool InPutSelectMultiple(char * String, vector<char> &pResult, char *texts[], int total, int Invalid = 0);

	// ��ȡһ��Radio����(����ѡ�е���)
	char* InPutRadio(char * String, char *Texts[], int Total, int Default = 0);

	// ��ȡSubmit����(�ύ�ɹ���������򷵻ؼ�)
	bool SubmitClicked(char * String);

	// Ϊվ������Cookie����
	void SetCookieString(char * Name, char * Value, char* Domain = "DefaultDomain");
	void SetCoolieInteger(char * Name,  int  Value, char* Domain = "DefaultDomain");

	// ��ȡվ��Cookie����
	char* GetCookieString(char * Name);
	int  GetCookieInteger(char * Name, int Default);

	//��ȡ����Cookie����
	char * GetCookies(bool OutPut = false);

	//��ȡ���б�����(Name)
	char* Entries(bool OutPut = false);

	// �������ݴ����ڴ����� 
	bool LoadEnvironment(char * FileName);

	// �Ӵ������ȡ������
	bool SaveEnvironment(char * FileName);

	// ��ȡ�ļ�����
	bool InPutFile(char* String, char* &FileName, int &FileSize, char* &contentType);

	// ��ȡ�ļ�����
	char* ReadFileData(char* String, bool OutPut = false);

	// �����ļ�����
	bool SaveFileData(char* String, char* FilePath);


	// ��ȡ��������������ƣ����δ֪����ָ����ַ�����
	char* GetServerSoftware();

	// ��ȡ�����������ƣ����δ֪����ָ����ַ�����
	char* GetServerName();

	// ��ȡ���ؽӿڣ�ͨ��ΪCGI / 1.1�������ƣ����δ֪����ָ����ַ�����
	char* GetGatewayInterface();

	// ��ȡʹ�õ�Э�飨ͨ��ΪHTTP / 1.0�������δ֪����ָ����ַ����� 
	char* GetServerProtocol();

	// ��ȡ���������ڼ���HTTP���ӣ�ͨ��Ϊ80���Ķ˿ںţ���δ֪�Ŀ��ַ����� 
	char* GetServerPort();

	// ��ȡ������ʹ�õķ�����ͨ��ΪGET��POST�������δ֪���ⲻӦ�÷���������Ϊ���ַ������� 
	char* GetRequestMethod();

	// �����Web�������������URL��ʶ�������CGI��������κθ���·����Ϣ����������Ϣ���ݸ�����cgiPathInfoָ����������·����Ϣ�� 
	char* GetPathInfo();

	// �����Web�������������URL��ʶ�������CGI��������κθ���·����Ϣ����������Ϣ���ݸ�����cgiPathTranslatedָ��˸���·����Ϣ���ɷ�����ת��Ϊ���ط������ϵ��ļ�ϵͳ·���� 
	char* GetPathTranslated();

	// ��ȡ���ó�������ơ� 
	char* GetScriptName();

	// ��ȡ������GET��������<ISINDEX>��ǩ�����û��ύ���κβ�ѯ��Ϣ����ע�⣬����ʹ��<ISINDEX>��ǣ�������Ҫֱ�ӽ�������Ϣ; ͨ�����Զ�������ʹ��cgiForm����ϵ�м�����������ֶ���ص�ֵ��
	char* GetQueryString();

	// ��ȡ���������ȫ�������������������֪������ַ��������δ֪���� 
	char* GetRemoteHost();

	// ��ȡ������ĵ��ʮ����IP��ַ�������֪������ַ��������δ֪���� 
	char* GetRemoteAddr();

	// ��ȡ�����������Ȩ���ͣ�����еĻ��������û�л�δ֪����ָ����ַ����� 
	char* GetAuthType();

	// ��ȡ�û��Ѿ���֤���û���; ���û�з��������֤����Ϊ���ַ�������Щ��Ϣ��ȷ����ȡ����ʹ����Ȩ������; 
	char* GetRemoteUser();

	// ���û�ͨ���û�ʶ��Э����Ըָ���û���; һ�����ַ��������δ֪������Ϣ����ȫ���������û���װ�ڲ���ȫ��ϵͳ����Windows�����ϡ� 
	char* GetRemoteIdent();

	// ��ȡ�û��ύ����Ϣ��MIME�������ͣ�����У�; ���û���ύ��Ϣ����Ϊ���ַ�����������ַ������� application/x-www-form-urlencoded�� multipart/form-data�����Զ�����ύ�ı����ݡ�������ַ��������κ������ǿ�ֵ������ύ��ͬ���͵����ݡ����Ƿǳ������ģ���Ϊ����������ֻ��ֱ���ύ�����ļ��ϴ��� 
	char* GetContentType();

	// ��ȡWeb������ύ��ԭʼCookie������������ô洢�����ݡ�Ӧ��ʹ�ú���GetCookies�� GetCookieString�� GetCookieInteger��������ֱ�Ӽ������ַ����� 
	char* GetCookie();

	// ��ȡ��������Խ��ܵ�MIME�������͵Ŀո�ָ��б������ cgiHeaderContentType����������ַ��������ҵ��ǣ��������ǰ���������������һ�����õ���ʽ�ṩ���������
	char* GetAccept();

	// ��ȡ����ʹ�õ�����������ƣ��������Ϣ�����ã���Ϊ���ַ����� 
	char* GetUserAgent();

	// ��ȡ�û����ʵ���һҳ��URL����ͨ���ǽ��û��������ĳ���ı���URL����ע�⣬�������Ϣ��ȫȡ���������������ѡ�������������ǣ��ñ���ͨ����׼ȷ�ġ�
	char* GetReferrer();

	// ��ȡ�յ��ı����ѯ���ݵ��ֽ�������ע�⣬����ύ���ύ�����ѯ���⽫ֱ�Ӵ�cgiIn��/��cgiQueryString��ȡ�ͽ���������Ϣ������������£�����Ա��Ӧ����������
	int GetContentLength();
};


// ����������
class CController: public CCgiManager
{
public:
	// �������۹�����
	CController();
	~CController();

	void HtmlView(char* FileName = "index.html", char Parameter[] = "");
};
