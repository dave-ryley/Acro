// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.
// Reference: https://wiki.unrealengine.com/index.php?title=Save_System,_Read_%26_Write_Any_Data_to_Compressed_Binary_Files#Pre-Summary

#include "LevelData.h"
#include "Runtime/Core/Public/Serialization/BufferArchive.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Serialization/MemoryReader.h"

void ULevelData::SaveLoad(FArchive & Ar)
{
    Ar << LevelSegments;
    Ar << LevelName;
}

ULevelData::ULevelData()
{
}

int32 ULevelData::GetLevelSegmentCount()
{
    return LevelSegments;
}

void ULevelData::SetLevelName(const FString & Name)
{
    LevelName = FName(*Name);
}

const FName & ULevelData::GetLevelName()
{
    return LevelName;
}

bool ULevelData::Save(const FString & filepath)
{
    FBufferArchive ByteArrayBuffer;
    SaveLoad(ByteArrayBuffer);
    if (ByteArrayBuffer.Num() == 0)
    {
        return false;
    }

    FString fullFilePath = filepath + FString("/levelData.acrolevelsave");

    bool success = FFileHelper::SaveArrayToFile(ByteArrayBuffer, *fullFilePath);

    ByteArrayBuffer.FlushCache();
    ByteArrayBuffer.Empty();

    return success;
}

bool ULevelData::Load(const FString & filepath)
{
    TArray<uint8> BinaryArray;
    FString fullFilePath = filepath + FString("/levelData.acrolevelsave");
    if (!FFileHelper::LoadFileToArray(BinaryArray, *fullFilePath))
    {
        return false;
    }
    if (BinaryArray.Num() <= 0)
    {
        return false;
    }
    FMemoryReader FromBinary = FMemoryReader(BinaryArray, true);
    FromBinary.Seek(0);
    SaveLoad(FromBinary);

    FromBinary.FlushCache();

    BinaryArray.Empty();
    FromBinary.Close();

    return true;
}

ULevelData::~ULevelData()
{
}
