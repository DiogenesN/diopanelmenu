/* creates initial config directory and file */

#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "configsgen.h"

void create_configs() {
	const char *HOME = getenv("HOME");

	if (HOME == NULL) {
        fprintf(stderr, "Unable to determine the user's home directory.\n");
        return;
    }

	const char  *configDirPath 				= "/.config/dioappfinder";
	const char  *nameOfConfig 				= "/dioappfinder.conf";
	const char  *configDioPanel 			= "/.config/diopanel";
	const char  *configDioPanelMenu			= "/.config/diopanel/diomenu";
	const char  *configDioPanelMenuFile		= "/diomenu.conf";
	const char  *configDioPanelMenuFileApp	= "/diomenuapp.conf";
	const char  *configDioPanelMenuPaths	= "/diomenupaths.conf";
	char 		fullPath[700];
	char 		fullPathWithName[700];
	char 		dioPanelFullPath[700];
	char 		dioPanelMenu[700];
	char 		dioPanelMenuFile[700];
	char 		dioPanelMenuFileApp[700];
	char 		dioPanelMenuFilePaths[700];

    snprintf(fullPath, sizeof(fullPath), "%s%s", HOME, configDirPath);
    snprintf(fullPathWithName, sizeof(fullPathWithName), "%s%s%s", HOME, configDirPath, nameOfConfig);
    snprintf(dioPanelFullPath, sizeof(dioPanelFullPath), "%s%s", HOME, configDioPanel);
    snprintf(dioPanelMenu, sizeof(dioPanelMenu), "%s%s", HOME, configDioPanelMenu);
    snprintf(dioPanelMenuFile, sizeof(dioPanelMenuFile), "%s%s%s", HOME, configDioPanelMenu, \
    																	configDioPanelMenuFile);

    snprintf(dioPanelMenuFileApp, sizeof(dioPanelMenuFileApp), "%s%s%s", HOME, configDioPanelMenu, \
    																	configDioPanelMenuFileApp);

    snprintf(dioPanelMenuFilePaths, sizeof(dioPanelMenuFilePaths), "%s%s%s", HOME, \
    															configDioPanelMenu, \
    															configDioPanelMenuPaths);

	DIR *dioappfinder = opendir(fullPath);
	DIR *dioPanel = opendir(dioPanelFullPath);
	DIR *dirdioPanelMenu = opendir(dioPanelMenu);

	// cheks if the file already exists
	if (dioappfinder && dioPanel && dirdioPanelMenu) {
		// directory exists
		closedir(dioappfinder);
		closedir(dioPanel);
		closedir(dirdioPanelMenu);
		return;
	}
	if (!dioPanel) {
    	// directory does not exist
		printf("Created initial main config files\n");
		mkdir(fullPath, 0755);
		mkdir(dioPanelFullPath, 0755);
		mkdir(dioPanelMenu, 0755);

		closedir(dioappfinder);
		closedir(dioPanel);
		closedir(dirdioPanelMenu);
	}
	if (!dirdioPanelMenu) {
    	// directory does not exist
		printf("Created initial subdir config files\n");
		mkdir(dioPanelMenu, 0755);

		closedir(dioappfinder);
		closedir(dioPanel);
		closedir(dirdioPanelMenu);
	}
		
		//char *path  = "/home/diogenes/.local/share/applications";
		//char *path2 = "/usr/local/share/applications";
		char *path3			= "/usr/share/applications";
		char *width			= "width=100";
		char *height		= "height=100";
		char *posy			= "posy=700";
		char *posx			= "posx=700";
		char *opacity		= "opacity=1";

		char *appheight		= "height=500";
		char *appposx		= "posx=100";
		char *sysettings	= "sysettings=none";
		char *restartbtn	= "restartcmd=none";
		char *poweroffbtn	= "poweroffcmd=none";

		FILE *confFile			= fopen(fullPathWithName, "w+");
		FILE *confFileMenu		= fopen(dioPanelMenuFile, "w+");
		FILE *confFileMenuApp	= fopen(dioPanelMenuFileApp, "w+");
		FILE *confFileMenuPaths	= fopen(dioPanelMenuFilePaths, "w+");
		//fprintf(confFile, "%s\n%s\n%s\n", path, path2, path3);

		fprintf(confFile, "%s\n", path3);
	
		fprintf(confFileMenu, "%s\n", width);
		fprintf(confFileMenu, "%s\n", height);
		fprintf(confFileMenu, "%s\n", posy);
		fprintf(confFileMenu, "%s\n", posx);
		fprintf(confFileMenu, "%s\n", opacity);
	
		fprintf(confFileMenuApp, "%s\n", appheight);
		fprintf(confFileMenuApp, "%s\n", appposx);
		fprintf(confFileMenuApp, "%s\n", sysettings);
		fprintf(confFileMenuApp, "%s\n", restartbtn);
		fprintf(confFileMenuApp, "%s\n", poweroffbtn);

		fprintf(confFileMenuPaths, "%s", "");

		fclose(confFileMenuPaths);
		fclose(confFileMenuApp);
		fclose(confFileMenu);
		fclose(confFile);
}
