#pragma once

#include "CoreMinimal.h"
#include "SimpleMysqlLinkType.h"
#include "SimpleMySqlMacro.h"

struct FMysqlFileType;


struct st_mysql;
struct st_mysql_res;
struct st_mysql_stmt;
struct st_mysql_bind;
/*
* 这个对象负责链接和管理Mysql服务器和客户端的通信操作，包含了对SQL语句的封装。是操作数据库的桥梁。
* C++直接使用这个即可。因为不含反射信息，
*/

struct FSimpleMysqlLink : public TSharedFromThis<FSimpleMysqlLink>
{
	FSimpleMysqlLink();
	
	FSimpleMysqlLink(const FString &InUser,
		const FString &InHost,
		const FString &InPawd,
		const FString &InDB,
		const uint32 InPort = 0,
		const FString &InUnix_Socket = TEXT(""),
		const TArray<ESimpleClientFlags>& InClientFlag =TArray<ESimpleClientFlags>() );

	~FSimpleMysqlLink();

	void InitMysqlOptions(){};
	///////////////////////////////////////////////////////////////////////////////////////////////
	//输入和执行SQL语句
	bool QueryLink(const FString &SQL,FString &ErrorMesg);
	//负责合成Select语句，并不负责执行。
	bool GetSelectTableDataSQL(FString& SQL,const FSimpleMysqlQueryParameters& QueryParam);
	//最上层的SQL的select查询执行命令
	bool GetSelectTableData(EMysqlQuerySaveType SaveType,const FSimpleMysqlQueryParameters& QueryParam, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,const FSimpleMysqlDebugResult& Debug = FSimpleMysqlDebugResult());
	//显示表所有信息SELECT * FORM XXXX
	bool ShowTables(const FString& TableName,EMysqlQuerySaveType SaveType, TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,const FSimpleMysqlDebugResult& Debug);
	//输入SQL语句进行查询并获取结果，但是不用QueryParam生成SQL语句
	bool QueryLinkResult(const FString &SQL,MysqlFuntionParam);
	////////////////////////////////////////////////////////////////////////////////////////////////
	///自定义函数和存储过程的封装
	//自定义函数
	//创建
	bool CreateFunction(const FString &InFunctionName,const TMap<FString,FMysqlFileType> &InParameter,const FString &InFunctionBody,FString& ErrorMesg,EMysqlVariableType InReturnsType = EMysqlVariableType::MYSQL_LONG,const FString & InComment = TEXT(""));
	//执行
	bool ExecuteFunction(const FString &InFunctionName,const TArray<FString>  &InParameter,MysqlFuntionParam);
	//删除
	bool DropFunction(const FString& InFunctionName,FString& ErrorMesg);
	//显示创建的函数
	bool ShowCreateFunction(const FString &InFunctionName,MysqlFuntionParam);
	//显示所有函数
	bool ShowFunctionsStatus(MysqlFuntionParam);
	//////////////////////////////////////////////////////////////////////////
	///自定义存储过程
	//定义存储变量
	bool DefiningVariables(const FString &InVariablesName,const FString &InValue,FString& ErrorMesg);
	//创建存储过程
	bool CreateProcedure(const FString &InProcedureName,const TMap<FString,FMysqlFileType> &InParameter,const FString &InProcedureBody,FString& ErrorMesg,const FString & InComment = TEXT(""));
	//执行存储过程
	bool ExecuteProcedure(const FString &InProcedureName,const TArray<FString>  &InParameter,MysqlFuntionParam);
	//删除存储过程
	bool DropProcedure(const FString& InProcedureName,FString& ErrorMesg);
	//显示创建的存储过程
	bool ShowCreateProcedure(const FString &InProcedureName,MysqlFuntionParam);
	//显示所有存储过程
	bool ShowProceduresStatus(MysqlFuntionParam);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
	///内置高级函数的封装
	/**
	 * Binary encoding of return x .
	 * @param Value					Number written .
	 * @param InSecond				Time seconds .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT BIN(15); 
		-> 1111
	*/
	bool Bin(const int64 Value, MysqlFuntionParam);

	/**
	 * Converts the string s to a binary string .
	 * @param Value					String to be converted to binary .
	 * @param InSecond				Time seconds .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT BINARY "Renzhai";
		-> Renzhai
	*/
	bool Binary(const FString &Value, MysqlFuntionParam);

	/**
	 * Shifting clause .
	 * @param Value					String to be converted .
	 * @param InSecond				Time seconds .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT CAST("2018-08-29" AS DATE);
		-> 2018-08-29
	*/
	bool Cast(const FString& Value, EMysqlVariableType VariableType,  MysqlFuntionParam);

	/**
	 * Return F1 base number to F2 base number .
	 * @param Value					Specific value .
	 * @param F1					Custom base conversion rules .
	 * @param F2					Custom base conversion rules .
	 * @param InSecond				Time seconds .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT CONV(15, 10, 2);
		-> 1111
	*/
	bool Conv(const FString& Value, int64 F1, int64 F2, MysqlFuntionParam);

	/**
	 * Function to change the character set of string s to CS .
	 * @param InString				String to be converted .
	 * @param Charset				Converted character set .
	 * @param InSecond				Time seconds .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT CHARSET('ABC')
		->utf-8
		SELECT CHARSET(CONVERT('ABC' USING gbk))
		->gbk
	*/
	bool Convert(const FString& InString, EMysqlCharset Charset, MysqlFuntionParam);

	/**
	 * If the expression expr holds, the result V1 is returned; otherwise, the result V2 is returned.
	 * @param Expression			Based on expression .
	 * @param ValueA				A.
	 * @param ValueB				B.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT IF(1 > 0,'Y','F')
		->Y
	*/
	bool BinocularOperation(const FString &Expression, const FString& ValueA, const FString& ValueB, MysqlFuntionParam);
	
