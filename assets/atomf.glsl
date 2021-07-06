varying vec2 offset;
varying vec3 color;

void main() {
	float dist = dot(offset, offset);
	dist *= dist;
	float alpha = 1.0 - dist;
	gl_FragColor = vec4(color, alpha);
}
