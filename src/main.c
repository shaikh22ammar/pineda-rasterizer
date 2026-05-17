#include "color.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include "types.h"

bool isRunning = true;

int windowWidth = 800, windowHeight = 800;
SDL_Window *window;
SDL_Renderer *renderer;
color32_t *colorBuffer;
SDL_Texture *colorBufferTexture;

extern exitCode_e initWindow();
extern void renderColorBuffer();
extern void clearColorBuffer(color32_t color);
extern void destroyWindow();
extern void drawPixel(int i, int j);
extern void rasterizeTriangle(fixedPoint2d_t p, fixedPoint2d_t q, fixedPoint2d_t r);

constexpr scalar2d_t p = {.x=0.75,.y=0.75};
constexpr scalar2d_t r = {.x=0.50,.y=0.25};
constexpr scalar2d_t q = {.x=0.25,.y=0.75};

static void processInput(void) {
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

	fixedPoint2d_t pf = {.x = (int) (p.x*windowWidth), .y = (int) (p.y*windowHeight)};
	fixedPoint2d_t qf = {.x = (int) (q.x*windowWidth), .y = (int) (q.y*windowHeight)};
	fixedPoint2d_t rf = {.x = (int) (r.x*windowWidth), .y = (int) (r.y*windowHeight)};

	rasterizeTriangle(pf, qf, rf);

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