	/**
	 * If the value of V1 is not null, V1 is returned, otherwise V2 is returned. 
	 * @param Expression			Based on expression .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT IFNULL(null,'Hello Word')
		->Hello Word
	*/
	bool IsNULL(const FString& Expression, MysqlFuntionParam);

	/**
	 * Compare two strings. If the strings expr1 and expr2 are equal, null is returned. Otherwise, expr1 is returned. 
	 * @param ValueA				Comparison value.
	 * @param ValueB				Compared value.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT NULLIF(25, 25);
		->
	*/
	bool NULLIf(const FString& ValueA, const FString& ValueB, MysqlFuntionParam);

	/**
	 * character set . 
	 * @param InExpression			Expression.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT CHARSET(CONVERT('ABC' USING gbk))
		->gbk
	*/
	bool Charset(const FString& InExpression, MysqlFuntionParam);

	/**
	 * Returns the number of connections to the server . 
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT CONNECTION_ID();
		-> 1
	*/
	///////////////////////////////////////////////////////////////////////////////////////////////////
	///元数据访问
	bool GetConnection_ID(MysqlFuntionParam);

	/**
	 * Return to current user .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT CURRENT_USER();
		-> root@loclhost
	*/
	bool GetCurrent_user(MysqlFuntionParam);

	/**
	 * Returns the most recently generated Auto_ Increment value .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT LAST_INSERT_ID();
		->6
	*/
	bool GetLast_Insert_ID(MysqlFuntionParam);

	/**
	 * Return to current user .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT CURRENT_USER();
		-> root@loclhost
	*/
	bool GetSession_user(MysqlFuntionParam);

	/**
	 * Return to current user .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT CURRENT_USER();
		-> root@loclhost
	*/
	bool GetSystem_user(MysqlFuntionParam);

	//元数据操作
public:
	/**
	 * Server version information .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	*/
	bool GetSelectVersion(MysqlFuntionParam);

	/**
	 * Current database name (or return empty) .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	*/
	bool GetSelectDatabase(MysqlFuntionParam);

	/**
	 * Current user name  .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	*/
	bool GetSelectUser(MysqlFuntionParam);

	/**
	 * Server status  .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	*/
	bool GetShowStatus(MysqlFuntionParam);

	/**
	 * Server configuration variables .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	*/
	bool GetShowVariables(MysqlFuntionParam);

	/////////////////////////////////////////////////////////////////////////////////
	///Mysql时间日期函数
	//////////////////////////////////////////////////////////////////////////
	/**
	 * Return the year and week (0 to 53), 0 in mode means Sunday, 1 means Monday, and so on .
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23 
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example : 
		SELECT YEARWEEK("2017-06-15");
		-> 201724
	*/
	bool YearWeek(const FString &InDate, MysqlFuntionParam);

	/**
	 * Calculation date InDate is the week ordinal of the year, ranging from 0 to 53 (About fifty-three weeks a year).
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT WEEKOFYEAR('2011-11-11 11:11:11')
		-> 45
	*/
	bool WeekOfYear(const FString& InDate, MysqlFuntionParam);

	/**
	 * Date InDate is the day of the week, 0 is Monday, 1 is Tuesday 
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT WEEKDAY("2017-06-15");
		-> 3
	*/
	bool WeekDay(const FString& InDate, MysqlFuntionParam);

	/**
	 * Return year .
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT YEAR("2017-06-15");
		-> 2017
	*/
	bool Year(const FString& InDate, MysqlFuntionParam);

	/**
	 * Calculation date InDate is the week ordinal of the year, ranging from 0 to 53 (About fifty-three weeks a year).
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT WEEK('2011-11-11 11:11:11')
		-> 45
	*/
	bool Week(const FString& InDate, MysqlFuntionParam);

	/**
	 * Days from calculation date d to January 1, 0000.
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT TO_DAYS('0001-01-01 01:01:01')
		-> 366
	*/	
	bool ToDays(const FString& InDate, MysqlFuntionParam);

	/**
	 * When a single parameter is used, the function returns a date or date time expression; when there are 2 parameters, the parameters are added.
	 * @param InExpression			Expression of statement or fill in date similar to 2022-10-12 10:25:23.
	 * @param InInterval			Time like content .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT TIMESTAMP("2017-07-23",  "13:10:11");
		-> 2017-07-23 13:10:11
	*/	
	bool TimeStamp(const FString& InExpression, const FString& InInterval, MysqlFuntionParam);

	/**
	 * Calculate time difference .
	 * @param InTime1				Given custom time 1.
	 * @param InTime2				Given custom time 2.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT TIMESTAMP("2017-07-23",  "13:10:11");
		-> 2017-07-23 13:10:11
	*/	
	bool TimeDiff(const FString& InTime1, const FString& InTime2, MysqlFuntionParam);

	/**
	 * Convert time InTime to seconds .
	 * @param InTime				Time to convert.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT TIME_TO_SEC('1:12:00')
		-> 4320
	*/	
	bool TimeToSec(const FString& InTime, MysqlFuntionParam);

	/**
	 * Display time InTime as required by expression format.
	 * @param InTime				Time to convert.
	 * @param InFormat				You can customize the layout format, such as %M %Y %D %r, etc .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT TIME_FORMAT('11:11:11','%r')
		-> 11:11:11 AM
	*/	
	bool TimeFormat(const FString& InTime,const FString& InFormat, MysqlFuntionParam);

