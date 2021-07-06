#ifndef GL_H_
#define GL_H_

#include "lib/glcorearb.h"
#include <stddef.h>
#include "mmath.h"
#include "core.h"

#define GLUBYTE_MAX 0xFFu
#define GLUSHORT_MAX 0xFFFFu
#define GLUINT_MAX 0xFFFFFFFFu
#define GLBYTE_MAX 0x7F
#define GLSHORT_MAX 0x7FFF
#define GLINT_MAX 0x7FFFFFFF

// so we dont have to type out all the procs a bunch
#define gl_for_each_proc(f) \
	f(ClearColor, CLEARCOLOR) \
	f(Clear, CLEAR) \
	f(BindBuffer, BINDBUFFER) \
	f(BufferData, BUFFERDATA) \
	f(GenBuffers, GENBUFFERS) \
	f(DeleteBuffers, DELETEBUFFERS) \
	f(GetError, GETERROR) \
	f(GenVertexArrays, GENVERTEXARRAYS) \
	f(DeleteVertexArrays, DELETEVERTEXARRAYS) \
	f(BindVertexArray, BINDVERTEXARRAY) \
	f(VertexAttribPointer, VERTEXATTRIBPOINTER) \
	f(VertexAttribIPointer, VERTEXATTRIBIPOINTER) \
	f(GetUniformLocation, GETUNIFORMLOCATION) \
	f(Uniform1f, UNIFORM1F) \
	f(Uniform2f, UNIFORM2F) \
	f(Uniform3f, UNIFORM3F) \
	f(Uniform4f, UNIFORM4F) \
	f(Uniform1i, UNIFORM1I) \
	f(Uniform2i, UNIFORM2I) \
	f(Uniform3i, UNIFORM3I) \
	f(Uniform4i, UNIFORM4I) \
	f(Uniform1ui, UNIFORM1UI) \
	f(UniformMatrix4fv, UNIFORMMATRIX4FV) \
	f(UseProgram, USEPROGRAM) \
	f(DrawElements, DRAWELEMENTS) \
	f(CreateShader, CREATESHADER) \
	f(ShaderSource, SHADERSOURCE) \
	f(GetShaderiv, GETSHADERIV) \
	f(GetShaderInfoLog, GETSHADERINFOLOG) \
	f(CreateProgram, CREATEPROGRAM) \
	f(GetProgramiv, GETPROGRAMIV) \
	f(GetProgramInfoLog, GETPROGRAMINFOLOG) \
	f(DeleteShader, DELETESHADER) \
	f(DeleteProgram, DELETEPROGRAM) \
	f(AttachShader, ATTACHSHADER) \
	f(LinkProgram, LINKPROGRAM) \
	f(GetAttribLocation, GETATTRIBLOCATION) \
	f(CompileShader, COMPILESHADER) \
	f(Enable, ENABLE) \
	f(Disable, DISABLE) \
	f(GetIntegerv, GETINTEGERV) \
	f(DebugMessageCallback, DEBUGMESSAGECALLBACK) \
	f(DebugMessageControl, DEBUGMESSAGECONTROL) \
	f(DrawArrays, DRAWARRAYS) \
	f(EnableVertexAttribArray, ENABLEVERTEXATTRIBARRAY) \
	f(PolygonMode, POLYGONMODE) \
	f(Flush, FLUSH) \
	f(Viewport, VIEWPORT) \
	f(GenFramebuffers, GENFRAMEBUFFERS) \
	f(DeleteFramebuffers, DELETEFRAMEBUFFERS) \
	f(BindFramebuffer, BINDFRAMEBUFFER) \
	f(FramebufferTexture2D, FRAMEBUFFERTEXTURE2D) \
	f(CheckFramebufferStatus, CHECKFRAMEBUFFERSTATUS) \
	f(GenTextures, GENTEXTURES) \
	f(DeleteTextures, DELETETEXTURES) \
	f(BindTexture, BINDTEXTURE) \
	f(TexImage2D, TEXIMAGE2D) \
	f(TexImage2DMultisample, TEXIMAGE2DMULTISAMPLE) \
	f(ActiveTexture, ACTIVETEXTURE) \
	f(TexParameteri, TEXPARAMETERI) \
	f(GenerateMipmap, GENERATEMIPMAP) \
	f(BlendFunc, BLENDFUNC) \
	f(BlendEquation, BLENDEQUATION) \
	f(DepthFunc, DEPTHFUNC) \
	f(DepthMask, DEPTHMASK)

typedef struct {
#define gl_declare_proc(lower, upper) PFNGL##upper##PROC lower;
gl_for_each_proc(gl_declare_proc)
#undef gl_declare_proc
} GLProcs;

extern GLProcs gl;

