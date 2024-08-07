// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TechnicalExercise : ModuleRules
{
	public TechnicalExercise(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
          new string[] {
                "TechnicalExercise"
          }
      );

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "MotionTrajectory", "UMG", "Niagara" });
	}
}
