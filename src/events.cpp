#include "events.h"

OurCameraEventHandler g_ourCameraEventHandler;
OurActionEventHandler g_ourActionEventHandler;
OurNiNodeUpdateEventHandler g_ourNinodeEventHandler;

bool Events::Hook(SKSEMessagingInterface* iface) {
	_MESSAGE("Events::Hook AddEventSink");
	EventDispatcher<SKSEActionEvent> * aed = (EventDispatcher<SKSEActionEvent> *)iface->GetEventDispatcher(SKSEMessagingInterface::kDispatcher_ActionEvent);
	aed->AddEventSink(&g_ourActionEventHandler);
	EventDispatcher<SKSECameraEvent> * ced = (EventDispatcher<SKSECameraEvent> *)iface->GetEventDispatcher(SKSEMessagingInterface::kDispatcher_CameraEvent);
	ced->AddEventSink(&g_ourCameraEventHandler);
	EventDispatcher<SKSENiNodeUpdateEvent> * ned = (EventDispatcher<SKSENiNodeUpdateEvent> *)iface->GetEventDispatcher(SKSEMessagingInterface::kDispatcher_NiNodeUpdateEvent);
	ned->AddEventSink(&g_ourNinodeEventHandler);
	return true;
}

// /d1reportAllClassLayout 

template <std::size_t ExpectedSize, std::size_t RealSize>
void check_offset() {
	static_assert(ExpectedSize == RealSize, "Size is off!");
}

void unused() {
	// check_offset<offsetof(NiAVObject, m_flags), 0x10C>();
}

void DumpNodeChildren(NiAVObject * node)
{
	if (!node) {
		_MESSAGE("DumpNodeChildren NULL");
		return;
	}

	node->m_flags &= ~1;
	_MESSAGE("NODE {%s} {%s} {%d} {%X}", node->GetRTTI()->name, node->m_name, node->m_flags, node);
	if (node->m_extraDataLen > 0) {
		gLog.Indent();
		for (UInt16 i = 0; i < node->m_extraDataLen; i++) {
			_MESSAGE("EXTRA {%s} {%s} {%X}", node->m_extraData[i]->GetRTTI()->name, node->m_extraData[i]->m_pcName, node);
		}
		gLog.Outdent();
	}

	/// don't use the the GetAsXXX methods, probably not right according to comment
	//NiNode * niNode = node->GetAsNiNode();
	NiNode * niNode = DYNAMIC_CAST(node, NiAVObject, NiNode);
	if (niNode && niNode->m_children.m_emptyRunStart > 0)
	{
		gLog.Indent();
		for (int i = 0; i < niNode->m_children.m_emptyRunStart; i++)
		{
			NiAVObject * object = niNode->m_children.m_data[i];
			if (object) {
				//NiNode * childNode = object->GetAsNiNode();
				NiNode * childNode = DYNAMIC_CAST(object, NiAVObject, NiNode);
				//BSGeometry * geometry = object->GetAsBSGeometry();
				BSGeometry * geometry = DYNAMIC_CAST(object, NiAVObject, BSGeometry);
				if (geometry) {
					/*NiGeometryData * geometryData = niptr_cast<NiGeometryData>(geometry->m_spModelData);
					if (geometryData)
					_MESSAGE("{%s} {%s} {%X} {%s} {%X}", object->GetRTTI()->name, object->m_name, object, geometryData->GetRTTI()->name, geometryData);
					else*/
					_MESSAGE("GEOMETRY {%s} {%s} {%X}", object->GetRTTI()->name, object->m_name, object);
				}
				else if (childNode) {
					DumpNodeChildren(childNode);
				}
				else {
					_MESSAGE("OTHER {%s} {%s} {%X}", object->GetRTTI()->name, object->m_name, object);
				}
			}
		}
		gLog.Outdent();
	}
}

void show_nodes(NiNode * n) {
	if (!n)
		return;

	// TODO: don't think n->m_flags is in the right place...
	//n->m_flags |= ~0;
	//n->m_flags &= ~1;

	//auto test = BSFixedString("NPC");
	//auto a = n->GetObjectByName(&test.data);
	//DumpNodeChildren(a);

	auto rootNode = n;
	while (rootNode->m_parent)
		rootNode = rootNode->m_parent;
	DumpNodeChildren(rootNode);
	return;


	_MESSAGE("Node: %#08X", n);
	if (n->m_name)
		_MESSAGE("Name: %s", n->m_name);

	for (UInt32 i = 0; i < n->m_children.m_emptyRunStart; i++) {
		auto child = n->m_children.m_data[i];
		if (child) {
			_MESSAGE("Child: %#08X", child);
			if (child->m_name)
				_MESSAGE("Child Name: %s", child->m_name);
			/*
			auto niNode = child->GetAsNiNode();
			if (niNode) {
				show_nodes(niNode);
			}
			*/
		}
	}
	_MESSAGE("Done");
}

void doThing() {
	if (!g_thePlayer)
		return;
	auto player = *g_thePlayer;
	if (!player)
		return;
	_MESSAGE("First person skeleton");
	show_nodes(player->firstPersonSkeleton);
	if (player->loadedState) {
		_MESSAGE("loaded state -> node");
		show_nodes(player->loadedState->node);
	}
}

EventResult OurCameraEventHandler::ReceiveEvent(SKSECameraEvent * evn, EventDispatcher<SKSECameraEvent> * dispatcher)
{
	_MESSAGE("Received camera event");
	doThing();

	return kEvent_Continue;
}

EventResult OurNiNodeUpdateEventHandler::ReceiveEvent(SKSENiNodeUpdateEvent * evn, EventDispatcher<SKSENiNodeUpdateEvent>* dispatcher)
{
	_MESSAGE("Received ninode event");
	doThing();
	return kEvent_Continue;
}

EventResult OurActionEventHandler::ReceiveEvent(SKSEActionEvent * evn, EventDispatcher<SKSEActionEvent>* dispatcher)
{
	_MESSAGE("Received action event");
	doThing();
	return kEvent_Continue;
}