	/**
	 * Extract the time part of the incoming expression .
	 * @param InExpression			Expression of statement or fill in date similar to 2022-10-12 10:25:23.
	 * @param InFormat				You can customize the layout format, such as %M %Y %D %r, etc .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT TIME("19:30:10");
		-> 19:30:10
	*/	
	bool Time(const FString& InExpression, MysqlFuntionParam);

	/**
	 * Returns the current date and time .
	 * @param InFormat				You can customize the layout format, such as %M %Y %D %r, etc .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT SYSDATE()
		-> 2020-06-19 20:59:41
	*/	
	bool SysDate(MysqlFuntionParam);

	/**
	 * Time InTime minus InSecond seconds  .
	 * @param InTime				Specific time like 2011-11-11 11:11:11.
	 * @param InSecond				Time seconds .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT SUBTIME('2011-11-11 11:11:11', 5)
		->2011-11-11 11:11:06 (Second)
	*/	
	bool SubTime(const FString& InTime,int64 InSecond,MysqlFuntionParam);

	/**
	 * Date InDate minus InDay days .
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23.
	 * @param InDay					Time day .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT SUBDATE('2011-11-11 11:11:11', 1)
		->2011-11-10 11:11:11 (Default is day)
	*/	
	bool SubDate(const FString& InDate,int64 InDay,MysqlFuntionParam);

	/**
	 * Convert string to date .
	 * @param InString				Text description week like august 10 2020.
	 * @param InFormatMask			You can customize the layout format, such as %M %Y %D %r, etc .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT STR_TO_DATE("August 10 2017", "%M %d %Y");
		-> 2017-08-10
	*/	
	bool StringToDate(const FString& InString, const FString& InFormatMask,MysqlFuntionParam);

	/**
	 * Converts time s in seconds to minutes and seconds .
	 * @param InSecond				Time seconds .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT SEC_TO_TIME(4320)
		-> 01:12:00
	*/	
	bool SecToTime(const int64 InSecond,MysqlFuntionParam);

	/**
	 * Returns the second value in InTime.
	 * @param InTime				Specific time like 2011-11-11 11:11:11.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT SECOND('1:2:3')
		-> 3
	*/	
	bool Second(const FString &InTime,MysqlFuntionParam);

	/**
	 * Return date D is the season, return 1 to 4 .
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT QUARTER('2011-11-11 11:11:11')
		-> 4
	*/	
	bool Quarter(const FString &InDate,MysqlFuntionParam);

	/**
	 * Returns the month difference between two periods.
	 * @param Period1				Month compared 1.
	 * @param Period2				Month compared 2.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT PERIOD_DIFF(201710, 201703);
		-> 7
	*/	
	bool PeriodDiff(const int64 Period1,const int64 Period2,MysqlFuntionParam);

	/**
	 * Add a period for the year month combination date.
	 * @param InPeriod				A year with numbers 202006.
	 * @param InMonths				Months.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT PERIOD_ADD(201703, 5);
		-> 201708
	*/
	bool PeriodAdd(const int64 InPeriod, const int64 InMonths, MysqlFuntionParam);

	/**
	 * Get current time .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT NOW()
		-> 2020-09-19 20:57:41
	*/
	bool Now(MysqlFuntionParam);

	/**
	 * Returns the minute value in InTime.
	 * @param InTime				Specific time like 2011-11-11 11:11:11.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT MINUTE('1:2:3')
		-> 2
	*/
	bool Minute(const FString &InTime,MysqlFuntionParam);

	/**
	 * Returns the number of microseconds corresponding to the date parameter .
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT MICROSECOND("2020-06-20 09:34:00.000023");
		-> 23
	*/
	bool MicroSecond(const FString &InDate,MysqlFuntionParam);

	/**
	 * Combination time, parameters are hour, minute and second respectively .
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT MAKETIME(11, 35, 4);
		-> 11:35:04
	*/
	bool MakeTime(const FString &InDate,MysqlFuntionParam);

	/**
	 * Returns a date based on the given parameter year and the number of days in the year day of year .
	 * @param InYear				Like 2020
	 * @param DayOfYear				Like 1 ~ 12
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT MAKEDATE(2020, 3);
		-> 2020-01-03
	*/
	bool MakeDate(const int32 InYear,const int64 DayOfYear,MysqlFuntionParam);

	/**
	 * Returns the current date and time .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT LOCALTIMESTAMP()
		-> 2018-09-19 20:57:43
	*/
	bool LocalTimeStamp(MysqlFuntionParam);

	/**
	 * Returns the current date and time .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT LOCALTIME()
		-> 2018-09-19 20:57:43
	*/
	bool LocalTime(MysqlFuntionParam);

	/**
	 * The last day of the month returned to the given date .
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT LAST_DAY("2020-06-20");
		-> 2020-06-30
	*/
	bool LastDay(const FString &InDate, MysqlFuntionParam);

	/**
	 * Returns the hour value in InTime.
	 * @param InTime				Specific time like 11:11:11.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT HOUR('1:2:3')
		-> 1
	*/
	bool Hour(const FString &InTime, MysqlFuntionParam);

	/**
	 * Calculate the date n days after January 1, 0000 .
	 * @param InTime				Specific time like 11:11:11.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT FROM_DAYS(1111)
		-> 0003-01-16
	*/
	bool FromDays(const int64 InDay, MysqlFuntionParam);

	/**
	 * Gets the specified value from date D, and type specifies the returned value .
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23.
	 * @param DateType				About the type of date .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT EXTRACT(MINUTE FROM '2011-11-11 11:11:11')
		-> 11
	*/
	bool Extract(const FString &InDate,const EMysqlDateType DateType, MysqlFuntionParam);

