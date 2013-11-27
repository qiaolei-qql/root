#if !defined _CMYSQL_H
#define _CMYSQL_H

#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <mysql.h>

#include "cexception.h"

typedef map<string, int> STRING2INT;

class CMysql
{
public:
	CMysql();
	CMysql(const char* szHost, const char* szUser, const char* szPass);
	~CMysql();

	int Close();
	int Connect(const char* szHost, const char* szUser, const char* szPass);
	int Connect();
	bool IfConnected(const char* szHost);

	int Query(char* szSqlString);
	int StoreResult();
	int FreeResult();
	char** FetchRow();
	const char* GetFieldName(int iField);
	char* GetField(unsigned int iField);
	char* GetField(char* szFieldName);
    char* GetFieldBinary(const char* szFieldName, int& iFieldLen);
    char* GetFieldBinary(unsigned int iField, int& iFieldLen);
    
	int GetAffectedRows();
	unsigned int GetAffectedCols() {
		return m_iField;
	};
	MYSQL* GetConnectHandle() { return &m_connection; }

	static int EscapeString (string &str);
    static void Escape(string& str_out, char* buf_in, int buf_in_len);

	unsigned int GetLastInsertId();
	unsigned int  m_iRows;


protected:
	char* m_ErrMsg;
	char m_szHost[65];	// 数据库主机名
	char m_szUser[65];	// 数据库用户名
	char m_szPass[65];	// 数据库用户密码
	
	unsigned int  m_iField;
	MYSQL m_connection;
	MYSQL_RES *m_result;
	MYSQL_ROW m_row;
	STRING2INT m_FieldIndex;
	bool m_bFieldIndexInitialized;
	bool m_bConnected, m_bIfConfig;

	int InitFieldName();
};

#endif //_CMYSQL_H
