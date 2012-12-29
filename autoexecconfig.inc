#if defined _autoexecconfig_included
	#endinput
#endif
#define _basics_clients_included


#include <sourcemod>




// Append
#define AUTOEXEC_APPEND_BAD_FILENAME    0
#define AUTOEXEC_APPEND_FILE_NOT_FOUND  1
#define AUTOEXEC_APPEND_BAD_HANDLE      2
#define AUTOEXEC_APPEND_SUCCESS         3



// Find
#define AUTOEXEC_FIND_BAD_FILENAME     10
#define AUTOEXEC_FIND_FILE_NOT_FOUND   11
#define AUTOEXEC_FIND_BAD_HANDLE       12
#define AUTOEXEC_FIND_NOT_FOUND        13
#define AUTOEXEC_FIND_SUCCESS          14



// Clean
#define AUTOEXEC_CLEAN_FILE_NOT_FOUND  20
#define AUTOEXEC_CLEAN_BAD_HANDLE      21
#define AUTOEXEC_CLEAN_SUCCESS         31



// General
#define AUTOEXEC_NO_CONFIG             30




static String:g_sConfigFile[PLATFORM_MAX_PATH];




/**
 * Set the global autoconfigfile used by functions of this file
 *
 * @param format		Formatting rules..
 * @param ...			Variable number of format parameters.
 * @noreturn
*/
stock AutoExecConfig_SetFile(String:format[], any:...)
{
	VFormat(g_sConfigFile, sizeof(g_sConfigFile), format, 2);
}




/**
 * Creates a convar and appends it to the autoconfigfile if not found
 * FCVAR_DONTRECORD will be skipped
 *
 * @param name			Name of new convar.
 * @param defaultValue	String containing the default value of new convar.
 * @param description	Optional description of the convar.
 * @param flags			Optional bitstring of flags determining how the convar should be handled. See FCVAR_* constants for more details.
 * @param hasMin		Optional boolean that determines if the convar has a minimum value.
 * @param min			Minimum floating point value that the convar can have if hasMin is true.
 * @param hasMax		Optional boolean that determines if the convar has a maximum value.
 * @param max			Maximum floating point value that the convar can have if hasMax is true.
 * @return				A handle to the newly created convar. If the convar already exists, a handle to it will still be returned.
 * @error				Convar name is blank or is the same as an existing console command.
*/
Handle:AutoExecConfig_CreateConVar(const String:name[], const String:defaultValue[], const String:description[]="", flags=0, bool:hasMin=false, Float:min=0.0, bool:hasMax=false, Float:max=0.0)
{
	// If configfile was set
	if(!(flags & FCVAR_DONTRECORD) && strlen(g_sConfigFile) > 0)
	{
		// Add it if not found
		decl String:buffer[64];
		if(AutoExecConfig_FindValue(name, buffer, sizeof(buffer), true) == AUTOEXEC_FIND_NOT_FOUND)
		{
			AutoExecConfig_AppendValue(name, defaultValue, description, flags, hasMin, min, hasMax, max);
		}
	}
	
	// Create the convar
	return CreateConVar(name, defaultValue, description, flags, hasMin, min, hasMax, max);
}





