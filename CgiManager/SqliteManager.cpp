#include "SqliteManager.h"

///////////////////////////////////构造折构代码///////////////////////////////////

// 静态成员初始化
sqlite3 *CSqliteManager::db    = NULL;
sqlite3_stmt * CSqliteManager::stmt = NULL;
char * CSqliteManager::errMsg  = NULL;
char **CSqliteManager::pRes    = NULL;
int CSqliteManager::nRow = NULL, CSqliteManager::nCol = NULL;
bool CSqliteManager::IsConnect = false;
int CSqliteManager::version    = NULL;


// 默认构造函数
CSqliteManager::CSqliteManager()
{
	// 默认连接失败
	IsConnect = false;
}


// 带数据库名称和路径参数的构造函数
CSqliteManager::CSqliteManager(string Path)
{
	// 默认连接失败
	IsConnect = false;

	// 打开目标数据库连接
	if (!OpenDataBase(Path))
	{
		std::cout<<"目标数据库对象创建失败!"<<std::endl;

		// 返回数据库对象创建失败的信息
		return;
	}
}


// 默认折构函数
CSqliteManager::~CSqliteManager()
{
	// 判断是否已完成销毁
	if(IsConnect)
	{
		// 关闭数据库连接
		CloseDataBase();
	}
}


///////////////////////////////////参数解析代码///////////////////////////////////

// 解析传入参数的数量
int CSqliteManager::FindCharCount(string csStr, char c)
{
	// 初始化参数个数为0
	int iCount = 0;

	// 循环查找参数
	for (int i = 0; i < (int)strlen(csStr.c_str()); i++)
	{
		// 循环遍历
		i = csStr.find(c, i + 1);

		// 参数个数自加1
		iCount++;
	}

	// 返回参数个数
	return iCount;
}


// 分割字符串
/*void CSqliteManager::Split(CString source, CString divKey, CStringArray &dest)
{
	dest.RemoveAll();
	int pos = 0;
	int pre_pos = 0;
	while ( -1 != pos )
	{
		pre_pos = pos;
		pos     = source.Find(divKey, (pos +1));

		CString temp(source.Mid(pre_pos , (pos -pre_pos )));
		temp.Replace(divKey, "");
		dest.Add(temp);
	}
}


//用于计算文件夹内的文件数量
int CSqliteManager::CountFile(string Path)
{
	int count = 0;
	CFileFind finder;
	BOOL working = finder.FindFile(Path + _T("./*.*"));

	while (working)
	{
		working = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())
			count += CountFile(finder.GetFilePath());
		else
		{
			count++;
		}
	}
	return count;
}*/

/*从字符串的左边截取n个字符*/  


// 字符串分割
void CSqliteManager::Split(const std::string& s, std::vector<std::string>& v, const std::string& c)
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
char* CSqliteManager::Replace(char* s1, char* s2, char* s3)
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


////////////////////////////////////事物处理代码//////////////////////////////////

bool CSqliteManager::transaction(sqlite3 *p)
{
	bool result = true;
	char *zErrorMsg = NULL;
	int ret = sqlite3_exec(p, "begin transaction", 0, 0, &zErrorMsg); // 开始一个事务  
	if (ret != SQLITE_OK)
	{
		//LOGI("start transaction failed:%s", zErrorMsg);
		result = false;
	}
	sqlite3_free(zErrorMsg);
	return result;
}


bool CSqliteManager::commitTransaction(sqlite3 *p)
{
	bool result = true;
	char *zErrorMsg = NULL;
	int ret = sqlite3_exec(p, "commit transaction", 0, 0, &zErrorMsg); // 提交事务  
	if (ret != SQLITE_OK)
	{
		//LOGI("commit transaction failed:%s", zErrorMsg);
		result = false;
	}
	sqlite3_free(zErrorMsg);
	return result;
}


bool CSqliteManager::rollbackTransaction(sqlite3 *p)
{
	bool result = true;
	char *zErrorMsg = NULL;
	int  ret = sqlite3_exec(p, "rollback transaction", 0, 0, &zErrorMsg);
	if (ret != SQLITE_OK)
	{
		//LOGI("rollback transaction failed:%s", zErrorMsg);
		result = false;
	}
	sqlite3_free(zErrorMsg);
	return result;
}

////////////////////////////////////数据处理代码///////////////////////////////////

Result* CSqliteManager::Query(sqlite3 *p, const string &sql)
{
	Result *pRe = NULL;
	char *errmsg = NULL;
	char **dbResult;
	int nRow = 0;
	int nColumn = 0;

	//开始查询数据库
	int result = sqlite3_get_table(p, sql.c_str(), &dbResult, &nRow, &nColumn, &errmsg);
	if (result == SQLITE_OK)
	{
		pRe = new Result(nRow, nColumn, dbResult);
	}
	else
	{
		//LOGI("[sqlite] SqliteBaseV Query error:dbName=%s,msg=%s sql:%s", dbName.c_str(), errmsg, sql.c_str());
	}

	// 不论数据库查询是否成功，都释放 char** 查询结果
	sqlite3_free_table(dbResult);

	//释放内存
	sqlite3_free(errmsg);

	// 返回结果
	return pRe;
}


bool CSqliteManager::Execute(const string sql)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回创建失败信息
		return false;
	}
	else
	{
	    // 尝试执行操作
		try
		{
			//数据库启动一个事物
			if (!transaction(db))
			{
				// 提示错误信息
				std::cout<<"无法启动事务处理!"<<std::endl;
				return false;
			}

			// 若操作执行失败
			if (sqlite3_exec(db, sql.c_str(), NULL, NULL, NULL) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回失败
				return false;
			}

			//提交事务
			if (commitTransaction(db) == false)
			{
				// 回滚事务
				rollbackTransaction(db);

				// 提示错误信息
				std::cout<<"Sql执行失败: "<<sqlite3_errmsg(db)<<std::endl;

				// 返回执行失败
				return false;
			}

			// 返回执行成功
			else
				return true;
		}
		catch (...)
		{
			// 提示错误信息
			std::cout<<"Sql执行失败!"<<std::endl;

			// 返回执行失败
			return false;
		}
	}

	// 默认返回执行失败
	return false;
}

///////////////////////////////////数据库操作代码//////////////////////////////////


// 打开目标数据库连接
bool CSqliteManager::OpenDataBase(string Path)
{
	// 数据库对象默认为空
	db = NULL;

	// 若数据库已连接成功
	if (IsConnect)
	{
		// 提示错误信息
		std::cout<<"该数据库已连接，不需要再次连接!"<<std::endl;

		// 返回连接失败信息
		return false;
	}
	else
	{
		// 尝试连接目标数据库
		try
		{
			// 创建并打开目标数据库
			if (sqlite3_open(Path.c_str(), &db) != SQLITE_OK) // 若无法连接数据库
			{
				// 弹出错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 关闭数据库连接
				CloseDataBase();

				// 返回数据库连接失败的信息
				return false;
			}

			// 若数据库成功连接
			else
			{
				// 标示数据库已连接
				IsConnect = true;

				// 返回数据库连接成功的信息
				return true;
			}
		}
		// 捕捉错误
		catch (...)
		{
			// 弹出错误信息
			std::cout<<"无法连接到数据库!"<<std::endl;

			// 返回数据库连接失败的信息
			return false;
		}
	}

	// 默认返回数据库连接失败的信息
	return false;
}


// 关闭目标数据库连接
bool CSqliteManager::CloseDataBase()
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"该数据库已关闭，不需要再次关闭!"<<std::endl;

		// 返回关闭失败信息
		return false;
	}
	else
	{
		// 尝试关闭数据库连接
		try
		{
			// 若关闭数据库失败
			if (sqlite3_close(db) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回关闭失败信息
				return false;
			}
			else
			{
				// 标示数据库已断开连接
				IsConnect = false;

				// 数据库对象重置为空
				db = NULL;

				// 返回关闭成功信息
				return true;
			}
		}
		catch (...)
		{
			// 提示错误信息
			std::cout<<"数据库关闭失败!"<<std::endl;


			// 返回关闭失败信息
			return false;
		}
	}

	// 默认返回关闭失败信息
	return false;
}


