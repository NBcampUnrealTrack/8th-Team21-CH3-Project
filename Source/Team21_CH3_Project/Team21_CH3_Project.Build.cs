// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Team21_CH3_Project : ModuleRules
{
	public Team21_CH3_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule" , "NavigationSystem", "GameplayTasks", "UMG", "LevelSequence", "MovieScene", "Niagara" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        //PublicDependencyModuleNames.AddRange(new string[] { "Team21_CH3_Project" });

        PublicIncludePaths.AddRange(new string[] { "Team21_CH3_Project" });
        // Uncomment if you are using Slate UI
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
