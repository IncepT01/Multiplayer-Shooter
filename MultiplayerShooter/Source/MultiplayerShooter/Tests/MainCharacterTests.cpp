#if WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "MultiplayerShooter/MainCharacter/MainCharacter.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOnRepReplicatedMovementTest, "Game.MainCharacter.OnRep_ReplicatedMovement",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOnRepReplicatedMovementTest::RunTest(const FString& Parameters)
{
	// Create test character instance
	AMainCharacter* TestCharacter = NewObject<AMainCharacter>();

	// Set a dummy non-zero value
	TestCharacter->SetTimeSinceLastMovementReplication(1.23f);

	// Call the function under test
	TestCharacter->OnRep_ReplicatedMovement();

	// Check that the timer was reset
	TestEqual(TEXT("TimeSinceLastMovementReplication should be reset to 0"), TestCharacter->GetTimeSinceLastMovementReplication(), 0.f);

	return true;
}

#endif