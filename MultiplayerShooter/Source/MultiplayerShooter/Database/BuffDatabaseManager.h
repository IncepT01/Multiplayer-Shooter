// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SQLiteDatabaseConnection.h"
#include "SQLiteCore/Public/IncludeSQLite.h"
#include "BuffDatabaseManager.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API UBuffDatabaseManager : public UObject
{
	GENERATED_BODY()

public:
	static UBuffDatabaseManager* Get();
	TMap<FString, float> LoadBuffs();

private:
	static UBuffDatabaseManager* Instance;
	FSQLiteDatabase* Database;
};
