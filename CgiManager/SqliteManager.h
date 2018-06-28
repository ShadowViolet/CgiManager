#pragma once
/* Sqlite 数据库管理
 * 用途: 操作Sqlite数据库
 * 作者: 紫辉(紫影龙)
 * 日期: 2018-6-22
 * 版权所有 2016 - 2018 紫影龙工作室
 * https://www.shadowviolet.com
 */


// 引用数据库头文件支持数据库操作
#include "sqlite3.h"
using namespace SqliteSpace;


// 引用字符串库头文件和名字空间
#include "iostream"
#include <algorithm>
using namespace std;

#include <string>  
#include <xstring>  
using std::string;
using std::wstring;

// 引用链表库头文件和名字空间
#include <afxtempl.h>
#include <vector>
using std::vector;



// 定义 ModifyDataTable 的参数

// --------- 数据表操作 ---------|
                              // |
// 重命名表                      |
#define REN_TABLE  0x3000     // |
// ------------------------------|
// 添加列                        |
#define ADD_COLUMN 0x3001     // |
// ------------------------------|
// 删除列                        |
#define DEL_COLUMN 0x3002     // |
// ------------------------------|
// 修改列                        |
#define REN_COLUMN 0x3003     // |
                              // |
// ------------------------------|


// 定义 SelectData 的参数

// ---------- 排序模式 ----------|
                              // |
// 升序排列                      |
#define MOD_ASC    0x4000     // |
// ------------------------------|
// 降序排列                      |
#define MOD_DESC   0x4001     // |
                              // |
// ------------------------------|


// 定义查询对象
typedef struct qureyResult
{
	int nRow;
	int nColumn;
	int nIndex;
	char** pResult;
	qureyResult(int row, int column, char**p)
	{
		nRow = row;
		nColumn = column;
		pResult = NULL;
		nIndex = 0;
		if (nRow > 0)
		{
			pResult = p;
		}
	}
	~qureyResult()
	{
		if (pResult)
		{
			sqlite3_free_table(pResult);
		}
	}
	bool next()
	{
		if (nIndex == 0)
		{
			if (nRow > 0)
			{
				++nIndex;
				return true;
			}
			else {
				return false;
			}
		}
		else {
			if ((nRow*nColumn) >= ((nIndex + 1)*nColumn))
			{
				++nIndex;
				return true;
			}
			else {
				return false;
			}
		}
	}
	char* value(char *pColumnName)
	{
		int index = nIndex*nColumn;
		for (int i = 0; i < nColumn; ++i)
		{
			if (strcmp(pColumnName, pResult[i]) == 0)
			{
				return pResult[index];
			}
			++index;
		}
		return NULL;
	}
} Result;


// CSqliteManager 数据库操作类
class CSqliteManager
{
private: bool Init;

public:
	/////////////////////////////////////构造与折构//////////////////////////////////////////////////////////////////////////

	// 默认构造函数
	CSqliteManager(bool Console = FALSE);

	// 带数据库名称和路径参数的构造函数
	CSqliteManager(CString Name, CString Path, bool Console = false);

	// 默认折构函数
	virtual ~CSqliteManager();

	/////////////////////////////////////类成员变量//////////////////////////////////////////////////////////////////////////

	// 数据库对象变量
	static sqlite3 *db;

	// 数据库记录集变量
	static sqlite3_stmt * stmt;

	// 数据库错误信息变量
	static char * errMsg;

	// 记录集
	static char **pRes;

	// 列对象
	static int nRow, nCol;

	// 标示数据库连接是否成功
	static BOOL IsConnect;

	// 控制台标识
	static bool IsConsole;

	// 数据库版本号
	static int version;

	// 查询结果链表
	static CList <CString, CString&> pResult;

	/////////////////////////////////////类成员方法//////////////////////////////////////////////////////////////////////////
	

	////////////////////////////////////参数解析代码/////////////////////////////////////////////////////////////////////////


	// 查找参数个数
	static int FindCharCount(CString csStr, char c);

	// 分割字符串
	static void Split(CString source, CString divKey, CStringArray &dest);

	//用于计算文件夹内的文件数量
	static int CountFile(CString Path);

	////////////////////////////////////事物处理代码/////////////////////////////////////////////////////////////////////////

	//开启事务  
	static bool transaction(sqlite3 *p);

	//提交事务  
	static bool commitTransaction(sqlite3 *p);

	//回滚事物  
	static bool rollbackTransaction(sqlite3 *p);

	////////////////////////////////////数据处理代码/////////////////////////////////////////////////////////////////////////

	//bool Update(sqlite3 *p, const string &sql);

	static Result *Query(sqlite3 *p, const string &sql);


	///////////////////////////////////数据库操作代码////////////////////////////////////////////////////////////////////////


	// 打开目标数据库连接
	static BOOL OpenDataBase(CString Name, CString Path = _T(""), bool Console = false);


	// 关闭目标数据库连接
	static BOOL CloseDataBase(bool Console = false);


