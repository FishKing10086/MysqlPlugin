
#include "Core/SimpleMysqlLinkType.h"

#include "SimpleMySqlAssist.h"

FMysqlFileType::FMysqlFileType()
	:bUnsignedVariable(false)
	,VariableType(EMysqlVariableType::MYSQL_LONG)
	,VariableLen(0)
	,DecimalPoint(0)
	,bNULL(true)
	,bAutoIncrement(false)
{
	
}

FString FMysqlFileType::ToString() const
{
	

	UEnum *MysqlVariableEnum = StaticEnum<EMysqlVariableType>();
	FString FieldTypeString = MysqlVariableEnum->GetNameStringByIndex((int32)VariableType);

	FieldTypeString.RemoveFromStart(TEXT("MYSQL_"));
	if(VariableLen > 0 || DecimalPoint > 0)
	{
		FieldTypeString += TEXT("(") + FString::Printf(TEXT("%lld"),VariableLen) + (DecimalPoint > 0 ?  FString::Printf(TEXT(",%lld"),DecimalPoint) : FString(""))+TEXT(")");
	}

	if(bUnsignedVariable)
	{
		FieldTypeString += TEXT(" UNSIGNED");
	}

	if(bNULL)
	{
		FieldTypeString += TEXT("");
	}
	else
	{
		FieldTypeString += TEXT(" NOT NULL");

		if(!DefaultValue.IsEmpty())
		{
			FieldTypeString += TEXT(" DEFAULT ") + DefaultValue;
		}
		else if(bAutoIncrement)
		{
			FieldTypeString += TEXT(" AUTO_INCREMENT");	
		}
	}
	
	if(!Comment.IsEmpty())
	{
		FieldTypeString += TEXT(" COMMENT '") + Comment + TEXT("'");
	}
	
	return FieldTypeString;
}

FSimpleMysqlModifyField::FSimpleMysqlModifyField()
{
	
}

FString FSimpleMysqlModifyField::ToString() const
{
	FString SQL = FieldOldName + TEXT(" ");
	SimpleMysqlAssist::FieldTypeToString(SQL,FieldOldName,NewFieldType);
	return SQL;
}

FString FMysqlCreateTableParam::ToString() const 
{
	FString Param;
	UEnum *SaveEngineEnum = StaticEnum<EMysqlSaveEngine>();
	FString SaveEngineString = SaveEngineEnum->GetNameStringByIndex((int32)SaveEngine);

	Param += (TEXT(" ENGINE=") + SaveEngineString+TEXT(" "));
	
	if(Charset != EMysqlCharset::None)
	{
		UEnum *CharsetEnum = StaticEnum<EMysqlCharset>();
		FString CharsetString = CharsetEnum->GetNameStringByIndex((int32)Charset);
		Param += (TEXT("DEFAULT CHARSET=") + CharsetString);
	}
	
	return Param;
}

FSimpleMysqlDebugResult::FSimpleMysqlDebugResult()
	:bPrinttToScreen(false)
	,bPrintToLog(false)
{
	
}

FSimpleMysqlQueryOrderBy::FSimpleMysqlQueryOrderBy()
	:bDesc(false)
{
	
}

FSimpleMysqlJoinParameters::FSimpleMysqlJoinParameters()
	:JoinType(EMysqlJoinType::None)
{
}

FString FSimpleMysqlJoinParameters::ToString() const
{
	//select * from role      INNER JOIN PPP1 ON ROLE.AGE=PPP1.ID;合成的后面这段。前面的select语句交给外面去合成
	
	FString SQL;
	if(JoinType != EMysqlJoinType::None)
	{
		UEnum *MysqljoinTypeEnum = StaticEnum<EMysqlJoinType>();
		SQL += MysqljoinTypeEnum->GetNameStringByIndex((int32)JoinType) + TEXT(" JOIN ");

		SQL += TableName + TEXT(" ON ") + Condition;
	}

	return SQL;
}

bool FSimpleMysqlVariableSettings::IsValid() const
{
	return !VariableName.IsEmpty();
}

bool FSimpleMysqlVariableSettings::IsFunction() const
{
	if(IsValid())
	{
		return SimpleMysqlAssist::IsMysqlFountion(VariableName);
	}
	return false;
}

FString FSimpleMysqlVariableSettings::ToString() const
{
	FString SQL;
	if(!VariableName.IsEmpty())
	{
		SQL = VariableName;

		if(!AsName.IsEmpty())
		{
			SQL += (TEXT(" AS ") + AsName);
		}
	}

	return SQL;
}

