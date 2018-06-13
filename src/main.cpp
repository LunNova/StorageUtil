#include <string>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include "common/ITypes.h"
#include "skse64_common/skse_version.h"
#include "skse64_common/Utilities.h"
#include "skse64_common/Relocation.h"
#include "skse64_common/BranchTrampoline.h"
#include "skse64_common/SafeWrite.h"
#include "common/IDebugLog.h"
#include "skse64/PluginAPI.h"
#include "skse64/PluginManager.h"

#include "common/IFileStream.h"
#include "skse64_common/skse_version.h"

#include "skse64/PluginAPI.h"

#include "skse64/GameAPI.h"
#include "skse64/GameData.h"
#include "skse64/GameObjects.h"
#include "skse64/GameMenus.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameResources.h"
#include "skse64/GameStreams.h"

#include "skse64/NiNodes.h"
#include "skse64/NiGeometry.h"
#include "skse64/NiMaterial.h"
#include "skse64/NiProperties.h"
#include "skse64/NiExtraData.h"
#include "skse64/NiSerialization.h"

#include "events.h"
#include "PapyrusStorageUtil.h"

#include "skse64/ScaleformMovie.h"

#include "skse64/HashUtil.h"

#include <shlobj.h>
#include <time.h>

#define PLUGIN_NAME "TEST"
#define PLUGIN_VERSION 1
#define PLUGIN_RUNTIME RUNTIME_VR_VERSION_1_4_15

static PluginHandle					g_pluginHandle = kPluginHandle_Invalid;
static SKSEPapyrusInterface         * g_papyrus = NULL;

static SKSEMessagingInterface* g_messaging;
static SKSEScaleformInterface* g_scaleform;
static SKSETaskInterface* g_tasks;

extern "C"	{

	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)	{	// Called by SKSE to learn about this plugin and check that it's safe to load it
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim VR\\SKSE\\Test Plugin.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
#ifdef _DEBUG
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);
#else
		gLog.SetLogLevel(IDebugLog::kLevel_Message);
#endif

		_MESSAGE(PLUGIN_NAME " SKSE Plugin");

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "Test";
		info->version = 1;

		// store plugin handle so we can identify ourselves later
		g_pluginHandle = skse->GetPluginHandle();

		if (skse->isEditor) {
			_MESSAGE("loaded in editor, marking as incompatible");

			return false;
		}
		else if (skse->runtimeVersion != PLUGIN_RUNTIME) {
			_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);

			return false;
		}

		// ### do not do anything else in this callback
		// ### only fill out PluginInfo and return true/false

		// supported runtime version
		return true;
	}

	bool SKSEPlugin_Load(const SKSEInterface * skse)	{	// Called by SKSE to load this plugin
		_MESSAGE(PLUGIN_NAME " SKSE Plugin loaded");

		g_papyrus = (SKSEPapyrusInterface *)skse->QueryInterface(kInterface_Papyrus);
		g_messaging = (SKSEMessagingInterface*)skse->QueryInterface(kInterface_Messaging);

		//Check if the function registration was a success...
		/*
		bool registerInventoryCrafting = g_papyrus->Register(SkaarSpecialInventoryCrafting::RegisterFuncs);
		bool registerSoulTrap = g_papyrus->Register(SkaarSoulTrap::RegisterFuncs);

		if (registerInventoryCrafting && registerSoulTrap) {
			srand(time(NULL));
			_MESSAGE("Register Succeeded");
		}
		*/
		g_papyrus->Register(RegisterFuncs);
		_MESSAGE(PLUGIN_NAME " Functions Registered");
		return true;
		if (!Events::Hook(g_messaging)) {
			return false;
		}
		_MESSAGE("Hooked events");

		return true;
	}

};
