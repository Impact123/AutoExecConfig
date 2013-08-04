#if defined _autoexecconfig_included
	#endinput
#endif
#define _autoexecconfig_included


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
#define AUTOEXEC_CLEAN_SUCCESS         22



// General
#define AUTOEXEC_NO_CONFIG             30



// Formatter
#define AUTOEXEC_FORMAT_BAD_FILENAME   40
#define AUTOEXEC_FORMAT_SUCCESS        41



// Global variables
static String:g_sConfigFile[PLATFORM_MAX_PATH];
static bool:g_bCreateFile = false;
static Handle:g_hPluginHandle = INVALID_HANDLE;



// Workaround for now
static g_iLastFindResult;
static g_iLastAppendResult;




/**
 * Returns the last result from the parser.
 *
 * @return			Returns one of the AUTOEXEC_FIND values or -1 if not set.
*/
stock AutoExecConfig_GetFindResult()
{
	return g_iLastFindResult;
}





/**
 * Returns the last result from the appender.
 *
 * @return			Returns one of the AUTOEXEC_APPEND values or -1 if not set.
*/
stock AutoExecConfig_GetAppendResult()
{
	return g_iLastAppendResult;
}


/**
 * Set if the config file should be created if it doesn't exist yet.
 *
 * @param create 	True if config file should be created, false otherwise.
 * @noreturn
 */
stock AutoExecConfig_SetCreateFile(bool:create)
{
	g_bCreateFile = create;
}


/**
 * Returns if the config file should be created if it doesn't exist.
 *
 * @return			Returns true, if the config file should be created or false if it should not.
 */
stock bool:AutoExecConfig_GetCreateFile()
{
	return g_bCreateFile;
}


/**
 * Set the plugin for which the config file should be created.
 * Set to INVALID_HANDLE to use the calling plugin.
 * Used to print the correct filename in the top comment when creating the file.
 *
 * @param plugin 	The plugin to create convars for or INVALID_HANDLE to use the calling plugin.
 * @noreturn
 */
stock AutoExecConfig_SetPlugin(Handle:plugin)
{
	g_hPluginHandle = plugin;
}


/**
 * Returns the plugin for which the config file is created.
 *
 * @return			The plugin handle
 */
stock Handle:AutoExecConfig_GetPlugin()
{
	return g_hPluginHandle;
}


/**
 * Set the global autoconfigfile used by functions of this file.
 *
 * @param file			Name of the config file, path and .cfg extension is being added if not given.
 * @param folder		Folder under cfg/ to use.  By default this is "sourcemod."
 * @return				True if formatter returned success, false otherwise.
*/
stock bool:AutoExecConfig_SetFile(String:file[], String:folder[]="sourcemod")
{
	Format(g_sConfigFile, sizeof(g_sConfigFile), "%s", file);
	
	
	// Format the filename
	return AutoExecConfig_FormatFileName(g_sConfigFile, sizeof(g_sConfigFile), folder) == AUTOEXEC_FORMAT_SUCCESS;
}






/**
 * Get the formatted autoconfigfile used by functions of this file.
 *
 * @param buffer		String to format.
 * @param size			Maximum size of buffer
 * @return				True if filename was set, false otherwise.
*/
stock AutoExecConfig_GetFile(String:buffer[], size)
{
	if(strlen(g_sConfigFile) > 0)
	{
		strcopy(buffer, size, g_sConfigFile);
		
		return true;
	}
	
	// Security for decl users
	buffer[0] = '\0';
	
	return false;
}






