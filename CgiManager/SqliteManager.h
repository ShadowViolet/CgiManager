#pragma once
/* Sqlite 数据库管理
 * 用途: 操作Sqlite数据库
 * 作者: 紫辉(紫影龙)
 * 日期: 2018-6-22
 * 版权所有 2016 - 2019 紫影龙工作室
 * https://www.shadowviolet.com
 */

// 引用数据库头文件支持数据库操作
#include "sqlite3/sqlite3.h"


// 引用字符串库头文件和名字空间
#include "iostream"
#include <algorithm>
using namespace std;

#include<cstdlib>
#include<ctime>

#ifdef _WIN32
#include <string>
using std::string;
#endif

#ifdef __linux
#include <cstring>  
using std::strcmp;
#endif

// 引用链表库头文件和名字空间
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
private:
	// 标示数据库连接是否成功
	static bool IsConnect;

public:
	/////////////////////////////////////构造与折构//////////////////////////////////////////////////////////////////////////

	// 默认构造函数
	CSqliteManager();

	// 带数据库名称和路径参数的构造函数
	CSqliteManager(string Path);

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

	// 数据库版本号
	static int version;

	/////////////////////////////////////类成员方法//////////////////////////////////////////////////////////////////////////
	

	////////////////////////////////////参数解析代码/////////////////////////////////////////////////////////////////////////


	// 查找参数个数
	static int FindCharCount(string csStr, char c);

	// 分割字符串
	//static void Split(CString source, string divKey, stringArray &dest);

	//用于计算文件夹内的文件数量
	//static int CountFile(string Path);

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

	static bool Execute(const string sql);

	// 字符串分割
    static void Split(const std::string& s, std::vector<std::string>& v, const std::string& c);

	// 字符串替换 s1里替换s2与s3
    static char* Replace(char* s1, char* s2, char* s3 = NULL);

	///////////////////////////////////数据库操作代码////////////////////////////////////////////////////////////////////////


	// 打开目标数据库连接 (若目标数据库不存在则创建新的数据库)
	static bool OpenDataBase(string Path);


	// 关闭目标数据库连接
	static bool CloseDataBase();


	// 检查目标数据库存在
	static bool CheckDataBase(string Path);


	// 建立本地数据库连接 (参数: 路径, 密码)
	static bool LocalConnect(string Path, string Password = "");


	// 远程连接目标数据库 (暂未实现，敬请期待)
	static bool RemoteConnect(string Url, string Password = "");


	// 加密目标数据库数据 (若不是第一次加密则需要旧密码用于修改新密码) (可将密码设为空""取消密码保护)
	static bool EncryptionDataBase(string Password, string OldPassword = "");


	// 解密目标数据库数据 
	static bool DecryptionDataBase(string Password);


	///////////////////////////////////数据表操作代码////////////////////////////////////////////////////////////////////////


	// 获取数据库中所有表的名称
	static bool GetTableName(string &TableData);


	// 获取数据库中所有表的数据
	static bool GetDataTable(string &TableData);


	// 获取数据库中数据表的信息
	static bool GetTableData(string TableName, string &TableData);


	// 获得数据表中所有列的名称
	static bool GetColName(string TableName, string &ColName);


	// 获得数据表中所有列的类型
	static bool GetColType(string TableName, string &ColType);


	// 获得数据表中所有列的数量
	static bool CountColName(string TableName, int &Count);


	// 在目标数据库中创建数据表
	static bool CreateDataTable(string TableName, string Params);


	// 在目标数据库中删除数据表
	static bool DeleteDataTable(string TableName);


	// 在目标数据库中修改数据表
	static bool UpdataDataTable(string TableName, int Operation, string Params, string NewParams = "");


	// 在目标数据库中检查数据表     ( 检查数据表是否存在 )
	static bool CheckDataTable (string TableName);


	// 在目标数据库中统计数据表     ( 统计数据表总共数量 )
	static bool CountDataTable (int &Count);


	// 从目标数据库中导入数据表 (暂未实现，敬请期待)
	static bool ImportDataTable(string TableName, string TargetName);


	// 向目标数据库中导出数据表 (暂未实现，敬请期待)
	static bool ExportDataTable(string TableName, string TargetName);


	///////////////////////////////////数据项操作代码////////////////////////////////////////////////////////////////////////


	// 向目标数据表中添加数据项
	static bool InsertData(string TableName, string Params);


	// 从目标数据表中删除数据项
	static bool DeleteData(string TableName, string Params);


	// 从目标数据表中修改单数据
	static bool UpdataData(string TableName, string Column, string NewData, string Params);


	// 从目标数据表中修改数据项
	static bool UpdataData(string TableName, string ColumnParams, string Params);


	// 从目标数据表中查询数据项
	static bool SelectData(string TableName, vector<string> &pResult, string Params = "", string Order = "", string Limit = "", int SortMode = MOD_ASC, bool DISTINCT = false, string COUNT = "", string COLUMN = "", string GROUP = "", string HAVING = "");


	// 从目标数据表中查询列数据
	static bool SelectData(string TableName, string &pResult, int Col, string Params = "", string Order = "", string Limit = "", int SortMode = MOD_ASC, bool DISTINCT = false, string COUNT = "", string COLUMN = "", string GROUP = "", string HAVING = "");
	
	
	// 从目标数据表中查询数据值
	static bool SelectData(string TableName, string &pResult, string Column, string Params = "", string Order = "", string Limit = "", int SortMode = MOD_ASC, bool DISTINCT = false, string COUNT = "", string COLUMN = "", string GROUP = "", string HAVING = "");


	// 在目标数据表中检查数据项
	static bool CheckData (string TableName, int Col, string Params = "", string Order = "", string Limit = "", int SortMode = MOD_ASC, bool DISTINCT = false, string COUNT = "", string COLUMN = "", string GROUP = "", string HAVING = "");


	// 在目标数据表中检查数据项
	static bool CheckData (string TableName, string Column, string Params = "",  string Order = "", string Limit = "", int SortMode = MOD_ASC, bool DISTINCT = false, string COUNT = "", string COLUMN = "", string GROUP = "", string HAVING = "");


	// 统计目标数据表中数据数项
	static bool CountData (string TableName, string Params, int &Count);


	// 重置目标数据表数据数编号 (ClearData为false则不清空数据 否则清空数据)
	static bool Truncate(string TableName);


	///////////////////////////////////数据项计算代码////////////////////////////////////////////////////////////////////////


	// 从目标数据表中求数据项和
	static bool SumData(string TableName, string Column, string Params, double &SumData);


	// 从目标数据表中求数据项积
	static bool ProductData(string TableName, string Column, string Params, double &ProductData);


	// 从目标数据表中求数据项平均数
	static bool AvgData(string TableName, string Column ,string Params, double &AvgData);


	// 从目标数据表中求数据项的众数
	static bool PluData(string TableName, string Column, string Params, double &PluData);


	// 从目标数据表中求数据项中位数
	static bool MidData(string TableName, string Column, string Params, double &MidData);


	// 从目标数据表中求数据项最大值
	static bool MaxData(string TableName, string Column, string Params, double &MaxData);


	// 从目标数据表中求数据项最小值
	static bool MinData(string TableName, string Column, string Params, double &MinData);


	// 从目标数据表中取数据项随机数
	static bool RandData(string TableName, string Column, string Params, int &RanData);
};

