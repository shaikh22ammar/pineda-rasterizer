#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include "errors.h"
#include "color.h"
#include <assert.h>

#include <arm_neon.h>

extern int windowWidth, windowHeight;
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern color32_t *colorBuffer;
extern SDL_Texture *colorBufferTexture;

exitCode_e initWindow () {
	bool allOkay = true;
// SDL Initiialization
	SDL_InitFlags flags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
	allOkay = !!SDL_Init(flags);
	if (!allOkay) goto cleanup;
// Window creation	
	SDL_PropertiesID props = SDL_CreateProperties();
	allOkay = !!props;
	if (!allOkay) goto cleanup;

	SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Pineda rasterizer");
	//SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, true);
	//SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);
	//SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN, true);
	if (windowWidth && windowHeight) {
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, windowWidth);
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, windowHeight);
	} else {
		SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, true);
		SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN, true);
	}

	window = SDL_CreateWindowWithProperties(props);
	allOkay = !!window;
	if (!allOkay) goto cleanup;
// Renderer creation
	renderer = SDL_CreateRenderer(window, NULL);
	allOkay = !!renderer;
	SDL_Rect rect;
	SDL_GetWindowSafeArea(window, &rect);
	SDL_GetCurrentRenderOutputSize(renderer, &windowWidth, &windowHeight);
 	if (!allOkay) goto cleanup;
// Colorbuffer
	assert(sizeof(color32_t)==4);
	colorBuffer = malloc(sizeof(color32_t)*windowWidth*windowHeight);
	allOkay = !!colorBuffer;
	if (!allOkay) goto cleanup;

	colorBufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, windowWidth, windowHeight);
	allOkay = !!colorBufferTexture;
	if (!allOkay) goto cleanup;
cleanup:
	exitCode_e error = allOkay ? PINEDA_SUCCESS : SDL_INIT_ERROR;
	handleError(error);
	return error;
}

void renderColorBuffer() {
	SDL_UpdateTexture(colorBufferTexture, NULL, colorBuffer, sizeof(color32_t)*windowWidth);
	SDL_RenderTexture(renderer, colorBufferTexture, NULL, NULL);
}

void drawPixel(int i, int j) {
	color32_t color = {.rgba = 0xFFFFFFFF}; 
	if (i < 0 || i >= windowHeight || j < 0 || j >= windowWidth) { 
		handleError(PINEDA_WARNING_COLOR_BUFFER_OUT_OF_BOUNDS_ACCESS); 
		return;
	}
	colorBuffer[i*windowWidth + j] = color;
}

void drawPixelsVectorized(int i, int j, int32x4_t mask) {
	if (i < 0 || i >= windowHeight || j < 0 || j >= windowWidth) { 
		handleError(PINEDA_WARNING_COLOR_BUFFER_OUT_OF_BOUNDS_ACCESS); 
		return;
	}

	int mask0 = vgetq_lane_s32(mask, 0);
	int mask1 = vgetq_lane_s32(mask, 1);
	int mask2 = vgetq_lane_s32(mask, 2);
	int mask3 = vgetq_lane_s32(mask, 3);
	
	if (mask0) drawPixel(i, j); 
	if (mask1) drawPixel(i, j+1); 
	if (mask2) drawPixel(i, j+2); 
	if (mask3) drawPixel(i, j+3); 
}

void clearColorBuffer(color32_t color) {
	for (int i = 0; i < windowHeight; i++) {
		for (int j = 0 ; j < windowWidth; j++) {
			colorBuffer[i*windowWidth + j].rgba = color.rgba;
		}
	}
}

void destroyWindow(void) {
	SDL_DestroyTexture(colorBufferTexture);
	free(colorBuffer);
	colorBuffer = nullptr;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