typedef void *(*GLProcFn)(char const *);

extern void gl_get_procs(GLProcFn get_proc_address);

typedef struct {
	GLuint id;
} GLProgram;

// make a new shader program with a vertex and fragment shader from files
extern GLProgram gl_program_new(const char *vshader_filename, const char *fshader_filename);
extern void gl_program_use(GLProgram program);
extern void gl_program_delete(GLProgram *program);
extern void gl_program_uniform1f(GLint loc, float x);
extern void gl_program_uniform2f(GLint loc, vec2 x);
extern void gl_program_uniform3f(GLint loc, vec3 x);
extern void gl_program_uniform4f(GLint loc, vec4 x);
extern void gl_program_uniform1i(GLint loc, int x);
extern void gl_program_uniform2i(GLint loc, vec2i x);
extern void gl_program_uniform3i(GLint loc, vec3i x);
extern void gl_program_uniform4i(GLint loc, vec4i x);

#if DEBUG
extern GLuint gl_program_in_use;
#define gl_check_program_in_use(must_be) \
	assert(gl_program_in_use == (must_be))
#else
#define gl_check_program_in_use(must_be) ((void)0)
#endif

#define gl_program_uniform(program, u_name, value) do {\
	GLint _loc = gl.GetUniformLocation(program.id, u_name);\
	gl_check_program_in_use(program.id);\
	if (_loc >= 0) {\
		_Generic((value),\
			float: gl_program_uniform1f,\
			vec2: gl_program_uniform2f,\
			vec3: gl_program_uniform3f,\
			vec4: gl_program_uniform4f,\
			int: gl_program_uniform1i,\
			vec2i: gl_program_uniform2i,\
			vec3i: gl_program_uniform3i,\
			vec4i: gl_program_uniform4i)(_loc, value);\
	} else {\
		debug_print("Uniform not found: %s\n", u_name);\
	}\
	} while (0)

typedef struct {
	GLuint id;
	// number of items of data
	GLuint count;
	// size of whatever type the elements are
	GLuint type_size;
} GLVBO;

extern GLVBO gl_vbo_new_with_type_size(size_t size);
#define gl_vbo_new(type) gl_vbo_new_with_type_size(sizeof(type))
extern void gl_vbo_set_static_data(GLVBO *vbo, const void *data, size_t count);
extern void gl_vbo_set_stream_data(GLVBO *vbo, const void *data, size_t count);
extern void gl_vbo_delete(GLVBO *vbo);

typedef struct {
	GLuint id;
	GLuint count;
} GLIBO;

GLIBO gl_ibo_new(const GLuint *indices, size_t n);
void gl_ibo_delete(GLIBO *ibo);

typedef struct {
	GLuint id;
	GLuint count;
	GLuint program_id;
} GLVAO;

extern GLVAO gl_vao_new(GLProgram program);
extern void gl_vao_add_data_with_offset(GLVAO *vao, GLVBO vbo, const char *attr_name, size_t type_size, size_t member_offset, int n_elements, GLenum element_kind);

// hooray for c11 _Generic!!!
#define gl_vao_add_data(vao, vbo, attr_name, type, member) \
	_Generic((&((type *)0)->member), \
		float *: gl_vao_add_data_with_offset(vao, vbo, attr_name, sizeof(type), offsetof(type, member), 1, GL_FLOAT), \
		vec2 *: gl_vao_add_data_with_offset(vao, vbo, attr_name, sizeof(type), offsetof(type, member), 2, GL_FLOAT), \
		vec3 *: gl_vao_add_data_with_offset(vao, vbo, attr_name, sizeof(type), offsetof(type, member), 3, GL_FLOAT), \
		vec4 *: gl_vao_add_data_with_offset(vao, vbo, attr_name, sizeof(type), offsetof(type, member), 4, GL_FLOAT), \
		int *: gl_vao_add_data_with_offset(vao, vbo, attr_name, sizeof(type), offsetof(type, member), 1, GL_INT), \
		vec2i *: gl_vao_add_data_with_offset(vao, vbo, attr_name, sizeof(type), offsetof(type, member), 2, GL_INT), \
		vec3i *: gl_vao_add_data_with_offset(vao, vbo, attr_name, sizeof(type), offsetof(type, member), 3, GL_INT), \
		vec4i *: gl_vao_add_data_with_offset(vao, vbo, attr_name, sizeof(type), offsetof(type, member), 4, GL_INT))
		

// pass NULL for ibo to just use indices 0, 1, 2, ... count-1
// make sure you are using vao's program before calling this!
extern void gl_vao_render(GLVAO vao, GLIBO const *ibo);
extern void gl_vao_delete(GLVAO *vao);

// call this when you're done with OpenGL
extern void gl_quit(void);

#endif // GL_H_
