/* DioMenu */

#include <glib.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include "structs.h"
#include "removeline.h"
#include "externvars.h"
#include <pango/pango.h>
#include "subprocessrun.h"
#include "diomenu-settings.c"
#include "processdirectory.h"
#include "getvaluefromconf.h"
#include "getlinesfromconf.h"
#include "getnumberoflines.h"
#include "populatenameswithdata.h"

/// creating labels to fill up the listview
static void setup_listitem(GtkListItemFactory *factory, GtkListItem *list_item) {
	(void)factory; // Marking the parameter as unused

	GtkWidget *label;
	label = gtk_label_new(NULL);

	// defining the size of text in label
	PangoAttrList *attr_list = pango_attr_list_new();
	PangoAttribute *attr = pango_attr_size_new_absolute(22 * PANGO_SCALE);
	pango_attr_list_insert(attr_list, attr);
	PangoLayout *layout = gtk_widget_create_pango_layout(label, NULL);
	pango_layout_set_attributes(layout, attr_list);

	gtk_label_set_attributes(GTK_LABEL(label), attr_list);

	GtkWidget *icon;
	icon = gtk_image_new();
	gtk_image_set_icon_size(GTK_IMAGE(icon), GTK_ICON_SIZE_LARGE);

	GtkWidget *box;
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
	gtk_box_prepend(GTK_BOX(box), icon);
	gtk_box_append(GTK_BOX(box), label);

	gtk_list_item_set_child(list_item, box);

	// freing resources
	g_object_unref(layout);
	pango_attr_list_unref(attr_list);
}

/// binding the items to the list, this function runs in a loop
static void bind_listitem(GtkListItemFactory *factory, GtkListItem *list_item) {
	(void)factory; // Marking the parameter as unused

	// Get the GtkBox widget
	GtkWidget *box;
	box = gtk_list_item_get_child(list_item);

	// Find the GtkImage widget within the GtkBox
	GtkWidget *icon;
	icon = gtk_widget_get_first_child(box);

	// Find the next child after GtkImage whoch is a label 
	GtkWidget *label;
	label = gtk_widget_get_next_sibling(icon);

	// gets the strings from GtkStringList *myStrings in startup function
	guint listitemPos = gtk_list_item_get_position(list_item); // index of items

	// getttins names to populate the menu
	const gchar *stringsNames = gtk_string_list_get_string(p_dataOptions->stringsNames, \
																				listitemPos);

	//fix the string that contains leading whitespace
	const gchar *ptrNames = stringsNames;
	while (isspace(*ptrNames)) {
		ptrNames++;
	}

	gchar noWhiteSpaceName[256];
	strcpy(noWhiteSpaceName, ptrNames);

	// sets label text to strings one at a time
	gtk_label_set_label(GTK_LABEL(label), noWhiteSpaceName);

	// get icons
	const gchar *stringsIcons = gtk_string_list_get_string(p_dataOptions->stringsIcons, \
																				listitemPos);

	// fix the icon name that contains leading whitespace
	const gchar *ptrIcon = stringsIcons;

	while (isspace(*ptrIcon)) {
		ptrIcon++;
	}

	gchar noWhiteSpaceIconName[256];
	strcpy(noWhiteSpaceIconName, ptrIcon);

	// if noWhiteSpaceIconName=icon or Icon=/path/icon.png
	if (strchr(noWhiteSpaceIconName, '/') == NULL) {
		gtk_image_set_from_icon_name(GTK_IMAGE(icon), (const gchar *)noWhiteSpaceIconName);
	}
	else {
		gtk_image_set_from_file(GTK_IMAGE(icon), (const gchar *)noWhiteSpaceIconName);
	}
}

/// action when menu autohides
static void menu_closed(GtkWidget *window) {
	// Clear (initialize) the data in the array
	for (int i = 0; i <= strings_counter + 1; i++) {
		for (int j = 0; j < 1; j++) {
			names[i][j] = NULL;
			p_dataOptions->stringsNames = NULL;
			g_free(names[i][j]);
			execs[i][j] = NULL;
			p_dataOptions->stringsExecs = NULL;
			g_free(execs[i][j]);
			icons[i][j] = NULL;
			p_dataOptions->stringsIcons = NULL;
			g_free(icons[i][j]);
			paths[i][j] = NULL;
			g_free(paths[i][j]);
		}
	}

	strings_counter = 0;
	gtk_popover_popdown(GTK_POPOVER(p_dataOptions->diomenu));

	// very important to close the window to prevernt memory leak
	gtk_window_close(GTK_WINDOW(window));
}