	// 创建一个目标数据库
	static BOOL CreateDataBase(CString Name, CString Path = _T(""), bool Console = false);


	// 删除一个目标数据库
	static BOOL DeleteDataBase(CString Name, CString Path = _T(""), bool Console = false);


	// 修改目标数据库名称
	static BOOL ReNameDataBase(CString OldName, CString OldPath = _T(""), CString NewName = _T(""), CString NewPath = _T(""), bool Console = false);


	// 移动一个目标数据库
	static BOOL ReMoveDataBase(CString OldName, CString OldPath = _T(""), CString NewName = _T(""), CString NewPath = _T(""), bool Console = false);


	// 检查目标数据库存在
	static BOOL CheckDataBase(CString Name, CString Path = _T(""), bool Console = false);


	// 统计目标数据库数量
	static BOOL CountDataBase(CString DataBasePath, int &Count, bool Console = false);


	// 备份目标数据库数据
	static BOOL BackupDataBase(CString DataBasePath, CString BackupPath, bool Console = false);


	// 还原目标数据库数据
	static BOOL RestoreDataBase(CString DataBasePath, CString RestorePath, bool Console = false);


	// 远程连接目标数据库
	static BOOL RemoteDataBase(CString Name, CString Path, bool Console = false);


	// 加密目标数据库数据
	static BOOL EncryptionDataBase(CString DataBaseName, CString Password, bool Console = false);


	// 解密目标数据库数据
	static BOOL DecryptionDataBase(CString DataBaseName, CString Password, bool Console = false);


	// 上传远程目标数据库
	static BOOL UploadDataBase(CString DataBasePath, CString UploadPath, bool Console = false);


	// 下载远程目标数据库
	static BOOL DownloadDataBase(CString DataBasePath, CString DownloadPath, bool Console = false);


	///////////////////////////////////数据表操作代码////////////////////////////////////////////////////////////////////////


	// 获取数据库中所有表的名称
	static BOOL GetTableName(CString &TableName, bool Console = false);


	// 获取数据库中所有表的数据
	static BOOL GetDataTable(CString &DataTable, bool Console = false);


	// 获取数据库中数据表的信息
	static BOOL GetTableData(CString TableName, CString &SQL_Data, bool Console = false);


	// 获得数据表中所有列的名称
	static BOOL GetColName(CString TableName, CString &ColName, bool Console = false);


	// 获得数据表中所有列的类型
	static BOOL GetColType(CString TableName, CString &ColType, bool Console = false);


	// 在目标数据库中创建数据表
	static BOOL CreateDataTable(CString TableName, CString Params, bool Console = false);


	// 在目标数据库中删除数据表
	static BOOL DeleteDataTable(CString TableName, bool Console = false);


	// 在目标数据库中修改数据表
	static BOOL UpdataDataTable(CString TableName, int Operation, CString Params, CString NewParams = _T(""), bool Console = false);


	// 在目标数据库中检查数据表     ( 检查数据表是否存在 )
	static BOOL CheckDataTable (CString TableName, bool Console = false);


	// 在目标数据库中统计数据表     ( 统计数据表总共数量 )
	static BOOL CountDataTable (int &Count, bool Console = false);


	// 向目标数据库中导入数据表
	static BOOL ImportDataTable(CString TableName, CString TargetName, bool Console = false);


	// 从目标数据库中导出数据表
	static BOOL ExportDataTable(CString TableName, CString TargetName, bool Console = false);


	///////////////////////////////////数据项操作代码////////////////////////////////////////////////////////////////////////


	// 向目标数据表中添加数据项
	static BOOL InsertData(CString TableName, CString Params, bool Console = false);


	// 从目标数据表中删除数据项
	static BOOL DeleteData(CString TableName, CString Params, bool Console = false);


	// 从目标数据表中修改单数据
	static BOOL UpdataData(CString TableName, CString Column, CString NewData, CString Params, bool Console = false);


	// 从目标数据表中修改数据项
	static BOOL UpdataData(CString TableName, CString ColumnParams, CString Params, bool Console = false);


	// 从目标数据表中查询数据项
	static BOOL SelectData(CString TableName, vector<CString> &pResult, CString Params = _T(""), bool Console = false, CString Order = _T(""), CString Limit = _T(""), int SortMode = MOD_ASC, BOOL DISTINCT = FALSE, CString COUNT = _T(""), CString COLUMN = _T(""), CString GROUP = _T(""), CString HAVING = _T(""));


	// 从目标数据表中查询某数据
	static BOOL SelectData(CString TableName, CString &pResult, int Col, CString Params = _T(""),bool Console = false, CString Order = _T(""), CString Limit = _T(""), int SortMode = MOD_ASC, BOOL DISTINCT = FALSE, CString COUNT = _T(""), CString COLUMN = _T(""), CString GROUP = _T(""), CString HAVING = _T(""));


