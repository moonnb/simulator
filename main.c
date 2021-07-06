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
typedef int MoleculeID;

typedef struct {
	unsigned char valence;
	unsigned char n_bonds; // current # of bonds
	vec2 pos;
	vec2 vel;
	MoleculeID molecule; // -1 = no molecule
} Atom;

typedef struct {
	AtomID a, b;
	unsigned char number; // a double bond is represented with Bond(a, b, 0) and Bond(a, b, 1), for example
} Bond;

typedef struct {
	float mass;
	unsigned n_atoms;
	unsigned capacity;
	AtomID *atoms;
} Molecule;

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
	Bond *bonds;
	unsigned n_bonds, bonds_capacity;
	Molecule *molecules;
	MoleculeID n_molecules, molecules_capacity;
} Universe;

float randf() {
	return (float)rand() / ((float)RAND_MAX + 1);
}

Cell *cell_for_atom(Universe const *universe, Atom const *a) {
	int idx = universe->width * (int)a->pos.y + (int)a->pos.x;
	assert(idx < universe->width * universe->height);
	return &universe->grid[idx];
}

float atom_mass(unsigned char valence) {
	switch (valence) {
	case 1: return 1.008f;
	case 2: return 15.999f;
	case 3: return 14.007f;
	case 4: return 12.011f;
	}
	assert(0);
	return 0;
}


void add_to_molecule(Universe *u, MoleculeID m_id, AtomID a_id) {
	Molecule *m = &u->molecules[m_id];
	Atom *atom = &u->atoms[a_id];
	m->mass += atom_mass(atom->valence);
	if (m->n_atoms >= m->capacity)
		memory_reallocate(m->atoms, m->capacity = m->capacity * 2 + 2);
	m->atoms[m->n_atoms++] = a_id;
	atom->molecule = m_id;
	assert(m->n_atoms <= u->n_atoms);
}

float bond_energy(int valence_a, int valence_b, int bond_number) {
	if (valence_a > valence_b)
		return bond_energy(valence_b, valence_a, bond_number);
	float factor = 0.001f; // kJ/mol -> our energy units/bond
	#define list(a, b, c) bond_number == 0 ? a*factor : bond_number == 1 ? b*factor : c*factor;
	switch (valence_a) {
	case 1:
		switch (valence_b) {
		case 1:
			return list(432, 0, 0); // H-H
		case 2:
			return list(467, 0, 0); // H-O
		case 3:
			return list(391, 0, 0); // H-N
		case 4:
			return list(413, 0, 0); // H-C
		}
		break;
	case 2:
		switch (valence_b) {
		case 2:
			return list(146, 495, 0); // O-O / O=O
		case 3:
			return list(201, 607, 0); // O-N / O=N
		case 4:
			return list(358, 745, 0); // O-C / O=C
		}
		break;
	case 3:
		switch (valence_b) {
		case 3:
			return list(160, 418, 941); // N-N / N=N / N=-N
		case 4:
			return list(305, 615, 891); // N-C / N=C / N=-C
		}
		break;
	case 4:
		assert(valence_b == 4);
		return list(347, 614, 839); // C-C / C=C / C=-C
	}
	assert(0);
	return 0;
}