/**
 * Creates a convar and appends it to the autoconfigfile if not found.
 * FCVAR_DONTRECORD will be skipped.
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
stock Handle:AutoExecConfig_CreateConVar(const String:name[], const String:defaultValue[], const String:description[]="", flags=0, bool:hasMin=false, Float:min=0.0, bool:hasMax=false, Float:max=0.0)
{
	// If configfile was set and convar has no dontrecord flag
	if(!(flags & FCVAR_DONTRECORD) && strlen(g_sConfigFile) > 0)
	{
		// Reset the results
		g_iLastFindResult = -1;
		g_iLastAppendResult = -1;
		
		
		// Add it if not found
		decl String:buffer[64];
		if( ( g_iLastFindResult = AutoExecConfig_FindValue(name, buffer, sizeof(buffer), true) ) == AUTOEXEC_FIND_NOT_FOUND)
		{
			g_iLastAppendResult = AutoExecConfig_AppendValue(name, defaultValue, description, flags, hasMin, min, hasMax, max);
		}
	}
	
	
	// Create the convar
	return CreateConVar(name, defaultValue, description, flags, hasMin, min, hasMax, max);
}






/**
 * Formats a autoconfigfile, prefixes path and adds .cfg extension if missed.
 *
 * @param buffer		String to format.
 * @param size			Maximum size of buffer.
 * @return				Returns one of the AUTOEXEC_FORMAT values..
*/
stock static AutoExecConfig_FormatFileName(String:buffer[], size, String:folder[]="sourcemod")
{
	// No config set
	if(strlen(g_sConfigFile) < 1)
	{
		return AUTOEXEC_NO_CONFIG;
	}
	
	
	// Can't be an cfgfile
	if(StrContains(g_sConfigFile, ".cfg") == -1 && strlen(g_sConfigFile) < 4)
	{
		return AUTOEXEC_FORMAT_BAD_FILENAME;
	}
	
	
	// Pathprefix
	decl String:pathprefixbuffer[PLATFORM_MAX_PATH];
	if(strlen(folder) > 0)
	{
		Format(pathprefixbuffer, sizeof(pathprefixbuffer), "cfg/%s/", folder);
	}
	else
	{
		Format(pathprefixbuffer, sizeof(pathprefixbuffer), "cfg/");
	}
	
	
	decl String:filebuffer[PLATFORM_MAX_PATH];
	filebuffer[0] = '\0';
	
	// Add path if file doesn't begin with it
	if(StrContains(buffer, pathprefixbuffer) != 0)
	{
		StrCat(filebuffer, sizeof(filebuffer), pathprefixbuffer);
	}
	
	StrCat(filebuffer, sizeof(filebuffer), g_sConfigFile);
	
	
	// Add .cfg extension if file doesn't end with it
	if(StrContains(filebuffer[strlen(filebuffer) - 4], ".cfg") != 0)
	{
		StrCat(filebuffer, sizeof(filebuffer), ".cfg");
	}
	
	strcopy(buffer, size, filebuffer);
	
	return AUTOEXEC_FORMAT_SUCCESS;
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
	
	
	decl String:filebuffer[PLATFORM_MAX_PATH];
	strcopy(filebuffer, sizeof(filebuffer), g_sConfigFile);
	
	
	//PrintToServer("pathbuffer: %s", filebuffer);
	
	new bool:bFileExists = FileExists(filebuffer);
	
	if(g_bCreateFile || bFileExists)
	{
		// If the file already exists we open it in append mode, otherwise we use a write mode which creates the file
		new Handle:hfile = OpenFile(filebuffer, (bFileExists ? "a" : "w"));
		decl String:writebuffer[2048];
		
		
		if(hfile == INVALID_HANDLE)
		{
			return AUTOEXEC_APPEND_BAD_HANDLE;
		}
		
		// We just created the file, so add some header about version and stuff
		if(g_bCreateFile && !bFileExists)
		{
			WriteFileLine(hfile, "// This file was auto-generated by AutoExecConfig read and append beta");
			
			GetPluginFilename(g_hPluginHandle, writebuffer, sizeof(writebuffer));
			Format(writebuffer, sizeof(writebuffer), "// ConVars for plugin \"%s\"", writebuffer);
			WriteFileLine(hfile, writebuffer);
		}
		
		// Spacer
		WriteFileLine(hfile, "\n");
		

		// This is used for multiline comments
		new newlines = GetCharCountInStr('\n', description);
		if(newlines == 0)
		{
			// We have no newlines, we can write the description to the file as is
			Format(writebuffer, sizeof(writebuffer), "// %s", description);
			WriteFileLine(hfile, writebuffer);
		}
		else
		{
			new String:newlineBuf[newlines +1][2048];
			ExplodeString(description, "\n", newlineBuf, newlines +1, sizeof(newlineBuf[]), false);
			
			// Each newline gets a commented newline
			for(new i; i <= newlines; i++)
			{
				if(strlen(newlineBuf[i]) > 0)
				{
					WriteFileLine(hfile, "// %s", newlineBuf[i]);
				}
			}
		}
		
		
		// Descspacer
		WriteFileLine(hfile, "// -");
		
		
		// Default
		Format(writebuffer, sizeof(writebuffer), "// Default: \"%s\"", defaultValue);
		WriteFileLine(hfile, writebuffer);
		
		
		// Minimum
		if(hasMin)
		{		
			Format(writebuffer, sizeof(writebuffer), "// Minimum: \"%f\"", min);
			WriteFileLine(hfile, writebuffer);
		}
		
		
		// Maximum
		if(hasMax)
		{
			Format(writebuffer, sizeof(writebuffer), "// Maximum: \"%f\"", max);
			WriteFileLine(hfile, writebuffer);
		}
		
		
		// Write end and defaultvalue
		Format(writebuffer, sizeof(writebuffer), "%s \"%s\"", name, defaultValue);
		WriteFileLine(hfile, writebuffer);
		
		
		CloseHandle(hfile);
		
		
		// Clean up the file
		//AutoExecConfig_CleanFile(filebuffer, false);
		
		
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
	// Security for decl users
	value[0] = '\0';
	
	
	// No config set
	if(strlen(g_sConfigFile) < 1)
	{
		return AUTOEXEC_NO_CONFIG;
	}
	
	
	decl String:filebuffer[PLATFORM_MAX_PATH];
	strcopy(filebuffer, sizeof(filebuffer), g_sConfigFile);
	
	
	
	//PrintToServer("pathbuffer: %s", filebuffer);
	
	new bool:bFileExists = FileExists(filebuffer);
	
	// We want to create the config file and it doesn't exist yet.
	if(g_bCreateFile && !bFileExists)
	{
		return AUTOEXEC_FIND_FILE_NOT_FOUND;
	}
	
	
	if(bFileExists)
	{
		new Handle:hfile = OpenFile(filebuffer, "r");
		new valuestart;
		new valueend;
		new cvarend;
		
		// Just an reminder to self, leave the values that high
		decl String:sConvar[64];
		decl String:sValue[64];
		decl String:readbuffer[2048];
		decl String:copybuffer[2048];
		
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
			
			
			// Has not enough spaces, must have at least 1
			if(GetCharCountInStr(' ', readbuffer) < 1)
			{
				continue;
			}

			
			// Ignore cvars which aren't quoted
			if(GetCharCountInStr('"', readbuffer) != 2)
			{
				continue;
			}
			
			
			
			// Get the start of the value
			if( (valuestart = StrContains(readbuffer, "\"")) == -1 )
			{
				continue;
			}
			
			
			// Get the end of the value
			if( (valueend = StrContains(readbuffer[valuestart+1], "\"")) == -1 )
			{
				continue;
			}
			
			
			// Get the start of the cvar, 
			if( (cvarend = StrContains(readbuffer, " ")) == -1 || cvarend >= valuestart)
			{
				continue;
			}
			
			
			// Skip if cvarendindex is before valuestartindex
			if(cvarend >= valuestart)
			{
				continue;
			}
			
			
			// Convar
			// Tempcopy for security
			strcopy(copybuffer, sizeof(copybuffer), readbuffer);
			copybuffer[cvarend] = '\0';
			
			strcopy(sConvar, sizeof(sConvar), copybuffer);
			

			// Value
			// Tempcopy for security
			strcopy(copybuffer, sizeof(copybuffer), readbuffer[valuestart+1]);
			copybuffer[valueend] = '\0';
			
			strcopy(sValue, sizeof(sValue), copybuffer);
			
			
			//PrintToServer("Cvar %s has a value of %s", sConvar, sValue);
			
			if(StrEqual(sConvar, cvar, caseSensitive))
			{
				Format(value, size, "%s", sConvar);
				
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
 * Cleans the global autoconfigfile from too much spaces
 *
 * @return					One of the AUTOEXEC_CLEAN values.
*/
stock AutoExecConfig_CleanFile()
{
	// No config set
	if(strlen(g_sConfigFile) < 1)
	{
		return AUTOEXEC_NO_CONFIG;
	}
	
	
	decl String:sfile[PLATFORM_MAX_PATH];
	strcopy(sfile, sizeof(sfile), g_sConfigFile);
	
	
	// Security
	if(!FileExists(sfile))
	{
		return AUTOEXEC_CLEAN_FILE_NOT_FOUND;
	}
	
	
	
	decl String:sfile2[PLATFORM_MAX_PATH];
	Format(sfile2, sizeof(sfile2), "%s_tempcopy", sfile);
	
	
	decl String:readbuffer[2048];
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
		
		if(hfile2 != INVALID_HANDLE)
		{
			//PrintToServer("Handle2 invalid");
			CloseHandle(hfile2);
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
		// No space, count from start
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
 * Returns how many times the given char occures in the given string.
 *
 * @param str		String to search for in.
 * @return			Count of newlines found.
*/
stock static GetCharCountInStr(character, const String:str[])
{
	new len = strlen(str);
	new count;
	
	for(new i; i < len; i++)
	{
		if(str[i] == character)
		{
			count++;
		}
	}
	
	return count;
}