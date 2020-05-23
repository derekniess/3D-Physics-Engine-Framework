#pragma once
// The base class for all types
// Used to allow events/notifications to be sent to any entity within the engine
class Object
{
public:
	Object();
	virtual void ReceiveEvent() {};
	virtual ~Object();
};

