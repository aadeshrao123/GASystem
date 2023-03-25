// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GASystem : ModuleRules
{
	public GASystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "MotionWarping" });
		PublicIncludePaths.Add("GASystem/");

		PrivateDependencyModuleNames.AddRange(new string[]{
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks"
		});
	}
}
