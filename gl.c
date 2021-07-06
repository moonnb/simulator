#include "gl.h"
#include "os.h"

#include <stdio.h>
#include <stdlib.h>

GLProcs gl;

void gl_get_procs(GLProcFn get_proc_address) {
#define get_proc(lower, upper) gl.lower = (PFNGL##upper##PROC)get_proc_address("gl" #lower);
	gl_for_each_proc(get_proc)
}

static GLuint shader_compile(char const *filename, GLenum type) {
	size_t file_size = fs_file_size(filename);
	char *code = memory_allocate(char, file_size);
	FILE *fp = fopen(filename, "rb");
	GLuint shader = 0;
	if (fp) {
		fread(code, 1, file_size, fp);
		fclose(fp);
		
		shader = gl.CreateShader(type);

		// prepend this before every shader
		char const *header = 
			"#version 130\n"
			"#define PI 3.14159265\n"
			"#line 1\n";
		char const *sources[] = {
			header,
			code
		};
		GLint lengths[] = {
			-1,
			(GLint)file_size
		};
		
		gl.ShaderSource(shader, 2, sources, lengths);
		gl.CompileShader(shader);
		GLint status = 0;
		gl.GetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (!status) {
			char log[1024] = {0};
			gl.GetShaderInfoLog(shader, sizeof log - 1, NULL, log);
			debug_print("Error compiling %s:\n%s\n", filename, log);
			gl.DeleteShader(shader);
			shader = 0;
		}


	} else {
		debug_print("Shader file not found: %s\n", filename);
	}
	free(code);
	return shader;
}

GLProgram gl_program_new(const char *vshader_filename, const char *fshader_filename) {
	GLProgram program = {0};
	
	GLuint vshader = shader_compile(vshader_filename, GL_VERTEX_SHADER);
	GLuint fshader = shader_compile(fshader_filename, GL_FRAGMENT_SHADER);

	if (vshader && fshader) {
		GLuint id = gl.CreateProgram();

		gl.AttachShader(id, vshader);
		gl.AttachShader(id, fshader);
		gl.LinkProgram(id);

		GLint status = 0;
		gl.GetProgramiv(id, GL_LINK_STATUS, &status);
		if (status) {
			program.id = id;
		} else {
			char log[1024] = {0};
			gl.GetProgramInfoLog(id, sizeof log - 1, NULL, log);
			debug_print("Error linking %s-%s:\n%s\n", vshader_filename, fshader_filename, log);
			gl.DeleteProgram(id);
		}
	}

	if (vshader) gl.DeleteShader(vshader);
	if (fshader) gl.DeleteShader(fshader);

	return program;
}

#if DEBUG
GLuint gl_program_in_use;
#endif
void gl_program_use(GLProgram program) {
#if DEBUG
	gl_program_in_use = program.id;
#endif
	gl.UseProgram(program.id);
}

void gl_program_delete(GLProgram *program) {
	gl.DeleteProgram(program->id);
	program->id = 0;
}

void gl_program_uniform1f(GLint loc, float v) {
	gl.Uniform1f(loc, v);
}
void gl_program_uniform2f(GLint loc, vec2 v) {
	gl.Uniform2f(loc, v.x, v.y);
}
void gl_program_uniform3f(GLint loc, vec3 v) {
	gl.Uniform3f(loc, v.x, v.y, v.z);
}
void gl_program_uniform4f(GLint loc, vec4 v) {
	gl.Uniform4f(loc, v.x, v.y, v.z, v.w);
}
void gl_program_uniform1i(GLint loc, int v) {
	gl.Uniform1i(loc, v);
}
void gl_program_uniform2i(GLint loc, vec2i v) {
	gl.Uniform2i(loc, v.x, v.y);
}
void gl_program_uniform3i(GLint loc, vec3i v) {
	gl.Uniform3i(loc, v.x, v.y, v.z);
}
void gl_program_uniform4i(GLint loc, vec4i v) {
	gl.Uniform4i(loc, v.x, v.y, v.z, v.w);
}

// how many vbos/vaos/etc to generate at a time
#define BATCH_SIZE 100

static GLuint buffers[BATCH_SIZE], buffers_used = 0;
static GLuint arrays[BATCH_SIZE], arrays_used = 0;

// to save time, generate vbos/vaos in batches
static GLuint buffer_new() {
	if (buffers_used == 0)
		gl.GenBuffers(BATCH_SIZE, buffers);
	GLuint id = buffers[buffers_used++];
	if (buffers_used >= BATCH_SIZE)
		buffers_used = 0;
	return id;
}

static void buffer_delete(GLuint buffer) {
	// ive found that glDeleteBuffers is much faster than glGenBuffers, so batching isn't necessary,
	// and saves gpu memory
	gl.DeleteBuffers(1, &buffer);
}