FSimpleMysqlQueryParameters::FSimpleMysqlQueryParameters()
	:bDistinct(false)
	,Limit(FVector2d::ZeroVector)
	,bWithRollup(false)
{
	
}

void FSimpleMysqlQueryParameters::AddField(const FString& Name)
{
	InFields.Add(FSimpleMysqlVariableSettings());
	InFields[InFields.Num()-1].VariableName = Name;
}

bool FSimpleMysqlQueryParameters::IsValid() const
{
	return InFields.Num() != 0 ||  FunctionSettings.IsFunction();
}

bool FSimpleMysqlQueryParameters::ToString(FString &SQL) const
{
	//SELECT COUNT(*) as 别名，字段，字段2 FROM 表名 GROUP BY 字段 HAVNG 条件

	if(!IsValid())
	{
		return false;
	}
	
	SQL += TEXT(" SELECT ");
	SQL += bDistinct == true ? TEXT("DISTINCT ") : TEXT("");
	
	if(FunctionSettings.IsValid() && FunctionSettings.IsFunction())
	{
		SQL += FunctionSettings.ToString() + TEXT(",");
	}
	
	if(InFields.Num())
	{
		
		for(auto &Tmp : InFields)
		{
			SQL += Tmp.ToString() + TEXT(",");
		}
		
		SQL.RemoveFromEnd(TEXT(","));

		//表名
		SQL += TEXT(" FROM ") + TableNmae;

		SimpleMysqlAssist::ConditionToString(SQL,Condition);
	
		if(JoinParameters.JoinType != EMysqlJoinType::None)
		{
			SQL +=TEXT(" ") + JoinParameters.ToString();
		}

		if(GroupBy.Num())
		{
			SQL += TEXT(" GROUP BY ");
			for(auto &Tmp : GroupBy)
			{
				SQL += Tmp  + TEXT(",");
			}
			SQL.RemoveFromEnd(TEXT(","));

			if(bWithRollup)
			{
				SQL += TEXT(" WITH ROLLUP");
			}
		}

		//排序
		if(OrderBy.Num())
		{
			SQL += TEXT(" ORDER BY ");
			for(auto &Tmp : OrderBy)
			{
				SQL += Tmp.FieldName + TEXT(" ");
				SQL += Tmp.bDesc ==true ? TEXT("DESC") : TEXT("ASC");
				SQL += TEXT(",");
			}
			SQL.RemoveFromEnd(TEXT(","));
		}

		//限制
		if(Limit != FVector2d::ZeroVector)
		{
			SQL += TEXT(" LIMIT ")+FString::Printf(TEXT("(%i,%i)"),(int32)Limit.X,(int32)Limit.Y);
		}

		if(!Having.IsEmpty())
		{
			SQL += (TEXT(" HAVING ") + Having);
		}
	}
	else
	{
		SQL.RemoveFromEnd(TEXT(","));
	}
 
	SQL += TEXT(";");
	
	return true;
}

FSimpleMysqlReplaceParamters::FSimpleMysqlReplaceParamters()
{
}

FString FSimpleMysqlReplaceParamters::ToString() const
{
	FString SQL;
	if(FieldName.Len()&&OldDataValue.Len()&&NewDataValue.Len())
	{
		SQL += FieldName + TEXT(" = REPLACE(");
		SQL += FieldName + TEXT(",'") + OldDataValue + TEXT("','") + NewDataValue + TEXT("')");
	}

	return SQL;

}

FSimpleMysqlQueryStmt::FSimpleMysqlQueryStmt()
{
}

uint32 FSimpleMysqlQueryStmt::GetValueTypeLen() const
{
	if(!SimpleMysqlAssist::IsTypesOfAutomaticRecognition(VariableType))
	{
		return Value.Len();
	}
	return 0;
}

unsigned long * FSimpleMysqlQueryStmt::GetValueTypeLenMemory() const
{
	if (!SimpleMysqlAssist::IsTypesOfAutomaticRecognition(VariableType))
	{
		return NULL;
	}
	
	unsigned long* Size = (unsigned long*)FMemory::Malloc(sizeof(unsigned long));
	*Size = GetValueTypeLen();

	return Size;
}

