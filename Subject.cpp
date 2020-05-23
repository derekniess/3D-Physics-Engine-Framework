#include "Subject.h"

void Subject::NotifyAllObservers(Event * aEvent)
{
	for (int i = 0; i < ObserverList.size(); ++i)
	{
		ObserverList[i]->OnNotify(aEvent);
	}
}

void Subject::NotifyScoped(Object * aEventOrigin, Event * aEvent, Observer * aObserver)
{
}

Subject::Subject()
{
}


Subject::~Subject()
{
}

void Subject::AddObserver(Observer* aObserver)
{
	ObserverList.push_back(aObserver);
}

void Subject::RemoveObserver(Observer* aObserver)
{
	for (int i = 0; i < ObserverList.size(); ++i)
	{
		// If observer is found in list, erase it from list, shift all following observers to left
		if (aObserver == ObserverList[i])
		{
			for (int j = i; j < ObserverList.size() - 1; ++j)
			{
				ObserverList[i] = ObserverList[i] + 1;
			}
			break;
		}
	}
}

void Subject::SendReceiver(Object * aObjectReceiver, Event * aEvent)
{
}
