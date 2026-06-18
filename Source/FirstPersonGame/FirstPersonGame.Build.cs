using UnrealBuildTool;

public class FirstPersonGame : ModuleRules
{
	public FirstPersonGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"NavigationSystem",
			"AIModule",
			"GameplayTasks",
			"UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
	}
}
