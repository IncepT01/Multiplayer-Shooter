// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MultiplayerShooter : ModuleRules
{
	public MultiplayerShooter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "SQLiteCore", "SQLiteSupport", "Slate", "SlateCore"});

		PrivateDependencyModuleNames.AddRange(new string[] { "Niagara"});

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
        
        /*

        if (Target.Configuration == UnrealTargetConfiguration.Development ||
            Target.Configuration == UnrealTargetConfiguration.Debug)
        {
            PublicDependencyModuleNames.AddRange(new string[] {
                "ApplicationCore",
                //"UnrealEd" // Required for AutomationTest headers
            });

            // Needed if you're writing tests that might use Editor features
            //PrivateDependencyModuleNames.Add("EditorSubsystem");
        }
        */
    }
}
