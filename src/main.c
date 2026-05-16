#include "color.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>

bool isRunning = true;

int windowWidth, windowHeight;
SDL_Window *window;
SDL_Renderer *renderer;
color32_t *colorBuffer;
SDL_Texture *colorBufferTexture;

extern exitCode_e initWindow();
extern void renderColorBuffer();
extern void clearColorBuffer(color32_t color);
extern void destroyWindow();

void processInput(void) {
	SDL_Event event;
	SDL_PollEvent(&event);
	switch(event.type) {
		case SDL_EVENT_QUIT:
			isRunning = false;
			break;
		case SDL_EVENT_KEY_DOWN:
			if (event.key.key == SDLK_ESCAPE) 
				isRunning = false;
			break;
	}
}

static void update() {

}

static void render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	color32_t clearColor = {.rgba = 0xFF00FFFF};
	clearColorBuffer(clearColor);

	renderColorBuffer();
	SDL_RenderPresent(renderer);
}

int main() {
	initWindow();
	while (isRunning) {
		processInput();
		update();
		render();
	}
	destroyWindow();
	return EXIT_SUCCESS;
}
