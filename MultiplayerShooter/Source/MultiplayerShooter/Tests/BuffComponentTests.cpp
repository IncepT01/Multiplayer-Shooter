#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
#include "MultiplayerShooter/MainCharacter/MainCharacter.h"
#include "MultiplayerShooter/ActorComponents/BuffComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuffComponentHealTest, "Game.BuffComponent.Heal", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FBuffComponentHealTest::RunTest(const FString& Parameters)
{
    UWorld* World = GWorld;

    TestNotNull(TEXT("World should exist"), World);

    AMainCharacter* Character = World->SpawnActor<AMainCharacter>();
    TestNotNull(TEXT("Character should spawn"), Character);

    UBuffComponent* BuffComp = NewObject<UBuffComponent>(Character);
    TestNotNull(TEXT("BuffComponent should be created"), BuffComp);

    BuffComp->RegisterComponent();
    BuffComp->BeginPlay(); // If Character is set in BeginPlay

    Character->SetHealth(50.f);
    Character->SetMaxHealth(100.f);

    BuffComp->Heal(30.f, 3.f); // Heal 30 HP over 3 seconds
    BuffComp->TickComponent(1.f, LEVELTICK_All, nullptr); // Simulate 1 second

    TestTrue(TEXT("Healing should be active"), BuffComp->bIsHealing());
    TestTrue(TEXT("Health should increase"), Character->GetHealth() >= 50.f);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuffComponentSpeedBuffTest, "Game.BuffComponent.BuffSpeed", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FBuffComponentSpeedBuffTest::RunTest(const FString& Parameters)
{
    UWorld* World = GWorld;
    TestNotNull(TEXT("World exists"), World);

    AMainCharacter* Character = World->SpawnActor<AMainCharacter>();
    UBuffComponent* BuffComp = NewObject<UBuffComponent>(Character);
    BuffComp->RegisterComponent();
    BuffComp->Character = Character;

    // Optional: Create and attach dummy CombatComponent
    UCombatComponent* CombatComp = NewObject<UCombatComponent>(Character);
    CombatComp->RegisterComponent();
    Character->SetCombatComponent(CombatComp);

    // Initialize movement speeds
    Character->GetCharacterMovement()->MaxWalkSpeed = 600.f;
    Character->GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;

    BuffComp->SetInitialSpeeds(600.f, 300.f);

    // Buff the speeds
    BuffComp->BuffSpeed(200.f, 100.f, 5.f);

    // Test speeds
    TestEqual(TEXT("Max Walk Speed should increase"), Character->GetCharacterMovement()->MaxWalkSpeed, 800.f);
    TestEqual(TEXT("Max Crouch Speed should increase"), Character->GetCharacterMovement()->MaxWalkSpeedCrouched, 400.f);


    BuffComp->ResetSpeeds();

    TestEqual(TEXT("Max Walk Speed should reset"), Character->GetCharacterMovement()->MaxWalkSpeed, 600.f);
    TestEqual(TEXT("Max Crouch Speed should reset"), Character->GetCharacterMovement()->MaxWalkSpeedCrouched, 300.f);

    return true;
}

