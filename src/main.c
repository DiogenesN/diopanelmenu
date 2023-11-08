/* diopanel menu*/

#include <glib.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "diomenu.c"
#include "structs.h"
#include <gtk/gtk.h>
#include <gio/gio.h>
#include "configsgen.h"
#include "externvars.h"
#include <pango/pango.h>
#include "getvaluefromconf.h"
#include "getlinesfromconf.h"
#include "getnumberoflines.h"
#include "populatenameswithdata.h"

GtkExpression *expressionWidth;
GtkExpressionWatch *watchWidth;

GtkExpression *expressionHeight;
GtkExpressionWatch *watchHeight;

GtkExpression *expressionPosy;
GtkExpressionWatch *watchPosy;

GtkExpression *expressionPosx;
GtkExpressionWatch *watchPosx;

GtkExpression *expressionOpacity;
GtkExpressionWatch *watchOpacity;

static void notify(gpointer data) {
	(void)data;
	// width
  	GValue valueWidth = G_VALUE_INIT;
	gtk_expression_watch_evaluate(watchWidth, &valueWidth);

	// height
  	GValue valueHeight = G_VALUE_INIT;
	gtk_expression_watch_evaluate(watchHeight, &valueHeight);

	// position y
  	GValue valuePosy = G_VALUE_INIT;
	gtk_expression_watch_evaluate(watchPosy, &valuePosy);

	// position x
  	GValue valuePosx = G_VALUE_INIT;
	gtk_expression_watch_evaluate(watchPosx, &valuePosx);

	// position x
  	GValue valueOpacity = G_VALUE_INIT;
	gtk_expression_watch_evaluate(watchOpacity, &valueOpacity);

	gint widthValue		= (gint)g_value_get_double(&valueWidth);
	gint heightValue	= (gint)g_value_get_double(&valueHeight);
	gint posyValue		= (gint)g_value_get_double(&valuePosy);
	gint posxValue		= (gint)g_value_get_double(&valuePosx);
	gboolean opacity	= g_value_get_boolean(&valueOpacity);

	// writing data to config file
	char *configName = "/.config/diopanel/diomenu/diomenu.conf";
	char pathToConfig[3000];
	char writeWidth[3000];
	char writeHeight[3000];
	char writePosy[3000];
	char writePosx[3000];
	char writeOpacity[256];

    snprintf(pathToConfig, sizeof(pathToConfig), "%s%s", HOME, configName);
    snprintf(writeWidth, sizeof(writeWidth), "%s=%d", "width", widthValue);
    snprintf(writeHeight, sizeof(writeHeight), "%s=%d", "height", heightValue);
    snprintf(writePosy, sizeof(writePosy), "%s=%d", "posy", posyValue);
    snprintf(writePosx, sizeof(writePosx), "%s=%d", "posx", posxValue);
    snprintf(writeOpacity, sizeof(writeOpacity), "%s=%d", "opacity", opacity);

	FILE *configPathOpen	= fopen(pathToConfig, "w+");

	fprintf(configPathOpen, "%s\n", writeWidth);
	fprintf(configPathOpen, "%s\n", writeHeight);
	fprintf(configPathOpen, "%s\n", writePosy);
	fprintf(configPathOpen, "%s\n", writePosx);
	fprintf(configPathOpen, "%s\n", writeOpacity);

	fclose(configPathOpen);

	// close and reopen the popup to apply the changer
	gtk_popover_popdown(GTK_POPOVER(p_dataOptions->pop));
	gtk_popover_popup(GTK_POPOVER(p_dataOptions->pop));

	gtk_widget_set_size_request(GTK_WIDGET(p_dataOptions->pop), widthValue, heightValue);
	gtk_popover_set_offset(GTK_POPOVER(p_dataOptions->pop), posxValue, posyValue);
	gtk_widget_set_opacity(p_dataOptions->pop, opacity);

	g_value_unset(&valueWidth);
	g_value_unset(&valueHeight);
	g_value_unset(&valuePosy);
	g_value_unset(&valuePosx);
	g_value_unset(&valueOpacity);
}