void FSimpleMysqlQueryStmt::GetValue(void **InValue) const
{
	switch (VariableType)
	{
	case EMysqlVariableType::MYSQL_DECIMAL:
		return;
	case EMysqlVariableType::MYSQL_TINY:
	{
		char *Data = (char *)FMemory::Malloc(sizeof(char));
		*Data = FCString::Atoi(*Value);

		*InValue = Data;
	}
	return;
	case EMysqlVariableType::MYSQL_SHORT:
	{
		short *Data = (short *)FMemory::Malloc(sizeof(short));
		*Data = (short)FCString::Atoi(*Value);

		*InValue = Data;
	}
		return;
//	case EMysqlVariableType::MYSQL_INT24:
	case EMysqlVariableType::MYSQL_LONG:
	{
		int32 *Data = (int32 *)FMemory::Malloc(sizeof(int32));
		*Data = FCString::Atoi(*Value);

		*InValue = Data;
	}
		return;
	case EMysqlVariableType::MYSQL_FLOAT:
	{
		float *Data = (float *)FMemory::Malloc(sizeof(float));
		*Data = FCString::Atof(*Value);

		*InValue = Data;
	}
	return;
	case EMysqlVariableType::MYSQL_DOUBLE:
	{
		double *Data = (double *)FMemory::Malloc(sizeof(double));
		*Data = FCString::Atod(*Value);

		*InValue = Data;
	}
	return;
	case EMysqlVariableType::MYSQL_LONGLONG:
	{
		long long *Data = (long long *)FMemory::Malloc(sizeof(long long));
		*Data = FCString::Atoi64(*Value);

		*InValue = Data;
	}
		return;
	case EMysqlVariableType::MYSQL_TIMESTAMP:
		return;	
	case EMysqlVariableType::MYSQL_DATE:
		return;
	case EMysqlVariableType::MYSQL_TIME:
		return;
	case EMysqlVariableType::MYSQL_DATETIME:
		return;
	case EMysqlVariableType::MYSQL_YEAR:
		return;
	case EMysqlVariableType::MYSQL_NEWDATE:
		return;
	case EMysqlVariableType::MYSQL_BIT:
		return;
	case EMysqlVariableType::MYSQL_TIMESTAMP2:
		return;
	case EMysqlVariableType::MYSQL_DATETIME2:
		return;
	case EMysqlVariableType::MYSQL_TIME2:
		return;
	case EMysqlVariableType::MYSQL_NEWDECIMAL:
		return;
	case EMysqlVariableType::MYSQL_ENUM:
		return;
	case EMysqlVariableType::MYSQL_SET:
		return;
	case EMysqlVariableType::MYSQL_VARCHAR:
	case EMysqlVariableType::MYSQL_JSON:
	case EMysqlVariableType::MYSQL_TINY_BLOB:
	case EMysqlVariableType::MYSQL_MEDIUM_BLOB:
	case EMysqlVariableType::MYSQL_LONG_BLOB:
	case EMysqlVariableType::MYSQL_BLOB:
	case EMysqlVariableType::MYSQL_VAR_STRING:
	case EMysqlVariableType::MYSQL_STRING:
	{
		int32 Len = Value.Len();
		char *CharValue = TCHAR_TO_ANSI(const_cast<TCHAR *>(Value.GetCharArray().GetData()));

		char *Data = (char *)FMemory::Malloc(Len);
		FMemory::Memzero(Data, Len);
		FMemory::Memcpy(Data, CharValue, Len + 1);

		*InValue = Data;
	}
	return;
	case EMysqlVariableType::MYSQL_GEOMETRY:
		return;
	}

	*InValue = nullptr;
}

FSimpleMysqlAssignment::FSimpleMysqlAssignment()
	:ComparisonOperator(EMysqlComparisonOperator::EQUAL)
{
	
}


FString FSimpleMysqlAssignment::ToString() const
{
	FString SQL;
	if(!A.IsEmpty())
	{
		SQL += A + TEXT(" ") + SimpleMysqlAssist::ComparisonOperatorToString(ComparisonOperator) + TEXT(" '") + B + TEXT("'");
	}
	
	return SQL;
}

FSimpleMysqlComparisonOperator::FSimpleMysqlComparisonOperator()
{
}

FString FSimpleMysqlComparisonOperator::ToString() const
{
	FString SQL = Assignment.ToString()+TEXT(" ");
	if(LogicalOperator != EMysqlLogicalOperator::NONE)
	{
		UEnum *LogicalOperatorEnum = StaticEnum<EMysqlLogicalOperator>();
		SQL += LogicalOperatorEnum->GetNameStringByIndex((int32)LogicalOperator);
	}

	return SQL;
}


