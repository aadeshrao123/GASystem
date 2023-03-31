// Fill out your copyright notice in the Description page of Project Settings.


#include "GASGameStatics.h"

const UItemStaticData* UGASGameStatics::GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass)
{
	if (IsValid(ItemDataClass))
	{
		return GetDefault<UItemStaticData>(ItemDataClass);
	}
	return nullptr;
}