/// freeing memory
static int close_request_cb(GtkWindow *window) {
	(void)window;

	gtk_expression_watch_unwatch(watchWidth);
	gtk_expression_unref(expressionWidth);
	gtk_expression_watch_unwatch(watchHeight);
	gtk_expression_unref(expressionHeight);
	gtk_expression_watch_unwatch(watchPosy);
	gtk_expression_unref(expressionPosy);
	gtk_expression_watch_unwatch(watchPosx);
	gtk_expression_unref(expressionPosx);
	gtk_expression_watch_unwatch(watchOpacity);
	gtk_expression_unref(expressionOpacity);

	for (int i = 0; i <= strings_counter + 1; i++) {
		for (int j = 0; j < 1; j++) {
			names[i][j] = NULL;
			execs[i][j] = NULL;
			icons[i][j] = NULL;
			paths[i][j] = NULL;
		}
	}

	p_dataOptions->posx = NULL;
	p_dataOptions->height = NULL;
	p_dataOptions->popleft = NULL;
	p_dataOptions->diomenu = NULL;
	p_dataOptions->selection = NULL;
	p_dataOptions->glistmodel = NULL;
	p_dataOptions->entryRestart = NULL;
	p_dataOptions->stringsNames = NULL;
	p_dataOptions->stringsExecs = NULL;
	p_dataOptions->stringsIcons = NULL;
	p_dataOptions->entryPoweroff = NULL;
	p_dataOptions->windowSettings = NULL;
	p_dataOptions->entrySysettings = NULL;
	p_dataOptions->windowAppSettings = NULL;

	return false;
}

static void remove_button(gpointer data) {
	(void)data;
	gtk_window_close(GTK_WINDOW(p_dataOptions->windowSettings));
	gtk_popover_popdown(GTK_POPOVER(p_dataOptions->pop));
	g_application_quit(G_APPLICATION(p_dataOptions->application));
	gtk_expression_watch_unwatch(watchWidth);
	gtk_expression_watch_unwatch(watchHeight);
	gtk_expression_watch_unwatch(watchPosy);
	gtk_expression_watch_unwatch(watchPosx);
	gtk_expression_watch_unwatch(watchOpacity);
}

static void close_button(gpointer data) {
	(void)data;
	gtk_expression_watch_unwatch(watchWidth);
	gtk_expression_watch_unwatch(watchHeight);
	gtk_expression_watch_unwatch(watchPosy);
	gtk_expression_watch_unwatch(watchPosx);
	gtk_expression_watch_unwatch(watchOpacity);
	gtk_window_close(GTK_WINDOW(p_dataOptions->windowSettings));
}