void gl_quit(void) {
	// delete unused buffers and arrays
	if (buffers_used)
		gl.DeleteBuffers(BATCH_SIZE - (GLint)buffers_used, buffers);
	if (arrays_used)
		gl.DeleteVertexArrays(BATCH_SIZE - (GLint)arrays_used, arrays);
}

static GLuint array_new() {
	if (arrays_used == 0)
		gl.GenVertexArrays(BATCH_SIZE, arrays);
	GLuint id = arrays[arrays_used++];
	if (arrays_used >= BATCH_SIZE)
		arrays_used = 0;
	return id;
}

static void array_delete(GLuint array) {
	gl.DeleteVertexArrays(1, &array);
}

GLVBO gl_vbo_new_with_type_size(size_t size) {
	assert(size < GLUINT_MAX);

	GLVBO vbo = {0};
	vbo.id = buffer_new();
	vbo.type_size = (GLuint)size;
	return vbo;
}


static void vbo_set_data(GLVBO *vbo, const void *data, size_t count, GLenum usage) {
	assert(count < GLINT_MAX / vbo->type_size); // check for overflow
	gl.BindBuffer(GL_ARRAY_BUFFER, vbo->id);
	gl.BufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(vbo->type_size * count),
		data, usage);
	vbo->count = (GLuint)count;
}

void gl_vbo_set_static_data(GLVBO *vbo, const void *data, size_t count) {
	vbo_set_data(vbo, data, count, GL_STATIC_DRAW);
}

void gl_vbo_set_stream_data(GLVBO *vbo, const void *data, size_t count) {
	vbo_set_data(vbo, data, count, GL_STREAM_DRAW);
}

void gl_vbo_delete(GLVBO *vbo) {
	buffer_delete(vbo->id);
	vbo->id = 0;
	vbo->count = 0;
	vbo->type_size = 0;
}

GLIBO gl_ibo_new(const GLuint *indices, size_t n) {
	GLIBO ibo = {0};
	assert(n < GLINT_MAX);
	ibo.count = (GLuint)n;
	ibo.id = buffer_new();
	gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.id);
	gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(n * sizeof *indices), indices, GL_STATIC_DRAW);
	return ibo;
}

void gl_ibo_delete(GLIBO *ibo) {
	buffer_delete(ibo->id);
	ibo->id = 0;
	ibo->count = 0;
}

GLVAO gl_vao_new(GLProgram program) {
	GLVAO vao = {0};
	assert(program.id);
	vao.program_id = program.id;
	vao.id = array_new();
	return vao;
}

void gl_vao_add_data_with_offset(GLVAO *vao, GLVBO vbo, const char *attr_name,
	size_t type_size, size_t member_offset, int n_elements, GLenum element_kind) {
	assert(type_size == vbo.type_size);
	assert(vao->count == 0 || vao->count == vbo.count);
	vao->count = vbo.count;
	gl.BindVertexArray(vao->id);
	gl.BindBuffer(GL_ARRAY_BUFFER, vbo.id);
	GLint location = gl.GetAttribLocation(vao->program_id, attr_name);
	if (location >= 0) {
		switch (element_kind) {
		case GL_FLOAT:
			gl.VertexAttribPointer((GLuint)location, n_elements, GL_FLOAT, 0, (GLsizei)type_size,
				(const GLvoid *)member_offset);
			break;
		case GL_INT:
			gl.VertexAttribIPointer((GLuint)location, n_elements, GL_INT, (GLsizei)type_size,
				(const GLvoid *)member_offset);
			break;
		}
		gl.EnableVertexAttribArray((GLuint)location);
	} else {
		debug_print("Couldn't find vertex attribute: %s\n", attr_name);
	}
}

static void gl_vao_render_with_mode(GLVAO vao, GLIBO const *ibo, GLenum mode) {
	gl_check_program_in_use(vao.program_id);
	gl.BindVertexArray(vao.id);
	if (ibo) {
		gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->id);
		gl.DrawElements(mode, (GLsizei)ibo->count, GL_UNSIGNED_INT, NULL);
	} else {
		gl.DrawArrays(mode, 0, (GLsizei)vao.count);
	}
}

void gl_vao_render(GLVAO vao, GLIBO const *ibo) {
	gl_vao_render_with_mode(vao, ibo, GL_TRIANGLES);
}

void gl_vao_render_lines(GLVAO vao, GLIBO const *ibo) {
	gl_vao_render_with_mode(vao, ibo, GL_LINES);
}

void gl_vao_clear(GLVAO *vao) {
	vao->count = 0;
}

void gl_vao_delete(GLVAO *vao) {
	array_delete(vao->id);
	vao->id = 0;
	vao->count = 0;
	vao->program_id = 0;
}
