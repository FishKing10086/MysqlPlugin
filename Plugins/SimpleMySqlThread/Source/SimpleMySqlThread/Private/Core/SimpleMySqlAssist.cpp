#include "SimpleMySqlAssist.h"
#include "Core/SimpleMysqlLinkType.h"

enum class EMysqlComparisonOperator : uint8;
FString SimpleMysqlAssist::ComparisonOperatorToString(EMysqlComparisonOperator C0)
{

	switch (C0)
	{
	case EMysqlComparisonOperator::EQUAL:
		return TEXT("=");
	case EMysqlComparisonOperator::GREATER_THAN:
		return TEXT(">");
	case EMysqlComparisonOperator::LESS_THAN:
		return TEXT("<");
	case EMysqlComparisonOperator::GREATER_THAN_OR_EQUAL:
		return TEXT(">=");
	case EMysqlComparisonOperator::LESS_THAN_OR_EQUAL:
		return TEXT("<=");
	case EMysqlComparisonOperator::NOT_EQUAL:
		return TEXT("!=");
	case EMysqlComparisonOperator::BETWEEN:
		return TEXT("BETWEEN");
	case EMysqlComparisonOperator::NOT_BETWEEN:
		return TEXT("NOT BETWEEN");
	case EMysqlComparisonOperator::SQL_IN:
		return TEXT("IN");
	case EMysqlComparisonOperator::NOT_IN:
		return TEXT("NOT IN");
	case EMysqlComparisonOperator::COMPARE_NULL_VALUES:
		return TEXT("<=>");
	case EMysqlComparisonOperator::LIKE:
		return TEXT("LIKE");
	case EMysqlComparisonOperator::REGEXP_OR_RLIKE:
		return TEXT("REGEXP");
	case EMysqlComparisonOperator::IS_NULL:
		return TEXT("IS NULL");
	case EMysqlComparisonOperator::IS_NOT_NULL:
		return TEXT("IS NOT NULL");
	}
	
	return TEXT("");
}

void SimpleMysqlAssist::ConditionToString(FString& SQL, const TArray<FSimpleMysqlComparisonOperator>& Condition)
{
	if(Condition.Num())
	{
		SQL += TEXT(" WHERE ");
		for(const auto &Tmp : Condition)
		{
			SQL += Tmp.ToString();
		}
	}
}

void SimpleMysqlAssist::FieldTypeArrayToString(FString& SQL, const TMap<FString, FMysqlFileType>& InNewFields)
{
	if(InNewFields.Num())
	{
		for(auto &Tmp : InNewFields)
		{
			FieldTypeToString(SQL,Tmp.Key,Tmp.Value);
			SQL += TEXT(",");
		}
		SQL.RemoveFromEnd(TEXT(","));
	}
}

void SimpleMysqlAssist::FieldTypeToString(FString& SQL, const FString& Fieldname, const FMysqlFileType& InType)
{
	SQL += Fieldname + TEXT(" ") + InType.ToString();
}

void SimpleMysqlAssist::PrimaryKeyToString(FString& SQL, const TArray<FString>& InPrimaryKeys)
{
	if(InPrimaryKeys.Num())
	{
		SQL+=TEXT(",PRIMARY KEY (");
		for(auto Tmp : InPrimaryKeys)
		{
			SQL+=Tmp+TEXT(",");
		}
		SQL.RemoveFromEnd(TEXT(","));
		SQL+=TEXT(")");
	}
}

void SimpleMysqlAssist::SpawnKeyAndValue(FString& SQL, const TMap<FString, FString>& InsertKeyValueDatas)
{
	auto IsAll = [&]()
	{
		for (const auto &Tmp : InsertKeyValueDatas)
		{
			if(Tmp.Key.Contains(TEXT("*")))
			{
				return true;
			}
		}

		return false;
	};

	//判断是不是通配符
	if(IsAll())
	{
		SQL +=TEXT("");  
	}
	else
	{
		if(!InsertKeyValueDatas.IsEmpty())
		{
			SQL +=TEXT("(");
			//插入字段
			for(auto &Tmp : InsertKeyValueDatas)
			{
				SQL += Tmp.Key + TEXT(",");
			}
			SQL.RemoveFromEnd(TEXT(","));
			SQL += TEXT(")");
		}
		
	}

	//生成Value的SQL语句
	SpawnValue(SQL,InsertKeyValueDatas);
	
}

void SimpleMysqlAssist::SpawnValue(FString& SQL, const TArray<FString>& InsertValueDatas)
{
	//VALUES(...) 值
	FString ValueString;
	for(auto &Tmp : InsertValueDatas)
	{
		if(!Tmp.IsEmpty())
		{
			if(IsMysqlFountion(Tmp))
			{
				ValueString += (Tmp + TEXT(","));
			}
			else
			{
				ValueString +=TEXT("'") + Tmp + TEXT("',");
			}
		}
	}
	ValueString.RemoveFromEnd(TEXT(","));
	if(!ValueString.IsEmpty())
	{
		SQL += TEXT(" VALUES(") + ValueString + TEXT(")"); 
	}
}

void SimpleMysqlAssist::SpawnValue(FString& SQL, const TMap<FString, FString>& InsertKeyValueDatas)
{
	TArray<FString> InsertValueDatas;
	for(const auto &Tmp : InsertKeyValueDatas)
	{
		InsertValueDatas.Add(Tmp.Value);
	}
	
	SpawnValue(SQL,InsertValueDatas);
}

bool SimpleMysqlAssist::IsMysqlFountion(const FString& Element)
{
	return Element.Contains(TEXT("(")) || Element.Contains(TEXT(")")) ||  Element.Contains(TEXT("()")) || Element.Contains(TEXT(" DIV "))|| Element.Contains(TEXT("@"));;
}

bool SimpleMysqlAssist::IsTypesOfAutomaticRecognition(EMysqlVariableType VariableType)
{
	uint8 Type = (uint8)VariableType;
	return Type != (uint8)EMysqlVariableType::MYSQL_DECIMAL &&(
		Type != (uint8)EMysqlVariableType::MYSQL_JSON &&
		Type != (uint8)EMysqlVariableType::MYSQL_NEWDECIMAL) &&
			Type <= (uint8)EMysqlVariableType::MYSQL_TINY_BLOB;
}
