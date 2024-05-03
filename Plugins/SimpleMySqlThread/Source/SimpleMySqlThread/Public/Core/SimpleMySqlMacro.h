#pragma once

#include "CoreMinimal.h"

#define Mysql_Printf(String,bPrintToScreen,bPrintToLog,Color,Time) \
if(bPrintToScreen)\
{GEngine->AddOnScreenDebugMessage(-1,Time,Color,String);}\
if(bPrintToLog)\
{UE_LOG(LogTemp, Log,TEXT("%s"),String);}

#define Mysql_Simple_Printf_all(Str)\
Mysql_Printf(Str,true,true,FColor::Red,100.f);

#define MysqlFuntionParam TArray<FSimpleMysqlResult>& Results, FString& ErrorMesg, EMysqlQuerySaveType SaveType = EMysqlQuerySaveType::STORE_RESULT, const FSimpleMysqlDebugResult& Debug = FSimpleMysqlDebugResult() 