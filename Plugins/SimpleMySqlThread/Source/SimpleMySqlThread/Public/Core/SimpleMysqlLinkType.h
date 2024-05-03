#pragma once

#include "CoreMinimal.h"

#include "SimpleMysqlLinkType.generated.h"


//Mysql所有的变量类型
UENUM(BlueprintType)
enum class EMysqlVariableType :uint8
{
	MYSQL_DECIMAL							 UMETA(DisplayName = "Decimal"),	//DECIMAL(M,D)/NUMERIC  M > D ? M+ 2 : D+ 2
	MYSQL_TINY								 UMETA(DisplayName = "Small Int"),	//SMALLINT		2 //s
	MYSQL_SHORT								 UMETA(DisplayName = "Short"),		//SHORT			2 //s
	MYSQL_LONG								 UMETA(DisplayName = "integer"),	//INTEGER		4 //s
	MYSQL_FLOAT								 UMETA(DisplayName = "Float"),		//FLOAT			4 //s
	MYSQL_DOUBLE							 UMETA(DisplayName = "Double"),		//DOUBLE/REAL	8 //s
	MYSQL_NULL								 UMETA(DisplayName = "Null"),		//NULL
	MYSQL_TIMESTAMP							 UMETA(DisplayName = "Time Stamp"), //TIMESTAMP		8
	MYSQL_LONGLONG							 UMETA(DisplayName = "Big Int"),	//BIGINT		8 //s
	MYSQL_INT24								 UMETA(DisplayName = "Medium Int"),	//MEDIUMINT		3 //ns
	MYSQL_DATE								 UMETA(DisplayName = "Date"),		//DATE			3
	MYSQL_TIME								 UMETA(DisplayName = "Time"),		//TIME			3
	MYSQL_DATETIME							 UMETA(DisplayName = "Date Time"),	//DATETIME		8
	MYSQL_YEAR								 UMETA(DisplayName = "Year"),		//YEAR			1
	MYSQL_NEWDATE							 UMETA(DisplayName = "New Date"),	//NEWDATE
	MYSQL_VARCHAR							 UMETA(DisplayName = "Varchar"),	//VARCHAR
	MYSQL_BIT								 UMETA(DisplayName = "Bit"),		//BIT(M)			
	MYSQL_TIMESTAMP2						 UMETA(DisplayName = "Time Stamp2"),
	MYSQL_DATETIME2							 UMETA(DisplayName = "Date Time2"),
	MYSQL_TIME2								 UMETA(DisplayName = "Time2"),
	MYSQL_JSON			= 245				 UMETA(DisplayName = "Json"),		//@@
	MYSQL_NEWDECIMAL	= 246				 UMETA(DisplayName = "New Decimal"),//
	MYSQL_ENUM			= 247				 UMETA(DisplayName = "Enum"),		//				1 or 2   65535个
	MYSQL_SET			= 248				 UMETA(DisplayName = "Set"),		//				1,2,4,8
	MYSQL_TINY_BLOB		= 249				 UMETA(DisplayName = "Tiny Blob"),  //@@				//s
	MYSQL_MEDIUM_BLOB	= 250				 UMETA(DisplayName = "Medium Blob"),//@@				//s
	MYSQL_LONG_BLOB		= 251				 UMETA(DisplayName = "Long Blob"),	//@@				//s
	MYSQL_BLOB			= 252				 UMETA(DisplayName = "Blob"),		//@@ 				//s
	MYSQL_VAR_STRING	= 253				 UMETA(DisplayName = "VarString"),	//@@ //VARCHAR
	MYSQL_STRING		= 254				 UMETA(DisplayName = "String"),		//@@ //CHAR/VARCHAR 
	MYSQL_GEOMETRY		= 255				 UMETA(DisplayName = "Geometry"),	//   Geometry(PINT(经度 维度)) 
};


//用来定义不同存储引擎的枚举，用于设置表属性
UENUM(BlueprintType)
enum class EMysqlSaveEngine : uint8
{
	innodb,
	myisam,
	memory,
	csv,
	blackhole,
	archive,
	mrg_myisam,
	performance_schema
};

