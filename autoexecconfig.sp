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
	
	
	new bool:appended;
	
	
	// Set file, second parameter is optional and defaults to sourcemod
	AutoExecConfig_SetFile("autoexecconfigtest", "sourcemod");
	
	
	// We want to create the file if it doesn't exists already
	// This eliminates the need to use the original AutoExecConfig function
	AutoExecConfig_SetCreateFile(true);
	
	
	AutoExecConfig_CreateConVar("listme", "Anvalue", "An description");
	SetAppend(appended);
	
	AutoExecConfig_CreateConVar("listme2", "Anothervalue", "An other description");
	SetAppend(appended);
	
	AutoExecConfig_CreateConVar("boundtest", "Anothervaluetoo", "Cvar for boundtest", FCVAR_PLUGIN, true, 5.0, true, 10.0);
	SetAppend(appended);
	
	AutoExecConfig_CreateConVar("CaSeSeNsItIvEtEsT", "Casesensitivecvar", "Weird written cvar with bounds", FCVAR_PLUGIN, false, 0.0, true, 12.5);
	SetAppend(appended);
	
	AutoExecConfig_CreateConVar("LongDescriptionTest", "LongDescxyz", "Really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, long description", FCVAR_PLUGIN, false, 4.0, true, 53.5);
	SetAppend(appended);

	
	// Only needed when AutoExecConfig_SetCreateFile isn't set to true
	AutoExecConfig(true, "autoexecconfigtest");
	
	
	
	// Cleaning is expensive
	if(appended)
	{
		AutoExecConfig_CleanFile();
	}
	


	StopProfiling(g_hProf);
	
	new Float:fProfilerTime = GetProfilerTime(g_hProf);
	PrintToServer("Benchmark: %f seconds, %f milliseconds", fProfilerTime, fProfilerTime * 1000);
	PrintToServer("Benchmark needed approximately %f %% of 1 Second", CalculateFloatPercentage(fProfilerTime, 0.01));
	PrintToServer("Benchmark needed approximately %f %% of 1 Frame", CalculateFloatPercentage(fProfilerTime, 0.01 / 66.7));
}



SetAppend(&appended)
{
	if(AutoExecConfig_GetAppendResult() == AUTOEXEC_APPEND_SUCCESS)
	{
		appended = true;
	}
}



stock Float:CalculateFloatPercentage(Float:value1, Float:value2)
{
	if(value1 == 0.0 || value2 == 0.0)
	{	
		return 0.0;
	}
	return ((value1 / value2));
}