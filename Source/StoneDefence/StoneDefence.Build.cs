// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StoneDefence : ModuleRules
{
	public StoneDefence(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject", 
			"Engine",
			"InputCore", 
			"EnhancedInput",
			"SimpleNetChannel",
			"StoneDefenceNetCommon",
			"UMG",
			"CommonUI",
			"DeveloperSettings",
			"SimpleCharacterBrowsing",
			"SimpleAdvancedAnimation",
			"GeniuxyGAS",
			"GeniuxyAnimation",
			"GeniuxyCommon",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			// ASdCharacterBase 在构造函数里创建 UMotionWarpingComponent（攀爬贴墙对齐用）
			"MotionWarping",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