	/**
	 * Calculation date InDate is the day of the year .
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT DAYOFYEAR('2011-11-11 11:11:11');
		->315
	*/
	bool DayOfYear(const FString &InDate, MysqlFuntionParam);

	/**
	 * Date InDate today is the day of the week, 1 Sunday, 2 Monday, and so on .
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT DAYOFWEEK('2011-11-11 11:11:11')
		->6
	*/
	bool DayOfWeek(const FString &InDate, MysqlFuntionParam);

	/**
	 * Calculation date InDate is the day of the month .
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT DAYOFMONTH('2011-11-11 11:11:11')
		->11
	*/
	bool DayOfMonth(const FString &InDate, MysqlFuntionParam);

	/**
	 * The return date InDate is the day of the week, such as Monday, Tuesday .
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT DAYNAME('2011-11-11 11:11:11')
		->Friday
	*/
	bool DayName(const FString& InDate, MysqlFuntionParam);

	/**
	 * Returns the date part of the date value InDate.
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT DAY("2017-06-15");
		-> 15
	*/
	bool Day(const FString& InDate, MysqlFuntionParam);

	/**
	 * Function to subtract a specified time interval from a date.
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23.
	 * @param InInterval			Time interval in days.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		//Subtract 2 days from the OrderDate field in the orders table: 
		SELECT OrderId,DATE_SUB(OrderDate,INTERVAL 2 DAY) AS OrderPayDate FROM Orders
	*/
	bool DateSub(const FString& InDate,const int64 InInterval, MysqlFuntionParam);

	/**
	 * Display date InDate as required by expression InFormat.
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23.
	 * @param InFormat				You can customize the layout format, such as %M %Y %D %r, etc .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT DATE_FORMAT('2011-11-11 11:11:11','%Y-%m-%d %r')
		-> 2011-11-11 11:11:11 AM
	*/
	bool DateFormat(const FString& InDate, const FString& InFormat, MysqlFuntionParam);

	/**
	 * Date after calculation start date D plus a time period .
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23.
	 * @param InInterval			Time interval in days.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT ADDDATE('2011-11-11 11:11:11',1)
		-> 2011-11-12 11:11:11    (Default is day)
		SELECT ADDDATE('2011-11-11 11:11:11', INTERVAL 5 MINUTE)
		-> 2011-11-11 11:16:11 (The value of type is similar to the function listed above)
	*/
	bool AddDate(const FString& InDate, const int64 InInterval, MysqlFuntionParam);

	/**
	 * Days between calculation dates InD1 - > InD2 .
	 * @param InD1					Fill in date similar to 2022-10-12 10:25:23.
	 * @param InD2					Fill in date similar to 2022-10-12 10:25:23.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT DATEDIFF('2001-01-01','2001-02-02')
		-> -32
	*/
	bool DateDiff(const FString& InD1, const FString& InD2, MysqlFuntionParam);

	/**
	 * Extract a date value from a date or datetime expression .
	 * @param InExpression			About date expression.
	 * @param InD2					Fill in date similar to 2022-10-12 10:25:23.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT DATE("2017-06-15");
		-> 2017-06-15
	*/
	bool Date(const FString& InExpression, MysqlFuntionParam);
	
	/**
	 * Return to current time .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT CURTIME();
		-> 19:59:02
	*/
	bool CurTime(MysqlFuntionParam);

	/**
	 * Returns the current date and time .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT CURRENT_TIME();
		-> 19:59:02
	*/
	bool CurrentTime(MysqlFuntionParam);

	/**
	 * Returns the current date and time .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT CURRENT_TIMESTAMP()
		-> 2018-09-19 20:57:43
	*/
	bool CurrentTimeStamp(MysqlFuntionParam);

	/**
	 * Time InDate plus InSecond seconds .
	 * @param InDate				Fill in date similar to 2022-10-12 10:25:23..
	 * @param InSecond				Time seconds .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT ADDTIME('2011-11-11 11:11:11', 5)
		->2011-11-11 11:11:16 (Second)
	*/
	bool AddTime(const FString& InDate,const int64 InSecond, MysqlFuntionParam);
	////////////////////////////////////////////////////////////////////////////////////////
	///字符串操作
public:
	/**
	 * Returns the ASCII code of the first character of the string InDate.
	 * @param InString				character string .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example : 
		SELECT ASCII(CustomerName) AS NumCodeOfFirstChar
		FROM Customers;
	*/
	bool GetASCII(const FString &InString, MysqlFuntionParam);

	/**
	 * Returns the number of characters in the string InString.
	 * @param InString				character string .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example : 
		SELECT CHAR_LENGTH("RENZHAI") AS LengthOfString;
	*/
	bool CharLength(const FString& InString, MysqlFuntionParam);

	/**
	 * Returns the number of characters in the string InString.
	 * @param InString				character string .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example : 
		SELECT CHARACTER_LENGTH("RENZHAI") AS LengthOfString;
	*/
	bool CharacterLength(const FString& InString, MysqlFuntionParam);
	
	/**
	 * Array and other strings are combined into one string .
	 * @param InStrings				Various strings .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT CONCAT("http:// ", "renzhai ", ". ", "net") AS RENZHAI_NET;
	*/
	bool Concat(const TArray<FString>& InStrings, MysqlFuntionParam);
	
	/**
	 * The same as the function of  Concat(const TArray<FString>& InStrings,...), but x should be added between each string. X can be a separator .
	 * @param InSeparator			The merged string can be divided according to the current separator and assembled into a new string such as 2020-6-30.
	 * @param InStrings				Various strings.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT CONCAT_WS("-", "2020", "6", "30 12:14:20") AS Listing_time;
	*/
	bool ConcatWS(const TCHAR *InSeparator,const TArray<FString>& InStrings, MysqlFuntionParam);
	
