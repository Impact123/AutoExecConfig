#include <sourcemod>
#include <profiler>

#pragma semicolon 1
#include "autoexecconfig"
#pragma newdecls required


Handle g_hProf;



public Plugin myinfo = 
{
	name = "AutoExecConfig Testsuite",
	author = "Impact",
	description = "Tests the autoexecconfig include",
	version = AUTOEXECCONFIG_VERSION,
	url = AUTOEXECCONFIG_URL
}



public void OnPluginStart()
{
	g_hProf = CreateProfiler();
	StartProfiling(g_hProf);
	
	
	bool appended;
	
	
	// Set file, extension is optional aswell as the second parameter which defaults to sourcemod
	AutoExecConfig_SetFile("autoexecconfigtest", "sourcemod");
	
	
	// We want to let the include file create the file if it doesn't exists already, otherwise we let sourcemod create it
	AutoExecConfig_SetCreateFile(true);
	
	// Reduces the need to read the config for each cvar, needs more testing
	AutoExecConfig_CacheConvars();
	
	AutoExecConfig_CreateConVar("listme", "Anvalue", "An description");
	SetAppend(appended);
	
	AutoExecConfig_CreateConVar("listme2", "Anothervalue", "An other description");
	SetAppend(appended);
	
	AutoExecConfig_CreateConVar("boundtest", "Anothervaluetoo", "Cvar for boundtest", FCVAR_PLUGIN, true, 5.0, true, 10.0);
	SetAppend(appended);
	
	AutoExecConfig_CreateConVar("newlinetest", "SomeCvar", "This\nIs\nA\nNewline\nTest", FCVAR_PLUGIN);
	SetAppend(appended);
	
	AutoExecConfig_CreateConVar("CaSeSeNsItIvEtEsT", "Casesensitivecvar", "Weird written cvar with bounds", FCVAR_PLUGIN, false, 0.0, true, 12.5);
	SetAppend(appended);
	
	AutoExecConfig_CreateConVar("LongDescriptionTest", "LongDescxyz", "Really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, long description", FCVAR_PLUGIN, false, 4.0, true, 53.5);
	SetAppend(appended);

	
	// Execute the given config
	AutoExecConfig_ExecuteFile();
	
	
	
	// Cleaning is an relatively expensive file operation
	if (appended)
	{
		AutoExecConfig_CleanFile();
	}
	


	StopProfiling(g_hProf);
	
	float fProfilerTime = GetProfilerTime(g_hProf);
	PrintToServer("Benchmark: %f seconds, %f milliseconds", fProfilerTime, fProfilerTime * 1000);
	PrintToServer("Benchmark needed approximately %f %% of 1 Second", CalculateFloatPercentage(fProfilerTime, 0.01));
	PrintToServer("Benchmark needed approximately %f %% of 1 Frame", CalculateFloatPercentage(fProfilerTime, 0.01 / 66.7));
}



void SetAppend(bool &appended)
{
	if (AutoExecConfig_GetAppendResult() == AUTOEXEC_APPEND_SUCCESS)
	{
		appended = true;
	}
}



stock float CalculateFloatPercentage(float value1, float value2)
{
	if (value1 == 0.0 || value2 == 0.0)
	{	
		return 0.0;
	}
	
	return (value1 / value2);
}