#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <CS2/Econ/CEconItem.h>
#include <Utils/StrongTypeAlias.h>

using CreateEconItemFn = cs2::CEconItem*(*)();
using SetDynamicAttributeValueUintFn = void(*)(cs2::CEconItem*, void*, void*);
using GetEconItemSystemFn = void*(*)(void* source2Client);
using InventoryManagerGetFn = void*(*)();
using GetAttributeDefinitionInterfaceFn = void*(*)(void* schema, int index);
using GetItemInLoadoutFn = void*(*)(void* inventory, int team, int slot);
using EquipItemInLoadoutFn = bool(*)(void* manager, int team, int slot, std::uint64_t itemId);
using CreateBaseTypeCacheFn = void*(*)(void* soCache, int classId);
using FindTypeCacheFn = void*(*)(void* soCache, int classId);
using UpdateSkinFn = void(*)(cs2::C_CSWeaponBase* weapon, bool update);
using SetAttributeValueByNameFn = void(*)(void* itemView, const char* name, float value);
using GetCustomPaintKitIndexFn = int(*)(void* itemView);
using GetStaticDataFn = void*(*)(void* itemView);
using SetModelFn = void(*)(void* entity, const char* model);
using SetMeshGroupMaskFn = void(*)(void* sceneNode, std::uint64_t mask);

STRONG_TYPE_ALIAS(PointerToInventoryManagerGet, InventoryManagerGetFn);
STRONG_TYPE_ALIAS(PointerToCreateEconItem, CreateEconItemFn);
STRONG_TYPE_ALIAS(PointerToSetDynamicAttributeValueUint, SetDynamicAttributeValueUintFn);
STRONG_TYPE_ALIAS(PointerToGetEconItemSystem, GetEconItemSystemFn);
STRONG_TYPE_ALIAS(PointerToGetAttributeDefinitionInterface, GetAttributeDefinitionInterfaceFn);
STRONG_TYPE_ALIAS(PointerToGetItemInLoadout, GetItemInLoadoutFn);
STRONG_TYPE_ALIAS(PointerToEquipItemInLoadout, EquipItemInLoadoutFn);
STRONG_TYPE_ALIAS(PointerToCreateBaseTypeCache, CreateBaseTypeCacheFn);
STRONG_TYPE_ALIAS(PointerToFindTypeCache, FindTypeCacheFn);
STRONG_TYPE_ALIAS(PointerToUpdateSkin, UpdateSkinFn);
STRONG_TYPE_ALIAS(PointerToSetAttributeValueByName, SetAttributeValueByNameFn);
STRONG_TYPE_ALIAS(PointerToGetCustomPaintKitIndex, GetCustomPaintKitIndexFn);
STRONG_TYPE_ALIAS(PointerToGetStaticData, GetStaticDataFn);
STRONG_TYPE_ALIAS(PointerToSetModel, SetModelFn);
STRONG_TYPE_ALIAS(PointerToSetMeshGroupMask, SetMeshGroupMaskFn);
