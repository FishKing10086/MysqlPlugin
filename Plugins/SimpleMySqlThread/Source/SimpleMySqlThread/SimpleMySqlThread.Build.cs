// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class SimpleMySqlThread : ModuleRules
{
	public string ProjectDirectory
	{
		get
		{
			return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../../"));   //退回到项目根目录，MoudleDirectory是Bulid.cs所在的位置
		}
	}

	private void CopyToProjectBinaries(string FilePathName, ReadOnlyTargetRules Target)  //FilePathName是动态库在插件中的位置。
	{
		//项目的Binaries/Winxx位置，exe执行文件位置
		string BinariesDirectory = Path.Combine(ProjectDirectory, "Binaries", Target.Platform.ToString());
		string FileName = Path.GetFileName(FilePathName);  //获取路径的纯文件名称

		if (!Directory.Exists(BinariesDirectory))  //判断路径存在？
		{
			Directory.CreateDirectory(BinariesDirectory);  //不存在创建一个
		}

		//完整的项目执行文件下加上给定路径文件（动态库）的路径
		string ProjectFileFullName = Path.Combine(BinariesDirectory, FileName);  
		//判断该exe目录是否存在了动态库相关文件
		if (!File.Exists(ProjectFileFullName))
		{
			//如果不存在直接从插件的动态库拷贝到项目执行文件路径，可覆盖
			File.Copy(FilePathName,ProjectFileFullName,true);
		}
		
		RuntimeDependencies.Add(ProjectFileFullName);  //添加该动态库文件到动态依赖，让项目运行时加载动态库
	}
	
	public SimpleMySqlThread(ReadOnlyTargetRules Target) : base(Target)
	{
		bEnableUndefinedIdentifierWarnings = false;  //关闭未定义标识符的警告
		PublicDefinitions.Add("NTDDI_WN7SP1");  //添加windows版本为WIN7SP1宏
		
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		//获取插件的根目录的位置，
		string PluginsDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", ".."));
		//获取动态库的上层根目录MySql
		string SQLLibDirectory = Path.Combine(PluginsDirectory, "MySql");
		//MySql动态库所使用的版本
		string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x32";
		//MySql动态库的真正路径
		string MySqlPath = Path.Combine(SQLLibDirectory, PlatformString);
		
		//PublicIncludePaths.Add(MySqlPath);
		
		//如果插件的动态库不在Source里就需要添加依赖
		RuntimeDependencies.Add(Path.Combine(MySqlPath,"libmysql.dll"));  
		//进行必要的静态链接符号。
		PublicAdditionalLibraries.Add(Path.Combine(MySqlPath,"libmysql.lib"));
		//执行动态库拷贝到项目可执行文件路径
		CopyToProjectBinaries(Path.Combine(MySqlPath,"libmysql.dll"),Target);
		CopyToProjectBinaries(Path.Combine(MySqlPath,"libmysql.lib"),Target);
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
