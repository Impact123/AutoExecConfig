# AutoExecConfig  
An includefile to read and append to autoconfigs created by sourcemod.  


## Warning
This currently is pretty much hacked together and should be considered a beta or proof of concept.  
I have tried to write it performant aswell as secure, but it currently hasn't been reviewed or tested enough.  
**No warranty is  given if it destroys something**, you have been warned.. 


## How does it work?
If you add an convar via `AutoExecConfig_CreateConVar` it will be searched in the autoconfigfile you set before.  
If it can't be found within the file it will add it with the informations you created the convar with.  
After that the file will be cleaned from unneccessary whitespaces created by user or autoexecconfig itself.  



## Implementing
The include was wrote it in a way that only the used command must be changed in order to use it.  

### An example

Lets assume you already wrote an plugin with some convars which might look like this.

    public OnPluginStart()
	{
		CreateConVar("sm_myplugin_enabled", "1", "Whether or not this plugin is enabled");
		CreateConVar("sm_myplugin_chattrigger", "myplugin", "Chattrigger to open the menu of this plugin");
		CreateConVar("sm_myplugin_adminflag", "b", "Adminflag needed to use the chattrigger");
		
		AutoExecConfig(true, "plugin.myplugin");
	}
    
In order to use this include it would simply have to be changed to this:  
    
    public OnPluginStart()
	{
		// Set the file for the include
		AutoExecConfig_SetFile("plugin.myplugin");
		
		AutoExecConfig_CreateConVar("sm_myplugin_enabled", "1", "Whether or not this plugin is enabled");
		AutoExecConfig_CreateConVar("sm_myplugin_chattrigger", "myplugin", "Chattrigger to open the menu of this plugin");
		AutoExecConfig_CreateConVar("sm_myplugin_adminflag", "b", "Adminflag needed to use the chattrigger");
		
		AutoExecConfig(true, "plugin.myplugin");
		
		// Cleaning is an expensive operation and should only be done once
		decl String:sfile[PLATFORM_MAX_PATH];
		AutoExecConfig_GetFile(sfile, sizeof(sfile));
		AutoExecConfig_CleanFile(sfile);
	}
    
## Notes
* Don't put spaces between cvarvalues, else the validation failes which results in endless appending
* The search by default is case sensitive