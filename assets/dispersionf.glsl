varying vec2 uv;

uniform sampler2D u_prev_heatmap;
uniform float u_dispersion_speed;

void main() {
	vec2 d = 1.0 / vec2(textureSize(u_prev_heatmap, 0));
	float h1 = texture2D(u_prev_heatmap, uv + vec2(d.x, 0)).x;
	float h2 = texture2D(u_prev_heatmap, uv + vec2(0, d.y)).x;
	float h3 = texture2D(u_prev_heatmap, uv + vec2(-d.x, 0)).x;
	float h4 = texture2D(u_prev_heatmap, uv + vec2(0, -d.y)).x;
	float hc = texture2D(u_prev_heatmap, uv).x;
	float around = 0.25 * (h1 + h2 + h3 + h4);
	gl_FragColor = vec4(mix(hc, around, u_dispersion_speed));
}