//用来定义不同字符集的枚举，用于设置表属性
UENUM(BlueprintType)
enum class EMysqlCharset : uint8
{
	None,
	armscii8,
	ascii,
	big5,
	binary,
	cp1250,
	cp1251,
	cp1256,
	cp1257,
	cp850,
	cp852,
	cp866,
	cp932,
	dec8,
	eucjpms,
	enckr,
	gb2312,
	gbk,
	geostd8,
	greek,
	hebrew,
	hp8,
	keybcs2,
	koi8r,
	koi8u,
	latin1,
	latin2,
	latin5,
	latin7,
	macce,
	macroman,
	sjis,
	swe7,
	tis620,
	ucs2,
	ujis,
	utf16,
	utf32,
	utf8,
	utf8mb4
};

//Connect时的ClientFlag
UENUM(BlueprintType)
enum class ESimpleClientFlags : uint8
{
	Client_None                         ,
	Client_Long_Password	 			,/* new more secure passwords */
	Client_Found_Rows	     			,/* Found instead of affected rows */
	Client_Long_Flag	     			,/* Get all column flags */
	Client_Connect_With_DB	 			,/* One can specify db on connect */
	Client_No_Schema	     			,/* Don't allow database.table.column */
	Client_Compress		     			,/* Can use compression protocol */
	Client_ODBC		         			,/* Odbc client */
	Client_Local_Files	     			,/* Can use LOAD DATA LOCAL */
	Client_Ignore_Space	     			,/* Ignore spaces before '(' */
	Client_Protocol_41	     			,/* New 4.1 protocol */
	Client_Interactive	     			,/* This is an interactive client */
	Client_SSL              			,/* Switch to SSL after handshake */
	Client_Ignore_Sigpipe    			,/* IGNORE sigpipes */
	Client_Transactions	     			,/* Client knows about transactions */
	Client_Reserved          			,/* Old flag for 4.1 protocol  */
	Client_Reserved2         			,/* Old flag for 4.1 authentication */
	Client_Multi_Statements  			,/* Enable/disable multi-stmt support */
	Client_Multi_Results     			,/* Enable/disable multi-results */
	Client_Ps_Multi_Results  			,/* Multi-results in PS-protocol */
};

UENUM(BlueprintType)
enum class EMysqlDateType : uint8
{
		MICROSECOND				UMETA(DisplayName = "Microsecond"),					
		SECOND					UMETA(DisplayName = "Second"),
		MINUTE					UMETA(DisplayName = "Minute"),
		HOUR					UMETA(DisplayName = "Hour"),
		DAY						UMETA(DisplayName = "Day"),
		WEEK					UMETA(DisplayName = "Week"),
		MONTH					UMETA(DisplayName = "Month"),
		QUARTER					UMETA(DisplayName = "Quarter"),
		YEAR					UMETA(DisplayName = "Year"),
		SECOND_MICROSECOND		UMETA(DisplayName = "Second Microsecond"),
		MINUTE_MICROSECOND		UMETA(DisplayName = "Minute Microsecond"),
		MINUTE_SECOND			UMETA(DisplayName = "Minute Second"),
		HOUR_MICROSECOND		UMETA(DisplayName = "Hour Microsecond"),
		HOUR_SECOND				UMETA(DisplayName = "Hour Second"),
		HOUR_MINUTE				UMETA(DisplayName = "Hour Minute"),
		DAY_MICROSECOND			UMETA(DisplayName = "Day Microsecond"),
		DAY_SECOND				UMETA(DisplayName = "Day Second"),
		DAY_MINUTE				UMETA(DisplayName = "Day Minute"),
		DAY_HOUR				UMETA(DisplayName = "Day Hour"),
		YEAR_MONTH				UMETA(DisplayName = "Year Month")
};

UENUM(BlueprintType)
enum class EMysqlJoinType : uint8
{
	None                      UMETA(DisplayName = "None"),
	INNER					  UMETA(DisplayName = "Inner"),
	LEFT                      UMETA(DisplayName = "Left"),
	RIGHT                     UMETA(DisplayName = "Right"),
};

