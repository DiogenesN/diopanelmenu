#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "externvars.h"
#include "getlinesfromconf.h"
#include "populatenameswithdata.h"

// processing directories stated in config file
void get_lines_from_conf(char *fullPathWithName) {
	FILE *file = fopen(fullPathWithName, "r");

	if (file == NULL) {
		perror("Error opening the file");
		return;
	}

	char line[256];     // Buffer for reading lines

	// Read lines from the file and store them in the strings array
	while (fgets(line, sizeof(line), file) != NULL) {
		// Remove the newline character, if present
		char *newline = strchr(line, '\n');
		if (newline != NULL) {
			*newline = '\0';
		}
		int  counter_index = 0;
		char *strings[256];

		// Allocate memory for the string and copy it
		strings[counter_index] = strdup(line);

		// process directories found in config file
		//printf("path lines in conf: %s\n", strings[counter_index]);
		process_desktop_files(strings[counter_index]);
		counter_index++;
		strings_counter = strings_counter + 1;

		if (counter_index >= 30) {
			printf("30 items limit reached\n");
			break;  // Stop reading after 30 lines
		}

		// Free the allocated memory for each line
		for (int i = 0; i < counter_index; i++) {
			free(strings[i]);
		}
	}
	fclose(file);
}
