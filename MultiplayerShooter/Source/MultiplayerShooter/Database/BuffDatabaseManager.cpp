#include "BuffDatabaseManager.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

UBuffDatabaseManager* UBuffDatabaseManager::Instance = nullptr;

UBuffDatabaseManager* UBuffDatabaseManager::Get()
{
	if (!Instance)
	{
		Instance = NewObject<UBuffDatabaseManager>();
		Instance->AddToRoot(); // Prevent it from being garbage collected
	}
	return Instance;
}


TMap<FString, float> UBuffDatabaseManager::LoadBuffs()
{
	TMap<FString, float> buffs;
	
	const FString DbFilePath = FPaths::ProjectContentDir() + "Data/Buffs.db";

	const FString ExampleSQL = TEXT("SELECT * FROM Buffs");

	const FString ExampleParameterName = TEXT("Amount");

	if (FSQLiteDatabase* MyDb = new FSQLiteDatabase(); MyDb->Open(*DbFilePath, ESQLiteDatabaseOpenMode::ReadWrite))
	{
		if (FSQLitePreparedStatement* PreparedStatement = new FSQLitePreparedStatement(); PreparedStatement->Create(*MyDb, *ExampleSQL, ESQLitePreparedStatementFlags::Persistent))
		{
			PreparedStatement->SetBindingValueByName(*ExampleParameterName, 1);

			while (PreparedStatement->Step() == ESQLitePreparedStatementStepResult::Row)
			{
				FString BuffName;
				float BuffAmount;
				if (PreparedStatement->GetColumnValueByIndex(0, BuffName) && PreparedStatement->GetColumnValueByIndex(1, BuffAmount))
				{
					//UE_LOG(LogTemp, Warning, TEXT("Buff Name: %s, Amount: %f"), *BuffName,BuffAmount);
					buffs.Add(BuffName, BuffAmount);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Row not found"));
				}
			}

			PreparedStatement->Destroy();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create prepareStatement"));
		}

		MyDb->Close();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to open db in LoadBuffs"));
	}
	
	return buffs;
}