UENUM(BlueprintType)
enum class EMysqlQuerySaveType : uint8
{
	  STORE_RESULT                  UMETA(DisplayName = "Store Result"),
	  USE_RESULT                    UMETA(DisplayName = "Use Result"),
	               
};
//判断条件 < > = !=
UENUM(BlueprintType)
enum class EMysqlComparisonOperator : uint8
{
	EQUAL								UMETA(DisplayName = "="),
	GREATER_THAN						UMETA(DisplayName = ">"),
	LESS_THAN							UMETA(DisplayName = "<"),
	GREATER_THAN_OR_EQUAL               UMETA(DisplayName = ">="),
	LESS_THAN_OR_EQUAL                  UMETA(DisplayName = "<="),

	NOT_EQUAL                           UMETA(DisplayName = "!="),
	BETWEEN                             UMETA(DisplayName = ">=min&&<=max"),
	NOT_BETWEEN						    UMETA(DisplayName = "!(>=min&&<=max)"),
	SQL_IN								UMETA(DisplayName = "In"),
	NOT_IN								UMETA(DisplayName = "Not In"),
	COMPARE_NULL_VALUES                 UMETA(DisplayName = "<=>"),
	LIKE								UMETA(DisplayName = "like"),
	REGEXP_OR_RLIKE						UMETA(DisplayName = "Regular matching"),
	IS_NULL								UMETA(DisplayName = "Is_null"),
	IS_NOT_NULL							UMETA(DisplayName = "Is not null"),
};
//逻辑条件 AND OR ...
UENUM(BlueprintType)
enum class EMysqlLogicalOperator : uint8
{
	NONE								UMETA(DisplayName = "NONE"),
	NOT                                 UMETA(DisplayName = "!"),
	AND                                 UMETA(DisplayName = "&&"),
	OR                                  UMETA(DisplayName = "||"),
	XOR                                 UMETA(DisplayName = "^"),
};

//描述字段属性的，通常配合TMap<FString,FMysqlFileType>来使用，因为这里面不指定字段名称
USTRUCT(BlueprintType)
struct FMysqlFileType
{
	GENERATED_USTRUCT_BODY()
	
	FMysqlFileType();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SimpleMySql|FileType")
	bool bUnsignedVariable;  //是不是无符号
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SimpleMySql|FileType")
	EMysqlVariableType VariableType;  //类型

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SimpleMySql|FileType")
	int64 VariableLen;  //长度

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SimpleMySql|FileType")
	int64 DecimalPoint;  //小数位数
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SimpleMySql|FileType")
	bool bNULL;   //是不是默认NULL值

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SimpleMySql|FileType")
	FString DefaultValue;   //默认值

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SimpleMySql|FileType")
	bool bAutoIncrement;   //是不是自增量

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SimpleMySql|FileType")
	FString Comment;  //描

	FString ToString() const;
};


//修改字段的
USTRUCT(BlueprintType)
struct FSimpleMysqlModifyField
{
	GENERATED_USTRUCT_BODY()
	
	FSimpleMysqlModifyField();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SimpleMySql|QueryParameters")
	FString FieldOldName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SimpleMySql|QueryParameters")
	FString FieldNewName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SimpleMySql|QueryParameters")
	FMysqlFileType NewFieldType;

	FString ToString() const;
};


USTRUCT(BlueprintType)
struct FMysqlCreateTableParam
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SimpleMySql|CreateTableParam")
	EMysqlSaveEngine SaveEngine;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SimpleMySql|CreateTableParam")
	EMysqlCharset Charset;

	FString ToString() const;
};

USTRUCT(BlueprintType)
struct FSimpleMysqlResult
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|MysqlResult")
	TArray<FString> Rows;
	
};


USTRUCT(BlueprintType)
struct FSimpleMysqlDebugResult
{
	GENERATED_USTRUCT_BODY()

	FSimpleMysqlDebugResult();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|DebugResult")
	bool bPrinttToScreen;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|DebugResult")
	bool bPrintToLog;
	
};

USTRUCT(BlueprintType)
struct FSimpleMysqlQueryOrderBy
{
	GENERATED_USTRUCT_BODY()

	FSimpleMysqlQueryOrderBy();
	
	//按谁排序
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "SimpleMySql|OrderBy")
	FString FieldName;

	//asc  由大到小        desc   由小到大
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySQL|OrderBy")
	bool bDesc;
	
};

//join参数，如果设置join功能的话
USTRUCT(BlueprintType)
struct FSimpleMysqlJoinParameters
{
	GENERATED_USTRUCT_BODY()

