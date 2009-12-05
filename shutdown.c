/*
 * shutdown.c
 *
 * (C) 2001 Rafael V. Aroca <rafael@linuxqos.cjb.net> 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {

	if (argv[1] != NULL) {

		if (strcmp((char*)argv[1], "-r") == 0) {
			char *teste;
			FILE *output;

			printf("Rebooting...\n");
			output = popen("/sbin/shutdown now -r", "r");
			teste = (char*) fgetc(output);
			while ((int)teste != EOF) {
				printf("%c", teste);
				teste = (char*) fgetc(output);
			}
			pclose(output);
		} 
		
		if (strcmp(argv[1], "-h") == 0) {
			char *teste;
			FILE *output;

			printf("Halting...\n");
			output = popen("/sbin/shutdown now -h", "r");
			teste = (char*) fgetc(output);
			while ((int)teste != EOF) {
				printf("%c", teste);
				teste = (char*) fgetc(output);
			}
			pclose(output);
		} 

		if (!strcmp(argv[1], "-h") == 0 && !strcmp(argv[1], "-r") == 0) 
			printf("Nothing done.\n");
	} else printf("Nothing done.\n"); 
}

