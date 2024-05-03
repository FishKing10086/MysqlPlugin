
#pragma once

#include "CoreMinimal.h"


class SIMPLEMYSQLTHREAD_API FTest:public TSharedFromThis<FTest>
{
public:
	FTest();
	~FTest();
	int number;
};