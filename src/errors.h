#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdlib.h>
typedef enum {
	PINEDA_SUCCESS,
	SDL_INIT_ERROR,
	PINEDA_ERROR_UNKNOWN
} exitCode_e;

static inline void handleError(exitCode_e error) {
	switch (error) {
		case PINEDA_SUCCESS:
			break;
		case SDL_INIT_ERROR:
			fprintf(stderr, "FATAL ERROR: SDL Initializatioin failed\n");
			exit(error);
			break;
		case PINEDA_ERROR_UNKNOWN:
			fprintf(stderr, "FATAL ERROR: Unknown\n");
			exit(error);
			break;
	}
}
#endif
