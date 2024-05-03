#pragma once
#include "CoreMinimal.h"


enum class EMysqlComparisonOperator : uint8;
struct FSimpleMysqlComparisonOperator;
struct FMysqlFileType;
enum class EMysqlVariableType :uint8;

namespace SimpleMysqlAssist
{
	//返回判断表达式的符号的字符形式  < > =
	FString ComparisonOperatorToString(EMysqlComparisonOperator C0);

	//返回多个判断表达式之间的连接  name != 'www' and hhhhh != 'gexcxx2'
	void ConditionToString(FString &SQL,const TArray<FSimpleMysqlComparisonOperator> &Condition);

	//用于在创建字段时指定属性用的语句：key int(20) NOT NULL DEFAULT 20
	void FieldTypeArrayToString(FString &SQL,const TMap<FString,FMysqlFileType> &InNewFields);

	void FieldTypeToString(FString &SQL,const FString &Fieldname,const FMysqlFileType &InType);
	
	//用来指定主键用的语句  PRIMARY KEY xxx
	void PrimaryKeyToString(FString &SQL,const TArray<FString>& InPrimaryKeys);

	//用来生成插入语句的key和values列表  insert into tb_name(key1,key2..) values('value1',‘'value2')
	void SpawnKeyAndValue(FString &SQL,const TMap<FString,FString> &InsertKeyValueDatas);

	//生成简单插入的values语句  insert into tb_name values(f1,f2...)
	void SpawnValue(FString &SQL,const TArray<FString> &InsertValueDatas);
	//做兼容的重载形式
	void SpawnValue(FString &SQL,const TMap<FString,FString> &InsertKeyValueDatas);
	
	//判断给的定值是不是Mysql的函数，例如 Now()之类的。
	bool IsMysqlFountion(const FString& Element);

	bool IsTypesOfAutomaticRecognition(EMysqlVariableType VariableType);
}
