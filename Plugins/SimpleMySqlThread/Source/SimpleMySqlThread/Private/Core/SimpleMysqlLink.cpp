#include "Core/SimpleMysqlLink.h"

#include "SimpleMySqlAssist.h"
#include "MySQL/mysql.h"
#include "Core/SimpleMySqlMacro.h"



FSimpleMysqlLink::FSimpleMysqlLink()
{
	
}

FSimpleMysqlLink::FSimpleMysqlLink(const FString& InUser, const FString& InHost, const FString& InPawd,const FString& InDB, const uint32 InPort, const FString& InUnix_Socket,const TArray<ESimpleClientFlags>& InClientFlag)
	:User(InUser),Host(InHost),Pawd(InPawd),DB(InDB),Port(InPort),Unix_Socket(InUnix_Socket),ClientFlag(0),Mysql(new st_mysql()),MysqlStmt(NULL)
{

	 for(auto &Tmp : InClientFlag)
	 {
		 ClientFlag |= ToMySqlClientFlag(Tmp);
	 }
	
	//保证线程安全
	mysql_library_init(NULL,NULL,NULL);
	mysql_init(Mysql);

	MysqlStmt = mysql_stmt_init(Mysql);
}

FSimpleMysqlLink::~FSimpleMysqlLink()
{
	mysql_stmt_close(MysqlStmt);
	
	mysql_close(Mysql);
	mysql_library_end();
	delete Mysql;
	Mysql = nullptr;
}

bool FSimpleMysqlLink::SelectNewDB(const FString& NewDB,FString &ErrorMesg)
{
	int32 Ret = mysql_ping(Mysql);
	if(Ret == 0)
	{
		if(mysql_select_db(Mysql,TCHAR_TO_UTF8(*NewDB)) == 0)
		{
			DB = NewDB;
			return true;
		}
	}
	else 
	{
		if(mysql_real_connect(Mysql,
			TCHAR_TO_UTF8(*Host),
			TCHAR_TO_UTF8(*User),
			TCHAR_TO_UTF8(*Pawd),
			TCHAR_TO_UTF8(*DB),
			Port,
			Unix_Socket == TEXT("") ? 0 : TCHAR_TO_UTF8(*Unix_Socket),
			ClientFlag))
		{
			if(mysql_select_db(Mysql,TCHAR_TO_UTF8(*NewDB)) == 0)
			{
				DB = NewDB;
				return true;
			}
		
			ErrorMesg = UTF8_TO_TCHAR(mysql_error(Mysql));
		}
		else  
		{
			ErrorMesg = UTF8_TO_TCHAR(mysql_error(Mysql));
		}
	}
	return false;
}

