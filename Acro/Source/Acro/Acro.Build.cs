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
        PCHUsage = PCHUsageMode.NoSharedPCHs;

        bUseRTTI = true;

        PublicDependencyModuleNames.AddRange(new string[] { "ApplicationCore", "Core", "CoreUObject", "Engine", "InputCore", "RuntimeMeshComponent" });

        LoadGorbClient(Target);

    }

    public bool LoadGorbClient(ReadOnlyTargetRules Target)
    {
        bool isLibrarySupported = false;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            isLibrarySupported = true;

            string LibrariesPath = Path.Combine(ThirdPartyPath, "GorbClient", "Libraries", Target.Configuration.ToString());

            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "capnp.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "capnp-rpc.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "kj.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "kj-async.lib"));
        }

        if (isLibrarySupported)
        {
            // Include path
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "GorbClient", "Includes"));
        }

        PublicDefinitions.Add(string.Format("WITH_GORB_CLIENT_BINDING={0}", isLibrarySupported ? 1 : 0));

        return isLibrarySupported;
    }
}
