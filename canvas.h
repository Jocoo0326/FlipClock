#ifndef CANVAS_H
#define CANVAS_H
#include "paint.h"
#include <GL/glew.h>

typedef struct {
	int width;
	int height;
	GLuint program;
	GLint vMatrix_uniform;
	GLint vPos_uniform;
	GLint fColor_uniform;
	GLint vbo;
} Canvas;

void canvas_init(Canvas *canvas);

void canvas_set_size(Canvas *canvas, int width, int height);

void canvas_draw_rect(Canvas *canvas, float l, float t, float r, float b, Paint *paint);

#endif /* CANVAS_H */
