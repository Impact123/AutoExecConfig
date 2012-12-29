#include <sourcemod>
#include <profiler>
#include "autoexecconfig"
#pragma semicolon 1



new Handle:g_hProf;



public Plugin:myinfo = 
{
	name = "AutoExecConfig Testsuite",
	author = "Impact",
	description = "Tests the autoexecconfig include",
	version = "0.0.1-dev",
	url = "http://gugyclan.eu"
}



public OnPluginStart()
{
	g_hProf = CreateProfiler();
	StartProfiling(g_hProf);
	
	
	// Set file
	AutoExecConfig_SetFile("autoexecconfigtest");
	
	
	AutoExecConfig_CreateConVar("listme", "Anvalue", "An description");
	AutoExecConfig_CreateConVar("listme2", "Anothervalue", "An other description");
	AutoExecConfig_CreateConVar("boundtest", "Anothervaluetoo", "Cvar for boundtest", FCVAR_PLUGIN, true, 5.0, true, 10.0);
	AutoExecConfig_CreateConVar("CaSeSeNsItIvEtEsT", "Casesensitivecvar", "Weird written cvar with bounds", FCVAR_PLUGIN, false, 0.0, true, 12.5);

	
	AutoExecConfig(true, "autoexecconfigtest");
	

	StopProfiling(g_hProf);
	
	new Float:fProfilerTime = GetProfilerTime(g_hProf);
	PrintToServer("Benchmark: %f seconds, %f milliseconds", fProfilerTime, fProfilerTime * 1000);
	PrintToServer("Benchmark needed approximately %f %% of 1 Second", CalculateFloatPercentage(fProfilerTime, 0.01));
	PrintToServer("Benchmark needed approximately %f %% of 1 Frame", CalculateFloatPercentage(fProfilerTime, 0.01 / 66.7));
}



stock Float:CalculateFloatPercentage(Float:value1, Float:value2)
{
	if(value1 == 0.0 || value2 == 0.0)
	{	
		return 0.0;
	}
	return ((value1 / value2));
}