using UnrealBuildTool;

public class FirstPersonGameEditorTarget : TargetRules
{
	public FirstPersonGameEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("FirstPersonGame");
	}
}