	/**
	 * Returns the position of the first string s in the string list (InQueryString, InStrings...) .
	 * @param InQueryString			String to prepare query and match .
	 * @param InStrings				Various strings.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT FIELD("c", "a", "b", "c", "d", "e");
	*/
	bool Field(const TCHAR* InQueryString, const TArray<FString>& InStrings, MysqlFuntionParam);
	
	/**
	 * Returns the position of the string matching InQueryString in string InString 
	 * @param InQueryString			String to prepare query and match .
	 * @param InString				String content .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT FIND_IN_SET("c", "a,b,c,d,e");
	*/
	bool FindInSet(const TCHAR* InQueryString, const FString& InString, MysqlFuntionParam);

	/**
	 * The function can format the number x as "##,###.##", keep x to the nth place after the decimal point, and round the last digit. 
	 * @param InNumber				Precision number .
	 * @param KeepDecimalPoint		Keep the last digits of the number .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT FORMAT(250500.5634, 2);     
		-- input 250,500.56
	*/
	bool Format(float InNumber,int8 KeepDecimalPoint, MysqlFuntionParam);

	/**
	 * String InReplacedString replaces the string whose InStartPos position of InSourceString starts with len .
	 * @param InSourceString		Source string .
	 * @param InStartPos			Where to start .
	 * @param InReplacedString		String to replace .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT INSERT("http://ue4.net", 1, 6, "renzhai");  
		-- input：http://renzhai.net
	*/
	bool Insert(const FString &InSourceString,int32 InStartPos,const TCHAR *InReplacedString, MysqlFuntionParam);

	/**
	 * Get the starting position of InFindString from string InSourceString.
	 * @param InSourceString		Source string .
	 * @param InFindString			String to match .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT LOCATE('st','myteststring');  
		-- 5
	*/
	bool Locate(const FString& InSourceString, const TCHAR* InFindString, MysqlFuntionParam);

	/**
	 * Change all letters of string s to lowercase .
	 * @param InSourceString		Source string .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT LCASE('RENZHAI') -- renzhai
	*/
	bool Lcase(const FString& InSourceString,MysqlFuntionParam);

	/**
	 * Change all letters of string s to lowercase .
	 * @param InSourceString		Source string .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT LOWER('RENZHAI') -- renzhai
	*/
	bool Lower(const FString& InSourceString, MysqlFuntionParam);

	/**
	 * Change all letters of string s to lowercase .
	 * @param InSourceString		Source string .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT UPPER("renzhai");
		-- RENZHAI.
	*/
	bool Upper(const FString& InSourceString,MysqlFuntionParam);

	/**
	 * Change all letters of string s to lowercase .
	 * @param InSourceString		Source string .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT UCASE("renzhai"); 
		-- RENZHAI.
	*/
	bool Ucase(const FString& InSourceString, MysqlFuntionParam);
	
	/**
	 * Returns the first ReturnStringLen characters of the string InSourceString.
	 * @param InSourceString		Source string .
	 * @param ReturnStringLen		How many strings are returned .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT RIGHT('renzhai',2)
		-- re.
	*/
	bool Left(const FString& InSourceString, int32 ReturnStringLen,MysqlFuntionParam);
	
	/**
	 * Returns the last ReturnStringLen characters of the string InSourceString.
	 * @param InSourceString		Source string .
	 * @param ReturnStringLen		How many strings are returned .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT RIGHT('renzhai',2) 
		-- ai.
	*/
	bool Right(const FString& InSourceString, int32 ReturnStringLen, MysqlFuntionParam);
	
	/**
	 * Fill in string InAddString at the beginning of string InSourceString to make the string length reach StringLen .
	 * @param InSourceString		Source string .
	 * @param StringLen				Length of string .
	 * @param InAddString			String to be added .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT LPAD('abc',5,'xx')
		 -- xxabc
	*/
	bool Lpad(const FString& InSourceString, int32 StringLen, const FString& InAddString, MysqlFuntionParam);
	
	/**
	 * Add string InAddString at the end of string InSourceString to make the length of string StringLen
	 * @param InSourceString		Source string .
	 * @param StringLen				Length of string .
	 * @param InAddString			String to be added .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT RPAD('abc',5,'xx')
		 -- abcxx
	*/
	bool Rpad(const FString& InSourceString, int32 StringLen, const FString& InAddString, MysqlFuntionParam);

	/**
	 * Remove the space at the beginning of string InSourceString;
	 * @param InSourceString		Source string .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT LTRIM("    Renzhai") AS c_Ltrim;
		-- Renzhai
	*/
	bool Ltrim(const FString& InSourceString,MysqlFuntionParam);

	/**
	 * Remove the space at the end of string InSourceString.
	 * @param InSourceString		Source string .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT RTRIM("Renzhai    ") AS c_Rtrim;
		-- Renzhai
	*/
	bool Rtrim(const FString& InSourceString,MysqlFuntionParam);

	/**
	 * Remove spaces at the beginning and end of string InSourceString.
	 * @param InSourceString		Source string .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT TRIM('    RENZHAI    ') AS TrimmedString;
	*/
	bool Trim(const FString& InSourceString, MysqlFuntionParam);

	/**
	 * Intercept the substring with length len from position n of string s, the same as SubString(InSourceString,InStartPos, InStringLen) .
	 * @param InSourceString		Source string .
	 * @param InStartPos			Wei'z at the beginning .
	 * @param InStringLen			Length of string .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT MID("Renzhai", 2, 3) AS ExtractString; 
		-- enz
	*/
	bool Mid(const FString& InSourceString,int32 InStartPos,int32 InStringLen,MysqlFuntionParam);
	