// 检查目标数据库存在
bool CSqliteManager::CheckDataBase(string Path)
{
	// 尝试打开数据库
	try
	{
		sqlite3 *Check_db;
		if (sqlite3_open_v2(Path.c_str(), &Check_db, SQLITE_OPEN_READWRITE, NULL) != SQLITE_OK)
		{
			// 返回检查失败
			return false;
		}
		else
		{
			// 关闭数据库连接
			if (sqlite3_close(Check_db) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(Check_db)<<std::endl;

				// 返回关闭失败信息
				return false;
			}

			// 返回检查成功
			return true;
		}
	}
	catch (...)
	{
		// 弹出错误信息
		std::cout<<"无法打开数据库!"<<std::endl;

		// 返回检查失败
		return false;
	}
	
	// 默认返回检查失败
	return false;
}


// 建立本地数据库连接
bool CSqliteManager::LocalConnect(string Path, string Password)
{
	// 数据库对象默认为空
	db = NULL;

	// 若数据库已连接成功
	if (IsConnect)
	{
		// 提示错误信息
		std::cout<<"该数据库已连接，不需要再次连接!"<<std::endl;

		// 返回连接失败信息
		return false;
	}

	// 判断是否需要解密
	if (!Password.empty())
		DecryptionDataBase(Password);

	// 判断数据库是否存在
	if (sqlite3_open_v2(Path.c_str(), &db, SQLITE_OPEN_READWRITE, NULL) != SQLITE_OK)
	{
		// 返回打开失败 (数据库不存在)
		std::cout<<sqlite3_errmsg(db)<<std::endl;
		return false;
	}
	else
	{
		// 标示数据库已连接
		IsConnect = true;

		// 返回数据库连接成功的信息
		return true;
	}
}


// 远程连接目标数据库
bool CSqliteManager::RemoteConnect(string Url, string Password)
{
	return true;
}


// 加密目标数据库数据
bool CSqliteManager::EncryptionDataBase(string Password, string OldPassword)
{
	// 先解密
	if (!OldPassword.empty())
	{
		if(!DecryptionDataBase(OldPassword))
		{
			// 旧密码错误
			return false;
		}
	}
	
	if(sqlite3_rekey(db, Password.c_str(), Password.length()) != SQLITE_OK)
	{
		// 提示错误信息
		std::cout<<sqlite3_errmsg(db)<<std::endl;

		// 返回错误信息
		return false;
	}

	// 返回数据库加密成功的信息
	return true;
}


// 解密目标数据库数据
bool CSqliteManager::DecryptionDataBase(string Password)
{
	if(sqlite3_key(db, Password.c_str(), Password.length()) != SQLITE_OK)
	{
		// 提示错误信息 (密码错误)
		std::cout<<sqlite3_errmsg(db)<<std::endl;

		// 返回错误信息
		return false;
	}

	// 返回数据库解密成功的信息
	return true;
}


///////////////////////////////////数据表操作代码///////////////////////////////////


// 获取数据库中所有表的名称
bool CSqliteManager::GetTableName(string &TableData)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回创建失败信息
		return false;
	}
	else
	{
		// 用于判断操作是否成功
		bool IsSuccess;

		// 尝试执行获取数据表的操作
		try
		{
			// 初始化SQL语句
			char* GetSQL = "Select name,sql From sqlite_master Where type = 'table' Order By name";

			// 若获取操作失败
			if (sqlite3_prepare(db, GetSQL, -1, &stmt, (const char**)errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回获取失败
				IsSuccess = false;
			}

			// 若获取成功
			else
			{
				// 若数据为空
				if (!stmt)
				{
					// 返回空的表名
					TableData = "";

					// 返回获取失败
					IsSuccess = false;
				}

				// 若数据不为空
				else
				{
					// 数据初始化
					TableData = "";

					// 循环获得表名
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						// 得到该表表名并自加
						TableData += (char*)sqlite3_column_text(stmt, 0);

						// 加上分隔符以区分表
						TableData += ";";
					}

					// 返回获取成功
					IsSuccess = true;
				}
			}

			//释放查询结果(无论查询是否成功)
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		catch (...)
		{
			// 弹出错误信息
			std::cout<<"无法从数据库中获得表名!"<<std::endl;

			// 返回空的表名
			TableData = "";

			// 返回获取失败
			return false;
		}

		// 返回操作结果
		return IsSuccess;
	}

	// 默认返回获取失败
	return false;
}


// 获取数据库中所有表的数据
bool CSqliteManager::GetDataTable(string &TableData)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回创建失败信息
		return false;
	}
	else
	{
		// 用于判断操作是否成功
		bool IsSuccess;

		// 尝试执行获取数据表的操作
		try
		{
			// 初始化SQL语句
			char* GetSQL = "Select name,sql From sqlite_master Where type = 'table' Order By name";

			// 若获取操作失败
			if (sqlite3_prepare(db, GetSQL, -1, &stmt, (const char**)errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回获取失败
				IsSuccess = false;
			}

			// 若获取成功
			else
			{
				// 若数据为空
				if (!stmt)
				{
					// 返回空的数据
					TableData = "";

					// 返回获取失败
					IsSuccess = false;
				}

				// 若数据不为空
				else
				{
					// 数据初始化
					TableData = "";

					// 循环获得表名
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						// 得到该表数据并自加
						TableData += (char*)sqlite3_column_text(stmt, 1);

						// 加上分隔符以区分表
						TableData += ";";
					}

					// 返回获取成功
					IsSuccess = true;
				}
			}

			//释放查询结果(无论查询是否成功)
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		catch (...)
		{
			// 弹出错误信息
			std::cout<<"无法从数据库中获得表名!"<<std::endl;

			// 返回空的数据
			TableData = "";

			// 返回获取失败
			return false;
		}

		// 返回操作结果
		return IsSuccess;
	}

	// 默认返回获取失败
	return false;
}


// 获取数据库中数据表的信息
bool CSqliteManager::GetTableData(string TableName, string &TableData)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回创建失败信息
		return false;
	}
	else
	{
		// 用于判断操作是否成功
		bool IsSuccess;

		// 尝试执行获取数据表的操作
		try
		{
			// 初始化SQL语句
			char* GetSQL = "Select name,sql From sqlite_master Where type = 'table' Order By name;";

			// 若获取操作失败
			if (sqlite3_prepare(db, GetSQL, -1, &stmt, (const char**)errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回获取失败
				IsSuccess = false;
			}

			// 若获取成功
			else
			{
				// 若数据为空
				if (!stmt)
				{
					// 返回空的数据
					TableData = "";

					// 返回获取失败
					IsSuccess = false;
				}

				// 若数据不为空
				else
				{
					// 数据初始化
					TableData = "";

					// 通过表名得到数据
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						string Name = (char*)sqlite3_column_text(stmt, 0);

						if (Name == TableName)
						{
							// 得到该表语句并自加
							TableData = (char*)sqlite3_column_text(stmt, 1);

							// 返回获取成功
							IsSuccess = true;
						}
						else
						{
							// 返回获取失败
							IsSuccess = false;
						}
					}
				}
			}

			//释放查询结果(无论查询是否成功)
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		catch (...)
		{
			// 弹出错误信息
			std::cout<<"无法从数据表中获得数据!"<<std::endl;

			// 返回空的语句
			TableData = "";

			// 返回获取失败
			return false;
		}

		// 返回操作结果
		return IsSuccess;
	}

	// 默认返回获取失败
	return false;
}


// 获得数据表中所有列的名称
bool CSqliteManager::GetColName(string TableName, string &ColName)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回创建失败信息
		return false;
	}
	else
	{
		// 用于判断操作是否成功
		bool IsSuccess = false;
		int count = 0;

		CountData(TableName, "", count);
		if(count <= 0)
		{
			// 提示错误信息
			std::cout<<"目标数据表没有数据, 无法获取列数据!"<<std::endl;

			// 返回获取失败
			return false;
		}

		// 尝试执行获取数据表中列名的操作
		try
		{
			// 初始化SQL语句
			string GetSQL = "SELECT * FROM " + TableName + " limit 0,1";

			// 若获取操作失败
			if (sqlite3_get_table(db, GetSQL.c_str(), &pRes, &nRow, &nCol, &errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回获取失败
				IsSuccess = false;
			}

			// 若获取成功
			else
			{
				// 若数据为空
				if (!nRow)
				{
					// 返回空的列名
					ColName = "";

					// 返回获取失败
					IsSuccess = false;
				}

				// 若数据不为空
				else
				{
					// 数据初始化
					ColName = "";

					// 循环获得列名
					for (int i = 0; i < nRow; i++)
					{
						for (int j = 0; j < nCol; j++)
						{
							char *pv = *(pRes + nCol*i + j);

							// 列名自加
							ColName += pv;

							// 加上分隔符以区分列
							ColName += ";";
						}
						break;
					}

					// 释放资源
					if (errMsg != NULL)
					{
						sqlite3_free(errMsg);
					}

					// 返回获取成功
					IsSuccess = true;
				}
			}

			//释放查询结果(无论查询是否成功)
			sqlite3_free_table(pRes);
			pRes = NULL;
		}
		catch (...)
		{
			// 弹出错误信息
			std::cout<<"无法从数据库中获得表名!"<<std::endl;

			// 返回空的列名
			ColName = "";

			// 返回获取失败
			return false;
		}

		// 返回操作结果
		return IsSuccess;
	}

	// 默认返回获取失败
	return false;
}