	FSimpleMysqlJoinParameters();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|QueryParameters")
	EMysqlJoinType JoinType;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|QueryParameters")
	FString TableName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|QueryParameters")
	FString Condition;

	FString ToString() const;
};


//判断表达式  age = '100'  size < '200'
USTRUCT(BlueprintType)
struct FSimpleMysqlAssignment
{
	GENERATED_USTRUCT_BODY()

	FSimpleMysqlAssignment();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|Assignment")
	FString A;

	//判断条件 < > = !=
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|Assignment")
	EMysqlComparisonOperator ComparisonOperator;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|Assignment")
	FString B;

	FString ToString() const;
};

//WHERE name != 'www' and hhhhh != 'gexcxx2' 用于连接俩个判断表达式
USTRUCT(BlueprintType)
struct FSimpleMysqlComparisonOperator
{
	GENERATED_USTRUCT_BODY()

	FSimpleMysqlComparisonOperator();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|ComparisonOperator")
	FSimpleMysqlAssignment Assignment;

	//逻辑条件 AND OR ...
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|ComparisonOperator")
	EMysqlLogicalOperator LogicalOperator;

	FString ToString() const;
};

USTRUCT(BlueprintType)
struct FSimpleMysqlVariableSettings
{
    GENERATED_USTRUCT_BODY()

	//函数
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|VariableSettings")
	FString VariableName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|VariableSettings")
	FString AsName;

	bool IsValid() const;

	bool IsFunction() const;
	
	FString ToString() const;

};



USTRUCT(BlueprintType)
struct FSimpleMysqlQueryParameters
{
	GENERATED_USTRUCT_BODY()

	FSimpleMysqlQueryParameters();

	/*
	 *Select n1,n2 from tb where order by xx...
	 */

	//
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|QueryParameters")
	FSimpleMysqlVariableSettings FunctionSettings;
	
	//表名
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|QueryParameters")
	FString TableNmae;

	//查询字段
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|QueryParameters")
	TArray<FSimpleMysqlVariableSettings> InFields;
	
	//只显示唯一
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|QueryParameters")
	bool bDistinct;

	//join功能
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|QueryParameters")
	FSimpleMysqlJoinParameters JoinParameters;

	//分组
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|QueryParameters")
	TArray<FString> GroupBy;

	//where 判断
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|QueryParameters")
	TArray<FSimpleMysqlComparisonOperator> Condition;

	//排序
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|QueryParameters")
	TArray<FSimpleMysqlQueryOrderBy> OrderBy;

	//限制查找范围
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|QueryParameters")
	FVector2D Limit;

	//统计字段数量之类的，汇总
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|QueryParameters")
	bool bWithRollup;

	//分组后再次筛选
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|QueryParameters")
	FString Having;

	void AddField(const FString &Name);
	
	bool IsValid() const;
	
	bool ToString(FString &SQL) const;
};


USTRUCT(BlueprintType)
struct FSimpleMysqlReplaceParamters
{
	GENERATED_USTRUCT_BODY()

	FSimpleMysqlReplaceParamters();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|ReplaceParamters")
	FString FieldName;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|ReplaceParamters")
	FString OldDataValue;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|ReplaceParamters")
	FString NewDataValue;

	FString ToString() const;
};

USTRUCT(BlueprintType)
struct FSimpleMysqlQueryStmt
{
	GENERATED_USTRUCT_BODY()

	FSimpleMysqlQueryStmt();
	/*
	char content[] = "Hello";
	Pram[1].buffer_type = MYSQL_TYPE_VARCHAR;  //VarChar类型，一个字符一个字节
	Pram[1].buffer = content;
	uint32 contentlen = strlen(content);  
	Pram[1].buffer_length = contentlen;  //指定存储缓冲区大小，可变的大小需要手动指定
	Pram[1].length = (unsigned long*)&contentlen;  //字符串的字符串字节大小，和缓冲区大小应该是一样的。 //长度必须指明，否则会报客户端缓冲区不足，需要更大空间的问题。
	 */

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|ueryStmt")
	FString Value;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SimpleMySql|ueryStmt")
	EMysqlVariableType VariableType;

	uint32 GetValueTypeLen() const;

	void GetValue(void **InValue) const;
	
	unsigned long *GetValueTypeLenMemory() const;
};