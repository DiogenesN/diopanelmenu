#include <gtk/gtk.h>

#ifndef EXTERNVARS_H_
#define EXTERNVARS_H_

extern gchar 		indexOfApp[];
extern gchar 		*names[256][1];
extern gchar 		*execs[256][1];
extern gchar 		*icons[256][1];
extern gchar 		*paths[256][1];
extern gint			totalItems;
extern gint			path_error;
extern gint	 		strings_counter;
extern gint 		selectedForRemove;
extern gint			remove_clicked_counter;

#endif
