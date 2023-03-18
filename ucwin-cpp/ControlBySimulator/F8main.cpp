#ifdef __GNUG__ || __MINGW64__
#define UNICODE
#endif
#include "ControlBySimulator.h"

ControlBySimulator cbs;

void StartProgram(void)
{
	//User Main Code Starts Here
	cbs.StartProgram();
	//User Main Code Ends Here
}
void StopProgram(void)
{
	//User Main Code Starts Here
	cbs.StopProgram();
	//User Main Code Ends Here
}
