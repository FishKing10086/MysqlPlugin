// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/SimpleMysqlLink.h"
#include "SimpleMysqlObject.generated.h"

/**
 * 这个对象负责创建和管理Link(Mysql对象管理器)的生命周期。作为一个管理器的形式存在。
 */
UCLASS(Blueprintable,BlueprintType)
class SIMPLEMYSQLTHREAD_API USimpleMysqlObject : public UObject
{
	GENERATED_BODY()
public:
	USimpleMysqlObject();

	void InitMysql(const FString& InUser, const FString& InHost, const FString& InPawd,const FString& InDB, const uint32 InPort, const FString& InUnix_Socket,const TArray<ESimpleClientFlags>& InClientFlag = TArray<ESimpleClientFlags>());

	TSharedRef<FSimpleMysqlLink> GetLink();
private:
	TSharedPtr<FSimpleMysqlLink> Link;
};