/**
 * Appends a convar to the global autoconfigfile
 *
 * @param name			Name of new convar.
 * @param defaultValue	String containing the default value of new convar.
 * @param description	Optional description of the convar.
 * @param flags			Optional bitstring of flags determining how the convar should be handled. See FCVAR_* constants for more details.
 * @param hasMin		Optional boolean that determines if the convar has a minimum value.
 * @param min			Minimum floating point value that the convar can have if hasMin is true.
 * @param hasMax		Optional boolean that determines if the convar has a maximum value.
 * @param max			Maximum floating point value that the convar can have if hasMax is true.
 * @return				Returns one of the AUTOEXEC_APPEND values
*/
stock AutoExecConfig_AppendValue(const String:name[], const String:defaultValue[], const String:description[], flags, bool:hasMin, Float:min, bool:hasMax, Float:max)
{
	// No config set
	if(strlen(g_sConfigFile) < 1)
	{
		return AUTOEXEC_NO_CONFIG;
	}
	
	
	// Can't be an cfgfile
	if(StrContains(g_sConfigFile, ".cfg") == -1 && strlen(g_sConfigFile) < 4)
	{
		return AUTOEXEC_APPEND_BAD_FILENAME;
	}
	
	
	decl String:filebuffer[PLATFORM_MAX_PATH] = "cfg/sourcemod/";
	StrCat(filebuffer, sizeof(filebuffer), g_sConfigFile);
	
	
	// Add .cfg extension if missed
	if(StrContains(filebuffer[strlen(filebuffer) - 4], ".cfg") != 0)
	{
		StrCat(filebuffer, sizeof(filebuffer), ".cfg");
	}
	
	
	//PrintToServer("pathbuffer: %s", filebuffer);
	
	
	
	if(FileExists(filebuffer))
	{
		new Handle:hfile = OpenFile(filebuffer, "a");
		decl String:buffer[64];
		
		
		if(hfile == INVALID_HANDLE)
		{
			return AUTOEXEC_APPEND_BAD_HANDLE;
		}
		
		
		// Spacer
		WriteFileLine(hfile, "\n");
		
		
		// Desc
		Format(buffer, sizeof(buffer), "// %s", defaultValue);
		WriteFileLine(hfile, buffer);
		
		
		Format(buffer, sizeof(buffer), "// -");
		WriteFileLine(hfile, buffer);
		
		
		// Default
		Format(buffer, sizeof(buffer), "// Default: \"%s\"", defaultValue);
		WriteFileLine(hfile, buffer);
		
		
		// Minimum
		if(hasMin)
		{		
			Format(buffer, sizeof(buffer), "// Minimum: \"%f\"", min);
			WriteFileLine(hfile, buffer);
		}
		
		
		// Maximum
		if(hasMax)
		{
			Format(buffer, sizeof(buffer), "// Maximum: \"%f\"", max);
			WriteFileLine(hfile, buffer);
		}
		
		
		// Write end and defaultvalue
		Format(buffer, sizeof(buffer), "%s \"%s\"", name, defaultValue);
		WriteFileLine(hfile, buffer);
		
		
		CloseHandle(hfile);
		
		
		// Clean up the file
		AutoExecConfig_CleanFile(filebuffer);
		
		
		return AUTOEXEC_APPEND_SUCCESS;
	}
	
	return AUTOEXEC_APPEND_FILE_NOT_FOUND;
}






/**
 * Returns a convars value from the global autoconfigfile
 *
 * @param cvar				Cvar to search for.
 * @param value				Buffer to store result into.
 * @param size				Maximum size of buffer.
 * @param caseSensitive		Whether or not the search should be case sensitive.
 * @return					Returns one of the AUTOEXEC_FIND values
*/
stock AutoExecConfig_FindValue(const String:cvar[], String:value[], size, bool:caseSensitive=false)
{
	// No config set
	if(strlen(g_sConfigFile) < 1)
	{
		return AUTOEXEC_NO_CONFIG;
	}
	
	
	// Can't be an cfgfile
	if(StrContains(g_sConfigFile, ".cfg") == -1 && strlen(g_sConfigFile) < 4)
	{
		return AUTOEXEC_APPEND_BAD_FILENAME;
	}
	
	
	decl String:filebuffer[PLATFORM_MAX_PATH] = "cfg/sourcemod/";
	StrCat(filebuffer, sizeof(filebuffer), g_sConfigFile);
	
	
	// Add .cfg extension if missed
	if(StrContains(filebuffer[strlen(filebuffer) - 4], ".cfg") != 0)
	{
		StrCat(filebuffer, sizeof(filebuffer), ".cfg");
	}
	
	
	//PrintToServer("pathbuffer: %s", filebuffer);
	
	
	
	if(FileExists(filebuffer))
	{
		new Handle:hfile = OpenFile(filebuffer, "r");
		decl String:readbuffer[128];
		decl String:copybuffer[128];
		new index;
		
		
		if(hfile == INVALID_HANDLE)
		{
			return AUTOEXEC_FIND_BAD_HANDLE;
		}
		
		
		while(!IsEndOfFile(hfile) && ReadFileLine(hfile, readbuffer, sizeof(readbuffer)))
		{
			// Is a comment or not valid
			if(IsCharSpace(readbuffer[0]) || readbuffer[0] == '/' || !IsCharAlpha(readbuffer[0]))
			{
				continue;
			}
			
			
			// Has too much spaces
			if(GetSpacesInStr(readbuffer) != 1)
			{
				continue;
			}

			
			ReplaceString(readbuffer, sizeof(readbuffer), "\"", "");
			
			
			// Has no value
			if( (index = StrContains(readbuffer, " ")) == -1 )
			{
				continue;
			}
			
			
			// Space is the last char, must have wrote this with something in mind i can't remember
			if(index > strlen(readbuffer)+1)
			{
				continue;
			}
			
			
			strcopy(copybuffer, sizeof(copybuffer), readbuffer);
			copybuffer[index] = '\0';
			
			
			//PrintToServer("Cvar %s has a value of %s", copybuffer, readbuffer[index +1]);
			
			if(StrEqual(copybuffer, cvar, caseSensitive))
			{
				Format(value, size, "%s", readbuffer[index +1]);
				
				CloseHandle(hfile);
				return AUTOEXEC_FIND_SUCCESS;
			}					
		}
		
		CloseHandle(hfile);	
		return AUTOEXEC_FIND_NOT_FOUND;
	}
	
	
	return AUTOEXEC_FIND_FILE_NOT_FOUND;
}





