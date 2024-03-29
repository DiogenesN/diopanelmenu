#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char *HOME = "HOME";

int get_int_value_from_conf(char *fullPathToConf, char *keyToGetValueFrom) {
	char buffer[777];
	char value[777];

	FILE *pathToConfig = fopen(fullPathToConf, "r");

	if (pathToConfig == NULL) {
		perror("Error opening file");
		return 1;
	}

	while (fgets(buffer, sizeof(buffer), pathToConfig) != NULL) {
		char *pos = strchr(buffer, '=');
		if (strstr(buffer, keyToGetValueFrom) != NULL && strstr(buffer, "#") == NULL) {
			if (pos != NULL) {
				strcpy(value, pos + 1);
				value[strlen(value) - 1] = '\0';
			}
		}
	}

	fclose(pathToConfig);

	int valueToNumber = atoi(value);
	return valueToNumber;
}

double get_double_value_from_conf(char *fullPathToConf, char *keyToGetValueFrom) {
	char buffer[777];
	char value[777];

	FILE *pathToConfig = fopen(fullPathToConf, "r");

	if (pathToConfig == NULL) {
		perror("Error opening file");
		return 1;
	}

	while (fgets(buffer, sizeof(buffer), pathToConfig) != NULL) {
		char *pos = strchr(buffer, '=');
		if (strstr(buffer, keyToGetValueFrom) != NULL && strstr(buffer, "#") == NULL) {
			if (pos != NULL) {
				strcpy(value, pos + 1);
				value[strlen(value) - 1] = '\0';
			}
		}
	}

	fclose(pathToConfig);

	double valueToNumber = atof(value);
	return valueToNumber;
}

char *get_char_value_from_conf(char *fullPathToConf, char *keyToGetValueFrom) {
	char buffer[777];
	char value[777];

	FILE *pathToConfig = fopen(fullPathToConf, "r");

	while (fgets(buffer, sizeof(buffer), pathToConfig) != NULL) {
		char *pos = strchr(buffer, '=');
		if (strstr(buffer, keyToGetValueFrom) != NULL && strstr(buffer, "#") == NULL) {
			if (pos != NULL) {
				strcpy(value, pos + 1);
				value[strlen(value) - 1] = '\0';
			}
		}
	}

	fclose(pathToConfig);

	return strdup(value);
}
