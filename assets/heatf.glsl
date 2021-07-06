varying vec2 uv;

uniform sampler2D u_heatmap;
uniform vec3 u_color_cold, u_color_hot;

void main() {
	gl_FragColor = vec4(mix(u_color_cold, u_color_hot, texture2D(u_heatmap, uv).x), 1.0);
}