// 获得数据表中所有列的类型
bool CSqliteManager::GetColType(string TableName, string &ColType)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回创建失败信息
		return false;
	}
	else
	{
		// 用于判断操作是否成功
		bool IsSuccess = false;
		int count = 0;

		CountData(TableName, "", count);
		if(count <= 0)
		{
			// 提示错误信息
			std::cout<<"目标数据表没有数据, 无法获取列数据!"<<std::endl;

			// 返回获取失败
			return false;
		}

		// 尝试执行获取数据表的操作
		try
		{
			// 初始化SQL语句
			string GetSQL = "SELECT * FROM " + TableName + " limit 0,1";

			// 若获取操作失败
			if (sqlite3_prepare(db, GetSQL.c_str(), -1, &stmt, (const char**)errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回获取失败
				IsSuccess = false;
			}

			// 若获取成功
			else
			{
				// 若数据为空
				if (!stmt)
				{
					// 返回空的类型
					ColType = "";

					// 返回获取失败
					IsSuccess = false;
				}

				// 若数据不为空
				else
				{
					// 循环获得类型
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						// 获得列数
						int nCount = sqlite3_column_count(stmt);

						// 数据初始化
						ColType = "";

						for (int i = 0; i < nCount; i++)
						{
							int nValue = sqlite3_column_int(stmt, 0);
							int nType = sqlite3_column_type(stmt, i);

							// 判断类型
							switch (nType)
							{
							case 1:
								// INTEGER 类型
								ColType += "SQLITE_INTEGER";

								// 加上分隔符以区分类型
								ColType += ";";
								break;
							case 2:
								// FLOAT 类型
								ColType += "SQLITE_FLOAT";

								// 加上分隔符以区分类型
								ColType += ";";
								break;
							case 3:
								// TEXT 类型
								ColType += "SQLITE_TEXT";

								// 加上分隔符以区分类型
								ColType += ";";
								break;
							case 4:
								// BLOB 类型
								ColType += "SQLITE_BLOB";

								// 加上分隔符以区分类型
								ColType += ";";
								break;
							case 5:
								// NULL 类型
								ColType += "SQLITE_NULL";

								// 加上分隔符以区分类型
								ColType += ";";
								break;
							}
						}
						break;
					}

					// 返回获取成功
					IsSuccess = true;
				}
			}

			//释放查询结果(无论查询是否成功)
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		catch (...)
		{
			// 弹出错误信息
			std::cout<<"无法从数据表中获得类型!"<<std::endl;

			// 返回空的类型名
			TableName = "";

			// 返回获取失败
			return false;
		}

		// 返回操作结果
		return IsSuccess;
	}

	// 默认返回获取失败
	return false;
}


// 获得数据表中所有列的数量
bool CSqliteManager::CountColName(string TableName, int &Count)
{
	// 获取列名
	string ColName;
	GetColName(TableName, ColName);

	// 分割字符串
	vector<string> pArray;
	Split(ColName, pArray, ";");

	// 获取列数并返回
	Count = pArray.size();
	return Count <= 0 ? false : true;
}


// 在目标数据库中创建数据表
bool CSqliteManager::CreateDataTable(string TableName, string Params)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回创建失败信息
		return false;
	}
	else
	{
		// 尝试执行创建数据表的操作
		try
		{
			// 初始化SQL语句
			string CreateSQL = "CREATE TABLE IF Not Exists " + TableName + " ( " + Params + " );";

			//数据库启动一个事物
			if (!transaction(db))
			{
				// 提示错误信息
				std::cout<<"无法启动事务处理!"<<std::endl;
				return false;
			}

			// 若创建操作失败
			if (sqlite3_exec(db, CreateSQL.c_str(), NULL, NULL, NULL) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回创建失败
				return false;
			}

			//提交事务
			if (commitTransaction(db) == false)
			{
				// 回滚事务
				rollbackTransaction(db);

				// 提示错误信息
				std::cout<<"创建数据表失败: "<<sqlite3_errmsg(db)<<std::endl;

				// 返回创建失败
				return false;
			}

			// 返回创建成功
			else
				return true;
		}
		catch (...)
		{
			// 提示错误信息
			std::cout<<"创建数据表失败!"<<std::endl;

			// 返回创建失败
			return false;
		}
	}

	// 默认返回创建失败
	return false;
}


// 在目标数据库中删除数据表
bool CSqliteManager::DeleteDataTable(string TableName)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回删除失败信息
		return false;
	}
	else
	{
		// 尝试执行删除数据表的操作
		try
		{
			// 初始化SQL语句
			string DeleteSQL = "Drop TABLE " + TableName + ";";

			//数据库启动一个事物
			if (!transaction(db))
			{
				// 提示错误信息
				std::cout<<"无法启动事务处理!"<<std::endl;
				return false;
			}

			// 若删除操作失败
			if (sqlite3_exec(db, DeleteSQL.c_str(), NULL, NULL, NULL) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回删除失败信息
				return false;
			}

			//提交事务
			if (commitTransaction(db) == false)
			{
				// 回滚事务
				rollbackTransaction(db);

				// 提示错误信息
				std::cout<<"删除数据表失败: "<<sqlite3_errmsg(db)<<std::endl;

				// 返回删除失败信息
				return false;
			}

			// 返回删除成功信息
			else
				return true;
		}
		catch (...)
		{
			// 提示错误信息信息
			std::cout<<"删除数据表失败!"<<std::endl;

			// 返回删除失败信息
			return false;
		}
	}

	// 默认返回删除失败信息
	return false;
}


