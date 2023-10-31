#include <glib.h>

gchar 		indexOfApp[256];
gchar 		*names[256][1];
gchar 		*execs[256][1];
gchar 		*icons[256][1];
gchar 		*paths[256][1];
gint		totalItems = 0;
gint		path_error = 1;
gint 		strings_counter = 0;
gint		selectedForRemove = 0;
gint		remove_clicked_counter = 0;
