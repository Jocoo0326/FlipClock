#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include "canvas.h"
#include "paint.h"

#define SDL_WINDOW_WIDTH 800
#define SDL_WINDOW_HEIGHT 600
void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, const GLchar* message,
                     const void* userParam)
{
  (void)source;
  (void)id;
  (void)length;
  (void)userParam;
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
          severity, message);
}

int main(int argc, char* argv[])
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window* window = SDL_CreateWindow(
      "flipClock", 0, 0, SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT,
      SDL_WINDOW_OPENGL);

  {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    int major, minor;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    printf("GL Version %d.%d\n", major, minor);
  }

  SDL_GL_CreateContext(window);

  if (GLEW_OK != glewInit()) {
		fprintf(stderr, "could not init glew\n");
		exit(-1);
	}

  if (GLEW_ARB_debug_output) {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
  }

  int quit = 0;

	Canvas canvas = {0};
	canvas_init(&canvas);
	canvas_set_size(&canvas, SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT);
	Paint paint = {
		.line_width = 1,
		.color = 0xFFFF0000
	};
  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: {
        quit = 1;
      } break;

      case SDL_KEYUP: {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          quit = 1;
        }
      } break;
      }
    }

    glClearColor(0.0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		canvas_draw_rect(&canvas, 10, 10, 150, 150, &paint);
    SDL_GL_SwapWindow(window);
  }

  return 0;
}
