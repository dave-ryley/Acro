// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Acro : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }

    public Acro(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
        PublicDependencyModuleNames.AddRange(new string[] { "ShaderCore", "RenderCore", "RHI", "UMG", "RuntimeMeshComponent" });
        //PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
        PublicDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemUtils" });

        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

        LoadGorbClient(Target);
    }

    public bool LoadGorbClient(ReadOnlyTargetRules Target)
    {
        bool isLibrarySupported = false;

        string CapnpPath = "C:/Program Files/Cap'n Proto/";

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            isLibrarySupported = true;

            string LibrariesPath = Path.Combine(ThirdPartyPath, "GorbClient", "Libraries");

            PublicAdditionalLibraries.Add(Path.Combine(CapnpPath, "lib", "capnp.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(CapnpPath, "lib", "capnp-rpc.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(CapnpPath, "lib", "kj.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(CapnpPath, "lib", "kj-async.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "gorbclient.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "schema.lib"));
        }

        if (isLibrarySupported)
        {
            // Include path
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "GorbClient", "Includes"));
            PublicIncludePaths.Add(Path.Combine(CapnpPath, "include"));
        }

        PublicDefinitions.Add(string.Format("WITH_GORB_CLIENT_BINDING={0}", isLibrarySupported ? 1 : 0));

        return isLibrarySupported;
    }
}