// 在目标数据库中修改数据表
bool CSqliteManager::UpdataDataTable(string TableName, int Operation, string Params, string NewParams)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回修改失败信息
		return false;
	}
	else
	{
		// 根据参数尝试不同SQL操作
		switch (Operation)
		{
		// 重命名数据表
		case REN_TABLE:
		{
			try
			{
				// 初始化SQL语句
				string ReNameTableSQL = "ALTER TABLE " + TableName + " RENAME TO " + Params + ";";

				//数据库启动一个事物
				if (!transaction(db))
				{
					// 提示错误信息
					std::cout<<"无法启动事务处理!"<<std::endl;
					return false;
				}

				// 若添加操作失败
				if (sqlite3_exec(db, ReNameTableSQL.c_str(), NULL, NULL, NULL) != SQLITE_OK)
				{
					// 提示错误信息
					std::cout<<sqlite3_errmsg(db)<<std::endl;

					// 返回重命名失败
					return false;
				}

				//提交事务
				if (commitTransaction(db) == false)
				{
					// 回滚事务
					rollbackTransaction(db);

					// 提示错误信息
					std::cout<<"重命名数据表失败: "<<sqlite3_errmsg(db)<<std::endl;

					// 返回重命名失败
					return false;
				}

				// 返回重命名成功
				else
					return true;
			}
			catch (...)
			{
				// 提示错误信息
				std::cout<<"重命名数据表失败!"<<std::endl;

				// 返回重命名失败
				return false;
			}

		}break;

		// 添加列
		case ADD_COLUMN:
		{
			try
			{
				// 初始化SQL语句
				string Add_Column_SQL = "ALTER TABLE " + TableName + " ADD COLUMN " + Params + ";";

				//数据库启动一个事物
				if (!transaction(db))
				{
					// 提示错误信息
					std::cout<<"无法启动事务处理!"<<std::endl;
					return false;
				}

				// 若添加操作失败
				if (sqlite3_exec(db, Add_Column_SQL.c_str(), NULL, NULL, NULL) != SQLITE_OK)
				{
					// 提示错误信息
					std::cout<<sqlite3_errmsg(db)<<std::endl;

					// 返回添加失败
					return false;
				}

				//提交事务
				if (commitTransaction(db) == false)
				{
					// 回滚事务
					rollbackTransaction(db);

					// 提示错误信息
					std::cout<<"向数据表中添加列失败: "<<sqlite3_errmsg(db)<<std::endl;

					// 返回添加失败
					return false;
				}

				// 返回添加成功
				else
					return true;
			}
			catch (...)
			{
				// 提示错误信息
				std::cout<<"向数据表中添加列失败!"<<std::endl;

				// 返回添加失败
				return false;
			}

		}break;

		// 删除列
		case DEL_COLUMN:
		{
			try
			{
				// 得到所有列名
				string ColumnName, params = "," + Params + ",";
				GetColName(TableName, ColumnName);

				// 若找不到目标列
				if (!strstr(ColumnName.c_str(), Params.c_str()))
				{
					// 提示错误信息
					std::cout<<"找不到目标列，删除失败!"<<std::endl;

					// 返回删除失败
					return false;
				}
				else
				{
					// 若无法分析目标列的数据
					if (Replace((char*)ColumnName.c_str(), ";", ",") == "")
					{
						// 提示错误信息
						std::cout<<"无法分析目标列，删除失败!"<<std::endl;

						// 返回删除失败
						return false;
					}
					else
					{
						//初始化SQL语句
						string Cre_Column_SQL = "Create Table Temp As Select " + ColumnName + " From " + TableName + ";";

						// 去掉目标列
					    Replace((char*)Cre_Column_SQL.c_str(), (char*)params.c_str(), "");

						//数据库启动一个事物
						if (!transaction(db))
						{
							// 提示错误信息
							std::cout<<"无法启动事务处理!"<<std::endl;
							return false;
						}

						// 若创建操作失败
						if (sqlite3_exec(db, Cre_Column_SQL.c_str(), NULL, NULL, NULL) != SQLITE_OK)
						{
							// 提示错误信息
							std::cout<<sqlite3_errmsg(db)<<std::endl;

							// 返回操作失败
							return false;
						}

						//提交事务
						if (commitTransaction(db) == false)
						{
							// 回滚事务
							rollbackTransaction(db);

							// 提示错误信息
							std::cout<<"从数据表中删除列失败: "<<sqlite3_errmsg(db)<<std::endl;

							// 返回删除失败
							return false;
						}
						else
						{
							// 删除原数据表
							if (!DeleteDataTable(TableName))
							{
								// 提示错误信息
								std::cout<<"无法删除原数据表!"<<std::endl;

								// 返回删除失败
								return false;
							}
							else
							{
								// 若重命名 Temp 表为目标数据表失败
								if (!UpdataDataTable("Temp", REN_TABLE, TableName))
								{
									// 提示错误信息
									std::cout<<"无法重命名Temp数据表!"<<std::endl;

									// 返回删除失败
									return false;
								}
								else
								{
									// 返回删除目标列成功
									return true;
								}
							}
						}
					}
				}
			}
			catch (...)
			{
				// 提示错误信息
				std::cout<<"从数据表中删除列失败!"<<std::endl;

				// 返回删除失败
				return false;
			}

		}break;

		// 重命名列
		case REN_COLUMN:
		{
			try
			{
				// 得到目标数据表的SQL数据
				string TableData;
				GetTableData(TableName, TableData);

				// 若找不到目标表
				if (Replace((char*)TableData.c_str(), (char*)TableName.c_str(), "Temp") == "")
				{
					// 提示错误信息
					std::cout<<"找不到目标表，修改失败!"<<std::endl;

					// 返回修改失败
					return false;
				}
				else
				{
					// 若找不到目标列
					if (Replace((char*)TableData.c_str(), (char*)Params.c_str(), (char*)NewParams.c_str()) == "")
					{
						// 提示错误信息
						std::cout<<"找不到目标列，修改失败!"<<std::endl;

						// 返回修改失败
						return false;
					}

					//数据库启动一个事物
					if (!transaction(db))
					{
						// 提示错误信息
						std::cout<<"无法启动事务处理:创建临时数据表!"<<std::endl;
						return false;
					}

					// 若创建临时表 Temp 失败
					if (sqlite3_exec(db, TableData.c_str(), NULL, NULL, NULL) != SQLITE_OK)
					{
						// 提示错误信息
						std::cout<<sqlite3_errmsg(db)<<std::endl;

						// 返回修改失败
						return false;
					}

					//提交事务
					if (commitTransaction(db) == false)
					{
						// 回滚事务
						rollbackTransaction(db);

						// 提示错误信息
						std::cout<<"无法创建临时数据表: "<<sqlite3_errmsg(db)<<std::endl;

						// 返回修改失败
						return false;
					}
					else
					{
						// 导入原表数据
						string Insert_Sql = "INSERT INTO Temp SELECT * FROM " + TableName + ";";

						//数据库启动一个事物
						if (!transaction(db))
						{
							// 提示错误信息
							std::cout<<"无法启动事务处理!"<<std::endl;
							return false;
						}

						// 若导入操作失败
						if (sqlite3_exec(db, Insert_Sql.c_str(), NULL, NULL, NULL) != SQLITE_OK)
						{
							// 提示错误信息
							std::cout<<sqlite3_errmsg(db)<<std::endl;

							// 返回修改失败
							return false;
						}

						//提交事务
						if (commitTransaction(db) == false)
						{
							// 回滚事务
							rollbackTransaction(db);

							// 提示错误信息
							std::cout<<"无法导入原数据表数据: "<<sqlite3_errmsg(db)<<std::endl;

							// 返回修改失败
							return false;
						}
						else
						{
							// 删除原数据表
							if (!DeleteDataTable(TableName))
							{
								// 提示错误信息
								std::cout<<"无法删除原数据表!"<<std::endl;

								// 返回修改失败
								return false;
							}
							else
							{
								// 若重命名 Temp 表为目标数据表失败
								if (!UpdataDataTable("Temp", REN_TABLE, TableName))
								{
									// 提示错误信息
									std::cout<<"无法重命名原数据表!"<<std::endl;

									// 返回修改失败
									return false;
								}
								else
								{
									// 返回修改列成功
									return true;
								}
							}
						}
					}
				}
			}
			catch (...)
			{
				// 提示错误信息
				std::cout<<"在数据表中修改列失败!"<<std::endl;

				// 返回修改失败
				return false;
			}

		}break;

		// 默认什么也不做
		default:
		{
			// 提示错误信息
			std::cout<<"无效的命令，本次SQL无操作!"<<std::endl;
		}break;
		}
	}

	// 默认返回修改失败信息
	return false;
}


// 在目标数据库中检查数据表     ( 检查数据表是否存在 )
bool CSqliteManager::CheckDataTable (string TableName)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回检查失败信息
		return false;
	}
	else
	{
		// 尝试执行检查数据表的操作
		try
		{
			// 定义比对变量
			string CheckName;
			if (!GetTableName(CheckName))
			{
				// 弹出错误信息
				std::cout<<"无法从数据库中获得表名!"<<std::endl;

				// 返回检查失败
				return false;
			}
			else
			{
				// 比对是否存在目标数据表
				if (strstr(CheckName.c_str(), TableName.c_str()))
				{
					// 返回存在目标数据表
					return true;
				}
				else
				{
					// 返回不存在目标数据表
					return false;
				}
			}
		}
		catch (...)
		{
			// 提示错误信息
			std::cout<<"无法从数据库中获得表名!"<<std::endl;

			// 返回检查失败
			return false;
		}
	}

	// 默认返回检查失败信息
	return false;
}


// 在目标数据库中统计数据表     ( 统计数据表总共数量 )
bool CSqliteManager::CountDataTable (int &Count)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回创建失败信息
		return false;
	}
	else
	{
		// 用于判断操作是否成功
		bool IsSuccess;

		// 初始化数量
		Count = 0;


		// 尝试执行获取数据表的操作
		try
		{
			// 初始化SQL语句
			string GetSQL = "Select name,sql From sqlite_master Where type = 'table' Order By name";

			// 若获取操作失败
			if (sqlite3_prepare(db, GetSQL.c_str(), -1, &stmt, (const char**)errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回获取失败
				IsSuccess = false;
			}

			// 若获取成功
			else
			{
				// 若数据为空
				if (!stmt)
				{
					// 返回 0 个数量
					Count = 0;

					// 返回获取失败
					IsSuccess = false;
				}

				// 若数据不为空
				else
				{
					// 数据初始化
					Count = 0;

					// 循环获得数据表
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						// 数据数量自加1
						Count ++;
					}

					// 返回获取成功
					IsSuccess = true;
				}
			}

			//释放查询结果(无论查询是否成功)
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		catch (...)
		{
			// 弹出错误信息
			std::cout<<"无法从数据库中获得表名!"<<std::endl;

			// 返回 0 个数量
			Count = 0;

			// 返回获取失败
			return false;
		}

		// 返回操作结果
		return IsSuccess;
	}

	// 默认返回获取失败
	return false;
}


