#pragma once

#include "Core/SimpleMysqlLinkType.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimpleMySQLLibrary.generated.h"

class USimpleMysqlObject;

/*
 * 这个对象是负责管理Mysql操作的全局蓝图静态函数库
 * 提供创建Mysql管理器的蓝图函数
 * 只为蓝图使用
 */

UCLASS()
class SIMPLEMYSQLTHREAD_API USimpleMySQLLibrary : public UBlueprintFunctionLibrary
{
public:
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable,meta = (Keywords = "SimpleMySQL Sample function"),Category="SimpleMySQLTesting")
	static void MySqlMain();

	//创建数据库管理联对象
	UFUNCTION(BlueprintCallable,meta = (WorldContext = "WorldContextObject",Category="SimpleMySQLTesting"))
	static USimpleMysqlObject *CreateMysqlObject(UObject* WorldContextObject,const FString& InUser, const FString& InHost, const FString& InPawd,const FString& InDB, const int InPort, const TArray<ESimpleClientFlags>& InClientFlag,const FString& InUnix_Socket = TEXT(""));

	//数据库操作
	/////////////////////////////////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|DataBase"))
	static bool SelectNewDB(USimpleMysqlObject *Object,const FString& NewDB,FString &ErrMesg);
	
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|DataBase"))
	static bool CreateDataBase(USimpleMysqlObject *Object,const FString& DataBaseName, EMysqlCharset Charset, FString& ErrorMeg);

	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|DataBase"))
	static bool CreateAndSelectDataBase(USimpleMysqlObject *Object,const FString& DataBaseName, EMysqlCharset Charset, FString& ErrorMeg);
	
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|DataBase"))
	static bool CreateTable(USimpleMysqlObject *Object,const FString &TableName,const TMap<FString,FMysqlFileType> &InFields,const TArray<FString>& InPrimaryKeys,const FMysqlCreateTableParam& Param,FString &ErrorMeg);

	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|DataBase"))
	static bool DropDataBase(USimpleMysqlObject *Object,const FString& DataBaseName, FString& ErrorMeg);

	//表操作
	/////////////////////////////////////////////////////////////////////////////////////////
	//删除表，释放内存，不支持数据回滚
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Table"))
	static bool DropTable(USimpleMysqlObject *Object,const FString& TableName,FString &ErrorMsg);

	//清除表数据，释放内存，不支持回滚
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Table"))
	static bool TruncateTable(USimpleMysqlObject *Object,const FString& TableName,FString &ErrorMsg);

	//删除表数据，内存保留，可回滚
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Table"))
	static bool DeleteFromTable(USimpleMysqlObject *Object,const FString& TableName,FString &ErrorMsg);

	//删除表里指定行数据，内存保留，可回滚
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Table"))
	static bool DeleteFromTableWhereData(USimpleMysqlObject *Object,const FString& TableName,TArray<FSimpleMysqlComparisonOperator>& Condietion,FString &ErrorMsg);
	
	//清除删除的数据的内存
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Table"))
	static bool OptimiseTable(USimpleMysqlObject *Object,const FString& TableName,FString &ErrorMsg);
	/////////////////////////////////////////////////////////////////////////////////////////
	///临时表的创建
	///单独创建临时表
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|TemporaryTable"))
	static bool CreateTemporaryTable(USimpleMysqlObject* Object,const FString& NewTableName,const TMap<FString,FMysqlFileType> &InFields,const TArray<FString>& InPrimaryKeys,const FMysqlCreateTableParam& Param,FString &ErrorMesg);
	//从其他表导入数据,并创建临时表，可自定义字段属性，复制表字段，where条件
	//	CREATE  TEMPORARY TABLE tmp_table(name VARCHAR(50) DEFAULT 'Unknown',age INT AUTO_INCREMENT PRIMARY KEY) SELECT name,age FROM exist_table;
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|TemporaryTable"))
	static bool CreateTemporaryTableFromOther(USimpleMysqlObject* Object,const FString& NewTableName,const TMap<FString,FMysqlFileType> &InFields,const TArray<FString>& InPrimaryKeys,const FSimpleMysqlQueryParameters& QueryParam,TArray<FSimpleMysqlComparisonOperator>& Condition,FString &ErrorMesg);
	
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|TemporaryTable"))
	static bool DropTemporaryTable(USimpleMysqlObject* Object,const FString& TemporaryTable, FString& ErrorMesg);
	//////////////////////////////////////
	//这个是我自己封装的，不是宅哥那个
	//create table 新表(新字段 类型 属性) AS (select * from 旧表...)  //复制的时候，除了拷贝旧的字段外，可以添加新的字段
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|TemporaryTable"))
	static bool CreateTemporaryTableAsOther(USimpleMysqlObject* Object,const FString& TemporaryTable,const TMap<FString,FMysqlFileType> &NewInFields,const TArray<FString>& InPrimaryKeys,const FSimpleMysqlQueryParameters &QueryParam ,FString& ErrorMesg);
	//快速复制表
	/*
	create  table 新表 AS (select 字段n，字段n...  from 原表 ...);   //只复制部分字段并进行表的创建，新表将只拥有给定的字段
	create table 新表 AS (select 字段n as 新字段名  ... from 原表...); 
	create table 新表(新字段 类型 属性) AS (select * from 旧表...)  //复制的时候，除了拷贝旧的字段外，可以添加新的字段
	*/
	//这个是宅哥创建的
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|TemporaryTable"))
	static bool CopyTableAToBFast(USimpleMysqlObject* Object,const FString& CopyTargetTable,const TMap<FString,FMysqlFileType> &InsertFields,const TArray<FString>& InPrimaryKeys,const FSimpleMysqlQueryParameters &QueryParam ,FString& ErrorMesg);
	//快速复制临时表
	/*
	create  table 新表 AS (select 字段n，字段n...  from 原表 ...);   //只复制部分字段并进行表的创建，新表将只拥有给定的字段
	create table 新表 AS (select 字段n as 新字段名  ... from 原表...); 
	create table 新表(新字段 类型 属性) AS (select * from 旧表...)  //复制的时候，除了拷贝旧的字段外，可以添加新的字段
	*/
	//这个是宅哥创建的
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|TemporaryTable"))
	static bool CopyTableAToTemporaryBFast(USimpleMysqlObject* Object,const FString& CopyTargetTable,const TMap<FString,FMysqlFileType> &InsertFields,const TArray<FString>& InPrimaryKeys,const FSimpleMysqlQueryParameters &QueryParam ,FString& ErrorMesg);
    //////////////////////////////////////////////////////////////////////////////////////////////////
	//表和临时表的拷贝
	//表拷贝 ，先like 复制结构，后在insert into 可以选择要复制的数据
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|CopyTable"))
	static bool CopyTableAToB(USimpleMysqlObject* Object,const FString& A,const FString& B,const TArray<FString> &Fields,FString &ErrorMesg);
	//拷贝数据到临时表，先like 复制结构，后在insert into 可以选择要复制的数据
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|CopyTable"))
	static bool CopyTableAToTemporaryB(USimpleMysqlObject* Object,const FString& Table,const FString& Temporary,const TArray<FString> &Fields,FString &ErrorMesg);
	///////////////////////////////////////////////////////////////////////////////////////////////
	///执行SELECT语句获取结果q
	//只负责生成SELECT的SQL语句，不负责执行
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Select"))
	static bool GetSelectTableDataSQL(USimpleMysqlObject *Object,FString& SQL,const FSimpleMysqlQueryParameters& QueryParam);
	//生成SQL语句并执行，并且获取结果
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Select"))
	static bool GetSelectTableData(USimpleMysqlObject *Object,EMysqlQuerySaveType SaveType,const FSimpleMysqlQueryParameters& QueryParam, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,const FSimpleMysqlDebugResult& Debug);
	//单独查询SQL语句
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Select"))
	static bool QueryLink(USimpleMysqlObject *Object,const FString &SQL,FString &ErrMesg);
	//debug打印结果
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Select"))
	static bool PrintResult(USimpleMysqlObject *Object,const TArray<FSimpleMysqlResult> &Results,bool bPrinttToScreen = true,bool bPrintToLog = true);
	//更新数据 updata
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Select"))
	static bool UpdataTableData(USimpleMysqlObject* Object,const FString& TableName,const TArray<FSimpleMysqlAssignment> &InFields,const TArray<FSimpleMysqlComparisonOperator> &Condition ,FString &ErrorMesg);
	//替换式更新 updata
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Select"))
	static bool ReplaceTableData(USimpleMysqlObject* Object,const FString& TableName,const TArray<FSimpleMysqlReplaceParamters> &InReplaces,const TArray<FSimpleMysqlComparisonOperator> &Condition ,FString &ErrorMesg);
	//快速显示一个表所有内容
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Select"))
	static bool ShowTables(USimpleMysqlObject* Object,const FString& TableName,EMysqlQuerySaveType SaveType,TArray<FSimpleMysqlResult>& Results,FString& ErrorMesg, const FSimpleMysqlDebugResult& Debug);
	///////////////////////////////////////////////////////////////////////////////////////
	//插入表数据
	//insert (ignore) into 表名 (字段1,字段2,字段3) values (值1，值2，值3);
	//insert into 临时表名 (字段1,字段2...)  select 字段1,字段2，...or * from 现有表   从其他表拷贝数据插入
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Insert"))
	static bool InsertTableData(USimpleMysqlObject* Object,const FString& TableNmae, const TMap<FString, FString>& InsertKeyValueData,const FSimpleMysqlQueryParameters &QueryParam,FString& ErrorMesg,bool bIgnore);
	//快捷插入，自动匹配
	//insert into values (值1，值2，值3);
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Insert"))
	static bool SimpleInsertTableData(USimpleMysqlObject* Object,const FString& TableNmae,const TArray<FString>& InsertValueData,FString& ErrorMesg,bool bIgnore);
	//Replace into tablename (字段1, 字段2, ...) VALUES (值1, 值2, ...);
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Insert"))
	static bool ReplaceIfExistence(USimpleMysqlObject* Object,const FString& TableNmae,const TMap<FString,FString> &InsertKeyValueData,FString& ErrorMesg);
	//////////////////////////////////////////////////////////////////////////////////////////
	///元数据
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|MetaData"))
	static bool GetSelectVersion(USimpleMysqlObject* Object,TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,EMysqlQuerySaveType SaveType = EMysqlQuerySaveType::STORE_RESULT,const FSimpleMysqlDebugResult& Debug = FSimpleMysqlDebugResult());
	//获取数据库名
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|MetaData"))
	static bool GetSelectDatabase(USimpleMysqlObject* Object,TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,EMysqlQuerySaveType SaveType = EMysqlQuerySaveType::STORE_RESULT,const FSimpleMysqlDebugResult& Debug = FSimpleMysqlDebugResult());
	//获取用户名
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|MetaData"))
	static bool GetSelectUser(USimpleMysqlObject* Object,TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,EMysqlQuerySaveType SaveType = EMysqlQuerySaveType::STORE_RESULT,const FSimpleMysqlDebugResult& Debug = FSimpleMysqlDebugResult());
	//显示服务器元数据
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|MetaData"))
	static bool GetShowStatus(USimpleMysqlObject* Object,TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,EMysqlQuerySaveType SaveType = EMysqlQuerySaveType::STORE_RESULT,const FSimpleMysqlDebugResult& Debug = FSimpleMysqlDebugResult());
	//查看服务器的变量信息
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|MetaData"))
	static bool GetShowVariables(USimpleMysqlObject* Object,TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug);
	///////////////////////////////////////////////////////////////////////////////////////////////
	//自定义函数
	UFUNCTION(BlueprintCallable, meta = (Category = "SimpleMySQL"))
	static bool CreateFunction(USimpleMysqlObject *Object, const FString &InFunctionName, const TMap<FString, FMysqlFileType> &InParameter, const FString &InFunctionBody, FString& ErrMesg, EMysqlVariableType InReturnsType, const FString &InComment);
	
	UFUNCTION(BlueprintCallable, meta = (Category = "SimpleMySQL"))
	static bool ExecuteFunction(USimpleMysqlObject *Object, const FString &InFunctionName, const TArray<FString> &InParameter, TArray<FSimpleMysqlResult>& Results, FString& ErrMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug);
	
	UFUNCTION(BlueprintCallable, meta = (Category = "SimpleMySQL"))
	static bool DropFunction(USimpleMysqlObject *Object, const FString &InFunctionName, FString& ErrMesg);
	
	UFUNCTION(BlueprintCallable, meta = (Category = "SimpleMySQL"))
	static bool ShowCreateFunction(USimpleMysqlObject *Object, const FString &InFunctionName, TArray<FSimpleMysqlResult>& Results, FString& ErrMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug);
	
	UFUNCTION(BlueprintCallable, meta = (Category = "SimpleMySQL"))
	static bool ShowFunctionsStatus(USimpleMysqlObject *Object, TArray<FSimpleMysqlResult>& Results, FString& ErrMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug);

	////////////////////////////////////////
	///自定义存储过程

	UFUNCTION(BlueprintCallable, meta = (Category = "SimpleMySQL"))
	static bool DefiningVariables(USimpleMysqlObject *Object, const FString &InVariablesName, const FString &InValue, FString &ErrMesg);
	
	UFUNCTION(BlueprintCallable, meta = (Category = "SimpleMySQL"))
	static bool CreateProcedure(USimpleMysqlObject *Object, const FString &InProcedureName, const TMap<FString, FMysqlFileType> &InParameter, const FString &InProcedureBody, FString& ErrMesg, const FString &InComment);
	
	UFUNCTION(BlueprintCallable, meta = (Category = "SimpleMySQL"))
	static bool ExecuteProcedure(USimpleMysqlObject *Object, const FString &InProcedureName, const TArray<FString> &InParameter, TArray<FSimpleMysqlResult>& Results, FString& ErrMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug);
	
	UFUNCTION(BlueprintCallable, meta = (Category = "SimpleMySQL"))
	static bool DropProcedure(USimpleMysqlObject *Object, const FString &InProcedureName, FString& ErrMesg);
	
	UFUNCTION(BlueprintCallable, meta = (Category = "SimpleMySQL"))
	static bool ShowCreateProcedure(USimpleMysqlObject *Object, const FString &InProcedureName, TArray<FSimpleMysqlResult>& Results, FString& ErrMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug);
	
	UFUNCTION(BlueprintCallable, meta = (Category = "SimpleMySQL"))
	static bool ShowProceduresStatus(USimpleMysqlObject *Object, TArray<FSimpleMysqlResult>& Results, FString& ErrMesg, EMysqlQuerySaveType SaveType, const FSimpleMysqlDebugResult& Debug);
	
	/////////////////////////////////////////////////////////////////////////////////////////
	//启动事务
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Transaction"))
	static bool StartTransaction(USimpleMysqlObject* Object,FString &ErrorMesg);
	//设置手动提交
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Transaction"))
	static bool SetAutocommit(USimpleMysqlObject* Object,bool bAuto,FString &ErrorMesg);
	//提交
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Transaction"))
	static bool Commit(USimpleMysqlObject* Object,FString& ErrorMesg);
	//设置保存点
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Transaction"))
	static bool SetSavePointName(USimpleMysqlObject* Object,const FString &SaveName,FString &ErrorMesg);
	//回滚
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Transaction"))
	static bool Rollback(USimpleMysqlObject* Object,const FString &SaveName,FString &ErrorMesg);
	//释放保存点
	UFUNCTION(BlueprintCallable,meta = (Category="SimpleMySQL|Transaction"))
	static bool ReleaseSavepoint(USimpleMysqlObject* Object,const FString& SaveName, FString& ErrorMesg);
	////////////////////////////////////////////////////////////////////////////////////////
	///获取空值以进行默认值传参，因为UE反射不支持参数默认值
	UFUNCTION(BlueprintCallable,BlueprintPure,meta = (Category="SimpleMySQL|NULL"))
	static TArray<FString> GetStringNULLArray();

	UFUNCTION(BlueprintCallable,BlueprintPure,meta = (Category="SimpleMySQL|NULL"))
	static TMap<FString, FString> GetTMPStringNULLArray();
	
	UFUNCTION(BlueprintCallable,BlueprintPure,meta = (Category="SimpleMySQL|NULL"))
	static TArray<FSimpleMysqlQueryOrderBy> GetOrderByNULLArray();

	UFUNCTION(BlueprintCallable,BlueprintPure,meta = (Category="SimpleMySQL|NULL"))
	static FSimpleMysqlJoinParameters GetJoinParametersNULL();

	UFUNCTION(BlueprintCallable,BlueprintPure,meta = (Category="SimpleMySQL|NULL"))
	static TArray<FSimpleMysqlComparisonOperator> GetComparisonOperatorNULLArray();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (Category = "SimpleMySQL|NULL"))
	static FSimpleMysqlQueryParameters GetNULLQueryParameters();
	
	UFUNCTION(BlueprintCallable,BlueprintPure,meta = (Category="SimpleMySQL|NULL"))
	static TArray<FSimpleMysqlAssignment> GetAssignmentNULLArray();

	UFUNCTION(BlueprintCallable,BlueprintPure,meta = (Category="SimpleMySQL|NULL"))
	static TArray<FSimpleMysqlReplaceParamters>GeReplaceParamtersNULLArray();

	UFUNCTION(BlueprintCallable,BlueprintPure,meta = (Category="SimpleMySQL|NULL"))
	static TMap<FString,FMysqlFileType> GetInSertFieldsNULL();

	UFUNCTION(BlueprintCallable,BlueprintPure,meta = (Category="SimpleMySQL|NULL"))
	static TMap<FString, FMysqlFileType> GetInParameterNULL();

	UFUNCTION(BlueprintCallable,BlueprintPure,meta = (Category="SimpleMySQL|NULL"))
	static FSimpleMysqlModifyField GetlModifyFieldNULL();
};
