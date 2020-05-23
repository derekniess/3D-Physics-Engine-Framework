#pragma once
#include "Object.h"
#include "Event.h"

// Abstract base class
// An observer is an object that can listen to other objects
class Observer : public Object
{
public:
	virtual ~Observer() {};
	// aObject is typically the object that sent the event
	// aEvent is a data packet that can be extended by an event sender to include
	// whatever kind of information is relevant to the event and the receiver
	virtual void OnNotify(Event * aEvent) = 0;
};

