#include "FTest.h"

FTest::FTest()
{
	number = 2;
	GEngine->AddOnScreenDebugMessage(-1,100,FColor::Cyan,TEXT("Construct"));
}

FTest::~FTest()
{
	GEngine->AddOnScreenDebugMessage(-1,100,FColor::Red,TEXT("Destroy"));
}