// 从目标数据库中导入数据表 ()
bool CSqliteManager::ImportDataTable(string TableName, string TargetDataBase)
{
	// 若数据库未连接成功
	/*if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回创建失败信息
		return false;
	}
	else
	{
		// 用于判断操作是否成功
		//bool IsSuccess;

		sqlite3 *pTarget;

		// 尝试连接目标数据库
		try
		{
			// 创建并打开目标数据库
			if (sqlite3_open(TargetDataBase.c_str(), &pTarget) != SQLITE_OK) // 若无法连接数据库
			{
				// 弹出错误信息
				std::cout<<sqlite3_errmsg(pTarget)<<std::endl;

				// 若关闭数据库失败
				if (sqlite3_close(pTarget) != SQLITE_OK)
				{
					// 提示错误信息
					std::cout<<sqlite3_errmsg(pTarget)<<std::endl;

					// 返回关闭失败信息
					return false;
				}

				// 返回数据库连接失败的信息
				return false;
			}

			// 若数据库成功连接
			else
			{
				try
				{
					// 检查数据表是否存在
					if (CheckDataTable(TableName))
					{
						//数据库启动一个事物
						if (!transaction(db))
						{
							// 提示错误信息
							std::cout<<"无法启动事务处理!"<<std::endl;
							return false;
						}

						// 初始化SQL语句
						string InsertSQL;//"INSERT INTO " + TableName + " VALUES( " + Params + " );";

						// 若添加操作失败
						if (sqlite3_exec(db, InsertSQL.c_str(), NULL, NULL, &errMsg) != SQLITE_OK)
						{
							// 提示错误信息
							std::cout<<sqlite3_errmsg(db)<<std::endl;

							// 返回添加失败信息
							return false;
						}

						//提交事务
						if (commitTransaction(db) == false)
						{
							// 回滚事务
							rollbackTransaction(db);

							// 提示错误信息
							std::cout<<"添加数据操作失败:"<<sqlite3_errmsg(db)<<std::endl;

							// 返回添加失败信息
							return false;
						}

					}
					else
					{
						// 弹出错误信息
						std::cout<<"目标数据表不存在,无法导入数据!"<<std::endl;

						// 返回数据导入失败的信息
						return false;
					}
				}
				// 捕捉错误
				catch (...)
				{
					// 弹出错误信息
					std::cout<<"无法导入数据到目标数据库!"<<std::endl;

					// 返回数据导入失败的信息
					return false;
				}
			}
		}
		// 捕捉错误
		catch (...)
		{
			// 弹出错误信息
			std::cout<<"无法连接到目标数据库!"<<std::endl;

			// 返回数据库连接失败的信息
			return false;
		}

		// 查找目标数据库是否存在
		//CFileFind Finder;
		//BOOL Status = Finder.FindFile(TargetDataBase);

		//// 若存在目标数据库则尝试连接目标数据库
		//if (Status)
		//{
		//	
		//}
		//else
		//{
		//	// 弹出错误信息
		//	std::cout<<"无法连接到目标数据库, 因为目标数据库不存在!"<<std::endl;

		//	// 返回数据库连接失败的信息
		//	return false;
		//}
	}*/

    // 默认返回导入失败
	return false;
}


// 向目标数据库中导出数据表
bool CSqliteManager::ExportDataTable(string TableName, string TargetDataBase)
{
	return true;
}


///////////////////////////////////数据项操作代码/////////////////////////////////////


// 向目标数据表中添加数据项
bool CSqliteManager::InsertData(string TableName, string Params)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回添加失败信息
		return false;
	}
	else
	{
		// 尝试执行添加数据的操作
		try
		{
			// 初始化SQL语句
			string InsertSQL = "INSERT INTO " + TableName + " VALUES( " + Params + " );";

			//数据库启动一个事物
			if (!transaction(db))
			{
				std::cout<<"无法启动事务处理!"<<std::endl;

				return false;
			}

			// 若添加操作失败
			if (sqlite3_exec(db, InsertSQL.c_str(), NULL, NULL, &errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回添加失败信息
				return false;
			}

			//提交事务
			if (commitTransaction(db) == false)
			{
				// 回滚事务
				rollbackTransaction(db);

				// 提示错误信息
				std::cout<<"添加数据操作失败: "<<sqlite3_errmsg(db)<<std::endl;

				// 返回添加失败信息
				return false;
			}

			// 返回添加成功信息
			else
				return true;
		}
		catch (...)
		{
			// 提示错误信息
			std::cout<<"添加数据操作失败!"<<std::endl;

			// 返回添加失败信息
			return false;
		}
	}

	// 默认返回添加失败信息
	return false;
}


// 从目标数据表中删除数据项
bool CSqliteManager::DeleteData(string TableName, string Params)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回删除失败信息
		return false;
	}
	else
	{
		// 尝试执行从数据表中删除数据的操作
		try
		{
			// 初始化SQL语句
			string DeleteSQL;
			if (!Params.empty())
				DeleteSQL = "Delete From " + TableName + " Where " + Params + ";";
			else
				DeleteSQL = "Delete From " + TableName + ";";

			//数据库启动一个事物
			if (!transaction(db))
			{
				std::cout<<"无法启动事务处理!"<<std::endl;

				return false;
			}

			// 若删除操作失败
			if (sqlite3_exec(db, DeleteSQL.c_str(), NULL, NULL, &errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回删除失败
				return false;
			}

			//提交事务
			if (commitTransaction(db) == false)
			{
				// 回滚事务
				rollbackTransaction(db);

				// 提示错误信息
				std::cout<<"添加数据操作失败: "<<sqlite3_errmsg(db)<<std::endl;

				// 返回删除失败
				return false;
			}

			// 返回删除成功
			else
				return true;
		}
		catch (...)
		{
			// 提示错误信息
			std::cout<<"删除数据失败!"<<std::endl;

			// 返回删除失败
			return false;
		}
	}
}


// 从目标数据表中修改数据项
bool CSqliteManager::UpdataData(string TableName, string Column, string NewData, string Params)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回修改失败信息
		return false;
	}
	else
	{
		// 尝试执行修改数据的操作
		try
		{
			// 初始化SQL语句
			string UpData_SQL;

			// 若参数为空
			if (Params.empty())
			{
				UpData_SQL = "UPDATE " + TableName + " SET " + Column + " = " + "'" + NewData + "'" + ";";
			}
			else
			{
				UpData_SQL = "UPDATE " + TableName + " SET " + Column + " = " + "'" + NewData + "'" + " Where " + Params + ";";
			}
			
			//数据库启动一个事物
			if (!transaction(db))
			{
				std::cout<<"无法启动事务处理!"<<std::endl;

				return false;
			}

			// 若修改操作失败
			if (sqlite3_exec(db, UpData_SQL.c_str(), NULL, NULL, &errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回修改失败信息
				return false;
			}

			//提交事务
			if (commitTransaction(db) == false)
			{
				// 回滚事务
				rollbackTransaction(db);

				// 提示错误信息
				std::cout<<"修改数据操作失败: "<<sqlite3_errmsg(db)<<std::endl;

				// 返回修改失败信息
				return false;
			}

			// 返回修改成功信息
			else
				return true;
		}
		catch (...)
		{
			// 提示错误信息
			std::cout<<"修改数据操作失败!"<<std::endl;

			// 返回修改失败信息
			return false;
		}
	}

	// 默认返回修改失败信息
	return false;
}


// 从目标数据表中修改数据项
bool CSqliteManager::UpdataData(string TableName, string ColumnParams, string Params)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回修改失败信息
		return false;
	}
	else
	{
		// 尝试执行修改数据的操作
		try
		{
			// 初始化SQL语句
			string UpData_SQL;

			// 若参数为空
			if (Params.empty())
			{
				UpData_SQL = "UPDATE " + TableName + " SET " + ColumnParams + ";";
			}
			else
			{
				UpData_SQL = "UPDATE " + TableName + " SET " + ColumnParams + " Where " + Params + ";";
			}

			//数据库启动一个事物
			if (!transaction(db))
			{
				std::cout<<"无法启动事务处理!"<<std::endl;

				return false;
			}

			// 若修改操作失败
			if (sqlite3_exec(db, UpData_SQL.c_str(), NULL, NULL, &errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回修改失败信息
				return false;
			}

			//提交事务
			if (commitTransaction(db) == false)
			{
				// 回滚事务
				rollbackTransaction(db);

				// 提示错误信息
				std::cout<<"修改数据操作失败: "<<sqlite3_errmsg(db)<<std::endl;

				// 返回修改失败信息
				return false;
			}

			// 返回修改成功信息
			else
				return true;
		}
		catch (...)
		{
			// 提示错误信息
			std::cout<<"修改数据操作失败!"<<std::endl;

			// 返回修改失败信息
			return false;
		}
	}

	// 默认返回修改失败信息
	return false;
}


