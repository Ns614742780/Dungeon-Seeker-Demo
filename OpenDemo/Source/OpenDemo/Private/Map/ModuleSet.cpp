#include "Map/ModuleSet.h"

#include "OpenDemo/OpenLogChannels.h"

AModuleSet::AModuleSet()
{
	PrimaryActorTick.bCanEverTick = false;
	MaybeModules.Empty();
}

void AModuleSet::AddModule(TSet<FOpenModuleState> AddModules)
{
	for (FOpenModuleState Module : AddModules)
	{
		if (!MaybeModules.Contains(Module))
		{
			MaybeModules.Add(Module);
		}
	}
}

void AModuleSet::RemoveModule(TSet<FOpenModuleState> RemoveModules)
{
	for (const FOpenModuleState Module : RemoveModules)
	{
		if (MaybeModules.Contains(Module))
		{
			MaybeModules.Remove(Module);
		}
	}
}

bool AModuleSet::RemoveModuleByTagsAndDirection(const TArray<FGameplayTag>& InTags, const int32 Direction)
{
	TSet<FOpenModuleState> RemoveModules;
	bool bHasRemove = false;
	if (InTags.Num() == 0)
	{
		return bHasRemove;
	}
	for (const FOpenModuleState Module : MaybeModules)
	{
		const FGameplayTag TagFromDirection = UModuleState::GetSelfConnectTagByDirection(Module, Direction);
		if (!InTags.Contains(TagFromDirection))
		{
			bHasRemove = true;
			RemoveModules.Add(Module);
		}
	}
	RemoveModule(RemoveModules);
	if (MaybeModules.Num() == 0)
	{
		for (FOpenModuleState RemoveModule : RemoveModules)
		{
			UE_LOG(LogOpen, Warning, TEXT("remove module %s cause empty"), *RemoveModule.Mesh->GetFName().ToString());
		}
		bHasRemove = false;
	}
	return bHasRemove;
}

bool AModuleSet::ForbidModuleByTagsAndDirection(const TArray<FGameplayTag>& InTags, const int32 Direction)
{
	TSet<FOpenModuleState> RemoveModules;
	bool bHasRemove = false;
	if (InTags.Num() == 0)
	{
		return bHasRemove;
	}
	for (const FOpenModuleState Module : MaybeModules)
	{
		const FGameplayTag TagFromDirection = UModuleState::GetSelfConnectTagByDirection(Module, Direction);
		if (InTags.Contains(TagFromDirection))
		{
			bHasRemove = true;
			RemoveModules.Add(Module);
		}
	}
	RemoveModule(RemoveModules);
	if (MaybeModules.Num() == 0)
	{
		bHasRemove = false;
	}
	return bHasRemove;
}

void AModuleSet::Empty()
{
	MaybeModules.Empty();
}

void AModuleSet::InitMaybeModules()
{
	for (const FOpenModuleState& ModuleState : ModuleStateInfo->ModuleStates)
	{
		for (int32 Rotation = 0 ; Rotation < 4 ; Rotation++)
		{
			FOpenModuleState NewModule = UModuleState::SetRotation(ModuleState, Rotation);;
			
			if (!MaybeModules.Contains(NewModule))
			{
				MaybeModules.Add(NewModule);
			}
		}
	}
}

TArray<FGameplayTag> AModuleSet::GetConnectTagsByDirection(const int32 Direction)
{
	TArray<FGameplayTag> ConnectTags;
	for (const FOpenModuleState Module : MaybeModules)
	{
		FGameplayTag ConnectTag = UModuleState::GetSelfConnectTagByDirection(Module, Direction);
		if (!ConnectTags.Contains(ConnectTag))
		{
			ConnectTags.Add(ConnectTag);
		}
	}
	return ConnectTags;
}
