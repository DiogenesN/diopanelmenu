#include <gtk/gtk.h>

#ifndef STRUCTS_H_
#define STRUCTS_H_

struct dataOptions {
	GtkWidget *pop;
	GtkWidget *popleft;
	GtkWidget *diomenu;
	GtkAdjustment *posx;
	GtkAdjustment *height;
	GListModel *glistmodel;
	GtkWidget *entryRestart;
	GtkWidget *entryPoweroff;
	GtkWidget *windowSettings;
	GtkWidget *entrySysettings;
	GtkStringList *stringsNames;
	GtkStringList *stringsExecs;
	GtkStringList *stringsIcons;
	GtkApplication* application;
	GtkWidget *windowAppSettings;
	GtkSingleSelection *selection;
};

struct dataOptions mydataOptions;
struct dataOptions *p_dataOptions = &mydataOptions;

#endif