// 从目标数据表中查询数据项
bool CSqliteManager::SelectData(string TableName, vector<string> &pResult,  string Params, string Order, string Limit, int SortMode, bool DISTINCT, string COUNT, string COLUMN, string GROUP, string HAVING)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回查询失败信息
		return false;
	}
	else
	{
		// 尝试执行查询数据的操作
		try
		{
			// 清空原先的结果集
			pResult.clear();

			// 定义查询参数
			int rc, i, ncols;
			const char *tail;

			// 初始化SQL语句
			string Select_SQL;
			string  DISTINCT_Text = "";

			string  SortText = "";

			// 若数据列为空
			if (COLUMN.empty())
			{
				if (DISTINCT)
				{
					// 提示错误
					std::cout<<"列为空的情况下, DISTINCT 不可以为真!"<<std::endl;

					return false;
				}
				if (!COUNT.empty())
				{
					// 提示错误
					std::cout<<"列为空的情况下, COUNT 不可以不为空!"<<std::endl;

					return false;
				}

				COLUMN = "*";
			}
			else
			{
				// 若去除重复为真
				if (DISTINCT)
				{
					DISTINCT_Text = "DISTINCT ";
				}

				// 若计数不为空
				if (!COUNT.empty())
				{
					COUNT = ", " + COUNT;
				}
			}

			// 若参数为空
			if (Params.empty())
			{
				if (!GROUP.empty())
				{
					// 提示错误
					std::cout<<"参数为空的情况下, 分组不可以不为空!"<<std::endl;

					return false;
				}

				if (!HAVING.empty())
				{
					// 提示错误
					std::cout<<"参数为空的情况下, 筛选不可以不为空!"<<std::endl;

					return false;
				}

				if (!Order.empty())
				{
					// 提示错误
					std::cout<<"参数为空的情况下, 排序不可以不为空!"<<std::endl;

					return false;
				}

				if (!Limit.empty())
				{
					// 提示错误
					std::cout<<"参数为空的情况下, 偏移量不可以不为空!"<<std::endl;

					return false;
				}

				// 构造查询SQL
				Select_SQL = "SELECT " + DISTINCT_Text + COLUMN + COUNT + " FROM " + TableName + ";";
			}
			else
			{
				// 若分组不为空
				if (!GROUP.empty())
				{
					GROUP = " GROUP By " + GROUP;
				}

				// 若筛选不为空
				if (!HAVING.empty())
				{
					HAVING = " HAVING " + HAVING;
				}

				// 若排序不为空
				if (!Order.empty())
				{
					Order = " Order by " + Order;

					// 根据代码进行排序
					if (SortMode == MOD_ASC)
					{
						SortText = " ASC ";
					}
					else
					{
						SortText = " DESC ";
					}
				}
				else
				{
					Order    = "";
					SortText = "";
				}

				// 若偏移量不为空
				if (!Limit.empty())
				{
					Limit = " Limit " + Limit;
				}

				// 构造查询SQL
				Select_SQL = "SELECT " + DISTINCT_Text + COLUMN + COUNT + " FROM " + TableName + " Where " + Params + GROUP + HAVING + Order + SortText + Limit + ";";
			}

			// 若预处理操作成功
			if (sqlite3_prepare(db, Select_SQL.c_str(), Select_SQL.length(), &stmt, &tail) == SQLITE_OK)
			{
				rc    = sqlite3_step(stmt);
				ncols = sqlite3_column_count(stmt);
				while (rc == SQLITE_ROW)
				{
					for (i = 0; i < ncols; i++)
					{
						// 循环将查询结果放入结果集
						pResult.push_back((char*)sqlite3_column_text(stmt, i));
					}
					
					rc = sqlite3_step(stmt);
				}

				//释放statement
				sqlite3_finalize(stmt);

				// 返回查询成功信息
				return true;
			}
			else
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回查询失败信息
				return false;
			}

			// 释放变量
			sqlite3_finalize(stmt);
		}
		catch (...)
		{
			// 提示错误信息
			std::cout<<"查询数据操作失败!"<<std::endl;

			// 返回查询失败信息
			return false;
		}
	}

	// 默认返回查询失败信息
	return false;
}


// 从目标数据表中查询列数据
bool CSqliteManager::SelectData(string TableName, string &pResult, int Col, string Params, string Order, string Limit, int SortMode, bool DISTINCT, string COUNT, string COLUMN, string GROUP, string HAVING)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回查询失败信息
		return false;
	}
	else
	{
		// 尝试执行查询数据的操作
		try
		{
			// 清空原先的结果
			pResult = "";

			// 定义查询参数
			int rc, i, ncols;
			const char *tail;

			// 初始化SQL语句
			string Select_SQL;
			string  DISTINCT_Text = "";

			string  SortText = "";

			// 若数据列为空
			if (COLUMN.empty())
			{
				if (DISTINCT)
				{
					// 提示错误
					std::cout<<"列为空的情况下, DISTINCT 不可以为真!"<<std::endl;

					return false;
				}
				if (!COUNT.empty())
				{
					// 提示错误
					std::cout<<"列为空的情况下, COUNT 不可以不为空!"<<std::endl;

					return false;
				}

				COLUMN = "*";
			}
			else
			{
				// 若去除重复为真
				if (DISTINCT)
				{
					DISTINCT_Text = "DISTINCT ";
				}

				// 若计数不为空
				if (!COUNT.empty())
				{
					COUNT = ", " + COUNT;
				}
			}

			// 若参数为空
			if (Params.empty())
			{
				if (!GROUP.empty())
				{
					// 提示错误
					std::cout<<"参数为空的情况下, 分组不可以不为空!"<<std::endl;

					return false;
				}

				if (!HAVING.empty())
				{
					// 提示错误
					std::cout<<"参数为空的情况下, 筛选不可以不为空!"<<std::endl;

					return false;
				}

				if (!Order.empty())
				{
					// 提示错误
					std::cout<<"参数为空的情况下, 排序不可以不为空!"<<std::endl;

					return false;
				}

				if (!Limit.empty())
				{
					// 提示错误
					std::cout<<"参数为空的情况下, 偏移量不可以不为空!"<<std::endl;

					return false;
				}

				// 构造查询SQL
				Select_SQL = "SELECT " + DISTINCT_Text + COLUMN + COUNT + " FROM " + TableName + ";";
			}
			else
			{
				// 若分组不为空
				if (!GROUP.empty())
				{
					GROUP = " GROUP By " + GROUP;
				}

				// 若筛选不为空
				if (!HAVING.empty())
				{
					HAVING = " HAVING " + HAVING;
				}

				// 若排序不为空
				if (!Order.empty())
				{
					Order = " Order by " + Order;

					// 根据代码进行排序
					if (SortMode == MOD_ASC)
					{
						SortText = " ASC ";
					}
					else
					{
						SortText = " DESC ";
					}
				}
				else
				{
					Order    = "";
					SortText = "";
				}

				// 若偏移量不为空
				if (!Limit.empty())
				{
					Limit = " Limit " + Limit;
				}

				// 构造查询SQL
				Select_SQL = "SELECT " + DISTINCT_Text + COLUMN + COUNT + " FROM " + TableName + " Where " + Params + GROUP + HAVING + Order + SortText + Limit + ";";
			}

			// 若预处理操作成功
			if (sqlite3_prepare(db, Select_SQL.c_str(), Select_SQL.length(), &stmt, &tail) == SQLITE_OK)
			{
				// r若目标列为0
				if (Col == 0)
					Col = 1;

				// 执行预处理
				rc = sqlite3_step(stmt);
				ncols = sqlite3_column_count(stmt);
				while (rc == SQLITE_ROW)
				{
					// 循环得到查询结果
					for (i = 0; i < ncols; i++)
					{
						// 若目标列与i相等
						if (i == Col)
						{
							// 得到目标数据
							pResult = (char*)sqlite3_column_text(stmt, i);
						}
					}

					rc = sqlite3_step(stmt);
				}

				//释放statement
				sqlite3_finalize(stmt);

				// 返回查询成功信息
				return true;
			}
			else
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回查询失败信息
				return false;
			}

			// 释放变量
			sqlite3_finalize(stmt);
		}
		catch (...)
		{
			// 提示错误信息
			std::cout<<"查询数据操作失败!"<<std::endl;

			// 返回查询失败信息
			return false;
		}
	}

	// 默认返回查询失败信息
	return false;
}