static void on_left_click(gpointer data) {
	(void)data;
	GtkWidget *window;
	window = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(window), "DioMenu Settings");
	gtk_window_set_icon_name(GTK_WINDOW(window), "application-x-deb");

	p_dataOptions->windowSettings = window;

	GtkWidget *removeButton;
	removeButton = gtk_button_new_with_label("Remove this panel");

	GtkWidget *closeButton;
	closeButton = gtk_button_new_with_label("Close");

	GtkWidget *grid;
	grid = gtk_grid_new();
	gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 100);
	gtk_grid_attach(GTK_GRID(grid), removeButton, 1, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), closeButton, 2, 1, 1, 1);

	// get width and height from config
	gchar configPath[3000];
	gchar *configName = "/.config/diopanel/diomenu/diomenu.conf";
    snprintf(configPath, sizeof(configPath), "%s%s", HOME, configName);

	gint width		= get_int_value_from_conf(configPath, "width");
	gint height		= get_int_value_from_conf(configPath, "height");
	gint posy		= get_int_value_from_conf(configPath, "posy");
	gint posx		= get_int_value_from_conf(configPath, "posx");
	gint opacity	= get_int_value_from_conf(configPath, "opacity");

	// panel width
	GtkWidget *labelWidth;
	labelWidth = gtk_label_new("Panel Width");

	GtkAdjustment *adjustmentSizeWidth;
	adjustmentSizeWidth = gtk_adjustment_new(width, 0, 700, 1, 1, 1);

	GtkWidget *entrySizeWidth;
	entrySizeWidth = gtk_spin_button_new(adjustmentSizeWidth, 1, 0);

	// panel height
	GtkWidget *labelHeight;
	labelHeight = gtk_label_new("Panel Height");

	GtkAdjustment *adjustmentSizeHeight;
	adjustmentSizeHeight = gtk_adjustment_new(height, 0, 700, 1, 1, 1);

	GtkWidget *entrySizeHeight;
	entrySizeHeight = gtk_spin_button_new(adjustmentSizeHeight, 1, 0);

	GtkWidget *boxWidthHeight;
	boxWidthHeight = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(GTK_BOX(boxWidthHeight), TRUE);
	gtk_box_prepend(GTK_BOX(boxWidthHeight), labelWidth);
	gtk_box_append(GTK_BOX(boxWidthHeight), entrySizeWidth);
	gtk_box_append(GTK_BOX(boxWidthHeight), labelHeight);
	gtk_box_append(GTK_BOX(boxWidthHeight), entrySizeHeight);

	// panel position y
	GtkWidget *labelY;
	labelY = gtk_label_new("Position Y");

	GtkAdjustment *adjustmentPosy;
	adjustmentPosy = gtk_adjustment_new(posy, 0, 4000, 1, 1, 1);

	GtkWidget *entryPosy;
	entryPosy = gtk_spin_button_new(adjustmentPosy, 1, 0);

	// panel position x
	GtkWidget *labelX;
	labelX = gtk_label_new("Position X");

	GtkAdjustment *adjustmentPosx;
	adjustmentPosx = gtk_adjustment_new(posx, 0, 4000, 1, 1, 1);

	GtkWidget *entryPosx;
	entryPosx = gtk_spin_button_new(adjustmentPosx, 1, 0);

	GtkWidget *boxPosition;
	boxPosition = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(GTK_BOX(boxPosition), TRUE);
	gtk_box_prepend(GTK_BOX(boxPosition), labelX);
	gtk_box_append(GTK_BOX(boxPosition), entryPosx);
	gtk_box_append(GTK_BOX(boxPosition), labelY);
	gtk_box_append(GTK_BOX(boxPosition), entryPosy);

	// panel opacity
	GtkWidget *labelOpacity;
	labelOpacity = gtk_label_new("\tPanel Opacity");

	GtkWidget *entryOpacity;
	entryOpacity = gtk_switch_new();
	gtk_switch_set_state(GTK_SWITCH(entryOpacity), opacity);

	GtkWidget *boxOpacity;
	boxOpacity = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 30);
	gtk_box_append(GTK_BOX(boxOpacity), labelOpacity);
	gtk_box_append(GTK_BOX(boxOpacity), entryOpacity);

	// main box that contains both vertical boxes
	GtkWidget *boxAll;
	boxAll = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
	gtk_box_prepend(GTK_BOX(boxAll), boxWidthHeight);
	gtk_box_set_homogeneous(GTK_BOX(boxAll), TRUE);
	gtk_box_append(GTK_BOX(boxAll), boxPosition);
	gtk_box_append(GTK_BOX(boxAll), boxOpacity);
	gtk_box_append(GTK_BOX(boxAll), grid);

	expressionHeight = gtk_property_expression_new(GTK_TYPE_ADJUSTMENT, NULL, "value");
	watchHeight = gtk_expression_watch(expressionHeight, adjustmentSizeHeight, notify, NULL, NULL);

	expressionWidth = gtk_property_expression_new(GTK_TYPE_ADJUSTMENT, NULL, "value");
	watchWidth = gtk_expression_watch(expressionWidth, adjustmentSizeWidth, notify, NULL, NULL);

	expressionPosy = gtk_property_expression_new(GTK_TYPE_ADJUSTMENT, NULL, "value");
	watchPosy = gtk_expression_watch(expressionPosy, adjustmentPosy, notify, NULL, NULL);

	expressionPosx = gtk_property_expression_new(GTK_TYPE_ADJUSTMENT, NULL, "value");
	watchPosx = gtk_expression_watch(expressionPosx, adjustmentPosx, notify, NULL, NULL);

	expressionOpacity = gtk_property_expression_new(GTK_TYPE_SWITCH, NULL, "state");
	watchOpacity = gtk_expression_watch(expressionOpacity, entryOpacity, notify, NULL, NULL);

	g_signal_connect_swapped(window, "close-request", G_CALLBACK(close_request_cb), NULL);
	g_signal_connect_swapped(removeButton, "clicked", G_CALLBACK(remove_button), \
															(gpointer)p_dataOptions);
	g_signal_connect_swapped(closeButton, "clicked", G_CALLBACK(close_button), \
															(gpointer)p_dataOptions);

	gtk_window_set_child(GTK_WINDOW(window), boxAll);
	gtk_window_present(GTK_WINDOW(window));
}

