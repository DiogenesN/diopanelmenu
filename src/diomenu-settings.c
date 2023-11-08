/* DioMenu Settings*/

#include <glib.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include "structs.h"
#include "externvars.h"
#include <pango/pango.h>
#include "processdirectory.h"
#include "getvaluefromconf.h"

static void save_close_btn(gpointer data) {
	(void)data;
	// height / x position
	gint heightValue = gtk_adjustment_get_value(p_dataOptions->height);
	gint posxValue	 = gtk_adjustment_get_value(p_dataOptions->posx);

	GtkEntryBuffer *buffSysettings;
	buffSysettings = gtk_entry_get_buffer(GTK_ENTRY(p_dataOptions->entrySysettings));
	const char *sysettingsText = gtk_entry_buffer_get_text(buffSysettings);

	GtkEntryBuffer *buffRestart;
	buffRestart = gtk_entry_get_buffer(GTK_ENTRY(p_dataOptions->entryRestart));
	const char *restartText = gtk_entry_buffer_get_text(buffRestart);

	GtkEntryBuffer *buffPoweroff;
	buffPoweroff = gtk_entry_get_buffer(GTK_ENTRY(p_dataOptions->entryPoweroff));
	const char *poweroffText = gtk_entry_buffer_get_text(buffPoweroff);

	// writing data to config file
	char *configName = "/.config/diopanel/diomenu/diomenuapp.conf";
	char pathToConfig[3000];
	char writeHeight[3000];
	char writePosx[3000];
	char writeSysettings[1000];
	char writeRestart[3000];
	char writePoweroff[3000];

	snprintf(pathToConfig, sizeof(pathToConfig), "%s%s", HOME, configName);
	snprintf(writeHeight, sizeof(writeHeight), "%s=%d", "height", heightValue);
	snprintf(writePosx, sizeof(writePosx), "%s=%d", "posx", posxValue);
	snprintf(writeSysettings, sizeof(writeSysettings), "%s=%s", "sysettings", sysettingsText);
	snprintf(writeRestart, sizeof(writeRestart), "%s=%s", "restartcmd", restartText);
	snprintf(writePoweroff, sizeof(writePoweroff), "%s=%s", "poweroffcmd", poweroffText);

	FILE *configPathOpen	= fopen(pathToConfig, "w+");

	fprintf(configPathOpen, "%s\n", writeHeight);
	fprintf(configPathOpen, "%s\n", writePosx);
	fprintf(configPathOpen, "%s\n", writeSysettings);
	fprintf(configPathOpen, "%s\n", writeRestart);
	fprintf(configPathOpen, "%s\n", writePoweroff);

	fclose(configPathOpen);

	// close and reopen the popup to apply the changer
	gtk_popover_popdown(GTK_POPOVER(p_dataOptions->diomenu));
	gtk_popover_popup(GTK_POPOVER(p_dataOptions->diomenu));

	gtk_window_close(GTK_WINDOW(p_dataOptions->windowAppSettings));
}

