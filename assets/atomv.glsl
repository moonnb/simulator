attribute vec2 v_pos;
attribute vec2 v_offset;
attribute int v_valence;

uniform vec2 u_atom_radius;

varying vec2 offset;
varying vec3 color;

void main() {
	gl_Position = vec4(v_pos + v_offset * u_atom_radius, 0.0, 1.0);
	offset = v_offset;
	switch (v_valence) {
	case 1:
		color = vec3(0.0, 0.0, 1.0);
		break;
	case 2:
		color = vec3(1.0, 0.0, 0.0);
		break;
	case 3:
		color = vec3(0.0, 1.0, 0.0);
		break;
	case 4:
		color = vec3(0.0, 0.0, 0.0);
		break;
	}
}