/// activate signal handle
static void activate(GtkApplication *app) {
	GtkWidget *window;
	window = gtk_application_window_new(GTK_APPLICATION(app));

	GtkWidget *popContext;
	popContext = gtk_image_new_from_icon_name("application-menu");

	// get width and height from config
	gchar configPath[3000];
	gchar *configName = "/.config/diopanel/diomenu/diomenu.conf";
    snprintf(configPath, sizeof(configPath), "%s%s", HOME, configName);

	gint width		= get_int_value_from_conf(configPath, "width");
	gint height		= get_int_value_from_conf(configPath, "height");
	gint posy		= get_int_value_from_conf(configPath, "posy");
	gint posx		= get_int_value_from_conf(configPath, "posx");
	gint opacity	= get_int_value_from_conf(configPath, "opacity");

	GtkWidget *pop;
	pop = gtk_popover_new();
	gtk_widget_set_size_request(pop, width, height);
	gtk_popover_set_autohide(GTK_POPOVER(pop), FALSE);
	gtk_popover_set_offset(GTK_POPOVER(pop), posx, posy);
	gtk_popover_set_has_arrow(GTK_POPOVER(pop), FALSE);
	gtk_popover_set_child(GTK_POPOVER(pop), popContext);
	gtk_widget_set_opacity(pop, opacity);

	p_dataOptions->pop = pop;

	GtkWidget *box;
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
	gtk_box_prepend(GTK_BOX(box), pop);

	// adds left click action for new panel
	GtkGesture *click;
	click = gtk_gesture_click_new();

	GtkGestureSingle *singleclick;
	singleclick = GTK_GESTURE_SINGLE(click);

	gtk_gesture_single_set_button(singleclick, 3);

	GtkEventController *leftclick;
	leftclick = GTK_EVENT_CONTROLLER(click);
	gtk_event_controller_set_propagation_phase(leftclick, GTK_PHASE_CAPTURE);
	gtk_widget_add_controller(pop, leftclick);

	// adds right click action for new panel
	GtkGesture *rclick;
	rclick = gtk_gesture_click_new();

	GtkGestureSingle *rsingleclick;
	rsingleclick = GTK_GESTURE_SINGLE(rclick);

	gtk_gesture_single_set_button(rsingleclick, 1);

	GtkEventController *rightclick;
	rightclick = GTK_EVENT_CONTROLLER(rclick);
	gtk_event_controller_set_propagation_phase(rightclick, GTK_PHASE_CAPTURE);
	gtk_widget_add_controller(pop, rightclick);

	g_signal_connect_swapped(rightclick, "pressed", G_CALLBACK(open_diomenu), pop);
	g_signal_connect_swapped(leftclick, "pressed", G_CALLBACK(on_left_click), \
														(gpointer)p_dataOptions);

	gtk_window_set_child(GTK_WINDOW(window), box);

	gtk_popover_present(GTK_POPOVER(pop));
	gtk_popover_popup(GTK_POPOVER(pop));
}

int main() {
	// creating initial default config
	create_configs();

	HOME = getenv("HOME");

	gint status;
	GtkApplication *app;
	app = gtk_application_new("com.github.DiogenesN.diopanel",	G_APPLICATION_DEFAULT_FLAGS);

	p_dataOptions->application = app;

	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), 0, NULL);
	g_object_unref(app);

	return status;
}


