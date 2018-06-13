#pragma once

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

#include "skse64/PapyrusEvents.h"

#include "skse64/NiNodes.h"
#include "skse64/NiGeometry.h"
#include "skse64/NiMaterial.h"
#include "skse64/NiProperties.h"
#include "skse64/NiExtraData.h"
#include "skse64/NiSerialization.h"

#include "skse64/ScaleformMovie.h"

#include "skse64/HashUtil.h"


class Events {
public:
	static bool Hook(SKSEMessagingInterface* iface);
};

class OurCameraEventHandler : public BSTEventSink <SKSECameraEvent>
{
public:
	virtual	EventResult ReceiveEvent(SKSECameraEvent * evn, EventDispatcher<SKSECameraEvent> * dispatcher);
};
class OurActionEventHandler : public BSTEventSink <SKSEActionEvent>
{
public:
	virtual	EventResult ReceiveEvent(SKSEActionEvent * evn, EventDispatcher<SKSEActionEvent> * dispatcher);
};
class OurNiNodeUpdateEventHandler : public BSTEventSink <SKSENiNodeUpdateEvent>
{
public:
	virtual	EventResult ReceiveEvent(SKSENiNodeUpdateEvent * evn, EventDispatcher<SKSENiNodeUpdateEvent> * dispatcher);
};

void DumpNodeChildren(NiAVObject * node);