float make_bond(Universe *u, AtomID id_a, AtomID id_b) {
	assert(id_a != id_b);
	Atom *a = &u->atoms[id_a], *b = &u->atoms[id_b];
	float energy = 0;
	unsigned char bond_number = 0;
	bool already_connected = a->molecule != -1 && a->molecule == b->molecule; // are a and b already in the same molecule?
	if (already_connected) {
		// double/triple bond
		for (unsigned i = 0; i < u->n_bonds; ++i) {
			Bond *bond_i = &u->bonds[i];
			if (bond_i->a == id_a && bond_i->b == id_b) {
				++bond_number;
			}
		}
	}
	if (bond_number >= 3) return 0; // only allow up to triple bonds
	energy = bond_energy(a->valence, b->valence, bond_number);

	++a->n_bonds;
	++b->n_bonds;

	if (u->n_bonds >= u->bonds_capacity)
		memory_reallocate(u->bonds, u->bonds_capacity = u->bonds_capacity * 2 + 2);
	Bond *bond = &u->bonds[u->n_bonds++];
	memset(bond, 0, sizeof *bond);
	bond->a = id_a; bond->b = id_b;
	bond->number = bond_number;
	if (already_connected) return energy; // remaining code is only for new additions to molecules

	bool a_in_molecule = a->molecule != -1;
	bool b_in_molecule = b->molecule != -1;
	Molecule *amol = NULL, *bmol = NULL;
	if (a_in_molecule)
		amol = &u->molecules[a->molecule];
	if (b_in_molecule)
		bmol = &u->molecules[b->molecule];
	Molecule *result_mol = NULL;
	float a_mass = 0, b_mass = 0;
	vec2 a_vel = a->vel, b_vel = b->vel;

	a_mass = amol ? amol->mass : atom_mass(a->valence);
	b_mass = bmol ? bmol->mass : atom_mass(b->valence);
	assert(a_mass > 1 && b_mass > 1);

	if (a_in_molecule && b_in_molecule) {
		// join molecules
		for (unsigned i = 0; i < bmol->n_atoms; ++i) {
			add_to_molecule(u, a->molecule, bmol->atoms[i]);
		}
		free(bmol->atoms);
		bmol->atoms = NULL;
		bmol = NULL;
		assert(a->molecule == b->molecule);
		result_mol = amol;
	} else if (a_in_molecule && !b_in_molecule) {
		// add b to a's molecule
		add_to_molecule(u, a->molecule, id_b);
		result_mol = amol;
	} else if (!a_in_molecule && b_in_molecule) {
		// add a to b's molecule
		add_to_molecule(u, b->molecule, id_a);
		result_mol = bmol;
	} else {
		// make a new molecule with a and b
		if (u->n_molecules >= u->molecules_capacity)
			memory_reallocate(u->molecules, (size_t)(u->molecules_capacity = u->molecules_capacity * 2 + 2));
		MoleculeID m_id = u->n_molecules++;
		result_mol = &u->molecules[m_id];
		memset(result_mol, 0, sizeof *result_mol);
		add_to_molecule(u, m_id, id_a);
		add_to_molecule(u, m_id, id_b);
	}

	// conservation of momentum:
	// a_mass * a_vel + b_mass * b_vel = (a_mass + b_mass) * result_vel
	// result_vel = (a_mass * a_vel + b_mass * b_vel) / (a_mass + b_mass)

	vec2 result_vel = scale(add(scale(a_vel, a_mass), scale(b_vel, b_mass)), 1.0f / (a_mass + b_mass));
	for (unsigned i = 0; i < result_mol->n_atoms; ++i) {
		u->atoms[result_mol->atoms[i]].vel = result_vel;
	}

	assert(a->molecule == result_mol - u->molecules);
	assert(b->molecule == result_mol - u->molecules);
	assert(a->vel.x == b->vel.x && a->vel.y == b->vel.y);
	return energy;
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
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
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
	GLProgram program_bond = gl_program_new("assets/bondv.glsl", "assets/bondf.glsl");

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

	typedef struct {
		vec2 pos;
	} BondVertex;

	GLVBO vbo_bonds = gl_vbo_new(BondVertex);
	GLVAO vao_bonds = gl_vao_new(program_bond);

	SDL_GL_SetSwapInterval(1); // vsync

	Universe *u = memory_allocate(Universe, 1);
	u->width = 100;
	u->height = 9*u->width/16;
	size_t universe_area = (size_t)u->width * (size_t)u->height;
	u->heatmap = memory_allocate(float, universe_area);
	float *heatmap_copy = memory_allocate(float, universe_area);

	bool random_heat = true;

	srand(0);
	float average_heat_per_cell = 10.0f;
	for (size_t  i = 0; i < universe_area; ++i) {
		u->heatmap[i] = random_heat ? randf() * average_heat_per_cell * 2.0f : average_heat_per_cell;
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
		atom->molecule = -1;
		atom->pos = mul(Vec2(randf(), randf()), universe_size);
		atom->vel = vec2_polar(5, randf() * 6.28f);
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
	float const atom_radius = 0.002f;
	bool paused = false;

	while (1) {
		SDL_Event event = {0};
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_p:
					paused = !paused;
					break;
				}
				break;
			case SDL_QUIT:
				goto quit;
			}
		}
		
		Time this_frame = time_now();
		float dt = (float)time_sub(this_frame, last_frame);
		// prevent really long frames
		dt = min(dt, 0.2f);

		last_frame = this_frame;

		int win_width = 0, win_height = 0;
		SDL_GetWindowSize(window, &win_width, &win_height);
		gl.Viewport(0, 0, win_width, win_height);

		gl.ClearColor(0, 0, 0, 1);
		gl.Clear(GL_COLOR_BUFFER_BIT);


		if (!paused) {
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
			memcpy(u->heatmap, heatmap_copy, universe_area * sizeof *u->heatmap);

			// atom movement
			for (AtomID i = 0; i < u->n_atoms; ++i) {
				Atom *atom = &u->atoms[i];
				Cell *prev_cell = cell_for_atom(u, atom);
				atom->pos = add(atom->pos, scale(atom->vel, dt));
				{
					float w = (float)u->width, h = (float)u->height;
					if (atom->pos.x < 0) atom->pos.x += w;
					if (atom->pos.y < 0) atom->pos.y += h;
					if (atom->pos.x >= w) atom->pos.x -= w;
					if (atom->pos.y >= h) atom->pos.y -= h;
				}
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

			{
				// bonding
				int i = 0;
				for (int y = 0; y < u->height; ++y) {
					for (int x = 0; x < u->width; ++x, ++i) {
						Cell *cell = &u->grid[i];
						float *heat = &u->heatmap[i];
						if (cell->n_atoms < 2) continue;
						
						float p_bond = powf(0.9f, 1.0f / dt);
						if (randf() >= p_bond)
							continue;

						int r1 = rand() % cell->n_atoms, r2;
						do
							r2 = rand() % cell->n_atoms;
						while (r1 == r2);

						AtomID id_a = cell->atoms[r1];
						AtomID id_b = cell->atoms[r2];
						Atom *a = &u->atoms[id_a];
						Atom *b = &u->atoms[id_b];
						if (sqdistance(a->pos, b->pos) < 0.1f) continue; // bond would be too short
						if (a->valence <= a->n_bonds || b->valence <= b->n_bonds) continue;
						if (bond_energy(a->valence, b->valence, 0) > *heat)
							continue; // no way can we form this bond!
						assert(cell_for_atom(u, a) == cell);
						assert(cell_for_atom(u, b) == cell);
						*heat -= make_bond(u, id_a, id_b);
					}
				}
			}

		}


		vec2 cell_size = Vec2(2.0f / (float)u->width, 2.0f / (float)u->height);
		#define world_to_render_pos(wpos) sub(mul((wpos), cell_size), Vec2(1, 1))
		{
			// generate atom geometry
			AtomVariableVertexData *data = atom_variable_data;
			for (AtomID i = 0; i < u->n_atoms; ++i) {
				Atom *atom = &u->atoms[i];
				AtomVariableVertexData *v1 = data++;
				AtomVariableVertexData *v2 = data++;
				AtomVariableVertexData *v3 = data++;
				AtomVariableVertexData *v4 = data++;
				vec2 pos = world_to_render_pos(atom->pos);
				v1->pos = v2->pos = v3->pos = v4->pos = pos;
			}
		}

		{
			// generate bond geometry
			BondVertex *data = memory_allocate(BondVertex, 2 * u->n_bonds), *p = data;
			for (unsigned i = 0; i < u->n_bonds; ++i) {
				Bond *bond = &u->bonds[i];
				Atom *a = &u->atoms[bond->a];
				Atom *b = &u->atoms[bond->b];
				vec2 a_pos = world_to_render_pos(a->pos);
				vec2 b_pos = world_to_render_pos(b->pos);
				if (sqdistance(a_pos, b_pos) > 0.5f)
					continue; // bond stretches across screen
				float bond_sep = atom_radius * 0.7f;
				switch (bond->number) {
				case 0: break;
				case 1: a_pos.x -= bond_sep; b_pos.x -= bond_sep; break;
				case 2: a_pos.x += bond_sep; b_pos.x += bond_sep; break;
				default: assert(0); break;
				}
				BondVertex *v1 = p++;
				BondVertex *v2 = p++;
				v1->pos = a_pos;
				v2->pos = b_pos;
			}
			gl_vbo_set_stream_data(&vbo_bonds, data, (size_t)(p - data));
			gl_vao_clear(&vao_bonds);
			gl_vao_add_data(&vao_bonds, vbo_bonds, "v_pos", BondVertex, pos);
			free(data);
		}

		gl_vbo_set_stream_data(&vbo_atom_variable, atom_variable_data, u->n_atoms * 4);
		gl_vao_add_data(&vao_atom, vbo_atom_variable, "v_pos", AtomVariableVertexData, pos);

		gl.BindTexture(GL_TEXTURE_2D, heatmap);
		gl.TexImage2D(GL_TEXTURE_2D, 0, GL_R32F, u->width, u->height,
			0, GL_RED, GL_FLOAT, u->heatmap);

		gl.Viewport(0, 0, win_width, win_height);
		gl_program_use(program_heat);
		gl.ActiveTexture(GL_TEXTURE0);
		gl.BindTexture(GL_TEXTURE_2D, heatmap);
		gl_program_uniform(program_heat, "u_heatmap", 0);
		gl_program_uniform(program_heat, "u_heat_max", average_heat_per_cell * 2);
		gl_program_uniform(program_heat, "u_color_cold", Vec3(0.0f, 0.1f, 0.5f));
		gl_program_uniform(program_heat, "u_color_hot", Vec3(1.0f, 0.3f, 0.3f));
		gl_vao_render(vao_heat, &ibo_heat);

		gl_program_use(program_bond);
		gl_vao_render_lines(vao_bonds, NULL);

		gl.Enable(GL_BLEND);
		gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		gl_program_use(program_atom);
		gl_program_uniform(program_atom, "u_atom_radius", Vec2(atom_radius, atom_radius * 16 / 9));
		gl_vao_render(vao_atom, &ibo_atom);
		gl.Disable(GL_BLEND);
		
		SDL_GL_SwapWindow(window);
	}
quit:
	free(u->heatmap);
	free(u->atoms);
	for (MoleculeID i = 0; i < u->n_molecules; ++i)
		free(u->molecules[i].atoms);
	for (size_t i = 0; i < universe_area; ++i)
		free(u->grid[i].atoms);
	free(u->grid);
	free(u->molecules);
	free(u->bonds);
	free(atom_variable_data);
	free(heatmap_copy);
	free(u);
	gl_program_delete(&program_heat);
	gl_program_delete(&program_atom);
	gl_vbo_delete(&vbo_heat);
	gl_vao_delete(&vao_heat);
	gl_vbo_delete(&vbo_atom_variable);
	gl_vbo_delete(&vbo_atom_const);
	gl_vao_delete(&vao_atom);
	gl_ibo_delete(&ibo_heat);
	gl_quit();
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}