// 从目标数据表中查询数据值
bool CSqliteManager::SelectData(string TableName, string &pResult, string Column, string Params, string Order, string Limit, int SortMode, bool DISTINCT, string COUNT, string COLUMN, string GROUP, string HAVING)
{
	// 链表对象
	vector<string> Values;

	// 查询数据
	if(! SelectData(TableName, Values, Params, Order, Limit, SortMode, DISTINCT, COUNT, COLUMN, GROUP, HAVING) )
	{
		// 提示错误信息
		std::cout<<"查询数据操作失败!"<<std::endl;

		// 返回查询失败
		return false;
	}

	// 获取列名
	string ColName;
	GetColName(TableName, ColName);

	// 分割字符串
	vector<string> pArray;
	Split(ColName, pArray, ";");

	/*stringrray pArray;
	Split(ColName, _T(";"), pArray);*/

	// 匹配列
	int col = 0, size = pArray.size();
	for(int c = 0; c < size; c++)
	{
		if(pArray.at(c) == Column)
			col = c;
	}

	// 循环取值
	for(unsigned int i = 0; i < Values.size(); i += size)
	{
		// 保存结果
		pResult = Values.at(i + col);
	}

	// 返回查询成功信息
	return true;
}


// 在目标数据表中检查数据项
bool CSqliteManager::CheckData (string TableName, int Col, string Params, string Order, string Limit, int SortMode, bool DISTINCT, string COUNT, string COLUMN, string GROUP, string HAVING)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回查询失败信息
		return false;
	}
	else
	{
		string CheckData = "";

		if (SelectData(TableName, CheckData, Col, Params, Order, Limit, SortMode, DISTINCT, COUNT, COLUMN, GROUP, HAVING))
		{
			if (!CheckData.empty() && CheckData != "")
			{
				// 返回目标数据存在
				return true;
			}
			else
			{
				// 返回目标数据不存在
				return false;
			}
		}
		else
		{
			// 提示错误信息
			std::cout<<"数据库查询失败!"<<std::endl;

			// 返回查询失败信息
			return false;
		}
	}

	// 默认返回查询失败信息
	return false;
}


// 在目标数据表中检查数据值
bool CSqliteManager::CheckData (string TableName, string Column, string Params, string Order, string Limit, int SortMode, bool DISTINCT, string COUNT, string COLUMN, string GROUP, string HAVING)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回查询失败信息
		return false;
	}
	else
	{
		string CheckData = "";
		if(Column == "" || Column.empty())
			Column = "*";

		if (SelectData(TableName, CheckData, Column, Params, Order, Limit, SortMode, DISTINCT, COUNT, COLUMN, GROUP, HAVING))
		{
			if (!CheckData.empty() && CheckData != "")
			{
				// 返回目标数据存在
				return true;
			}
			else
			{
				// 返回目标数据不存在
				return false;
			}
		}
		else
		{
			// 提示错误信息
			std::cout<<"数据库查询失败!"<<std::endl;

			// 返回查询失败信息
			return false;
		}
	}

	// 默认返回查询失败信息
	return false;
}


// 统计目标数据表中数据数项
bool CSqliteManager::CountData (string TableName, string Params, int &Count)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回创建失败信息
		return false;
	}
	else
	{
		// 用于判断操作是否成功
		bool IsSuccess;

		// 尝试执行获取数据表中列名的操作
		try
		{
			// 初始化SQL语句
			string GetSQL;

			// 若参数为空
			if (Params.empty())
			{
				GetSQL = "SELECT * FROM " + TableName + ";";
			}

			// 若参数不为空
			else
			{
				GetSQL = "SELECT * FROM " + TableName + " Where " + Params + ";";
			}
			

			// 若获取操作失败
			if (sqlite3_get_table(db, GetSQL.c_str(), &pRes, &nRow, &nCol, &errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回获取失败
				IsSuccess = false;
			}

			// 若获取成功
			else
			{
				// 若数据为空
				if (!nRow)
				{
					// 返回0个数据
					Count = 0;

					// 返回获取失败
					IsSuccess = false;
				}

				// 若数据不为空
				else
				{
					// 得到数据个数
					Count = nRow;

					// 释放资源
					if (errMsg != NULL)
					{
						sqlite3_free(errMsg);
					}

					// 返回获取成功
					IsSuccess = true;
				}
			}

			//释放查询结果(无论查询是否成功)
			sqlite3_free_table(pRes);
		}
		catch (...)
		{
			// 弹出错误信息
			std::cout<<"无法从数据库中获得表名!"<<std::endl;

			// 返回0个数据
			Count = 0;

			// 返回获取失败
			return false;
		}

		// 返回操作结果
		return IsSuccess;
	}

	// 默认返回获取失败
	return false;
}


// 重置目标数据表数据数编号
bool CSqliteManager::Truncate  (string TableName)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		std::cout<<"数据库未连接，请先连接数据库!"<<std::endl;

		// 返回修改失败信息
		return false;
	}
	else
	{
		// 尝试执行修改数据的操作
		try
		{
			// 初始化SQL语句
			string Truncate_SQL = "DELETE FROM sqlite_sequence WHERE name='" + TableName + "';";

			//数据库启动一个事物
			if (!transaction(db))
			{
				std::cout<<"无法启动事务处理!"<<std::endl;

				return false;
			}

			// 若修改操作失败
			if (sqlite3_exec(db, Truncate_SQL.c_str(), NULL, NULL, &errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				std::cout<<sqlite3_errmsg(db)<<std::endl;

				// 返回修改失败信息
				return false;
			}

			//提交事务
			if (commitTransaction(db) == false)
			{
				// 回滚事务
				rollbackTransaction(db);

				// 提示错误信息
				std::cout<<"重置数据编号操作失败: "<<sqlite3_errmsg(db)<<std::endl;

				// 返回修改失败信息
				return false;
			}
			else
			{
				Truncate_SQL = "DELETE FROM '" + TableName + "';";
				//数据库启动一个事物
				if (!transaction(db))
				{
					std::cout<<"无法启动事务处理!"<<std::endl;

					return false;
				}

				// 若修改操作失败
				if (sqlite3_exec(db, Truncate_SQL.c_str(), NULL, NULL, &errMsg) != SQLITE_OK)
				{
					// 提示错误信息
					std::cout<<sqlite3_errmsg(db)<<std::endl;

					// 返回修改失败信息
					return false;
				}

				//提交事务
				if (commitTransaction(db) == false)
				{
					// 回滚事务
					rollbackTransaction(db);

					// 提示错误信息
					std::cout<<"重置数据编号操作失败: "<<sqlite3_errmsg(db)<<std::endl;

					// 返回修改失败信息
					return false;
				}

				// 返回修改成功信息
				return true;
			}
		}
		catch (...)
		{
			// 提示错误信息
			std::cout<<"重置数据编号操作失败!"<<std::endl;

			// 返回修改失败信息
			return false;
		}
	}

	// 默认返回修改失败信息
	return false;
}


///////////////////////////////////数据项计算代码////////////////////////////////////////////////////


// 从目标数据表中求数据项和
bool CSqliteManager::SumData(string TableName, string Column, string Params, double &SumData)
{
	// 链表对象
	vector<string> Result;

	// 查询数据
	if(!SelectData(TableName, Result, Params))
	{
		// 提示错误信息
		std::cout<<"查询数据操作失败!"<<std::endl;

		return false;
	}

	// 获取列名
	string ColName;
	GetColName(TableName, ColName);

	// 分割字符串
	vector<string> pArray;
	Split(ColName, pArray, ";");

	// 匹配列
	int col = 0, size = pArray.size();
	for(int c = 0; c < size; c++)
	{
		if(pArray.at(c) == Column)
		{
			col = c;
			break;
		}
	}

	//从这里开始进行转化，这是一个宏定义
	//USES_CONVERSION;

	// 初始化
	SumData = 0;

	// 循环相加
	for(unsigned int i = 0; i < Result.size(); i += size)
	{
		/*const char* temp = T2A(Result.at(i + col).GetBuffer(0));
		Result.at(i + col).ReleaseBuffer();*/

		// 转换类型并求和
		double value = atof(Result.at(i + col).c_str());
		SumData += value;
	}

	//返回查询成功 
	return true;
}


