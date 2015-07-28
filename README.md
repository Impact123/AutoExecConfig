# AutoExecConfig  
An includefile to read and append to autoconfigs created by sourcemod.  


## How does it work?
If you add an convar via `AutoExecConfig_CreateConVar` it will be searched in the autoconfigfile you have set before.  
If it can't be found within the file it will add it with the informations you created the convar with.  
After that the file should be cleaned from unneccessary whitespaces created by user or autoexecconfig itself.  



## Implementing
One thing i had in mind while writing this was easy implementation. For basic usage you only have to add 2 lines and rename the command you created your convars with.  

### An example

Lets assume you already wrote a plugin with some convars which might look like this.

```SourcePawn
public OnPluginStart()
{
	CreateConVar("sm_myplugin_enabled", "1", "Whether or not this plugin is enabled");
	CreateConVar("sm_myplugin_chattrigger", "myplugin", "Chattrigger to open the menu of this plugin");
	CreateConVar("sm_myplugin_adminflag", "b", "Adminflag needed to use the chattrigger");
	
	AutoExecConfig(true, "plugin.myplugin");
}
```

In order to use this include it would simply have to be changed to this:  

```SourcePawn
public OnPluginStart()
{
	// Set the file for the include
	AutoExecConfig_SetFile("plugin.myplugin");
	
	AutoExecConfig_CreateConVar("sm_myplugin_enabled", "1", "Whether or not this plugin is enabled");
	AutoExecConfig_CreateConVar("sm_myplugin_chattrigger", "myplugin", "Chattrigger to open the menu of this plugin");
	AutoExecConfig_CreateConVar("sm_myplugin_adminflag", "b", "Adminflag needed to use the chattrigger");
	
	AutoExecConfig_ExecuteFile();
	
	// Cleaning is an (optional) expensive operation and should be done at the end
	AutoExecConfig_CleanFile();
}
```
    
## Notes
* The parser will ignore spaces between cvars and values, inside values or behind values for security.  
* The search of the parser, by default, is case sensitive.
* Convars with a FCVAR_DONTRECORD flag will be skipped by the appender.
* The cleaner will format your file the way autoexecconfig does, 2 spaces after information, 1 after cvars.