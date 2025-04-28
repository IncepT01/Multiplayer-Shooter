#if WITH_EDITOR

#include "Components/AudioComponent.h"
#include "Misc/AutomationTest.h"
#include "MultiplayerShooter/Persistence/SettingsSaveGame.h"
#include "MultiplayerShooter/PlayerController/MyPlayerController.h"
#include "Sound/SoundMix.h"
#include "Tests/AutomationEditorCommon.h"

// Test for ClientJoinMidgame_Implementation
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMyPlayerControllerJoinMidgameTest, "Game.PlayerController.ClientJoinMidgame", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMyPlayerControllerJoinMidgameTest::RunTest(const FString& Parameters)
{
    // Create a test instance of AMyPlayerController
    AMyPlayerController* PlayerController = NewObject<AMyPlayerController>();

    // Setup test values
    FName TestStateOfMatch = FName(TEXT("TestState"));
    float TestWarmup = 10.f;
    float TestMatch = 20.f;
    float TestCooldown = 5.f;
    float TestStartingTime = 0.f;

    // Call the function you want to test
    PlayerController->ClientJoinMidgame_Implementation(TestStateOfMatch, TestWarmup, TestMatch, TestCooldown, TestStartingTime);

    // Assert that the values are correctly set
    TestEqual(TEXT("WarmupTime should be set correctly"), PlayerController->GetWarmupTime(), TestWarmup);
    TestEqual(TEXT("MatchTime should be set correctly"), PlayerController->GetMatchTime(), TestMatch);
    TestEqual(TEXT("CooldownTime should be set correctly"), PlayerController->GetCooldownTime(), TestCooldown);
    TestEqual(TEXT("LevelStartingTime should be set correctly"), PlayerController->GetLevelStartingTime(), TestStartingTime);
    TestEqual(TEXT("MatchState should be set correctly"), PlayerController->GetMatchState(), TestStateOfMatch);

    return true;
}


#endif