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
	// Ĭ�Ϸ���HTML
	SetHead("text/html");

	// �õ�����·��
	CString Path(cgiPathInfo), Script(cgiScriptName);
	Path.Replace(Script, _T(""));
	Jump((char*)Path.GetBuffer());
}


CCgiManager::~CCgiManager()
{
}


// ��ת�����󷽷�
void CCgiManager::Jump(char* Method)
{
	OutPut("path:");
	OutPut(Method);
}


// ����ͷ
void CCgiManager::SetHead(char * type)
{
	cgiHeaderContentType(type);
}


// �ض���Url
void CCgiManager::Redirect(char* Url)
{
	cgiHeaderLocation(Url);
}


// ���HTTP����״̬����
void CCgiManager::SetStatus(int Status, char* Message)
{
	cgiHeaderStatus(Status, Message);
}


// ת�벢���
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


// ת�벢���
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


// �������
void CCgiManager::OutPut(char * string, bool Newlines)
{
	fprintf(cgiOut, string);

	if(Newlines)
	    fprintf(cgiOut, "\n");
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

	char* result;
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






/* ��������***********************************************************************************************************************************************************************************
*  ��;������ģ������ͼ�Ľ���
*/
CController::CController()
{
}


CController::~CController()
{

}


