// BaseWeaponTests.cpp

#include "Misc/AutomationTest.h"
#include "MultiplayerShooter/BaseWeapon/BaseWeapon.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBaseWeaponInitialStateTest, "Game.BaseWeapon.InitialState", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBaseWeaponInitialStateTest::RunTest(const FString& Parameters)
{
	ABaseWeapon* Weapon = NewObject<ABaseWeapon>();
	TestNotNull("Weapon should be created", Weapon);
	TestEqual("Initial ammo should be 0", Weapon->GetAmmo(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBaseWeaponSpendRoundTest, "Game.BaseWeapon.SpendRound", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBaseWeaponSpendRoundTest::RunTest(const FString& Parameters)
{
	ABaseWeapon* Weapon = NewObject<ABaseWeapon>();
	TestNotNull("Weapon should be created", Weapon);

	Weapon->Ammo = 1;
	Weapon->SpendRound();
	TestEqual("Ammo should decrease by 1", Weapon->GetAmmo(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBaseWeaponSetWeaponStateTest, "Game.BaseWeapon.SetWeaponState", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBaseWeaponSetWeaponStateTest::RunTest(const FString& Parameters)
{
	ABaseWeapon* Weapon = NewObject<ABaseWeapon>();
	TestNotNull("Weapon should be created", Weapon);

	Weapon->SetWeaponState(EWeaponState::EWS_Equipped);
	TestEqual("Weapon should be equipped", Weapon->GetWeaponState(), EWeaponState::EWS_Equipped);

	Weapon->SetWeaponState(EWeaponState::EWS_Dropped);
	TestEqual("Weapon should be dropped", Weapon->GetWeaponState(), EWeaponState::EWS_Dropped);

	return true;
}