	/**
	 * Intercept the substring with length len from position n of string s, the same as Mid(InSourceString,InStartPos, InStringLen) .
	 * @param InSourceString		Source string .
	 * @param InStartPos			Wei'z at the beginning .
	 * @param InStringLen			Length of string .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT SUBSTRING("Renzhai", 2, 3) AS ExtractString;
		-- enz
	*/
	bool SubString(const FString& InSourceString, int32 InStartPos, int32 InStringLen, MysqlFuntionParam);

	/**
	 * Intercept the substring with length len from position n of string s, the same as Mid(InSourceString,InStartPos, InStringLen) .
	 * @param InSourceString		Source string .
	 * @param InStartPos			Wei'z at the beginning .
	 * @param InStringLen			Length of string .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT SUBSTR("Renzhai", 2, 3) AS ExtractString;
		-- enz
	*/
	bool SubStr(const FString& InSourceString, int32 InStartPos, int32 InStringLen, MysqlFuntionParam);

	/**
	 * Returns the substring after the delimiter that appears from the number of the string s. 
	 * If number is a positive number, returns the string to the left of the number character.            
	 * If number is negative, returns the string to the right of the (number's absolute value (from the right)) character. 
	 * @param S						Source string .
	 * @param InDelimiter			Split string .
	 * @param InNumber				Specified number .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT SUBSTRING_INDEX('a*b','*',1) 
		-- a
		SELECT SUBSTRING_INDEX('a*b','*',-1)   
		-- b
		SELECT SUBSTRING_INDEX(SUBSTRING_INDEX('a*b*c*d*e','*',3),'*',-1)    
		-- c
	*/
	bool SubStringIndex(const FString& S,const TCHAR *InDelimiter, int32 InNumber, MysqlFuntionParam);

	/**
	 * Get the starting position of InSourceString from string InMatchingString.
	 * @param InSourceString		Source string .
	 * @param InMatchingString		String matching InSourceString.
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT POSITION('b' in 'abc') 
		-- 2
	*/
	bool Position(const FString& InSourceString,const TCHAR * InMatchingString, MysqlFuntionParam);

	/**
	 * Repeat string InSourceString InTimes times .
	 * @param InSourceString		Source string .
	 * @param InTimes				Number of repetitions .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT REPEAT('renzhai',3) 
		-- renzhairenzhairenzhai
	*/
	bool Repeat(const FString& InSourceString,int32 InTimes, MysqlFuntionParam);

	/**
	 * Replace string ToReplace in string s with string InReplaced .
	 * @param InSourceString		Source string .
	 * @param ToReplace				Prepare string to replace .
	 * @param InReplaced			What string to replace .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT REPLACE('abc','a','x') 
		--xbc
	*/
	bool Replace(const FString& InSourceString,const TCHAR * ToReplace, const TCHAR* InReplaced, MysqlFuntionParam);

	/**
	 * Reverse the order of string InSourceString.
	 * @param InSourceString		Source string .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT REVERSE('abc')
		-- cba
	*/
	bool Reverse(const FString& InSourceString,MysqlFuntionParam);

	/**
	 * Return n spaces .
	 * @param InSpaceNumber			Returns the number of spaces .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT SPACE(10);
	*/
	bool Space(const int32 InSpaceNumber,MysqlFuntionParam);

	/**
	 *Compare the strings S1 and S2. If S1 and S2 are equal, 0 is returned. If S1 > S2, 1 is returned. If S1 < S2, 1 is returned .
	 * @param S1					character string .
	 * @param S2					character string .
	 * @param Results				The result set is used for content obtained through MySQL server .
	 * @param ErrMesg				Error message if there is an error or warning in the query, MySQL will be notified in the form of text.
	 * @param SaveType				Whether you want to read the query results from the local cache or get the query results from the server through the network.
	 * @param Debug					It is used for testing in UE, whether to print to screen or log or to process all .
	 * Return whether the query is successful.
	 * @Example :
		SELECT STRCMP("Renzhai", "Renzhai");  
		-- 0
	*/
	bool Strcmp(const FString &S1, const FString& S2,MysqlFuntionParam);
	
