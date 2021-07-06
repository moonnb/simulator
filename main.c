#include <SDL2/SDL.h>

#include <stdbool.h>
#include "gl.h"
#include "os.h"

#if DEBUG
#define DEBUG_GL 1
#endif

#if DEBUG_GL
static void APIENTRY gl_message_callback(GLenum source, GLenum type, unsigned int id, GLenum severity, 
	GLsizei length, const char *message, const void *userParam) {
	(void)source; (void)type; (void)id; (void)length; (void)userParam; (void)severity;
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
	printf("Message from OpenGL: %s\n", message);
}
#endif

typedef unsigned AtomID;
typedef unsigned CellID;

typedef struct {
	unsigned char valence;
	unsigned char nbonds; // current # of bonds
	vec2 pos;
	vec2 vel;
	CellID cell;
} Atom;


typedef struct {
	int n_atoms;
	AtomID *atoms;
} Cell;

typedef struct {
	int width, height;
	AtomID n_atoms;
	Atom *atoms;
	Cell *grid;
	float *heatmap;
} Universe;

float randf() {
	return (float)rand() / ((float)RAND_MAX + 1);
}

Cell *cell_for_atom(Universe const *universe, Atom const *a) {
	return &universe->grid[universe->width * (int)a->pos.y + (int)a->pos.x];
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow("simulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		1280, 720, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
	if (!window)
		die("Couldn't create window: %s", SDL_GetError());

#if DEBUG_GL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif
	SDL_GLContext glctx = SDL_GL_CreateContext(window);
	if (!glctx)
		die("Couldn't create GL context: %s", SDL_GetError());
	
	gl_get_procs(SDL_GL_GetProcAddress);

#if DEBUG_GL
	gl.Enable(GL_DEBUG_OUTPUT);
	gl.Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	gl.DebugMessageCallback(gl_message_callback, NULL);
	gl.DebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
		GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif

	GLProgram program_heat = gl_program_new("assets/heatv.glsl", "assets/heatf.glsl");
	GLProgram program_atom = gl_program_new("assets/atomv.glsl", "assets/atomf.glsl");

	typedef struct {
		vec2 pos;
	} HeatVertex;

	GLVAO vao_heat = gl_vao_new(program_heat);
	GLVBO vbo_heat = gl_vbo_new(HeatVertex);
	GLIBO ibo_heat;
	{
		HeatVertex vertices[] = {
			{-1, -1},
			{1, -1},
			{1, 1},
			{-1, 1}
		};
		GLuint indices[] = {
			0, 1, 2,
			0, 2, 3
		};
		ibo_heat = gl_ibo_new(indices, 6);
		gl_vbo_set_static_data(&vbo_heat, vertices, sizeof vertices / sizeof *vertices);
		gl_vao_add_data(&vao_heat, vbo_heat, "v_pos", HeatVertex, pos);
	}


	SDL_GL_SetSwapInterval(1); // vsync
	bool wireframe = false;

	Universe *u = memory_allocate(Universe, 1);
	u->width = 400;
	u->height = 9*u->width/16;
	size_t universe_area = (size_t)u->width * (size_t)u->height;
	u->heatmap = memory_allocate(float, universe_area);
	float *heatmap_copy = memory_allocate(float, universe_area);

	srand(0);
	for (size_t i = 0; i < universe_area; ++i) {
		u->heatmap[i] = randf();
	}

	GLuint heatmap = 0;
	gl.GenTextures(1, &heatmap);
	gl.BindTexture(GL_TEXTURE_2D, heatmap);
	gl.TexImage2D(GL_TEXTURE_2D, 0, GL_RED, u->width, u->height,
		0, GL_RED, GL_FLOAT, u->heatmap);
	gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	u->n_atoms = 10000;
	u->grid = memory_allocate(Cell, universe_area);
	u->atoms = memory_allocate(Atom, u->n_atoms);

	vec2 universe_size = Vec2((float)u->width, (float)u->height);

	for (AtomID i = 0; i < u->n_atoms; ++i) {
		Atom *atom = &u->atoms[i];
		atom->pos = mul(Vec2(randf(), randf()), universe_size);
		atom->vel = vec2_polar(10, randf() * 6.28f);
		atom->valence = rand() % 4 + 1;
		Cell *cell = cell_for_atom(u, atom);
		memory_reallocate(cell->atoms, (size_t)cell->n_atoms + 1);
		cell->atoms[cell->n_atoms++] = i;
	}

	typedef struct {
		vec2 offset;
		GLint valence;
	} AtomConstVertexData;

	typedef struct {
		vec2 pos;
	} AtomVariableVertexData;

	GLVBO vbo_atom_const = gl_vbo_new(AtomConstVertexData);
	GLVBO vbo_atom_variable = gl_vbo_new(AtomVariableVertexData);
	GLVAO vao_atom = gl_vao_new(program_atom);
	GLIBO ibo_atom;

	// generate constant render data
	{
		AtomConstVertexData *data = memory_allocate(AtomConstVertexData, 4 * u->n_atoms);
		for (AtomID i = 0; i < u->n_atoms; ++i) {
			Atom *atom = &u->atoms[i];
			AtomConstVertexData *v1 = &data[4*i+0];
			AtomConstVertexData *v2 = &data[4*i+1];
			AtomConstVertexData *v3 = &data[4*i+2];
			AtomConstVertexData *v4 = &data[4*i+3];
			v1->offset = Vec2(-1, -1);
			v2->offset = Vec2(+1, -1);
			v3->offset = Vec2(+1, +1);
			v4->offset = Vec2(-1, +1);
			v1->valence = v2->valence = v3->valence = v4->valence = atom->valence;
		}
		gl_vbo_set_static_data(&vbo_atom_const, data, 4 * u->n_atoms);
		gl_vao_add_data(&vao_atom, vbo_atom_const, "v_offset", AtomConstVertexData, offset);
		gl_vao_add_data(&vao_atom, vbo_atom_const, "v_valence", AtomConstVertexData, valence);
		free(data);
	}

	{
		GLuint *indices = memory_allocate(GLuint, u->n_atoms * 6);
		for (GLuint i = 0; i < u->n_atoms; ++i) {
			indices[6*i+0] = 4*i+0;
			indices[6*i+1] = 4*i+1;
			indices[6*i+2] = 4*i+2;
			indices[6*i+3] = 4*i+0;
			indices[6*i+4] = 4*i+2;
			indices[6*i+5] = 4*i+3;
		}
		ibo_atom = gl_ibo_new(indices, u->n_atoms * 6);
		free(indices);
	}	

	AtomVariableVertexData *atom_variable_data = memory_allocate(AtomVariableVertexData, 4 * u->n_atoms);

	Time last_frame = time_now();

	while (1) {
		SDL_Event event = {0};
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_z:
					wireframe = !wireframe;
					break;
				}
				break;
			case SDL_QUIT:
				goto quit;
			}
		}
		
		Time this_frame = time_now();
		float dt = (float)time_sub(this_frame, last_frame);
		last_frame = this_frame;

		int win_width = 0, win_height = 0;
		SDL_GetWindowSize(window, &win_width, &win_height);
		gl.Viewport(0, 0, win_width, win_height);

		gl.ClearColor(0, 0, 0, 1);
		gl.Clear(GL_COLOR_BUFFER_BIT);

		gl.PolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

		// disperse heat
		for (int y = 0; y < u->height; ++y) {
			for (int x = 0; x < u->width; ++x) {
				int xp = x - 1;
				int yp = y - 1;
				int xn = x + 1;
				int yn = y + 1;
				if (xp < 0) xp += u->width;
				if (yp < 0) yp += u->height;
				if (xn >= u->width) xn -= u->width;
				if (yn >= u->height) yn -= u->height;

				float hc = u->heatmap[y * u->width + x];
				float h1 = u->heatmap[yn * u->width + x];
				float h2 = u->heatmap[yp * u->width + x];
				float h3 = u->heatmap[y * u->width + xn];
				float h4 = u->heatmap[y * u->width + xp];
				heatmap_copy[y * u->width + x] = lerp(0.03f, hc, 0.25f * (h1 + h2 + h3 + h4));
			}
		}
		{
			float *tmp = u->heatmap;
			u->heatmap = u->heatmap = heatmap_copy;
			heatmap_copy = tmp;
		}

		{
			// atom movement
			for (AtomID i = 0; i < u->n_atoms; ++i) {
				Atom *atom = &u->atoms[i];
				Cell *prev_cell = cell_for_atom(u, atom);
				atom->pos = mod(add(atom->pos, scale(atom->vel, dt)), universe_size);
				Cell *new_cell = cell_for_atom(u, atom);
				if (prev_cell != new_cell) {
					int index_in_prev_cell = -1;
					for (int a = 0; a < prev_cell->n_atoms; ++a) {
						if (prev_cell->atoms[a] == i)
							index_in_prev_cell = a;
					}
					assert(index_in_prev_cell >= 0);
					// remove atom from this cell
					if (--prev_cell->n_atoms > 0)
						prev_cell->atoms[index_in_prev_cell] = prev_cell->atoms[prev_cell->n_atoms];
					else
						memory_reallocate(prev_cell->atoms, 1);
					// put it in new cell
					memory_reallocate(new_cell->atoms, (size_t)new_cell->n_atoms + 1);
					new_cell->atoms[new_cell->n_atoms++] = i;
				}
			}
		}

		{
			// generate atom geometry
			AtomVariableVertexData *data = atom_variable_data;
			vec2 cell_size = Vec2(2.0f / (float)u->width, 2.0f / (float)u->height);
			for (AtomID i = 0; i < u->n_atoms; ++i) {
				Atom *atom = &u->atoms[i];
				AtomVariableVertexData *v1 = data++;
				AtomVariableVertexData *v2 = data++;
				AtomVariableVertexData *v3 = data++;
				AtomVariableVertexData *v4 = data++;
				vec2 pos = sub(mul(atom->pos, cell_size), Vec2(1, 1));
				v1->pos = v2->pos = v3->pos = v4->pos = pos;
			}
		}

		gl_vbo_set_stream_data(&vbo_atom_variable, atom_variable_data, u->n_atoms * 4);
		gl_vao_add_data(&vao_atom, vbo_atom_variable, "v_pos", AtomVariableVertexData, pos);

		gl.BindTexture(GL_TEXTURE_2D, heatmap);
		gl.TexImage2D(GL_TEXTURE_2D, 0, GL_RED, u->width, u->height,
			0, GL_RED, GL_FLOAT, u->heatmap);

		gl.Viewport(0, 0, win_width, win_height);
		gl_program_use(program_heat);
		gl.ActiveTexture(GL_TEXTURE0);
		gl.BindTexture(GL_TEXTURE_2D, heatmap);
		gl_program_uniform(program_heat, "u_heatmap", 0);
		gl_program_uniform(program_heat, "u_color_cold", Vec3(0.0f, 0.1f, 0.5f));
		gl_program_uniform(program_heat, "u_color_hot", Vec3(1.0f, 0.3f, 0.3f));
		gl_vao_render(vao_heat, &ibo_heat);

		gl.Enable(GL_BLEND);
		gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		gl_program_use(program_atom);
		float const atom_radius = 0.002f;
		gl_program_uniform(program_atom, "u_atom_radius", Vec2(atom_radius, atom_radius * 16 / 9));
		gl_vao_render(vao_atom, &ibo_atom);
		gl.Disable(GL_BLEND);
		
		SDL_GL_SwapWindow(window);
	}
quit:
	gl_program_delete(&program_heat);
	gl_program_delete(&program_atom);
	gl_vbo_delete(&vbo_heat);
	gl_vao_delete(&vao_heat);
	gl_vbo_delete(&vbo_atom_variable);
	gl_vbo_delete(&vbo_atom_const);
	gl_vao_delete(&vao_atom);
	gl_ibo_delete(&ibo_heat);
	gl_quit();
	return 0;
}