/**
 * Accepts a preformatted path to the the global autoconfigfile and cleans it from too much spaces
 *
 * @param sfile				Path to autoconfigfile.
 * @return					True if cleaned, false otherwise.
*/
AutoExecConfig_CleanFile(String:sfile[])
{
	// Security
	if(!FileExists(sfile))
	{
		return AUTOEXEC_CLEAN_FILE_NOT_FOUND;
	}
	
	
	decl String:sfile2[PLATFORM_MAX_PATH];
	Format(sfile2, sizeof(sfile2), "%s_tempcopy", sfile);
	
	
	decl String:readbuffer[128];
	new count;
	new bool:firstreached;
	
	
	// Open files
	new Handle:hfile1 = OpenFile(sfile, "r");
	new Handle:hfile2 = OpenFile(sfile2, "w");
	
	
	
	// Check filehandles
	if(hfile1 == INVALID_HANDLE || hfile2 == INVALID_HANDLE)
	{
		if(hfile1 != INVALID_HANDLE)
		{
			//PrintToServer("Handle1 invalid");
			CloseHandle(hfile1);
		}
		
		if(hfile1 != INVALID_HANDLE)
		{
			//PrintToServer("Handle2 invalid");
			CloseHandle(hfile1);
		}
		
		return AUTOEXEC_CLEAN_BAD_HANDLE;
	}
	
	
	
	while(!IsEndOfFile(hfile1) && ReadFileLine(hfile1, readbuffer, sizeof(readbuffer)))
	{
		// Is space
		if(IsCharSpace(readbuffer[0]))
		{
			count++;
		}
		// No space, begin from start
		else
		{
			count = 0;
		}
		
		
		// Don't write more than 1 space if seperation after informations have been reached
		if(count < 2 || !firstreached)
		{
			ReplaceString(readbuffer, sizeof(readbuffer), "\n", "");
			WriteFileLine(hfile2, readbuffer);
		}
		
		
		// First bigger seperation after informations has been reached
		if(count == 2)
		{
			firstreached = true;
		}
	}
	
	
	CloseHandle(hfile1);
	CloseHandle(hfile2);
	
	
	// This might be a risk, for now it works
	DeleteFile(sfile);
	RenameFile(sfile, sfile2);
	
	return AUTOEXEC_CLEAN_SUCCESS;
}





/**
 * Returns how much spaces are found within a given string
 *
 * @param str		String to search for in.
 * @return			Count of spaces found.
*/
stock GetSpacesInStr(String:str[])
{
	new len = strlen(str);
	new count;
	
	for(new i; i < len; i++)
	{
		if(str[i] == ' ')
		{
			count++;
		}
	}
	
	return count;
}