	/////////////////////////////////////////////////////////////////////////////////////
	//创建数据库
	bool CreateDateBase(const FString& DataBaseName, EMysqlCharset Charset, FString& ErrorMesg);
	//创建并选择数据库
	bool CreateAndSelectDataBase(const FString& DataBaseName, EMysqlCharset Charset, FString& ErrorMesg);
	//丢弃数据库
	bool DropDataBase(const FString& DataBaseName, FString& ErrorMesg);
	//选择默认数据库
	bool SelectNewDB(const FString& NewDB,FString &ErrorMesg);
	///////////////////////////////////////////////////////////////////////////////////
	//临时表
	bool CreateTemporaryTable(const FString& NewTableName,const TMap<FString,FMysqlFileType> &InFields,const TArray<FString>& InPrimaryKeys,const FMysqlCreateTableParam& Param,FString &ErrorMesg);
	//从其他表导入数据创建临时表，可自定义字段属性，复制表字段，where条件
	bool CreateTemporaryTableFromOther(const FString& NewTableName,const TMap<FString,FMysqlFileType> &InFields,const TArray<FString>& InPrimaryKeys,const FSimpleMysqlQueryParameters& QueryParam,TArray<FSimpleMysqlComparisonOperator>& Condition,FString &ErrorMesg);
	//删除临时表
	bool DropTemporaryTable(const FString& TemporaryTable, FString& ErrorMesg);
	/////////////////////////////////////////////////////////////////////////////////////
	///表和数据拷贝：
	//创建表拷贝其他表的结构 like，不复制数据
	bool CreateTableALikeB(const FString& A,const FString& B,FString &ErrorMesg);
	//创建临时表拷贝其他表的结构 like  不复制数据
	bool CreateTemporaryTableALikeB(const FString& A,const FString& B, FString &ErrorMesg);
	//拷贝数据到表，并选择数据进行复制
	bool CopyTableAToB(const FString& A,const FString& B,const TArray<FString> &Fields,FString &ErrorMesg);
	//拷贝数据到临时表,create like 后 insert into拷贝数据
	bool CopyTableAToTemporaryB(const FString& Table,const FString& Temporary,const TArray<FString> &Fields,FString &ErrorMesg);
	//As创建临时表，我自己写的
	bool CreateTemporaryTableAsOther(const FString& TemporaryTable,const TMap<FString,FMysqlFileType> &NewInFields,const TArray<FString>& InPrimaryKeys,const FSimpleMysqlQueryParameters &QueryParam ,FString& ErrorMesg);
	//AS拷贝数据创建表
	/*
	create table tb AS (select id from ppp1);  //这里只复制id来进行表的创建
	create table 新表 AS (select 字段n as 新字段名  ... from 原表...);
	create table 新表(新字段 类型 属性) AS (select * from 旧表...)  //复制的时候，除了拷贝旧的字段外，添加新的字段
	*/
	bool CopyTableAToBFast(const FString& CopyTargetTable,const TMap<FString,FMysqlFileType> &InsertFields,const TArray<FString>& InPrimaryKeys,const FSimpleMysqlQueryParameters &QueryParam ,FString& ErrorMesg);
	/*
	create table tb AS (select id from ppp1);  //这里只复制id来进行表的创建
	create table 新表 AS (select 字段n as 新字段名  ... from 原表...);
	create table 新表(新字段 类型 属性) AS (select * from 旧表...)  //复制的时候，除了拷贝旧的字段外，添加新的字段
	*/
	bool CopyTableAToTemporaryBFast(const FString& CopyTargetTable,const TMap<FString,FMysqlFileType> &InsertFields,const TArray<FString>& InPrimaryKeys,const FSimpleMysqlQueryParameters &QueryParam ,FString& ErrorMesg);
	//////////////////////////////////////////////////////////////////////////////////
	//创建表
	bool CreateTable(const FString &TableName,const TMap<FString,FMysqlFileType> &InFields,const TArray<FString>& InPrimaryKeys,const FMysqlCreateTableParam& Param,FString &ErrorMesg);
	//删除表，释放内存，不支持数据回滚
	bool DropTable(const FString& TableName,FString &ErrorMesg);
	//清除表数据，释放内存，不支持回滚
	bool TruncateTable(const FString& TableName,FString &ErrorMesg);
	//删除表数据，内存保留，可回滚
	bool DeleteFromTable(const FString& TableName,FString &ErrorMesg);
	//删除表里指定行数据，内存保留，可回滚
	bool DeleteFromTableWhereData(const FString& TableName,TArray<FSimpleMysqlComparisonOperator>& Condition,FString &ErrorMesg);
	//清除删除的数据的内存
	bool OptimiseTable(const FString& TableName,FString &ErrorMesg);
	//////////////////////////////////////////////////////////////////////////////////////
	//更新数据
	bool UpdataTableData(const FString& TableName,const TArray<FSimpleMysqlAssignment> &InFields,const TArray<FSimpleMysqlComparisonOperator> &Condition ,FString &ErrorMesg);
	//Updata的replace形式
	bool ReplaceTableData(const FString& TableName,const TArray<FSimpleMysqlReplaceParamters> &InReplacs,const TArray<FSimpleMysqlComparisonOperator> &Condition ,FString &ErrorMesg);
	///////////////////////////////////////////////////////////////////////////////////
	///插入数据
	bool InsertTableData(const FString& TableNmae,const TMap<FString,FString> &InsertKeyValueData,const FSimpleMysqlQueryParameters &QueryParam,FString& ErrorMesg,bool bIgnore = true);
	//Insert into 表 values(a,b,c)
	bool SimpleInsertTableData(const FString& TableNmae,const TArray<FString> &InsertValueData,FString& ErrorMesg,bool bIgnore = true);
	//替换型插入，可以替换唯一约束键和主键
	bool ReplaceIfExistence(const FString& TableNmae,const TMap<FString,FString> &InsertKeyValueData,FString& ErrorMesg);
	///////////////////////////////////////////////////////////////////////////////////
	///ALTER函数，表结构操作
	//切换引擎
	bool ChangeEngine(const FString &InTableName,EMysqlSaveEngine InEngine,FString &ErrorMesg);
	//删除表字段
	bool DropTableField(const FString &InTableName,const TArray<FString> &InFieldName, FString& ErrorMesg);
	//添加表字段
	bool AddTableField(const FString &InTableName,const TMap<FString,FMysqlFileType> &InFields,FString& ErrorMesg);
	//更改表某个字段的属性或类型
	bool ModifyTableFieldType(const FString &InTableName,const TMap<FString,FMysqlFileType> &InFields,FString& ErrorMesg);
	//更改某个字段的名称和属性
	bool ChangeTableField(const FString &InTableName,const TArray<FSimpleMysqlModifyField> &InChangeFields,FString &ErrorMesg);
	bool RenameTable(const FString &InOldTableName,const FString &InNewTableName,FString &ErrorMesg);
	//////////////////////////////////////////////////////////////////////////////////
	///Stmt预处理函数
	bool QueryLinkStmt(const FString& SQL,const TArray<FSimpleMysqlQueryStmt> &InQueryStmtParam,FString &ErrorMesg);

	//准备SQL语句
	bool PrepareStmtSQL(const FString &SQL,FString &ErrorMesg);
	
