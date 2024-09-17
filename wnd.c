#include <SDL2/SDL.h>
#include <glad/gl.h>
#include "draw.h"
#include "ball.h"

int main(int argc, char **argv) {
	SDL_Window *wnd;
	int w0, h0, w1, h1;
	Uint64 t0, t1;
	Uint64 freq;
	float dt;
	float acc;
	if (SDL_Init(SDL_INIT_EVERYTHING))
		die("SDL_Init: %s\n", SDL_GetError());
	if (atexit(SDL_Quit))
		die("atexit: SDL_Quit\n");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 
			SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	wnd = SDL_CreateWindow("Billiards", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, 640, 480, 
			SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
	if (!wnd)
		die("SDL_CreateWindow: %s\n", SDL_GetError());
	if (!SDL_GL_CreateContext(wnd))
		die("SDL_GL_CreateContext: %s\n", SDL_GetError());
	gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress);
	SDL_GL_SetSwapInterval(1);
	init_balls();
	init_draw();
	freq = SDL_GetPerformanceFrequency();
	SDL_ShowWindow(wnd);
	w0 = h0 = 0;
	t0 = SDL_GetPerformanceCounter();
	while (!SDL_QuitRequested()) {
		t1 = SDL_GetPerformanceCounter();
		dt = (t1 - t0) / (float) freq;
		t0 = t1;
		SDL_GL_GetDrawableSize(wnd, &w1, &h1);
		if (w0 != w1 || h0 != h1)
			glViewport(0, 0, w1, h1);
		w0 = w1;
		h0 = h1;
		acc += dt;
		if (acc >= 0.05F) {
			simulate();
			acc -= 0.05F;
		}
		draw();
		SDL_GL_SwapWindow(wnd);
	}
	return 1;
}
