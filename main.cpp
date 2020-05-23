#include "Engine.h"
int main(void)
{
	// Create a new instance of the engine and run it
	Engine Instance;
	Instance.Init();
	Instance.Load();
	Instance.Tick();
}