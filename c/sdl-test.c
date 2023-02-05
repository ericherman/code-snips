/* sdl-test, Eric Herman <eric@freesa.org>  */
/* SPDX-License-Identifier: Zlib */
/* https://github.com/libsdl-org/SDL/blob/main/LICENSE.txt */
/*
   gcc -O0 -g -Wextra -Wall -Wpedantic `sdl2-config --cflags` \
	sdl-test.c -o sdl-test `sdl2-config --libs` && \
   valgrind --leak-check=full ./sdl-test
*/

#include <SDL.h>
#include <unistd.h>		/* usleep */

static void render_pixels(SDL_Renderer *renderer, SDL_Texture *texture,
			  Uint32 *pixels, size_t pitch)
{
	SDL_RenderClear(renderer);

	const SDL_Rect *rect = NULL;
	const void *cv_pixels = (const void *)pixels;
	int i_pitch = (int)pitch;
	if (SDL_UpdateTexture(texture, rect, cv_pixels, i_pitch)) {
		fprintf(stderr, "SDL_UpdateTexture: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	const SDL_Rect *srcrect = NULL;
	const SDL_Rect *dstrect = NULL;
	SDL_RenderCopy(renderer, texture, srcrect, dstrect);

	/* valgrind: "bytes are definitely lost" */
	SDL_RenderPresent(renderer);
}

int main(int argc, char **argv)
{
	int max_count = argc > 1 ? atoi(argv[1]) : 255;

	Uint32 flags = SDL_INIT_VIDEO;
	if (SDL_Init(flags) != 0) {
		fprintf(stderr, "SDL_Init(%u): %s\n", flags, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	const char *title = argv[0];
	int x = SDL_WINDOWPOS_UNDEFINED;
	int y = SDL_WINDOWPOS_UNDEFINED;
	int win_x = 640;
	int win_y = 480;
	flags = SDL_WINDOW_RESIZABLE;
	SDL_Window *window = SDL_CreateWindow(title, x, y, win_x, win_y, flags);
	if (!window) {
		fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	const int r_idx = -1;	// first renderer
	flags = 0;
	SDL_Renderer *renderer = SDL_CreateRenderer(window, r_idx, flags);
	if (!renderer) {
		fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	int height = 0;
	int width = 0;
	SDL_GetWindowSize(window, &width, &height);
	SDL_Texture *texture = SDL_CreateTexture(renderer,
						 SDL_PIXELFORMAT_ARGB8888,
						 SDL_TEXTUREACCESS_STREAMING,
						 width, height);
	if (!texture) {
		fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	/* pitch is bytes in a row of pixel data, including padding */
	Uint32 pitch = sizeof(Uint32) * width;
	size_t pixels_len = height * width;
	Uint32 *pixels = calloc(pixels_len, sizeof(Uint32));
	if (!pixels) {
		fprintf(stderr, "calloc(%zu, sizeof(Uint32))\n", pixels_len);
		exit(EXIT_FAILURE);
	}
	for (size_t i = 0; i < pixels_len; ++i) {
		pixels[i] = 0x0000FF;
	}
	render_pixels(renderer, texture, pixels, pitch);

	unsigned int wanted_fps = 60;
	unsigned int usecs_per_second = (1000 * 1000);
	unsigned int usecs_between_frames = usecs_per_second / wanted_fps;

	SDL_Event event;
	int shutdown = 0;
	int print_count = 0;
	while (!shutdown && print_count < max_count) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				shutdown = 1;
				break;
			}
		}
		if (shutdown) {
			break;
		}

		usleep(usecs_between_frames);

		for (size_t i = 0; i < pixels_len; ++i) {
			pixels[i] = (0x00FFFFFF & (pixels[i] + 0x010000));
		}
		render_pixels(renderer, texture, pixels, pitch);
		++print_count;
	}

	free(pixels);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
