#if WITH_EDITOR
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMyBasicTest, "MultiplayerShooter.Unit.MyOwnTests", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMyBasicTest::RunTest(const FString& Parameters)
{
    TestTrue(TEXT("True should be true"), true);
    return true;
}

// Second test
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMySecondTest, "MultiplayerShooter.Unit.MathCheck", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMySecondTest::RunTest(const FString& Parameters)
{
    int32 Result = 2 + 2;
    TestEqual(TEXT("2 + 2 should be 4"), Result, 4);
    return true;
}

#endif