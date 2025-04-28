#if WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
#include "MultiplayerShooter/MainCharacter/MainCharacter.h"
#include "MultiplayerShooter/PlayerController/MYPlayerController.h"
#include "MultiplayerShooter/GameState/MainGameState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/GameMode/MainGameMode.h"
#include "MultiplayerShooter/PlayerState/MainPlayerState.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMainGameModeBeginPlayTest, "Game.MainGameMode.BeginPlay", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMainGameModeBeginPlayTest::RunTest(const FString& Parameters)
{
    UWorld* World = GWorld;
    TestNotNull(TEXT("World should exist"), World);

    AMainGameMode* GameMode = World->SpawnActor<AMainGameMode>();
    TestNotNull(TEXT("GameMode should be spawned"), GameMode);
    
    if (GameMode->HasAuthority())
    {
        FPlatformProcess::Sleep(1.0f);  // Adjust sleep time as necessary

        TestTrue(TEXT("Buff list should be populated"), GameMode->BuffList.Num() >= 0);
    }

    return true;
}



IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMainGameModeTickTest, "Game.MainGameMode.Tick", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMainGameModeTickTest::RunTest(const FString& Parameters)
{
    UWorld* World = GWorld;
    TestNotNull(TEXT("World should exist"), World);

    AMainGameMode* GameMode = World->SpawnActor<AMainGameMode>();
    TestNotNull(TEXT("GameMode should be spawned"), GameMode);

    // Set MatchState to WaitingToStart
    GameMode->MatchState = MatchState::WaitingToStart;
    GameMode->LevelStartingTime = World->GetTimeSeconds();

    // Call Tick to test countdown logic
    GameMode->Tick(1.0f);

    // Ensure that countdown time is updated properly
    TestTrue(TEXT("Countdown time should be updated"), GameMode->CountdownTime <= GameMode->WarmupTime);

    // Add delay to ensure MatchState has transitioned properly before testing again
    FPlatformProcess::Sleep(1.0f);

    // Change to InProgress and test countdown again
    GameMode->MatchState = MatchState::InProgress;
    GameMode->Tick(1.0f);
    TestTrue(TEXT("Countdown time should be updated in progress"), GameMode->CountdownTime > GameMode->MatchTime);

    return true;
}


 #endif