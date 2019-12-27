/**
 * AutoExecConfig 
 *
 * Copyright (C) 2013-2019 Impact
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
 */
 
#include <sourcemod>
#include <profiler>

#pragma semicolon 1
#pragma newdecls required
#include "autoexecconfig"


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
	bool error;
	
	
	// Order of this is important, the setting has to be known before we set the file path
	AutoExecConfig_SetCreateDirectory(true);
	
	// We want to let the include file create the file if it doesn't exists already, otherwise we let sourcemod create it
	AutoExecConfig_SetCreateFile(true);
	
	// Set file, extension is optional aswell as the second parameter which defaults to sourcemod
	AutoExecConfig_SetFile("autoexecconfigtest", "sourcemod");

	
	AutoExecConfig_CreateConVar("listme", "Avalue", "An description");
	SetAppend(appended);
	SetError(error);
	
	AutoExecConfig_CreateConVar("listme2", "Anothervalue", "An other description");
	SetAppend(appended);
	SetError(error);
	
	AutoExecConfig_CreateConVar("boundtest", "Anothervaluetoo", "Cvar for boundtest", FCVAR_NONE, true, 5.0, true, 10.0);
	SetAppend(appended);
	SetError(error);
	
	AutoExecConfig_CreateConVar("newlinetest", "SomeCvar", "This\nIs\nA\nNewline\nTest", FCVAR_NONE);
	SetAppend(appended);
	SetError(error);
	
	AutoExecConfig_CreateConVar("CaSeSeNsItIvEtEsT", "Casesensitivecvar", "Weird written cvar with bounds", FCVAR_NONE, false, 0.0, true, 12.5);
	SetAppend(appended);
	SetError(error);
	
	AutoExecConfig_CreateConVar("LongDescriptionTest", "LongDescxyz", "Really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, really, long description", FCVAR_NONE, false, 4.0, true, 53.5);
	SetAppend(appended);
	SetError(error);
	
	AutoExecConfig_CreateConVar("321isgreaterthan123", "Anothervalue", "Convar with numbers");
	SetAppend(appended);
	SetError(error);

	
	// Execute the given config
	AutoExecConfig_ExecuteFile();
	
	
	
	// Cleaning is an relatively expensive file operation
	if (appended)
	{
		PrintToServer("Some convars were appended to the config, clean it up");
		AutoExecConfig_CleanFile();
	}
	
	if (error)
	{
		PrintToServer("Non successfull result occured, last find/append result: %d, %d", AutoExecConfig_GetFindResult(), AutoExecConfig_GetAppendResult());
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


void SetError(bool &error)
{
	int findRes = AutoExecConfig_GetAppendResult();
	int appendRes = AutoExecConfig_GetFindResult();
	
	if ( (findRes != -1 && findRes != AUTOEXEC_APPEND_SUCCESS) ||
	     (appendRes != -1 && appendRes != AUTOEXEC_FIND_SUCCESS) )
	{
		error = true;
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