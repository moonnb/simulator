varying vec2 uv;

uniform sampler2D u_heatmap;
uniform vec3 u_color_cold, u_color_hot;
uniform float u_heat_max;

void main() {
	float heat = texture2D(u_heatmap, uv).x;
	gl_FragColor = vec4(mix(u_color_cold, u_color_hot, heat * (1.0 / u_heat_max)), 1.0);
}