bool FSimpleMysqlLink::CreateTemporaryTable(const FString& NewTableName, const TMap<FString, FMysqlFileType>& InFields,
	const TArray<FString>& InPrimaryKeys, const FMysqlCreateTableParam& Param, FString& ErrorMesg)
{
	/*
	CREATE TEMPORARY TABLE temp_table_name (
	id int auto_increment primary key,
	name varchar(50) default 'Unknown',
	age INT UNSIGNED,
	created_at timestamp default current_timestamp,
	index index_name (name) comment 'Index on name column',
	email varchar(100) character SET utf8mb4,
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
	*/
	
	FString SQL = TEXT("CREATE TEMPORARY TABLE `");
	SQL+=NewTableName + TEXT("` (");

	//设置字段和其属性
	SimpleMysqlAssist::FieldTypeArrayToString(SQL,InFields);
	
	//创建主键字段
	SimpleMysqlAssist::PrimaryKeyToString(SQL,InPrimaryKeys);
	
	SQL += TEXT(")");
	
	//设置表属性
	SQL += Param.ToString();
	
	SQL += ";";

	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::CreateTemporaryTableFromOther(const FString& NewTableName,const TMap<FString,FMysqlFileType> &InFields,const TArray<FString>& InPrimaryKeys,const FSimpleMysqlQueryParameters& QueryParam,TArray<FSimpleMysqlComparisonOperator>& Condition,FString &ErrorMesg)
{
	/*
	CREATE  TEMPORARY TABLE tmp_table(
	name VARCHAR(50) DEFAULT 'Unknown',  //自定义旧表存在字段的属性
	age INT AUTO_INCREMENT PRIMARY KEY   //自定义旧表存在字段的属性
) SELECT name,age FROM exist——table;
	 */

	//确保查询参数的字段是有值的
	if(!QueryParam.IsValid())
	{
		return false;
	}

	//这段代码是我自己加的，用来判断设置旧字段属性和拷贝源表selcet字段的匹配性，如果不匹配就直接返回false不用在执行了。
	{
		bool IsSelectALL = false;
		for(auto &Tmp : QueryParam.InFields)
		{
			if(Tmp.VariableName.Contains(TEXT("*")))
			{
				IsSelectALL = true;
				break;
			}
		}
		if(!IsSelectALL)
		{
			for(auto &Tmp : QueryParam.InFields)
			{
				for(auto &Field : InFields)
				{
					if(!Tmp.VariableName.Contains(Field.Key))
					{
						return false;
					}
				}
			}
		}
	}
	

	FString SQL = TEXT("CREATE TEMPORARY TABLE `");
	SQL+=NewTableName + TEXT("`");
	
	//设置已存在字段的属性
	if(InFields.Num())
	{
		SQL += TEXT("(");
		
		//创建拷贝字段
		SimpleMysqlAssist::FieldTypeArrayToString(SQL,InFields);
	
		//创建主键字段
		SimpleMysqlAssist::PrimaryKeyToString(SQL,InPrimaryKeys);
	
		SQL += TEXT(")");
	}
	else
	{
		return false;
	}

	//创建查询语句
	FString ContentString;
	QueryParam.ToString(ContentString);
	//设置复制字段和原表
	SQL+=ContentString;

	SimpleMysqlAssist::ConditionToString(SQL,Condition);
	
	SQL += TEXT(";");
	GEngine->AddOnScreenDebugMessage(-1,100,FColor::Green,SQL);
	
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::DropTemporaryTable(const FString& TemporaryTable, FString& ErrorMesg)
{
	FString SQL = TEXT("DROP TEMPORARY TABLE IF EXISTS ")+TemporaryTable+TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::CreateTemporaryTableAsOther(const FString& TemporaryTable,const TMap<FString, FMysqlFileType>& NewInFields,const TArray<FString>& InPrimaryKeys,const FSimpleMysqlQueryParameters& QueryParam, FString& ErrorMesg)
{
	/*
	 create table tb(age integer NOT NULL AUTO_INCREMENT PRIMARY KEY) AS (select name as tb_name,age from ppp1 where age < 3)  
	 */

	FString SQL = TEXT("CREATE TEMPORARY TABLE ") + TemporaryTable + TEXT(" ");

	if(NewInFields.Num())
	{
		SQL += TEXT("(");
		
		//创建拷贝字段
		SimpleMysqlAssist::FieldTypeArrayToString(SQL,NewInFields);
		
		//创建主键字段
		SimpleMysqlAssist::PrimaryKeyToString(SQL,InPrimaryKeys);
	
		SQL += TEXT(")");
	}

	SQL += TEXT(" AS (");
	QueryParam.ToString(SQL);
	SQL += TEXT(");");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::CopyTableAToBFast(const FString& CopyTargetTable,const TMap<FString, FMysqlFileType>& InsertFields, const TArray<FString>& InPrimaryKeys,const FSimpleMysqlQueryParameters& QueryParam, FString& ErrorMesg)
{
	FString SQL = TEXT("CREATE TABLE ") + CopyTargetTable + TEXT(" ");

	return CopyTableAToBFastInternal(SQL,InsertFields,InPrimaryKeys,QueryParam,ErrorMesg);
}

bool FSimpleMysqlLink::CopyTableAToTemporaryBFast(const FString& CopyTargetTable,const TMap<FString, FMysqlFileType>& InsertFields, const TArray<FString>& InPrimaryKeys,const FSimpleMysqlQueryParameters& QueryParam, FString& ErrorMesg)
{
	FString SQL = TEXT("CREATE TEMPORARY TABLE ") + CopyTargetTable + TEXT(" ");
	return CopyTableAToBFastInternal(SQL,InsertFields,InPrimaryKeys,QueryParam,ErrorMesg);
}


bool FSimpleMysqlLink::CreateTemporaryTableALikeB(const FString& A, const FString& B, FString& ErrorMesg)
{
	FString SQL = TEXT("CREATE TEMPORARY TABLE ")+ A + TEXT(" LIKE ") + B +TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::CreateTableALikeB(const FString& A, const FString& B, FString& ErrorMesg)
{
	FString SQL = TEXT("CREATE TABLE ")+ A + TEXT(" LIKE ") + B +TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::CopyTableAToB(const FString& A, const FString& B, const TArray<FString>& Fields,
	FString& ErrorMesg)
{
	FString Error;
	if(CreateTableALikeB(B,A,Error))
	{
		//设置拷贝字段和值
		TMap<FString,FString> MapFields;
		for(auto &Tmp : Fields)
		{
			MapFields.Add(Tmp,TEXT(""));
		}
		
		FSimpleMysqlQueryParameters QueryParameters;
		QueryParameters.TableNmae = A;

		//设置查询select参数
		for(auto &Tmp : Fields)
		{
			QueryParameters.InFields.Add(FSimpleMysqlVariableSettings());
			QueryParameters.InFields[QueryParameters.InFields.Num()-1].VariableName = Tmp;
		}
	
		InsertTableData(B,MapFields,QueryParameters,ErrorMesg);

		ErrorMesg += Error;
		
	}
	return ErrorMesg.IsEmpty();
}

bool FSimpleMysqlLink::CopyTableAToTemporaryB(const FString& Table, const FString& Temporary, const TArray<FString>& Fields,
	FString& ErrorMesg)
{
	FString Error;
	if(CreateTemporaryTableALikeB(Temporary,Table,Error))
	{
		TMap<FString,FString> MapFields;
		for(auto &Tmp : Fields)
		{
			MapFields.Add(Tmp,TEXT(""));
		}

		FSimpleMysqlQueryParameters QueryParameters;

		for(auto &Tmp : Fields)
		{
			QueryParameters.InFields.Add(FSimpleMysqlVariableSettings());
			QueryParameters.InFields[QueryParameters.InFields.Num()-1].VariableName = Tmp;
		}
		InsertTableData(Temporary,MapFields,QueryParameters,ErrorMesg);

		ErrorMesg += Error;
		
	}
	return ErrorMesg.IsEmpty();
}

bool FSimpleMysqlLink::QueryLink(const FString& SQL,FString &ErrorMesg)
{
	int32 ret = mysql_ping(Mysql);  //先ping一下
	if(ret == 0)  //如果成功进行查询
	{
		if((ret = mysql_query(Mysql,TCHAR_TO_UTF8(*SQL))) != 0)
		{
			ErrorMesg = UTF8_TO_TCHAR(mysql_error(Mysql));
			return false;
		}
	}
	else  //如果失败重新链接
	{
		if(mysql_real_connect(Mysql,
			TCHAR_TO_UTF8(*Host),
			TCHAR_TO_UTF8(*User),
			TCHAR_TO_UTF8(*Pawd),
			TCHAR_TO_UTF8(*DB),
			Port,
			Unix_Socket == TEXT("") ? 0 : TCHAR_TO_UTF8(*Unix_Socket),
			ClientFlag))
		{
			if((ret = mysql_query(Mysql,TCHAR_TO_UTF8(*SQL))) != 0)  //重新链接后再次进行查询
			{
				ErrorMesg = UTF8_TO_TCHAR(mysql_error(Mysql));
				return false;
			}
		}
		else  //再次链接失败打印错误
		{
			ErrorMesg = UTF8_TO_TCHAR(mysql_error(Mysql));
			return false;
		}
	}

	return true;
}

bool FSimpleMysqlLink::GetSelectTableData(EMysqlQuerySaveType SaveType, const FSimpleMysqlQueryParameters& QueryParam, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,
const FSimpleMysqlDebugResult& Debug)
{
	switch (SaveType)
	{
	case EMysqlQuerySaveType::STORE_RESULT:
		return GetSelectTableDataSR(QueryParam,Results,ErrorMesg,Debug);
	case EMysqlQuerySaveType::USE_RESULT:
		return GetSelectTableDataUR(QueryParam,Results,ErrorMesg,Debug);
	}

	ErrorMesg = FString::Printf(TEXT("This type is not supported.%i"),SaveType);

	return false;
}

bool FSimpleMysqlLink::ShowTables(const FString& TableName,EMysqlQuerySaveType SaveType, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,
	const FSimpleMysqlDebugResult& Debug)
{
	FSimpleMysqlQueryParameters QueryParame;
	QueryParame.TableNmae = TableName;
	QueryParame.InFields.Add(FSimpleMysqlVariableSettings());
	QueryParame.AddField( TEXT("*"));
	
	return GetSelectTableData(SaveType,QueryParame,Results,ErrorMesg,Debug);
}

bool FSimpleMysqlLink::QueryLinkResult(const FString& SQL, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,
	EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	FString ErrMsgQuery;
	if(QueryLink(SQL,ErrMsgQuery))
	{
		switch (SaveType) {
		case EMysqlQuerySaveType::STORE_RESULT:
			GetStroeResult(Results,ErrorMesg,Debug);
			break;
		case EMysqlQuerySaveType::USE_RESULT:
			GetUseResult(Results,ErrorMesg,Debug);
			break;
		}
	}
	ErrorMesg += ErrMsgQuery;
	return ErrorMesg.IsEmpty();
}

bool FSimpleMysqlLink::CreateFunction(const FString& InFunctionName, const TMap<FString, FMysqlFileType>& InParameter,
                                      const FString& InFunctionBody,FString& ErrorMesg, EMysqlVariableType InReturnsType, const FString& InComment)
{
	//create function 函数名([参数列表]) return 数据类型 COMMENT "xxx"
	//  begin
	//    sql语句;
	//   return 值;
	//  end;

	FString SQL = TEXT("CREATE FUNCTION ") + InFunctionName + TEXT("(");

	UEnum *MysqlVariableEnum = StaticEnum<EMysqlVariableType>();
	FString FieldTypeString = MysqlVariableEnum->GetNameStringByIndex((int32)InReturnsType);

	//填参数
	SimpleMysqlAssist::FieldTypeArrayToString(SQL,InParameter);
	SQL += TEXT(") RETURNS ") + FieldTypeString + TEXT(" ");
	if(!InComment.IsEmpty())
	{
		SQL += (TEXT("COMMENT \"") + InComment + TEXT("\" "));
	}
	SQL += TEXT("BEGIN ") + InFunctionBody + TEXT(" END;");

	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::ExecuteFunction(const FString& InFunctionName,const TArray<FString>& InParameter,
	TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType,
	const FSimpleMysqlDebugResult& Debug)
{
	FString SQL = InFunctionName + TEXT("(");
	for(auto &Tmp : InParameter)
	{
		SQL += TEXT("'") + Tmp + TEXT("',");
	}
	SQL.RemoveFromEnd(TEXT(","));
	SQL += TEXT(")");
	return GetSelectTableDataByFuntion(SQL,SaveType,Results,ErrorMesg,Debug);
		
}

bool FSimpleMysqlLink::DropFunction(const FString& InFunctionName,FString& ErrorMesg)
{
	FString SQL = TEXT("DROP FUNCTION ") + InFunctionName + TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::ShowCreateFunction(const FString& InFunctionName,TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	FString SQL = TEXT("SHOW CREATE FUNCTION ") + InFunctionName + TEXT(";");
	return GetSelectTableDataInternal(SQL,Results,ErrorMesg,SaveType,Debug);
}

bool FSimpleMysqlLink::ShowFunctionsStatus(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	FString SQL = TEXT("SHOW FUNCTION STATUS;");
	return GetSelectTableDataInternal(SQL,Results,ErrorMesg,SaveType,Debug);
}

bool FSimpleMysqlLink::DefiningVariables(const FString& InVariablesName, const FString& InValue, FString& ErrorMesg)
{
	//SET @Q1 = 1;  设置存储临时变量1
	//SET @Q2 = 2;  设置存储临时变量2
	FString SQL = TEXT("SET ") + InVariablesName + TEXT("=") + InValue + TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}


bool FSimpleMysqlLink::CreateProcedure(const FString& InProcedureName, const TMap<FString, FMysqlFileType>& InParameter,
                                       const FString& InProcedureBody, FString& ErrorMesg, const FString& InComment)
{
	/*
*CREATE PROCEDURE TestP_4(INOUT a INT, INOUT b INT) COMMENT "xxx"
BEGIN
	SET a = b + 1;
	SET b = a + 1;
END;
*/
	FString SQL = TEXT("CREATE PROCEDURE ") + InProcedureName + TEXT("(");
	
	//填参数
	SimpleMysqlAssist::FieldTypeArrayToString(SQL,InParameter);
	SQL += TEXT(") ");
	if(!InComment.IsEmpty())
	{
		SQL += (TEXT("COMMENT \"") + InComment + TEXT("\" "));
	}
	SQL += TEXT("BEGIN ") + InProcedureBody + TEXT(" END;");

	return QueryLink(SQL,ErrorMesg);
	
}

bool FSimpleMysqlLink::ExecuteProcedure(const FString& InProcedureName,const TArray<FString>& InParameter,
	TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType,
	const FSimpleMysqlDebugResult& Debug)
{
	//CALL TestP_2(@Q1,@Q2);
	FString SQL = TEXT("CALL ") + InProcedureName + TEXT("(");
	for(auto &Tmp : InParameter)
	{
		if(Tmp.Contains("IN "))
		{
			SQL += TEXT("'") + Tmp + TEXT("',");
		}
		else
		{
			SQL += Tmp + TEXT(",");
		}
		
	}
	SQL.RemoveFromEnd(TEXT(","));
	SQL += TEXT(");");
	return QueryLinkResult(SQL,Results,ErrorMesg,SaveType,Debug);
}

bool FSimpleMysqlLink::DropProcedure(const FString& InProcedureName, FString& ErrorMesg)
{
	FString SQL = TEXT("DROP PROCEDURE ") + InProcedureName + TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::ShowCreateProcedure(const FString& InProcedureName, TArray<FSimpleMysqlResult>& Results,
										   FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	FString SQL = TEXT("SHOW CREATE PROCEDURE ") + InProcedureName + TEXT(";");
	return GetSelectTableDataInternal(SQL,Results,ErrorMesg,SaveType,Debug);
}

bool FSimpleMysqlLink::ShowProceduresStatus(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,
											EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	FString SQL = TEXT("SHOW PROCEDURE STATUS;");
	return GetSelectTableDataInternal(SQL,Results,ErrorMesg,SaveType,Debug);
}

bool FSimpleMysqlLink::GetSelectVersion(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,
                                        EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	FSimpleMysqlQueryParameters QueryParam;
	QueryParam.FunctionSettings.VariableName = TEXT("VERSION()");

	return GetSelectTableData(SaveType,QueryParam,Results,ErrorMesg,Debug);
	
	//这种方法直接输入SQL语句即可。上面不需要输入select，而且可带查询参数，因为select方法返回的是一张表。
	//FString SQL = TEXT("SELECT VERSION;");
	//return GetSelectTableDataInternal(SQL,Results,ErrorMesg,SaveType,Debug);
}

bool FSimpleMysqlLink::GetSelectDatabase(TArray<FSimpleMysqlResult> &Results, FString &ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult &Debug /*= FSimpleMysqlDebugResult()*/)
{
	return GetSelectTableDataByFuntion(TEXT("DATABASE()"), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::GetSelectUser(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType,
	const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(TEXT("USER()"), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::GetShowStatus(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,
	EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	//这种方法适合不是select语句的sql命令
	FString SQL = TEXT("SHOW STATUS;");
	return GetSelectTableDataInternal(SQL,Results,ErrorMesg,SaveType,Debug);
}

bool FSimpleMysqlLink::GetShowVariables(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,
	EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	FString SQL = TEXT("SHOW VARIABLES;");
	return GetSelectTableDataInternal(SQL,Results,ErrorMesg,SaveType,Debug);
}

bool FSimpleMysqlLink::GetConnection_ID(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,
	EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(TEXT("CONNECTION_ID()"), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::GetCurrent_user(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(TEXT("CURRENT_USER()"), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::GetLast_Insert_ID(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(TEXT("LAST_INSERT_ID()"), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::GetSession_user(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(TEXT("SESSION_USER()"), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::GetSystem_user(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(TEXT("SESSION_USER()"), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Bin(const int64 Value, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("BIN('%lld')"),Value),SaveType,Results,ErrorMesg,Debug);
}

bool FSimpleMysqlLink::Convert(const FString& InString, EMysqlCharset Charset, TArray<FSimpleMysqlResult>& Results,
	FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	if(Charset != EMysqlCharset::None)
	{
		UEnum *CharsetEnum = StaticEnum<EMysqlCharset>();
		FString CharsetString = CharsetEnum->GetNameStringByIndex((int32)Charset);
	
		return GetSelectTableDataByFuntion(FString::Printf(TEXT("CONVERT('%s' USING %s)"),*InString,*CharsetString),SaveType,Results,ErrorMesg,Debug);
	}
	return false;
}

bool FSimpleMysqlLink::BinocularOperation(const FString& Expression, const FString& ValueA, const FString& ValueB, TArray<FSimpleMysqlResult> &Results, FString &ErrorMesg, EMysqlQuerySaveType SaveType /*= EMysqlQuerySaveType::STORE_RESULT*/, const FSimpleMysqlDebugResult &Debug /*= FSimpleMysqlDebugResult()*/)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("IF(%s,'%s','%s')"), *Expression, *ValueA, *ValueB), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::NULLIf(const FString& ValueA, const FString& ValueB, TArray<FSimpleMysqlResult>& Results,
	FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("NULLIF('%s','%s')"),*ValueA,*ValueB),SaveType,Results,ErrorMesg,Debug);
}

bool FSimpleMysqlLink::IsNULL(const FString& Expression, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,
	EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("ISNULL('%s')"),*Expression),SaveType,Results,ErrorMesg,Debug);
}

bool FSimpleMysqlLink::Charset(const FString& InExpression, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,
	EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("CHARSET('%s')"),*InExpression),SaveType,Results,ErrorMesg,Debug);
}

bool FSimpleMysqlLink::Conv(const FString& Value, int64 F1, int64 F2, TArray<FSimpleMysqlResult>& Results,
	FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("CONV('%s',%lld,%lld)"),*Value,F1,F2),SaveType,Results,ErrorMesg,Debug);
}

bool FSimpleMysqlLink::Cast(const FString& Value, EMysqlVariableType VariableType, TArray<FSimpleMysqlResult>& Results,
	FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	UEnum *MysqlVariableEnum = StaticEnum<EMysqlVariableType>();
	FString FieldTypeString = MysqlVariableEnum->GetNameStringByIndex((int32)VariableType);
	
	if(FieldTypeString.Compare(TEXT("INT")))
	{
		FieldTypeString = TEXT("SIGNED");
	}

	return GetSelectTableDataByFuntion(FString::Printf(TEXT("Cast('%s' AS %s)"),*Value,*FieldTypeString),SaveType,Results,ErrorMesg,Debug);
}

bool FSimpleMysqlLink::Binary(const FString& Value, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,
	EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("BINARY('%s')"),*Value),SaveType,Results,ErrorMesg,Debug);
}


bool FSimpleMysqlLink::YearWeek(const FString &InDate, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("YEARWEEK('%s')"), *InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::WeekOfYear(const FString& InDate, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("WEEKOFYEAR('%s')"), *InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::WeekDay(const FString& InDate, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("WEEKDAY('%s')"), *InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Year(const FString& InDate, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("YEAR('%s')"), *InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Week(const FString& InDate, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("WEEK('%s')"), *InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::ToDays(const FString& InDate, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("TO_DAYS('%s')"), *InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::TimeStamp(const FString& InExpression, const FString& InInterval, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("TIMESTAMP('%s','%s')"), *InExpression,*InInterval), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::TimeDiff(const FString& InTime1, const FString& InTime2, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("TIMEDIFF('%s','%s')"), *InTime1, *InTime2), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::TimeToSec(const FString& InTime, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("TIME_TO_SEC('%s')"), *InTime), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::TimeFormat(const FString& InTime, const FString& InFormat, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("TIME_FORMAT('%s','%s')"), *InTime,*InFormat), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Time(const FString& InExpression, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("TIME('%s')"), *InExpression), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::SysDate(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(TEXT("SYSDATE('%s')"), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::SubTime(const FString& InTime, int64 InSecond, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("SUBTIME('%s','%lld')"),*InTime, InSecond),SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::SubDate(const FString& InDate, int64 InDay, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("SUBDATE('%s','%lld')"), *InDate, InDay), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::StringToDate(const FString& InString, const FString& InFormatMask, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("STR_TO_DATE('%s','%s')"), *InString, *InFormatMask), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::SecToTime(const int64 InSecond, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("SEC_TO_TIME('%lld')"), InSecond), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Second(const FString &InTime, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("SECOND('%s')"), *InTime), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Quarter(const FString &InDate, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("QUARTER('%s')"), *InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::PeriodDiff(const int64 Period1, const int64 Period2, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("PERIOD_DIFF('%lld','%lld')"),Period1 ,Period2), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::PeriodAdd(const int64 InPeriod, const int64 InMonths, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("PERIOD_ADD('%lld','%lld')"), InPeriod, InMonths), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Now(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(TEXT("NOW()"), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Minute(const FString &InTime, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("MINUTE('%s')"),*InTime), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::MicroSecond(const FString &InDate, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("MICROSECOND('%s')"), *InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::MakeTime(const FString &InDate, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("MAKETIME('%s')"), *InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::MakeDate(const int32 InYear, const int64 DayOfYear, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("MAKEDATE('%i','%lld')"), InYear, DayOfYear), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::LocalTimeStamp(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(TEXT("LOCALTIMESTAMP()"), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::LocalTime(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(TEXT("LOCALTIME()"), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::LastDay(const FString &InDate, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("LAST_DAY('%s')"),*InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Hour(const FString &InTime, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("HOUR('%s')"), *InTime), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::FromDays(const int64 InDay, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("FROM_DAYS('%lld')"), InDay), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Extract(const FString &InDate, const EMysqlDateType DateType, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	UEnum *TypeEnum = StaticEnum<EMysqlDateType>();
	FString DT = TypeEnum->GetNameStringByIndex((int32)DateType);

	return GetSelectTableDataByFuntion(FString::Printf(TEXT("EXTRACT( %s FROM '%s')"),*DT,*InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::DayOfYear(const FString &InDate, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("DAYOFYEAR('%s')"), *InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::DayOfWeek(const FString &InDate, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("DAYOFWEEK('%s')"), *InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::DayOfMonth(const FString &InDate, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("DAYOFMONTH('%s')"), *InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::DayName(const FString& InDate, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("DAYNAME('%s')"), *InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Day(const FString& InDate, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("DAY('%s')"), *InDate), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::DateSub(const FString& InDate, const int64 InInterval, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("DATE_SUB('%s', INTERVAL %lld DAY)"), *InDate, InInterval), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::DateFormat(const FString& InDate, const FString& InFormat, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("DATE_FORMAT('%s','%s')"), *InDate,*InFormat), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::AddDate(const FString& InDate, const int64 InInterval, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("ADDDATE('%s','%lld')"), *InDate, InInterval), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::DateDiff(const FString& InD1, const FString& InD2, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("DATEDIFF('%s','%s')"), *InD1, *InD2), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Date(const FString& InExpression, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("DATE('%s')"),*InExpression), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::CurTime(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(TEXT("CURTIME()"), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::CurrentTime(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(TEXT("CURRENT_TIME()"), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::CurrentTimeStamp(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(TEXT("CURRENT_TIMESTAMP()"), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::AddTime(const FString& InDate, const int64 InSecond, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("DATEDIFF('%s','%lld')"),*InDate, InSecond), SaveType, Results, ErrorMesg, Debug);
}


bool FSimpleMysqlLink::GetASCII(const FString& InString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("ASCII('%s')"), *InString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::CharLength(const FString& InString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("CHAR_LENGTH('%s')"), *InString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::CharacterLength(const FString& InString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("CHARACTER_LENGTH('%s')"), *InString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Concat(const TArray<FString>& InStrings, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	FString SQL;
	for (auto &Tmp : InStrings)
	{
		SQL += TEXT("'") + Tmp + TEXT("'") + TEXT(",");
	}
	SQL.RemoveFromEnd(TEXT(","));

	return GetSelectTableDataByFuntion(FString::Printf(TEXT("CONCAT(%s)"), *SQL), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::ConcatWS(const TCHAR* InSeparator, const TArray<FString>& InStrings, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	FString SQL;
	for (auto& Tmp : InStrings)
	{
		SQL += TEXT("'") + Tmp + TEXT("'") + TEXT(",");
	}
	SQL.RemoveFromEnd(TEXT(","));

	return GetSelectTableDataByFuntion(FString::Printf(TEXT("CONCAT_WS('%s',%s)"), InSeparator, *SQL), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Field(const TCHAR* InQueryString, const TArray<FString>& InStrings, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	FString SQL;
	for (auto& Tmp : InStrings)
	{
		SQL += TEXT("'") + Tmp + TEXT("'") + TEXT(",");
	}
	SQL.RemoveFromEnd(TEXT(","));

	return GetSelectTableDataByFuntion(FString::Printf(TEXT("FIELD('%s',%s)"), InQueryString, *SQL), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::FindInSet(const TCHAR* InQueryString, const FString& InString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("ASCII('%s')"), *InString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Format(float InNumber, int8 KeepDecimalPoint, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("FORMAT('%f','%i')"), InNumber, KeepDecimalPoint), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Insert(const FString& InSourceString, int32 InStartPos, const TCHAR* InReplacedString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("INSERT('%s',%i,%i,%s)"), *InSourceString, InStartPos,FString(InReplacedString).Len(), InReplacedString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Locate(const FString& InSourceString, const TCHAR* InFindString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("LOCATE('%s','%s')"), *InSourceString, InFindString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Lcase(const FString& InSourceString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("LCASE('%s')"), *InSourceString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Lower(const FString& InSourceString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("LOWER('%s')"), *InSourceString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Upper(const FString& InSourceString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("ASCII('%s')"), *InSourceString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Ucase(const FString& InSourceString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("UCASE('%s')"), *InSourceString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Left(const FString& InSourceString, int32 ReturnStringLen, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("LEFT('%s',%i)"), *InSourceString, ReturnStringLen), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Right(const FString& InSourceString, int32 ReturnStringLen, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("RIGHT('%s',%i)"), *InSourceString, ReturnStringLen), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Lpad(const FString& InSourceString, int32 StringLen, const FString& InAddString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("LPAD('%s',%i,'%s')"), *InSourceString, StringLen,*InAddString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Rpad(const FString& InSourceString, int32 StringLen, const FString& InAddString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("RPAD('%s',%i,'%s')"), *InSourceString, StringLen, *InAddString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Ltrim(const FString& InSourceString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("LTRIM('%s')"), *InSourceString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Rtrim(const FString& InSourceString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("RTRIM('%s')"), *InSourceString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Trim(const FString& InSourceString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("TRIM('%s')"), *InSourceString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Mid(const FString& InSourceString, int32 InStartPos, int32 InStringLen, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("MID('%s',%i,%i)"), *InSourceString, InStartPos, InStringLen), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::SubString(const FString& InSourceString, int32 InStartPos, int32 InStringLen, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("SUBSTRING('%s',%i,%i)"), *InSourceString, InStartPos, InStringLen), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::SubStr(const FString& InSourceString, int32 InStartPos, int32 InStringLen, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("SUBSTR('%s')"), *InSourceString, InStartPos, InStringLen), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::SubStringIndex(const FString& S, const TCHAR* InDelimiter, int32 InNumber, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("SUBSTRING_INDEX('%s','%s',%i)"), *S, InDelimiter, InNumber), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Position(const FString& InSourceString, const TCHAR* InMatchingString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("POSITION('%s','%s')"), *InSourceString, InMatchingString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Repeat(const FString& InSourceString, int32 InTimes, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("REPEAT('%s',%i)"), *InSourceString, InTimes), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Replace(const FString& InSourceString, const TCHAR* ToReplace, const TCHAR* InReplaced, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("REPLACE('%s','%s','%s')"), *InSourceString, ToReplace, InReplaced), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Reverse(const FString& InSourceString, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("REVERSE('%s')"), *InSourceString), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Space(const int32 InSpaceNumber, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("SPACE(%i)"), InSpaceNumber), SaveType, Results, ErrorMesg, Debug);
}

bool FSimpleMysqlLink::Strcmp(const FString& S1, const FString& S2, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug )
{
	return GetSelectTableDataByFuntion(FString::Printf(TEXT("STRCMP('%s','%s')"), *S1,*S2), SaveType, Results, ErrorMesg, Debug);
}

//真正执行SQL查询和获取结果集的的地方。
bool FSimpleMysqlLink::QueryLinkStroeResult(const FString& SQL, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,const FSimpleMysqlDebugResult &Debug)
{
	FString ErrMsgQuery;
	if(QueryLink(SQL,ErrMsgQuery))
	{
		if(Debug.bPrintToLog | Debug.bPrinttToScreen)
		{
			GetStroeResult(Results,ErrorMesg,Debug);
		}
	}
	ErrorMesg += ErrMsgQuery;
	return ErrorMesg.IsEmpty();
}

//真正执行SQL查询和获取结果集的的地方。
bool FSimpleMysqlLink::QueryLinkUseResult(const FString& SQL, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,const FSimpleMysqlDebugResult &Debug)
{
	FString ErrMsgQuery;
	if(QueryLink(SQL,ErrMsgQuery))
	{
		if(Debug.bPrintToLog | Debug.bPrinttToScreen)
		{
			GetUseResult(Results,ErrorMesg,Debug);
		}
	}
	ErrorMesg += ErrMsgQuery;
	return ErrorMesg.IsEmpty();
}

void FSimpleMysqlLink::FBindAllocation::Free()
{
	if (Size != 0)
	{
		uint8 *Byteode = (uint8*)BindParam;
		for (uint32 i = 0;i < Size;i++)
		{
			MYSQL_BIND * TmpBind = (MYSQL_BIND*)Byteode;

			FMemory::Free(TmpBind->buffer);
			TmpBind->buffer = nullptr;

			FMemory::Free(TmpBind->length);
			TmpBind->length = nullptr;

			FMemory::Free(TmpBind);

			Byteode += sizeof(MYSQL_BIND);
		}

		Size = 0;
		BindParam = nullptr;
	}
}

bool FSimpleMysqlLink::GetSelectTableDataSR(const FSimpleMysqlQueryParameters& QueryParam,TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, const FSimpleMysqlDebugResult& Debug)
{
	FString SQL;
	if(GetSelectTableDataSQL(SQL,QueryParam))
	{
		return QueryLinkStroeResult(SQL,Results,ErrorMesg,Debug);	
	}
	
	return false;
}

bool FSimpleMysqlLink::GetSelectTableDataUR(const FSimpleMysqlQueryParameters& QueryParam, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,
	const FSimpleMysqlDebugResult& Debug)
{
	FString SQL;
	if(GetSelectTableDataSQL(SQL,QueryParam))
	{
		return QueryLinkUseResult(SQL,Results,ErrorMesg,Debug);	
	}
	
	return false;
}

bool FSimpleMysqlLink::GetSelectTableDataByFuntion(const FString& SQL, EMysqlQuerySaveType SaveType,
	TArray<FSimpleMysqlResult> Results, FString& ErrorMesg, const FSimpleMysqlDebugResult& Debug)
{
	FSimpleMysqlQueryParameters QueryParam;
	QueryParam.FunctionSettings.VariableName = SQL;

	return GetSelectTableData(SaveType,QueryParam,Results,ErrorMesg,Debug);
}

bool FSimpleMysqlLink::CopyTableAToBFastInternal(FString& SQL,const TMap<FString, FMysqlFileType>& InsertFields,const TArray<FString>& InPrimaryKeys, const FSimpleMysqlQueryParameters& QueryParam, FString& ErrorMesg)
{
	/*
	create table tb(age integer NOT NULL AUTO_INCREMENT PRIMARY KEY) AS (select name as tb_name,age from ppp1 where age < 3)
	
	create table tb AS (select id from ppp1);  //这里只复制id来进行表的创建
	create table 新表 AS (select 字段n as 新字段名  ... from 原表...);
	create table 新表(新字段 类型 属性) AS (select * from 旧表...)  //复制的时候，除了拷贝旧的字段外，添加新的字段
	*/
	if(InsertFields.Num())
	{
		SQL += TEXT("(");
		
		//创建拷贝字段
		SimpleMysqlAssist::FieldTypeArrayToString(SQL,InsertFields);
		
		//创建主键字段
		SimpleMysqlAssist::PrimaryKeyToString(SQL,InPrimaryKeys);
	
		SQL += TEXT(")");
	}

	if(QueryParam.IsValid())
	{
		SQL += TEXT(" AS (");
		QueryParam.ToString(SQL);
		SQL += TEXT(");");
	}
	else
	{
		ErrorMesg = TEXT("Query Parameters are meaningless.");
		return false;
	}

	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::GetSelectTableDataInternal(FString &SQL,TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,
	EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug)
{
	switch (SaveType) {
	case EMysqlQuerySaveType::STORE_RESULT:
		return QueryLinkStroeResult(SQL,Results,ErrorMesg,Debug);
	case EMysqlQuerySaveType::USE_RESULT:
		return QueryLinkUseResult(SQL,Results,ErrorMesg,Debug);
	}
	ErrorMesg = FString::Printf(TEXT("This type is not supported.%i"),SaveType);
	return false;
}


bool FSimpleMysqlLink::CreateAndSelectDataBase(const FString& DataBaseName, EMysqlCharset Charset, FString& ErroMesg)
{
	FString CreateDataBaseError;
	if(CreateDateBase(DataBaseName,Charset,CreateDataBaseError))
	{
		return SelectNewDB(DataBaseName,ErroMesg);
	}
	ErroMesg += CreateDataBaseError;
	
	return ErroMesg.IsEmpty();
}


bool FSimpleMysqlLink::CreateDateBase(const FString& DataBaseName, EMysqlCharset Charset,FString &ErrorMesg)
{
	if(Charset != EMysqlCharset::None)
	{
		UEnum *CharsetEnum = StaticEnum<EMysqlCharset>();
		FString CharsetString = CharsetEnum->GetNameStringByIndex((int32)Charset);
	
		FString SQL = TEXT("CREATE DATABASE ")+DataBaseName+TEXT(" CHARACTER SET ")+CharsetString+TEXT(";");
		return QueryLink(SQL,ErrorMesg);
	}
	else
	{
		FString SQL = TEXT("CREATE DATABASE ")+DataBaseName+TEXT(";");
		return QueryLink(SQL,ErrorMesg);
	}
}

bool FSimpleMysqlLink::CreateTable(const FString& TableName, const TMap<FString, FMysqlFileType>& InFields,const TArray<FString>& InPrimaryKeys,const FMysqlCreateTableParam& Param,FString &ErrorMesg)
{
	//拼成格式演示：
//"create table `hp`(
//`id` UNSIGNED int(10) auto_increment DEFAULT 9,
//`age` int PRIMARY KEY,
//`sorce` int DEFAULT 22 COMMENT 'xxxxxx',
//`name` varchar(56) not null,
////`skill` varchar(56) not null DEFAULT 'FireAndIce', 
//`table date` date, 
//primary key(`id`)
//)engine = innodb default charset = utf8 COLLATE=utf8mb4_general_ci;";
	
	FString SQL = TEXT("CREATE TABLE `");
	SQL+=(TableName + TEXT("` ("));
	
	//创建字段
	SimpleMysqlAssist::FieldTypeArrayToString(SQL,InFields);
	//创建主键字段
	SimpleMysqlAssist::PrimaryKeyToString(SQL,InPrimaryKeys);
	
	SQL += TEXT(")");
	
	//设置表属性
	SQL += Param.ToString();
	
	SQL += ";";
	
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::DropDataBase(const FString& DataBaseName, FString& ErrorMesg)
{
	FString SQL = TEXT("DROP DATABASE ")+DataBaseName+TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::DropTable(const FString& TableName, FString& ErrorMesg)
{
	FString SQL = TEXT("DROP TABLE ")+TableName+TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::TruncateTable(const FString& TableName, FString& ErrorMesg)
{
	FString SQL = TEXT("TRUNCATE TABLE ")+TableName+TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::DeleteFromTable(const FString& TableName, FString& ErrorMesg)
{
	FString SQL = TEXT("DELETE FROM ")+TableName+TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::DeleteFromTableWhereData(const FString& TableName,TArray<FSimpleMysqlComparisonOperator>& Condition,FString& ErrorMesg)
{
	FString SQL = TEXT("DELETE FROM ")+TableName+TEXT(" WHERE ");
	SimpleMysqlAssist::ConditionToString(SQL,Condition) ;
	SQL+=+TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::OptimiseTable(const FString& TableName,FString& ErrorMesg)
{
	FString SQL = TEXT("OPTIMISE TABLE ")+TableName+TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::UpdataTableData(const FString& TableName, const TArray<FSimpleMysqlAssignment> &InFields, const TArray<FSimpleMysqlComparisonOperator> & Condition,FString& ErrorMesg)
{
	//UPDATE role set age = '100',size ='200' WHERE name != 'www' and hhhhh != "gexcxx2"
	
	FString SQL = TEXT("UPDATE ")+TableName+TEXT(" SET ");
	for (auto &Tmp : InFields)
	{
		SQL += (Tmp.ToString()) + TEXT(",");
	}
	SQL.RemoveFromEnd(TEXT(","));

	SimpleMysqlAssist::ConditionToString(SQL,Condition);
	
	SQL += TEXT(";");
	
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::ReplaceTableData(const FString& TableName, const TArray<FSimpleMysqlReplaceParamters>& InReplacs,
	const TArray<FSimpleMysqlComparisonOperator>& Condition, FString& ErrorMesg)
{
	//update role set age=REPLACE(age,20,30) WHERE name != 'www' and hhhhh != "gexcxx2"
	FString SQL = TEXT("UPDATE ")+TableName+TEXT(" SET ");
	for (auto &Tmp : InReplacs)
	{
		SQL += (Tmp.ToString()) + TEXT(",");
	}
	SQL.RemoveFromEnd(TEXT(","));

	SimpleMysqlAssist::ConditionToString(SQL,Condition);
	
	SQL += TEXT(";");
	
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::InsertTableData(const FString& TableNmae, const TMap<FString, FString>& InsertKeyValueData,const FSimpleMysqlQueryParameters &QueryParam,FString& ErrorMesg,bool bIgnore)
{
	//insert into 表名 (字段1,字段2,字段3) values (值1，值2，值3);       插入给定数据和值  
	//insert into 临时表名 (字段1,字段2...)  select 字段1,字段2，...or * from 现有表   从其他表拷贝数据插入
	FString SQL = TEXT("INSERT ");
	if(bIgnore)
	{
		SQL += TEXT("IGNORE ");
	}
	SQL += TEXT("INTO ") + TableNmae + TEXT(" ");

	SimpleMysqlAssist::SpawnKeyAndValue(SQL,InsertKeyValueData);
	
	//在拷贝时的选择参数
	if(QueryParam.IsValid())
	{
		QueryParam.ToString(SQL);
	}

	SQL += TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::SimpleInsertTableData(const FString& TableNmae, const TArray<FString>& InsertValueData,FString& ErrorMesg,bool bIgnore)
{
	//insert (IGNORE) into 表名 values(值1，值2，值3...)
	
	FString SQL = TEXT("INSERT ");
	if(bIgnore)
	{
		SQL += TEXT("IGNORE ");
	}

	SQL += TEXT("INTO ") + TableNmae + TEXT(" ");

	SimpleMysqlAssist::SpawnValue(SQL,InsertValueData);
	
	SQL += TEXT(";");

	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::ReplaceIfExistence(const FString& TableNmae,const TMap<FString, FString>& InsertKeyValueData,FString& ErrorMesg)
{
	// Replace into tablename (字段1, 字段2, ...) VALUES (值1, 值2, ...);
	//忽略主键唯一性，会替换主键或唯一约束键

	FString SQL = TEXT("REPLACE INTO ") + TableNmae + TEXT(" ");
	
	SimpleMysqlAssist::SpawnKeyAndValue(SQL,InsertKeyValueData);
	SQL += TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::ChangeEngine(const FString& InTableName, EMysqlSaveEngine InEngine, FString& ErrorMesg)
{
	//ALTER TABLE 表名 ENGINE = xxx;
	UEnum *EngineEnum = StaticEnum<EMysqlSaveEngine>();
	FString FieldTypeString = EngineEnum->GetNameStringByIndex((int32)InEngine);

	FString SQL =TEXT("ALTER TABLE ") + InTableName + TEXT(" ENGINE = ");
	SQL +=  FieldTypeString + TEXT(";");
	
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::DropTableField(const FString& InTableName,const TArray<FString>& InFieldName, FString& ErrorMesg)
{
	//alter table 表名  drop column 字段名,column 字段名2。。。
	FString SQL = TEXT("ALTER TABLE ") + InTableName + TEXT(" DROP ");
	
	for (auto &Tmp : InFieldName)
	{
		SQL += TEXT(" COLUMN ") + Tmp + TEXT(",");
	}
	SQL.RemoveFromEnd(",");
	SQL += TEXT(";");
	
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::AddTableField(const FString& InTableName, const TMap<FString, FMysqlFileType>& InFields,
	FString& ErrorMesg)
{
	//ALTER TABLE 表名 ADD 字段名 类型 (更多字段属性)
	FString SQL = TEXT("ALTER TABLE ") + InTableName + TEXT(" ADD ");
	
	SimpleMysqlAssist::FieldTypeArrayToString(SQL,InFields);

	SQL += TEXT(";");
	
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::ModifyTableFieldType(const FString& InTableName, const TMap<FString, FMysqlFileType>& InFields,
	FString& ErrorMesg)
{
	//alter table 表名 modify 选定字段名 新类型 新属性
	FString SQL = TEXT("ALTER TABLE ") + InTableName + TEXT(" MODIFY ");
	SimpleMysqlAssist::FieldTypeArrayToString(SQL,InFields);
	SQL += TEXT(";");
	
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::ChangeTableField(const FString& InTableName, const TArray<FSimpleMysqlModifyField>& InChangeFields,
	FString& ErrorMesg)
{
	//alter table 表名 change 源字段名 新字段名 新类型   //可将字段列的名称和类型进行改变
	FString SQL = TEXT("ALTER TABLE ") + InTableName + TEXT(" CHANGE ");
	for(const auto &Tmp : InChangeFields)
	{
		SQL += Tmp.ToString() + TEXT(",");
	}
	SQL.RemoveFromEnd(TEXT(","));
	SQL += TEXT(";");
	
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::RenameTable(const FString& InOldTableName, const FString& InNewTableName,FString &ErrorMesg)
{
	//alter table 表名 rename to 新表名;      更改表名
	FString SQL = TEXT("ALTER TABLE ") + InOldTableName + TEXT(" RENAME TO ") + InNewTableName + TEXT(";");

	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::QueryLinkStmt(const FString& SQL, const TArray<FSimpleMysqlQueryStmt>& InQueryStmtParam,
	FString& ErrorMesg)
{
	bool bQuerySucess = false;
	
	FString PrepareSQLError;
	FString PrepareParametersError;
	FString ExecuteStmtError;
	if(PrepareStmtSQL(SQL,PrepareSQLError) && PrepareStmtParameters(InQueryStmtParam,PrepareParametersError) && ExecuteStmt(ExecuteStmtError))
	{
		bQuerySucess = true;
	}
	ErrorMesg +=( PrepareSQLError + PrepareParametersError + ExecuteStmtError);
	return bQuerySucess;
}

bool FSimpleMysqlLink::PrepareStmtSQL(const FString &SQL, FString& ErrorMesg)
{
	/*
	*int mysql_stmt_prepare(MYSQL_STMT* stmt,const char *query,unsigned long length)  准备预处理语句
	*query 查询语句
	*Length 查询语句的长度
	 */

	if(mysql_stmt_prepare(MysqlStmt,TCHAR_TO_UTF8(*SQL),SQL.Len()) == 0)
	{
		return true;
	}
	ErrorMesg = UTF8_TO_TCHAR(mysql_stmt_error(MysqlStmt));
	return false;
}

bool FSimpleMysqlLink::PrepareStmtParameters(const TArray<FSimpleMysqlQueryStmt>& InQueryStmtParam, FString& ErrorMesg)
{
	/*
	char content[] = "Hello";
	Pram[1].buffer_type = MYSQL_TYPE_VARCHAR;  //VarChar类型，一个字符一个字节
	Pram[1].buffer = content;
	uint32 contentlen = strlen(content);  
	Pram[1].buffer_length = contentlen;  //指定存储缓冲区大小，可变的大小需要手动指定
	Pram[1].length = (unsigned long*)&contentlen;  //字符串的字符串字节大小，和缓冲区大小应该是一样的。 //长度必须指明，否则会报客户端缓冲区不足，需要更大空间的问题。
	 */
	
	if(InQueryStmtParam.Num() != 0)
	{
		int32 MemorySize = sizeof(MYSQL_BIND) * InQueryStmtParam.Num();
		BindAllocation.Size = InQueryStmtParam.Num();
		BindAllocation.BindParam = (MYSQL_BIND*)FMemory::Malloc(MemorySize);
		FMemory::Memzero(BindAllocation.BindParam,MemorySize);

		uint8 *ByteCode = (uint8*)BindAllocation.BindParam;
		for(const auto &Tmp : InQueryStmtParam)
		{
			MYSQL_BIND * TmpBind = (MYSQL_BIND*)ByteCode;
			TmpBind->buffer_type = (enum_field_types)Tmp.VariableType;
			Tmp.GetValue(&TmpBind->buffer);
			TmpBind->length = (unsigned long*)FMemory::Malloc(sizeof(unsigned long));
			*TmpBind->length = Tmp.GetValueTypeLen();
			TmpBind->buffer_length = Tmp.GetValueTypeLen();
			ByteCode += sizeof(MYSQL_BIND);
		}
	}

	return false;
}

bool FSimpleMysqlLink::ExecuteStmt(FString& ErrorMesg)
{
	if(mysql_stmt_execute(MysqlStmt) == 0)
	{
		return true;
	}

	ErrorMesg = UTF8_TO_TCHAR(mysql_stmt_error(MysqlStmt));

	BindAllocation.Free();
	return false;
}

bool FSimpleMysqlLink::StartTransaction(FString& ErrorMesg)
{
	FString SQL = TEXT("START TRANSACTION;");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::SetAutocommit(bool bAuto, FString& ErrorMesg)
{
	FString SQL = TEXT("SET AUTOCOMMIT =")+ FString::Printf(TEXT("i"),bAuto) + TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::Commit(FString& ErrorMesg)
{
	FString SQL = TEXT("COMMIT;");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::SetSavePointName(const FString& SaveName, FString& ErrorMesg)
{
	FString SQL = TEXT("SAVEPOINT ") + SaveName +TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::ReleaseSavepoint(const FString& SaveName, FString& ErrorMesg)
{
	FString SQL = TEXT("RELEASE SAVEPOINT ") + SaveName +TEXT(";");
	return QueryLink(SQL,ErrorMesg);
}

bool FSimpleMysqlLink::Rollback(const FString& SaveName, FString& ErrorMesg)
{
	FString SQL = TEXT("ROLLBACK");
	if(!SaveName.IsEmpty())
	{
		SQL += TEXT(" TO SAVEPOINT ") + SaveName;
	}
	
	SQL += TEXT(";");
	
	return QueryLink(SQL,ErrorMesg);
	
}




bool FSimpleMysqlLink::GetStroeResult(TArray<FSimpleMysqlResult>& Results,FString& ErrorMesg,const FSimpleMysqlDebugResult &Debug)
{
	if(MYSQL_RES* Result = mysql_store_result(Mysql)) 
	{
		GetResult(Result,Results);
		
			PrintResult(Results,Debug.bPrinttToScreen,Debug.bPrintToLog);
	}
		ErrorMesg = mysql_error(Mysql);
		
		return ErrorMesg.IsEmpty();
}

bool FSimpleMysqlLink::GetUseResult(TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,const FSimpleMysqlDebugResult &Debug)
{
	
	if(MYSQL_RES* Result = mysql_use_result(Mysql)) 
	{
		GetResult(Result,Results);
		
			PrintResult(Results,Debug.bPrinttToScreen,Debug.bPrintToLog);
	}
		ErrorMesg = mysql_error(Mysql);

		return ErrorMesg.IsEmpty();
}

bool FSimpleMysqlLink::PrintResult(const TArray<FSimpleMysqlResult>& Results, bool bPrintToScreen /* = true */, bool bPrintToLog /* = true */)
{
	
	for(auto &Tmp : Results)
	{
		FString RowString;
		for(int i = 0;i < Tmp.Rows.Num();i++)
		{
			int32 len =(Tmp.Rows[i].Len()) / 2;
			RowString+="|";
			int32 AddLen = 10 - len;
			for (int j = 0;j < AddLen ;j++)
			{
				RowString+=" ";
			}
			RowString+=Tmp.Rows[i];
				
			for (int j = 0;j < AddLen ;j++)
			{
				RowString+=" ";
			}
		}
		RowString+="|";
		//Mysql_Printf(*RowString,bPrinttToScreen,bPrintToLog,FColor::Red,100.f);
		if(bPrintToScreen)
		{GEngine->AddOnScreenDebugMessage(-1,10,FColor::Cyan,*RowString);}
		if(bPrintToLog)
		{UE_LOG(LogTemp, Log,TEXT("%s"),*RowString);}
	}

	return Results.Num() != 0;
}


//表示每一行数据
void FSimpleMysqlLink::GetResult(MYSQL_RES* Result, TArray<FSimpleMysqlResult>& Results)
{
	int NumRow = mysql_num_fields(Result); 
	while (MYSQL_ROW SQLRow = mysql_fetch_row(Result))  
	{
		Results.Add(FSimpleMysqlResult());  //添加一行数据
		FSimpleMysqlResult *R = &Results[Results.Num() - 1];
			
		for (int i = 0; i < NumRow; ++i)
		{
			if(SQLRow[i] != NULL)
			{
				R->Rows.Add(ANSI_TO_TCHAR(SQLRow[i]));  //Rows表示每个字段的数组
					
			}
			else
			{
				R->Rows.Add(TEXT("NULL"));
			}
		}
	}
}



bool FSimpleMysqlLink::GetSelectTableDataSQL(FString& SQL,const FSimpleMysqlQueryParameters& QueryParam)
{
	bool bSelectTableDataSQLSuccess = QueryParam.ToString(SQL);
	SQL += TEXT(";");
	return bSelectTableDataSQLSuccess;
}


uint32 FSimpleMysqlLink::ToMySqlClientFlag(ESimpleClientFlags ClientFlags) const
{
	switch (ClientFlags)
	{
	case ESimpleClientFlags::Client_Long_Password:
		
		return 1;
	case ESimpleClientFlags::Client_Found_Rows:
		
		return 2;
	case ESimpleClientFlags::Client_Long_Flag:
		
		return 4;
	case ESimpleClientFlags::Client_Connect_With_DB:
		
		return 8;
	case ESimpleClientFlags::Client_No_Schema:
		
		return 16;
	case ESimpleClientFlags::Client_Compress:
		
		return 32;
	case ESimpleClientFlags::Client_ODBC:
		
		return 64;
	case ESimpleClientFlags::Client_Local_Files:
		
		return 128;
	case ESimpleClientFlags::Client_Ignore_Space:
		
		return 256;
	case ESimpleClientFlags::Client_Protocol_41:
		
		return 512;
	case ESimpleClientFlags::Client_Interactive:
		
		return 1024;
	case ESimpleClientFlags::Client_SSL:
		
		return 2048;
	case ESimpleClientFlags::Client_Ignore_Sigpipe:
		
		return 4096;
	case ESimpleClientFlags::Client_Transactions:
		
		return 8192;
	case ESimpleClientFlags::Client_Reserved:
		
		return 16384;
	case ESimpleClientFlags::Client_Reserved2:
		
		return 32768;
	case ESimpleClientFlags::Client_Multi_Statements:
		
		return (1UL << 16);
	case ESimpleClientFlags::Client_Multi_Results:
		
		return (1UL << 17);
	case ESimpleClientFlags::Client_Ps_Multi_Results:
		return (1UL << 18);
	}

	return 0;
}
