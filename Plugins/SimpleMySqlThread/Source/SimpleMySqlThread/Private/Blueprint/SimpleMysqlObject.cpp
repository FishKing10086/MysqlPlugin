// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprint/SimpleMysqlObject.h"


USimpleMysqlObject::USimpleMysqlObject()
{
}

void USimpleMysqlObject::InitMysql(const FString& InUser, const FString& InHost, const FString& InPawd,const FString& InDB, const uint32 InPort, const FString& InUnix_Socket,const TArray<ESimpleClientFlags>& InClientFlag)
{
		Link = MakeShareable(new FSimpleMysqlLink(InUser,InHost,InPawd,InDB,InPort,InUnix_Socket,InClientFlag));
}

TSharedRef<FSimpleMysqlLink> USimpleMysqlObject::GetLink()
{
	return Link.ToSharedRef();
}