/// removes selected item from the list
static void remove_selected() {
	// important to reset this counter to avoid double items in menu
	strings_counter = 0;

	// get width and height from config
	gchar configPath[1000];
	gchar *configName = "/.config/diopanel/diomenu/diomenupaths.conf";
	snprintf(configPath, sizeof(configPath), "%s%s", HOME, configName);

	remove_line_number(configPath, selectedForRemove + 1);

	gtk_popover_popdown(GTK_POPOVER(p_dataOptions->popleft));
}

/// on diomenu items left click
static void left_click_menu_item() {
	// gets currenty selected item
	GtkStringObject *myItem;
	myItem = gtk_single_selection_get_selected_item(p_dataOptions->selection);
	selectedForRemove = gtk_single_selection_get_selected(p_dataOptions->selection);

	const gchar *selectedItem = gtk_string_object_get_string(myItem);
	gchar *rmessage[77];
	gchar *remmessage = "Selected item:";
	snprintf((char *)rmessage, sizeof(rmessage), "%s %s", remmessage, selectedItem); 

	GtkWidget *window;
	window = gtk_window_new();

	GtkWidget *label;
	label = gtk_label_new((const char *)rmessage);

	GtkWidget *removeSelected;
	removeSelected = gtk_button_new_with_label("Remove Selected?");

	g_signal_connect_swapped(removeSelected, "clicked", G_CALLBACK(remove_selected), NULL);
	
	GtkWidget *cancelPop;
	cancelPop = gtk_button_new_with_label("Cancel");

	g_signal_connect_swapped(cancelPop, "clicked", G_CALLBACK(gtk_window_close), window);

	GtkWidget *boxButton;
	boxButton = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
	gtk_box_prepend(GTK_BOX(boxButton), label);
	gtk_box_append(GTK_BOX(boxButton), removeSelected);
	gtk_box_append(GTK_BOX(boxButton), cancelPop);

	// get width and height from config
	gchar configPath[1000];
	gchar *configName = "/.config/diopanel/diomenu/diomenu.conf";
    snprintf(configPath, sizeof(configPath), "%s%s", HOME, configName);

	gint posy	= get_int_value_from_conf(configPath, "posy");
	gint posx	= get_int_value_from_conf(configPath, "posx");

	GtkWidget *popLeft;
	popLeft = gtk_popover_new();
	gtk_widget_set_focusable(popLeft, TRUE);
	gtk_widget_set_size_request(popLeft, 300, 30);
	gtk_popover_set_autohide(GTK_POPOVER(popLeft), FALSE);
	gtk_popover_set_child(GTK_POPOVER(popLeft), boxButton);
	gtk_popover_set_has_arrow(GTK_POPOVER(popLeft), FALSE);
	gtk_popover_set_offset(GTK_POPOVER(popLeft), posx + 220, posy - 250);

	p_dataOptions->popleft = popLeft;

	gtk_window_set_child(GTK_WINDOW(window), popLeft);
	gtk_widget_set_focus_child(window, popLeft);

	gtk_popover_popup(GTK_POPOVER(popLeft));
	gtk_widget_grab_focus(popLeft);
}

/// click on system settings button on the menu window
static void sysettings_cb() {
	gtk_popover_popdown(GTK_POPOVER(p_dataOptions->diomenu));

	// sysettings value from config file
	gchar configPath[3000];
	gchar *configName = "/.config/diopanel/diomenu/diomenuapp.conf";
    snprintf(configPath, sizeof(configPath), "%s%s", HOME, configName);

	char *sysettings = get_char_value_from_conf(configPath, "sysettings");

	run_cmd(sysettings);
}

/// click restart button on diomenu window
static void restart_cb() {
	gtk_popover_popdown(GTK_POPOVER(p_dataOptions->diomenu));

	// sysettings value from config file
	gchar configPath[3000];
	gchar *configName = "/.config/diopanel/diomenu/diomenuapp.conf";
    snprintf(configPath, sizeof(configPath), "%s%s", HOME, configName);

	char *restartcmd = get_char_value_from_conf(configPath, "restartcmd");

	run_cmd(restartcmd);
}

/// click poweroff button on diomenu window
static void poweroff_cb() {
	gtk_popover_popdown(GTK_POPOVER(p_dataOptions->diomenu));

	// sysettings value from config file
	gchar configPath[3000];
	gchar *configName = "/.config/diopanel/diomenu/diomenuapp.conf";
    snprintf(configPath, sizeof(configPath), "%s%s", HOME, configName);

	char *poweroffcmd = get_char_value_from_conf(configPath, "poweroffcmd");

	run_cmd(poweroffcmd);
}

/// runs the app when clicked on it
static void activate_item(GtkListView *self, guint position, gpointer user_data) {
	(void)self;
	(void)position;
	(void)user_data;

	selectedForRemove = gtk_single_selection_get_selected(p_dataOptions->selection);

	run_cmd(*execs[selectedForRemove]);

	gtk_popover_popdown(GTK_POPOVER(p_dataOptions->diomenu));
}

