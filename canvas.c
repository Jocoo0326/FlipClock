#include "canvas.h"
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

#define INITIAL_RESTORE_STATE_SIZE 8
#define MATRIX_SIZE 16
GLfloat temp_color[4];
float tmp_matrix[32] = {0};
float matrices[INITIAL_RESTORE_STATE_SIZE * MATRIX_SIZE] = {0};
int cur_matrix_index = 0;
float projection_matrix[MATRIX_SIZE] = {0};

const GLfloat box_coordinates[] = {
    0, 0, 0, 1, 1, 0, 1, 1,  // fill rectangle
    0, 0, 1, 1,              // draw line
    0, 0, 0, 1, 1, 1, 1, 0   // draw rectangle box
};
const GLint FILL_RECT_INDEX = 0;
const GLint FILL_RECT_COUNT = 4;
const GLint DRAW_LINE_INDEX = FILL_RECT_INDEX + FILL_RECT_COUNT;
const GLint DRAW_LINE_COUNT = 2;
const GLint DRAW_RECT_INDEX = DRAW_LINE_INDEX + DRAW_LINE_COUNT;
const GLint DRAW_RECT_COUNT = 4;

const GLchar* vertex_shader_source =
    "#version 330\n"
    "uniform mat4 vMatrix;\n"
    "layout(location = 0) in vec2 vPos;\n"
    "void main() {\n"
    "  vec4 pos = vec4(vPos, 0.0, 1.0);\n"
    "  gl_Position = vMatrix * pos;\n"
    "}\n";

const GLchar* frag_shader_source =
    "#version 330\n"
    "uniform vec4 fColor;\n"
    "void main() {\n"
    "  gl_FragColor = fColor;\n"
    "}";
void check_error() {
	int error = glGetError();
	if (error) {
		fprintf(stderr, "GL error %d\n", error);
		exit(-1);
	}
}

const char* shader_type_as_cstr(GLenum shader)
{
  switch (shader) {
  case GL_VERTEX_SHADER: {
    return "GL_VERTEX_SHADER";
  }
  case GL_FRAGMENT_SHADER: {
    return "GL_FRAGMENT_SHADER";
  }
  default:
    return "(unknown)";
  }
}

int compile_shader_source(const GLchar* source, GLenum shader_type,
                          GLuint* shader)
{
  *shader = glCreateShader(shader_type);
  glShaderSource(*shader, 1, &source, NULL);
  glCompileShader(*shader);

  GLint status;
  glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetShaderInfoLog(*shader, sizeof(message), &log_length, message);
    fprintf(stderr, "Error: could not compile %s\n",
            shader_type_as_cstr(shader_type));
    fprintf(stderr, "%.*s\n", log_length, message);
    return 0;
  }
  return 1;
}

int link_program(GLuint vert_shader, GLuint frag_shader, GLuint* program)
{
  *program = glCreateProgram();

  glAttachShader(*program, vert_shader);
  glAttachShader(*program, frag_shader);
  glLinkProgram(*program);

  GLint linked = 0;
  glGetProgramiv(*program, GL_LINK_STATUS, &linked);
  if (linked != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetProgramInfoLog(*program, sizeof(message), &log_length, message);
    fprintf(stderr, "Error: program linking %.*s\n", log_length, message);
    return 0;
  }
  return 1;
}

GLint upload_data(GLvoid* data, GLsizeiptr size)
{
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	check_error();
  return vbo;
}

void canvas_init(Canvas* canvas)
{
	matrix_set_identity(tmp_matrix, 0);
	matrix_set_identity(matrices, cur_matrix_index);

  GLuint vert_shader;
  GLuint frag_shader;
  if (!compile_shader_source(vertex_shader_source, GL_VERTEX_SHADER,
                             &vert_shader)) {
    exit(-1);
  }
  if (!compile_shader_source(frag_shader_source, GL_FRAGMENT_SHADER,
                             &frag_shader)) {
    exit(-1);
  }
  if (!link_program(vert_shader, frag_shader, &canvas->program)) {
    exit(-1);
  }
	glUseProgram(canvas->program);
  canvas->vMatrix_uniform =
      glGetUniformLocation(canvas->program, "vMatrix");
  canvas->fColor_uniform = glGetUniformLocation(canvas->program, "fColor");

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	canvas->vbo =
      upload_data((GLvoid*)box_coordinates, sizeof(box_coordinates));
}

void get_color(int color)
{
  GLfloat a = ((color >> 24) & 0xFF) / 255;
  GLfloat r = ((color >> 16) & 0xFF) / 255;
  GLfloat g = ((color >> 8) & 0xFF) / 255;
  GLfloat b = (color & 0xFF) / 255;
  temp_color[0] = r;
  temp_color[1] = g;
  temp_color[2] = b;
  temp_color[3] = a;
}

void prepare_draw(Canvas* canvas, GLint offset)
{
  glBindBuffer(GL_ARRAY_BUFFER, canvas->vbo);
	check_error();
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void *)(offset * 2 * sizeof(GLfloat)));
	check_error();
  glBindBuffer(GL_ARRAY_BUFFER, 0);
	check_error();
  get_color(0xFF00FFFF);
  glUniform4fv(canvas->fColor_uniform, 1, temp_color);
}

void print_matrix(float *m, int moffset)
{
	printf("___\n");
	for (int i = 0; i < 4; ++i) {
    int mi = moffset + i;
		printf("| %.5f %.5f %.5f %.5f\n", m[mi], m[mi + 4], m[mi + 8], m[mi + 12]);
	}
	printf("---\n");
}

void canvas_set_size(Canvas *canvas, int width, int height)
{
	canvas->width = width;
	canvas->height = height;
	glViewport(0, 0, width, height);
	matrix_set_identity(matrices, cur_matrix_index);
	matrix_ortho_m(projection_matrix, 0, 0, width, 0, height, -1, 1);

	matrix_translate(matrices, cur_matrix_index, 0, height, 0);
	matrix_scale(matrices, cur_matrix_index, 1, -1, 1);
}

void set_matrix(Canvas *canvas, GLfloat x, GLfloat y, GLfloat width, GLfloat height)
{
	matrix_translate_m(tmp_matrix, 0, matrices, cur_matrix_index, x, y, 0.0);
	matrix_scale(tmp_matrix, 0, width, height, 1.0);
	matrix_multiply_m(tmp_matrix, MATRIX_SIZE, projection_matrix, 0, tmp_matrix, 0);
	glUniformMatrix4fv(canvas->vMatrix_uniform, 1, GL_FALSE, tmp_matrix + MATRIX_SIZE);
}

void draw(GLint type, GLint count, GLfloat x, GLfloat y, GLfloat width, GLfloat height)
{
  glEnableVertexAttribArray(0);
	check_error();
  glDrawArrays(type, 0, count);
	check_error();
	glDisableVertexAttribArray(0);
	check_error();
}

void canvas_draw_rect(Canvas* canvas, float x, float y, float w, float h,
                      Paint* paint)
{
  prepare_draw(canvas, FILL_RECT_INDEX);
	set_matrix(canvas, x, y, w, h);
	draw(GL_TRIANGLE_STRIP, FILL_RECT_COUNT, x, y, w, h);
}