// 从目标数据表中求数据项积
bool CSqliteManager::ProductData(string TableName, string Column, string Params, double &ProductData)
{
	// 链表对象
	vector<string> Result;

	// 查询数据
	if(!SelectData(TableName, Result, Params))
	{
		// 提示错误信息
		std::cout<<"查询数据操作失败!"<<std::endl;

		return false;
	}
	
	// 获取列名
	string ColName;
	GetColName(TableName, ColName);

	// 分割字符串
	vector<string> pArray;
	Split(ColName, pArray, ";");

	// 匹配列
	int col = 0, size = pArray.size();
	for(int c = 0; c < size; c++)
	{
		if(pArray.at(c) == Column)
			col = c;
	}

	// 初始化
	ProductData = 1;

	// 循环相加
	for(unsigned int i = 0; i < Result.size(); i += size)
	{
		// 转换类型并求和
		double value = atof(Result.at(i + col).c_str());
		ProductData *= value;
	}

	//返回查询成功 
	return true;
}


// 从目标数据表中求数据项平均数
bool CSqliteManager::AvgData(string TableName, string Column, string Params, double &AvgData)
{
	// 链表对象
	vector<string> Result;

	// 查询数据
	if(!SelectData(TableName, Result, Params))
	{
		// 提示错误信息
		std::cout<<"查询数据操作失败!"<<std::endl;

		return false;
	}
	
	// 先初始化并求和
	int Number   = 0;
	double value = 0;
	AvgData      = 0;

	SumData(TableName, Column, Params, value);
	CountColName(TableName, Number);

	// 求平均数
	AvgData = value / ( Result.size() / Number);

	//返回查询成功 
	return true;
}


// 从目标数据表中求数据项众数
bool CSqliteManager::PluData(string TableName, string Column, string Params, double &PluData)
{
	// 链表对象
	vector<string> Result;
	vector<double>  Values;

	// 查询数据
	if(!SelectData(TableName, Result, Params))
	{
		// 提示错误信息
		std::cout<<"查询数据操作失败!"<<std::endl;

		return false;
	}

	// 初始化
	PluData = 0;

	// 获取列名
	string ColName;
	GetColName(TableName, ColName);

	// 分割字符串
	vector<string> pArray;
	Split(ColName, pArray, ";");

	// 匹配列
	int col = 0, size = pArray.size();
	for(int c = 0; c < size; c++)
	{
		if(pArray.at(c) == Column)
			col = c;
	}

	// 循环取值
	for(unsigned int i = 0; i < Result.size(); i += size)
	{
		// 转换类型
		double value = atof(Result.at(i + col).c_str());
		
		// 保存结果
		Values.push_back(value);
	}

	// 升序排序
	sort(Values.begin(), Values.end());

	// 出现次数 & 最大众数 & 众数下标
	int count = 0, max = 0, index = 0;

	// 遍历结果
	for(unsigned int j = 0; j < Values.size(); j++)
	{
		// 记录出现次数,存在连续两个数相等，则当前众数+1
		if(Values[j] == Values[j +1])
		{
			count++;
		}
		else
		{
			// 充值计数
			count = 0;
		}

		// 如果当前众数超过最大众数则替换
		if(count > max)
		{
			max = count;
			index = j;
		}
	}

	// 不存在众数
	if(max == 0)
	{
		// 提示错误信息
		std::cout<<"众数不存在!"<<std::endl;
	}
	else
	{
		// 求出众数
		PluData = Values[index];
	}

	//返回查询成功
	return true;
}


// 从目标数据表中求数据项中位数
bool CSqliteManager::MidData(string TableName, string Column, string Params, double &MidData)
{
	// 链表对象
	vector<string> Result;
	vector<double>  Values;

	// 查询数据
	if(!SelectData(TableName, Result, Params))
	{
		// 提示错误信息
		std::cout<<"查询数据操作失败!"<<std::endl;

		return false;
	}

	// 初始化
	MidData = 0;

	// 获取列名
	string ColName;
	GetColName(TableName, ColName);

	// 分割字符串
	vector<string> pArray;
	Split(ColName, pArray, ";");

	// 匹配列
	int col = 0, size = pArray.size();
	for(int c = 0; c < size; c++)
	{
		if(pArray.at(c) == Column)
			col = c;
	}

	// 循环取值
	for(unsigned int i = 0; i < Result.size(); i += size)
	{
		// 转换类型
		double value = atof(Result.at(i + col).c_str());
		
		// 保存结果
		Values.push_back(value);
	}

	// 升序排序
	sort(Values.begin(), Values.end());

	// 得到有几条数据
	int count = Values.size();

	//取模
	if( count % 2 == 0 )
	{
		// 偶数
		int num = count / 2;

		// 求中位数
		MidData = ( Values.at(num -1) + Values.at(num) ) / 2;
	}
	else
	{
		// 奇数
		int num = (count +1) / 2;

		// 求中位数
		MidData =  Values.at(num -1);
	}

	//返回查询成功
	return true;
}


// 从目标数据表中求数据项最大值
bool CSqliteManager::MaxData(string TableName, string Column, string Params, double &MaxData)
{
	// 链表对象
	vector<string> Result;
	vector<double>  Values;

	// 查询数据
	if(!SelectData(TableName, Result, Params))
	{
		// 提示错误信息
		std::cout<<"查询数据操作失败!"<<std::endl;

		return false;
	}

	// 初始化
	MaxData = 0;

	// 获取列名
	string ColName;
	GetColName(TableName, ColName);

	// 分割字符串
	vector<string> pArray;
	Split(ColName, pArray, ";");

	// 匹配列
	int col = 0, size = pArray.size();
	for(int c = 0; c < size; c++)
	{
		if(pArray.at(c) == Column)
			col = c;
	}

	// 循环取值
	for(unsigned int i = 0; i < Result.size(); i += size)
	{
		// 转换类型
		double value = atof(Result.at(i + col).c_str());
		
		// 保存结果
		Values.push_back(value);
	}

	// 降序排序
	sort(Values.rbegin(), Values.rend());

	// 求最大值
	MaxData = Values.at(0);

	//返回查询成功
	return true;
}


// 从目标数据表中求数据项最小值
bool CSqliteManager::MinData(string TableName, string Column, string Params, double &MinData)
{
	// 链表对象
	vector<string> Result;
	vector<double>  Values;

	// 查询数据
	if(!SelectData(TableName, Result, Params))
	{
		// 提示错误信息
		std::cout<<"查询数据操作失败!"<<std::endl;

		return false;
	}

	// 初始化
	MinData = 0;

	// 获取列名
	string ColName;
	GetColName(TableName, ColName);

	// 分割字符串
	vector<string> pArray;
	Split(ColName, pArray, ";");

	// 匹配列
	int col = 0, size = pArray.size();
	for(int c = 0; c < size; c++)
	{
		if(pArray.at(c) == Column)
			col = c;
	}

	// 循环取值
	for(unsigned int i = 0; i < Result.size(); i += size)
	{
		// 转换类型
		double value = atof(Result.at(i + col).c_str());
		
		// 保存结果
		Values.push_back(value);
	}

	// 升序排序
	sort(Values.begin(), Values.end());

	// 求最小值
	MinData = Values.at(0);

	//返回查询成功
	return true;
}


// 从目标数据表中取数据项随机数
bool CSqliteManager::RandData(string TableName, string Column, string Params, int &RanData)
{
	// 链表对象
	vector<string> Result;
	vector<int>  Values;

	// 查询数据
	if(!SelectData(TableName, Result, Params))
	{
		// 提示错误信息
		std::cout<<"查询数据操作失败!"<<std::endl;

		return false;
	}

	// 初始化
	RanData = 0;

	// 获取列名
	string ColName;
	GetColName(TableName, ColName);

	// 分割字符串
	vector<string> pArray;
	Split(ColName, pArray, ";");

	// 匹配列
	int col = 0, size = pArray.size();
	for(int c = 0; c < size; c++)
	{
		if(pArray.at(c) == Column)
			col = c;
	}

	// 循环取值
	for(unsigned int i = 0; i < Result.size(); i += size)
	{
		// 转换类型
		int value = atoi(Result.at(i + col).c_str());
		
		// 保存结果
		Values.push_back(value);
	}

	// 产生随机数种子 (利用系统时钟，产生不同的随机数种子)
	srand((unsigned)time(NULL));

	// 在最小值与最大值之间产生随机数 rand()%(b-a+1)+a;
	double Min, Max;
	MinData(TableName, Column, Params, Min);
	MaxData(TableName, Column, Params, Max);
	RanData = rand()% ((int)Max - (int)Min +1) +(int)Min;

	//返回查询成功
	return true;
}