	//绑定参数
	bool PrepareStmtParameters(const TArray<FSimpleMysqlQueryStmt> &InQueryStmtParam,FString &ErrorMesg);

	//执行查询
	bool ExecuteStmt(FString &ErrorMesg);
	
	//////////////////////////////////////////////////////////////////////////////////////
	///事务操作
// 	1、ENGINE = innoDB; 选择MySqlinnoDB引擎，支持事务操作，和回滚。
// 2、START TRANSACTION  开启一个事务，begin命令也可以
// 3、set autocommit = 0   将自动提交设置为手动，不是InnoDB的表不能使用手动提交。
// 4、准备SQL语句，使用mysql_query，它会把每次运行后的语句存储在客户端内存，当COMMIT时全部启动。注意如果客户端链接断开，则所有存储信息丢失。
// 5、COMMIT  提交，将所有进行mysql_query的而存储的SQL语句进行一次性提交。
// - ROLLBACK 回滚，ROLLBACK WORK命令也可以。非InnoDB的表不能回滚，它会撤回上一条存储的SQL语句，在提交时就不会执行了。注意**已经COMMIT的操作，ROLLBACK不能进行回滚**。，因为已经进行的SQL操作是不可逆的。
// - set autocommit = 1  自动提交设置回自动
	//savepoint  xxx  设置存储点
	//release savepoint  xxx  释放一个存储点
	//rollback to savepoint xxx  回滚到存储点
	bool StartTransaction(FString &ErrorMesg);
	bool SetAutocommit(bool bAuto,FString &ErrorMesg);
	//提交使用QueryLink
	bool Commit( FString& ErrorMesg);
	bool SetSavePointName(const FString &SaveName,FString &ErrorMesg);
	bool ReleaseSavepoint(const FString &SaveName,FString &ErrorMesg);
	bool Rollback(const FString &SaveName,FString &ErrorMesg);
	
	////////////////////////////////////////////////////////////////////////////////////////
	///打印
	bool PrintResult(const TArray<FSimpleMysqlResult> &Results,bool bPrintToScreen = true,bool bPrintToLog = true);
protected:
	//转换客户端Flag
	uint32 ToMySqlClientFlag(ESimpleClientFlags ClientFlags) const;
	//获取结果集
	void GetResult(st_mysql_res* Result,TArray<FSimpleMysqlResult> &Results);
	//生成Select语句，并进行本地缓存读取结果集，并不处理具体功能，而是把生成和执行和读取步骤进行整合
	bool GetSelectTableDataSR(const FSimpleMysqlQueryParameters& QueryParam,TArray<FSimpleMysqlResult> &Results,FString &ErrorMesg,const FSimpleMysqlDebugResult &Debug = FSimpleMysqlDebugResult());
	//生成Select语句，并进行网络获取结果集，并不处理具体功能，而是把生成和执行和读取步骤进行整合
	bool GetSelectTableDataUR(const FSimpleMysqlQueryParameters& QueryParam,TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, const FSimpleMysqlDebugResult& Debug= FSimpleMysqlDebugResult());
	//用于提供快捷查询高级SQL语句做兼容用的，例如SELECT BIN(X)... ,SELECT ISNULL(..)...
	bool GetSelectTableDataByFuntion(const FString& SQL,EMysqlQuerySaveType SaveType,TArray<FSimpleMysqlResult> Results,FString& ErrorMesg,const FSimpleMysqlDebugResult& Debug = FSimpleMysqlDebugResult());
private:
	//内部使用的快速复制表。AS
	bool CopyTableAToBFastInternal(FString &SQL,const TMap<FString,FMysqlFileType> &InsertFields,const TArray<FString>& InPrimaryKeys,const FSimpleMysqlQueryParameters &QueryParam ,FString& ErrorMesg);
	//用来在读取select查询结果的时候选择什么读取方式 Use或Stroe
	bool GetSelectTableDataInternal(FString &SQL,TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg,EMysqlQuerySaveType SaveType = EMysqlQuerySaveType::STORE_RESULT,const FSimpleMysqlDebugResult& Debug = FSimpleMysqlDebugResult());
	//获取本地缓存数据
	bool GetStroeResult(TArray<FSimpleMysqlResult> &Results,FString& ErrorMesg,const FSimpleMysqlDebugResult &Debug = FSimpleMysqlDebugResult());
	//获取网络缓存数据
	bool GetUseResult(TArray<FSimpleMysqlResult> &Results,FString& ErrorMesg,const FSimpleMysqlDebugResult &Debug = FSimpleMysqlDebugResult());
	//执行SQL查询和获取结果集的的地方。
	bool QueryLinkStroeResult(const FString &SQL,TArray<FSimpleMysqlResult> &Results,FString &ErrorMesg,const FSimpleMysqlDebugResult &Debug = FSimpleMysqlDebugResult());
	//执行SQL查询和获取结果集的的地方。
	bool QueryLinkUseResult(const FString &SQL,TArray<FSimpleMysqlResult> &Results,FString &ErrorMesg,const FSimpleMysqlDebugResult &Debug = FSimpleMysqlDebugResult());
private:
	struct FBindAllocation
	{
		FBindAllocation()
			:Size(0),
			BindParam(NULL)
		{
			
		}

		void Free();
		uint32 Size;
		st_mysql_bind *BindParam;
	};
	
	//用户基础信息
	const FString User;
	const FString Host;
	const FString Pawd;
	FString DB;
	const FString Unix_Socket;
	const uint32   Port = 0;
	uint32 ClientFlag = 0;

	//mysql对象
	st_mysql *Mysql;
	st_mysql_stmt *MysqlStmt;
	FBindAllocation BindAllocation;
};

