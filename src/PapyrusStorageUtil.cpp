#include "PapyrusStorageUtil.h"
#include "GameReferences.h"
#include <unordered_map>

using namespace std;

struct CstrHash {
	unsigned long operator()(const char* str) const {
		unsigned long hash = 5381;
		int c;

		while (c = *str++)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		return hash;
	}
};

struct CstrCmp {
	bool operator()(const char* a, const char* b) const {
		return strcmp(a, b) < 0;
	}
};

template <typename T>
//using str_map = unordered_map<const char *, T, CstrHash, CstrCmp>;
using str_map = unordered_map<string, T>;

typedef bool PapyrusBool;
typedef SInt32 PapyrusInt;

template <typename T>
using form_map = unordered_map<UInt32, T>;

template <typename T>
using form_str_map = form_map<str_map<T>>;


//map<const char *, SInt32, CstrCmp> globalIntStorage;
//unordered_map<int, int> test;

template<typename T>
inline T find_inner(form_str_map<T>* map, T def, SInt32 formId, BSFixedString field) {
	auto iter1 = map->find(formId);
	if (iter1 == map->end()) {
		return def;
	}
	auto iter2 = iter1->second.find(string(field));
	if (iter2 == iter1->second.end()) {
		return def;
	}
	return iter2->second;
};

template<typename T>
inline bool remove_inner(form_str_map<T>* map, SInt32 formId, BSFixedString field) {
	auto iter1 = map->find(formId);
	if (iter1 == map->end()) {
		return false;
	}
	str_map<T> m = iter1->second;
	auto success = m.erase(string(field)) > 0;
	if (success && m.empty()) {
		map->erase(formId);
	}
	return success;
};

inline UInt32 FormId(TESForm* form) {
	return form == nullptr ? 0 : form->formID;
}

namespace papyrusStorageUtil {
	form_str_map<SInt32> globalIntStorage;

	SInt32 GetIntValue(StaticFunctionTag* base, TESForm* form, BSFixedString field, SInt32 def) {
		return find_inner(&globalIntStorage, def, FormId(form), field);
	}

	SInt32 SetIntValue(StaticFunctionTag* base, TESForm* form, BSFixedString field, SInt32 val) {
		globalIntStorage[FormId(form)][string(field)] = val;
		return val;
	}

	PapyrusBool UnsetIntValue(StaticFunctionTag* base, TESForm* form, BSFixedString field) {
		return remove_inner(&globalIntStorage, FormId(form), field) ? 1 : 0;
	}

	SInt32 AdjustIntValue(StaticFunctionTag* base, TESForm* form, BSFixedString field, SInt32 val) {
		SInt32 old = find_inner(&globalIntStorage, (SInt32) 0, FormId(form), field);
		SInt32 result = old + val;
		globalIntStorage[FormId(form)][string(field)] = result;
		return result;
	}

	form_str_map<float> globalFloatStorage;

	float GetFloatValue(StaticFunctionTag* base, TESForm* form, BSFixedString field, float def) {
		return find_inner(&globalFloatStorage, def, FormId(form), field);
	}

	float SetFloatValue(StaticFunctionTag* base, TESForm* form, BSFixedString field, float val) {
		globalFloatStorage[FormId(form)][string(field)] = val;
		return val;
	}

	PapyrusBool UnsetFloatValue(StaticFunctionTag* base, TESForm* form, BSFixedString field) {
		return remove_inner(&globalFloatStorage, FormId(form), field) ? 1 : 0;
	}

	float AdjustFloatValue(StaticFunctionTag* base, TESForm* form, BSFixedString field, float val) {
		float old = find_inner(&globalFloatStorage, 0.0f, FormId(form), field);
		float result = old + val;
		globalFloatStorage[FormId(form)][string(field)] = result;
		return result;
	}

	bool RegisterFuncs(VMClassRegistry* registry) {
		const char* PAPYRUS_CLASS = "StorageUtil";

		registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, TESForm*, BSFixedString, SInt32>("GetIntValue", PAPYRUS_CLASS, papyrusStorageUtil::GetIntValue, registry));
		registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, TESForm*, BSFixedString, SInt32>("SetIntValue", PAPYRUS_CLASS, papyrusStorageUtil::SetIntValue, registry));
		registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, PapyrusBool, TESForm*, BSFixedString>("UnsetIntValue", PAPYRUS_CLASS, papyrusStorageUtil::UnsetIntValue, registry));
		registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, TESForm*, BSFixedString, SInt32>("AdjustIntValue", PAPYRUS_CLASS, papyrusStorageUtil::AdjustIntValue, registry));

		registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, TESForm*, BSFixedString, float>("GetFloatValue", PAPYRUS_CLASS, papyrusStorageUtil::GetFloatValue, registry));
		registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, TESForm*, BSFixedString, float>("SetFloatValue", PAPYRUS_CLASS, papyrusStorageUtil::SetFloatValue, registry));
		registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, PapyrusBool, TESForm*, BSFixedString>("UnsetFloatValue", PAPYRUS_CLASS, papyrusStorageUtil::UnsetFloatValue, registry));
		registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, TESForm*, BSFixedString, float>("AdjustFloatValue", PAPYRUS_CLASS, papyrusStorageUtil::AdjustFloatValue, registry));

		registry->SetFunctionFlags(PAPYRUS_CLASS, "GetIntValue", VMClassRegistry::kFunctionFlag_NoWait);
		registry->SetFunctionFlags(PAPYRUS_CLASS, "GetFloatValue", VMClassRegistry::kFunctionFlag_NoWait);

		return true;
	}
}

namespace papyrusMiscUtil {
	BSFixedString GetActorRaceEditorId(StaticFunctionTag* base, Actor* actor) {
		return actor->race->editorId;
	}

	bool RegisterFuncs(VMClassRegistry* registry) {
		const char* PAPYRUS_CLASS = "MiscUtil";

		registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, BSFixedString, Actor*>("GetActorRaceEditorId", PAPYRUS_CLASS, GetActorRaceEditorId, registry));

		registry->SetFunctionFlags(PAPYRUS_CLASS, "GetActorRaceEditorId", VMClassRegistry::kFunctionFlag_NoWait);

		return true;
	}
}

bool RegisterFuncs(VMClassRegistry* registry) {
	papyrusStorageUtil::RegisterFuncs(registry);
	papyrusMiscUtil::RegisterFuncs(registry);

	return true;
}
