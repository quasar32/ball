#include <glad/gl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ball.h"

#define N_VERTS 256
#define MAX_LOG 256

static GLuint vao;
static GLuint vbo;
static GLuint prog;
static GLint trans_loc;
static GLint scale_loc;

#define VERTS_SIZE (N_VERTS * sizeof(struct vec2))

void die(const char *fmt, ...) {
	va_list ap;	
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(1);
}

static void init_vertices(void) {
	struct vec2 *vertices, *v;
	float theta = 0.0F;
	vertices = malloc(VERTS_SIZE);
	if (!vertices)
		die("malloc: out of memory\n");
	for (v = vertices; v < vertices + N_VERTS; v++) {
		v->x = cosf(theta);
		v->y = sinf(theta);
		theta += 2.0F * M_PI / (N_VERTS - 1); 
	}
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, VERTS_SIZE, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct vec2), NULL);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	free(vertices);
	vertices = NULL;
}

static const char vs_src[] = 
	"#version 330 core\n"
	"layout(location = 0) in vec2 pos;"
	"uniform vec2 trans;"
	"uniform vec2 scale;"
	"void main() {"
		"gl_Position = vec4(trans + pos * scale, 0.0F, 1.0F);"
	"}";

static const char fs_src[] = 
	"#version 330 core\n"
	"out vec4 color;"
	"void main() {"
		"color = vec4(1.0F, 1.0F, 1.0F, 1.0F);"
	"}";

#define N_GLSL 2

struct shader_desc {
	GLenum type;
	const char *src;
};

static struct shader_desc descs[N_GLSL] = {
	{GL_VERTEX_SHADER, vs_src},
	{GL_FRAGMENT_SHADER, fs_src},
};

static void init_prog(void) {
	GLuint shaders[N_GLSL];
	char log[MAX_LOG];
	int success, i;
	for (i = 0; i < N_GLSL; i++) {
		shaders[i] = glCreateShader(descs[i].type);
		glShaderSource(shaders[i], 1, &descs[i].src, NULL);
		glCompileShader(shaders[i]);
		glGetShaderiv(shaders[i], GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shaders[i], MAX_LOG, NULL, log);
			die("shader: %s\n", log);
		}
	}
	prog = glCreateProgram();
	for (i = 0; i < N_GLSL; i++)
		glAttachShader(prog, shaders[i]);
	glLinkProgram(prog);
	for (i = 0; i < N_GLSL; i++) {
		glDetachShader(prog, shaders[i]);
		glDeleteShader(shaders[i]);
	}
	glGetProgramiv(prog, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(prog, MAX_LOG, NULL, log);
		die("program: %s\n", log);
	}
	trans_loc = glGetUniformLocation(prog, "trans");
	scale_loc = glGetUniformLocation(prog, "scale");
}

void init_draw(void) {
	init_vertices();
	init_prog();
}

void draw(void) {
	int i;
	glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
	glClear(GL_COLOR_BUFFER_BIT);
	for (i = 0; i < N_BALLS; i++) {
		glUseProgram(prog);
		glBindVertexArray(vao);
		glUniform2f(trans_loc, balls[i].pos.x, balls[i].pos.y);
		glUniform2f(scale_loc, balls[i].radius, balls[i].radius);
		glDrawArrays(GL_TRIANGLE_FAN, 0, N_VERTS);
	}
}