static void panel_settings() {
	gtk_popover_popdown(GTK_POPOVER(p_dataOptions->diomenu));

	GtkWidget *window;
	window = gtk_window_new();
	gtk_window_set_default_size(GTK_WINDOW(window), 700, 0);
	gtk_window_set_title(GTK_WINDOW(window), "DioMenu App Settings");
	gtk_window_set_icon_name(GTK_WINDOW(window), "application-x-deb");

	p_dataOptions->windowAppSettings = window;

	GtkWidget *savecloseButton;
	savecloseButton = gtk_button_new_with_label("Save Settings");

	g_signal_connect_swapped(savecloseButton, "clicked", G_CALLBACK(save_close_btn), window);

	GtkWidget *grid;
	grid = gtk_grid_new();
	gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 100);
	gtk_grid_attach(GTK_GRID(grid), savecloseButton, 2, 1, 1, 1);

	// get width and height from config
	gchar configPath[3000];
	gchar *configName = "/.config/diopanel/diomenu/diomenuapp.conf";
    snprintf(configPath, sizeof(configPath), "%s%s", HOME, configName);

	gint height					= get_int_value_from_conf(configPath, "height");
	gint posx					= get_int_value_from_conf(configPath, "posx");
	const gchar *sysettings		= get_char_value_from_conf(configPath, "sysettings");
	const gchar *restartcmd		= get_char_value_from_conf(configPath, "restartcmd");
	const gchar *poweroffcmd	= get_char_value_from_conf(configPath, "poweroffcmd");

	// panel height
	GtkWidget *labelHeight;
	labelHeight = gtk_label_new("Panel Height");

	GtkAdjustment *adjustmentSizeHeight;
	adjustmentSizeHeight = gtk_adjustment_new(height, 0, 900, 1, 1, 1);

	p_dataOptions->height = adjustmentSizeHeight;

	GtkWidget *entrySizeHeight;
	entrySizeHeight = gtk_spin_button_new(adjustmentSizeHeight, 1, 0);

	GtkWidget *boxWidthHeight;
	boxWidthHeight = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(GTK_BOX(boxWidthHeight), TRUE);
	gtk_box_prepend(GTK_BOX(boxWidthHeight), labelHeight);
	gtk_box_append(GTK_BOX(boxWidthHeight), entrySizeHeight);

	// panel posx
	GtkWidget *labelPosx;
	labelPosx = gtk_label_new("Panel Position");

	GtkAdjustment *adjustmentPosx;
	adjustmentPosx = gtk_adjustment_new(posx, 0, 2000, 1, 1, 1);

	p_dataOptions->posx = adjustmentPosx;

	GtkWidget *entryPosx;
	entryPosx = gtk_spin_button_new(adjustmentPosx, 1, 0);

	GtkWidget *boxPosx;
	boxPosx = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(GTK_BOX(boxPosx), TRUE);
	gtk_box_prepend(GTK_BOX(boxPosx), labelPosx);
	gtk_box_append(GTK_BOX(boxPosx), entryPosx);

	// System settings command
	GtkWidget *labelSysettings;
	labelSysettings = gtk_label_new("System Settings CMD:");

	GtkEntryBuffer *bufferSysettings;
	bufferSysettings = gtk_entry_buffer_new(sysettings, -1);

	GtkWidget *entrySysettings;
	entrySysettings = gtk_entry_new_with_buffer(bufferSysettings);

	p_dataOptions->entrySysettings = entrySysettings;

	GtkWidget *boxSysettings;
	boxSysettings = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(GTK_BOX(boxSysettings), TRUE);
	gtk_box_prepend(GTK_BOX(boxSysettings), labelSysettings);
	gtk_box_append(GTK_BOX(boxSysettings), entrySysettings);

	// restart command
	GtkWidget *labelRestart;
	labelRestart = gtk_label_new("Restart CMD:");

	GtkEntryBuffer *bufferRestart;
	bufferRestart = gtk_entry_buffer_new(restartcmd, -1);

	GtkWidget *entryRestart;
	entryRestart = gtk_entry_new_with_buffer(bufferRestart);

	p_dataOptions->entryRestart = entryRestart;

	GtkWidget *boxRestart;
	boxRestart = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(GTK_BOX(boxRestart), TRUE);
	gtk_box_prepend(GTK_BOX(boxRestart), labelRestart);
	gtk_box_append(GTK_BOX(boxRestart), entryRestart);

	// poweroff command
	GtkWidget *labelPoweroff;
	labelPoweroff = gtk_label_new("Poweroff CMD:");

	GtkEntryBuffer *bufferPoweroff;
	bufferPoweroff = gtk_entry_buffer_new(poweroffcmd, -1);

	GtkWidget *entryPoweroff;
	entryPoweroff = gtk_entry_new_with_buffer(bufferPoweroff);

	p_dataOptions->entryPoweroff = entryPoweroff;

	GtkWidget *boxPoweroff;
	boxPoweroff = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(GTK_BOX(boxPoweroff), TRUE);
	gtk_box_prepend(GTK_BOX(boxPoweroff), labelPoweroff);
	gtk_box_append(GTK_BOX(boxPoweroff), entryPoweroff);

	// main box that contains both vertical boxes
	GtkWidget *boxAll;
	boxAll = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
	gtk_box_set_homogeneous(GTK_BOX(boxAll), TRUE);
	gtk_box_prepend(GTK_BOX(boxAll), boxWidthHeight);
	gtk_box_append(GTK_BOX(boxAll), boxPosx);
	gtk_box_append(GTK_BOX(boxAll), boxSysettings);
	gtk_box_append(GTK_BOX(boxAll), boxRestart);
	gtk_box_append(GTK_BOX(boxAll), boxPoweroff);
	gtk_box_append(GTK_BOX(boxAll), grid);

	gtk_window_set_child(GTK_WINDOW(window), boxAll);
	gtk_window_present(GTK_WINDOW(window));
}