	// 在目标数据表中检查数据项
	static BOOL CheckData (CString TableName, int Col, CString Params = _T(""), bool Console = false, CString Order = _T(""), CString Limit = _T(""), int SortMode = MOD_ASC, BOOL DISTINCT = FALSE, CString COUNT = _T(""), CString COLUMN = _T(""), CString GROUP = _T(""), CString HAVING = _T(""));


	// 统计目标数据表中数据数项
	static BOOL CountNumber(CString TableName, CString Params, int &Count, bool Console = false);


	// 向目标数据表中导入数据项
	static BOOL ImportData (CString TableName, CString TargetTableName, CString Params, bool Console = false);


	// 从目标数据表中导出数据项
	static BOOL ExportData (CString TableName, CString TargetTableName, CString Params, bool Console = false);


	// 删除目标数据表中行据数项
	static BOOL DeleteRowData(CString TableName, int Row, bool Console = false);


	// 删除目标数据表中列据数项
	static BOOL DeleteColData(CString TableName, int Col, bool Console = false);


	// 删除目标数据表中初据数项
	static BOOL DeleteMinData(CString TableName, bool Console = false);


	// 删除目标数据表中间据数项
	static BOOL DeleteMidData(CString TableName, bool Console = false);


	// 删除目标数据表中末据数项
	static BOOL DeleteMaxData(CString TableName, bool Console = false);


	// 修改目标数据表中行据数项
	static BOOL UpdataRowData(CString TableName, int Row, CString UpdataData, bool Console = false);


	// 修改目标数据表中列据数项
	static BOOL UpdataColData(CString TableName, int Col, CString UpdataData, bool Console = false);


	// 修改目标数据表中初据数项
	static BOOL UpdataMinData(CString TableName, CString UpdataData, bool Console = false);


	// 修改目标数据表中间据数项
	static BOOL UpdataMidData(CString TableName, CString UpdataData, bool Console = false);


	// 修改目标数据表中末据数项
	static BOOL UpdataMaxData(CString TableName, CString UpdataData, bool Console = false);


	// 检索目标数据表中行据数项
	static BOOL SelectRowData(CString TableName, int Row, CString &RowData, bool Console = false);


	// 检索目标数据表中列据数项
	static BOOL SelectColData(CString TableName, int Col, CString &ColData, bool Console = false);


	// 检索目标数据表中初据数项
	static BOOL SelectMinData(CString TableName, CString &MinData, bool Console = false);


	// 修改目标数据表中间据数项
	static BOOL SelectMidData(CString TableName, CString &MidData, bool Console = false);


	// 检索目标数据表中末据数项
	static BOOL SelectMaxData(CString TableName, CString &MaxData, bool Console = false);


	// 为目标数据表中据数项排序
	static BOOL SelectSortData(CString TableName, int Sort, CString &SortData, bool Console = false);


	// 重新排列数据表据数项顺序
	static BOOL ChangeSortData(CString TableName, int Sort, bool Console = false);


	// 从目标数据表中筛选数据项
	static BOOL FilterData(CString TableName, CString Params, CString &FilterData, bool Console = false);


	// 从目标数据表中得到随机数据项
	static BOOL RanData(CString TableName, CString Params, CString &RanData, bool Console = false);


	///////////////////////////////////数据项计算代码////////////////////////////////////////////////////////////////////////


	// 从目标数据表中求数据项和
	static BOOL SumData(CString TableName, CString Column, CString Params, int &SumData, bool Console = false);


	// 从目标数据表中求数据项积
	static BOOL ProductData(CString TableName, CString Column, CString Params, int &ProductData, bool Console = false);


	// 从目标数据表中求数据项平均数
	static BOOL AvgData(CString TableName, CString Column ,CString Params, int &AvgData, bool Console = false);


	// 从目标数据表中求数据项中位数
	static BOOL MidData(CString TableName, CString Column, CString Params, int &MidData, bool Console = false);


	// 从目标数据表中求数据项最大值
	static BOOL MaxData(CString TableName, CString Column, CString Params, int &AbsData, bool Console = false);


	// 从目标数据表中求数据项最小值
	static BOOL MinData(CString TableName, CString Column, CString Params, int &AbsData, bool Console = false);


	// 从目标数据表中取数据项随机数
	static BOOL RanData(CString TableName, CString Column, CString Params, int &RanData, bool Console = false);


	///////////////////////////////////数据编码转换代码////////////////////////////////////////////////////////////////////////


	//ACSII转Unicode
	static wstring AcsiiToUnicode( const string  & acsii_string);

	//ACSII转UTF8  
	static string  AcsiiToUtf8(    const string  & acsii_string);

	//Unicode转ACSII  
	static string  UnicodeToAcsii( const wstring & unicode_string);

	//Unicode转UTF8  
	static string  UnicodeToUtf8(  const wstring & unicode_string);

	//UTF8转ACSII 
	static string  Utf8ToAcsii(    const string  & utf8_string);

	//UTF8转Unicode  
	static wstring Utf8ToUnicode(  const string  & utf8_string);
};