///******************** opening DioMenu when right click on panel ********************///
static void open_diomenu() {
	// getting diomenupaths.conf path
	char diomenuPathsConfPath[777];
	strcpy(diomenuPathsConfPath, HOME);
	strcat(diomenuPathsConfPath, "/.config/diopanel/diomenu/diomenupaths.conf");

	// populate names[] execs[] icons[]
	get_lines_from_conf(diomenuPathsConfPath);

	GtkWidget *window;
	window = gtk_window_new();

	int yOffset;
	int xOffset;

	gtk_popover_get_offset(GTK_POPOVER(p_dataOptions->pop), &yOffset, &xOffset);

	// getting face icon path
	char *faceIconPath = malloc(strlen(HOME) + strlen("/.face") + 1);

	strcpy(faceIconPath, HOME);
	strcat(faceIconPath, "/.face");

	GtkWidget *faceIcon;
	faceIcon = gtk_image_new_from_file(faceIconPath);
	gtk_image_set_icon_size(GTK_IMAGE(faceIcon), GTK_ICON_SIZE_LARGE);

	GtkWidget *emptyLabel;
	emptyLabel = gtk_label_new(NULL);

	// getting username
	char *username = getenv("USER");

	GtkWidget *userName;
	userName = gtk_label_new(username);

	// customizing the style, text size of the GtkEntry Widget
	PangoAttrList *attr_list = pango_attr_list_new();
	PangoAttribute *attr = pango_attr_size_new_absolute(20 * PANGO_SCALE);
	pango_attr_list_insert(attr_list, attr);
	PangoLayout *layout = gtk_widget_create_pango_layout(userName, NULL);
	pango_layout_set_attributes(layout, attr_list);

	gtk_label_set_attributes(GTK_LABEL(userName), attr_list);

	GtkWidget *panelSettings;
	panelSettings = gtk_button_new_from_icon_name("settings-app-symbolic");
	gtk_button_set_has_frame(GTK_BUTTON(panelSettings), FALSE);
	gtk_widget_set_focusable(panelSettings, FALSE);

	g_signal_connect_swapped(panelSettings, "clicked", G_CALLBACK(panel_settings), NULL);

	GtkWidget *emptyLabel2;
	emptyLabel2 = gtk_label_new("\t\t\t\t");

	GtkWidget *systemSettings;
	systemSettings = gtk_button_new_from_icon_name("system-settings-symbolic");
	gtk_button_set_has_frame(GTK_BUTTON(systemSettings), FALSE);
	gtk_widget_set_focusable(systemSettings, FALSE);

	g_signal_connect_swapped(systemSettings, "clicked", G_CALLBACK(sysettings_cb), NULL);

	GtkWidget *restartButton;
	restartButton = gtk_button_new_from_icon_name("system-restart-symbolic");
	gtk_button_set_has_frame(GTK_BUTTON(restartButton), FALSE);
	gtk_widget_set_focusable(restartButton, FALSE);

	g_signal_connect_swapped(restartButton, "clicked", G_CALLBACK(restart_cb), NULL);

	GtkWidget *powertButton;
	powertButton = gtk_button_new_from_icon_name("system-shutdown-symbolic");
	gtk_button_set_has_frame(GTK_BUTTON(powertButton), FALSE);
	gtk_widget_set_focusable(powertButton, FALSE);

	g_signal_connect_swapped(powertButton, "clicked", G_CALLBACK(poweroff_cb), NULL);

	// topbar box to pack icon, username and sesstion buttons
	GtkWidget *topBar;
	topBar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_prepend(GTK_BOX(topBar), faceIcon);
	gtk_box_append(GTK_BOX(topBar), emptyLabel);
	gtk_box_append(GTK_BOX(topBar), userName);
	gtk_box_append(GTK_BOX(topBar), panelSettings);
	gtk_box_append(GTK_BOX(topBar), emptyLabel2);
	gtk_box_append(GTK_BOX(topBar), systemSettings);
	gtk_box_append(GTK_BOX(topBar), restartButton);
	gtk_box_append(GTK_BOX(topBar), powertButton);

	GtkWidget *topBarFrame;
	topBarFrame = gtk_frame_new(NULL);
	gtk_frame_set_child(GTK_FRAME(topBarFrame), topBar);
	gtk_widget_set_size_request(topBarFrame, 0, 50);

	/**************************************** MENU LIST ****************************************/

	// turns all strings containing names into objects
	GtkStringList *myStringsNames;
	myStringsNames = gtk_string_list_new((const gchar * const *)names);

	p_dataOptions->stringsNames = myStringsNames;

	// turns all strings containing names into objects
	GtkStringList *myStringsExecs;
	myStringsExecs = gtk_string_list_new((const gchar * const *)execs);

	p_dataOptions->stringsExecs = myStringsExecs;

	// turns all strings containing names into objects
	GtkStringList *myStringsIcons;
	myStringsIcons = gtk_string_list_new((const gchar * const *)icons);

	p_dataOptions->stringsIcons = myStringsIcons;

	// creates a list model from the object of string objects
	GListModel *myGlistModel;
	myGlistModel = G_LIST_MODEL(myStringsNames);

	p_dataOptions->glistmodel = myGlistModel;

	// creats a list item factory to setup and bind the string object to the list view
	GtkListItemFactory *myGtkListItemFactory;
	myGtkListItemFactory = gtk_signal_list_item_factory_new();

	// callbacks (funtions) to run on signals that widgets emit
	g_signal_connect(myGtkListItemFactory, "setup", G_CALLBACK(setup_listitem), NULL);
	g_signal_connect(myGtkListItemFactory, "bind", G_CALLBACK(bind_listitem), NULL);

	// defining how selection of items behaves
	GtkSingleSelection *mySingleSelection;
	mySingleSelection = gtk_single_selection_new(G_LIST_MODEL(myGlistModel));
	gtk_single_selection_set_autoselect(mySingleSelection, FALSE);
	gtk_single_selection_set_can_unselect(mySingleSelection, TRUE);

	p_dataOptions->selection = mySingleSelection;

	GtkSelectionModel *mySelectionModel;
	mySelectionModel = GTK_SELECTION_MODEL(mySingleSelection);
	gtk_selection_model_unselect_all(mySelectionModel);
	gtk_selection_model_unselect_item(mySelectionModel, 0);

	// creating the list view to display the list items but it needs to be a child of a widget
	GtkWidget *myGtkListView;
	myGtkListView = gtk_list_view_new(mySelectionModel, myGtkListItemFactory);
	gtk_list_view_set_single_click_activate(GTK_LIST_VIEW(myGtkListView), TRUE);

	g_signal_connect(myGtkListView, "activate", G_CALLBACK(activate_item), NULL);

	// adds left click action for items in listview
	GtkGesture *click;
	click = gtk_gesture_click_new();

	GtkGestureSingle *singleclick;
	singleclick = GTK_GESTURE_SINGLE(click);

	gtk_gesture_single_set_button(singleclick, 3);

	GtkEventController *leftclick;
	leftclick = GTK_EVENT_CONTROLLER(click);
	gtk_event_controller_set_propagation_phase(leftclick, GTK_PHASE_CAPTURE);
	gtk_widget_add_controller(myGtkListView, leftclick);

    g_signal_connect_swapped(leftclick, "pressed", G_CALLBACK(left_click_menu_item), NULL);

	// adding the list view as the child
	GtkWidget *myScrolledWindow;
	myScrolledWindow = gtk_scrolled_window_new();
	gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(myScrolledWindow), true);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(myScrolledWindow), myGtkListView);
	gtk_scrolled_window_set_max_content_height(GTK_SCROLLED_WINDOW(myScrolledWindow), 500);

	// get width and height from config
	gchar configPath[3000];
	gchar *configName = "/.config/diopanel/diomenu/diomenuapp.conf";
    snprintf(configPath, sizeof(configPath), "%s%s", HOME, configName);

	gint height	= get_int_value_from_conf(configPath, "height");

	GtkWidget *listviewFrame;
	listviewFrame = gtk_frame_new(NULL);
	gtk_frame_set_child(GTK_FRAME(listviewFrame), myScrolledWindow);
	gtk_widget_set_size_request(listviewFrame, 0, height);

	/************************************* END OF MENU LIST *************************************/

	GtkWidget *boxAll;
	boxAll = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
	gtk_box_prepend(GTK_BOX(boxAll), topBarFrame);
	gtk_box_append(GTK_BOX(boxAll), listviewFrame);

	gint posx = get_int_value_from_conf(configPath, "posx");

	GtkWidget *diomenu;
	diomenu = gtk_popover_new();
	gtk_popover_set_child(GTK_POPOVER(diomenu), boxAll);
	gtk_popover_set_has_arrow(GTK_POPOVER(diomenu), FALSE);
	gtk_popover_set_offset(GTK_POPOVER(diomenu), yOffset + 210, posx);

	g_signal_connect_swapped(diomenu, "closed", G_CALLBACK(menu_closed), window);

	p_dataOptions->diomenu = diomenu;

	gtk_window_set_child(GTK_WINDOW(window), diomenu);

	gtk_popover_popup(GTK_POPOVER(diomenu));

	g_free(faceIconPath);
	g_object_unref(layout);
	pango_attr_list_unref(attr_list);
}
