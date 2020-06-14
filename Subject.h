#pragma once
#include <vector>

class Observer;
class Event;
class Object;

// Utility class
// Any event than can be observed or received is called a subject
class Subject
{
	/*----------MEMBER FUNCTIONS----------*/
public:
	Subject();
	virtual ~Subject();

	// An "observer" is an active listener to any events that an object might be performing
	// Notifies all observers
	void NotifyAllObservers(Event * aEvent);
	// Notifies specified observer
	void NotifyScoped(Object * aEventOrigin, Event * aEvent, Observer * aObserver);
	void AddObserver(Observer* aObserver);
	void RemoveObserver(Observer* aObserver);

	// A "receiver" is a one time listener to the event and can be thought of as a "fire-and-forget" pattern
	void SendReceiver(Object * aObjectReceiver, Event * aEvent);
	/*----------MEMBER VARIABLES----------*/
private:
	std::vector<Observer *> ObserverList;

};

