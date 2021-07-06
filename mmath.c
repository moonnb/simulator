#include "mmath.h"

#include <stdlib.h> // abs
#include <stdio.h>

void vec2_print(vec2 v) {
	printf("(%f, %f)", v.x, v.y);
}

void vec3_print(vec3 v) {
	printf("(%f, %f, %f)", v.x, v.y, v.z);
}

void vec4_print(vec4 v) {
	printf("(%f, %f, %f, %f)", v.x, v.y, v.z, v.w);
}

void vec2i_print(vec2i v) {
	printf("(%d, %d)", v.x, v.y);
}

void vec3i_print(vec3i v) {
	printf("(%d, %d, %d)", v.x, v.y, v.z);
}

void vec4i_print(vec4i v) {
	printf("(%d, %d, %d, %d)", v.x, v.y, v.z, v.w);
}

vec2 vec2_polar(float r, float theta) {
	return Vec2(r * cosf(theta), r * sinf(theta));
}

vec3 vec3_spherical(float r, float alpha, float beta) {
	return Vec3(r * cosf(alpha) * sinf(beta), r * sinf(alpha) * sinf(beta), r * cosf(beta));
}

vec2 vec3_xy(vec3 v) {
	return Vec2(v.x, v.y);
}

vec2 vec2_add(vec2 a, vec2 b) {
	return Vec2(a.x + b.x, a.y + b.y);
}

vec3 vec3_add(vec3 a, vec3 b) {
	return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

vec4 vec4_add(vec4 a, vec4 b) {
	return Vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

vec2i vec2i_add(vec2i a, vec2i b) {
	return Vec2i(a.x + b.x, a.y + b.y);
}

vec3i vec3i_add(vec3i a, vec3i b) {
	return Vec3i(a.x + b.x, a.y + b.y, a.z + b.z);
}

vec4i vec4i_add(vec4i a, vec4i b) {
	return Vec4i(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

vec2 vec2_sub(vec2 a, vec2 b) {
	return Vec2(a.x - b.x, a.y - b.y);
}

vec3 vec3_sub(vec3 a, vec3 b) {
	return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

vec4 vec4_sub(vec4 a, vec4 b) {
	return Vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

vec2i vec2i_sub(vec2i a, vec2i b) {
	return Vec2i(a.x - b.x, a.y - b.y);
}

vec3i vec3i_sub(vec3i a, vec3i b) {
	return Vec3i(a.x - b.x, a.y - b.y, a.z - b.z);
}

vec4i vec4i_sub(vec4i a, vec4i b) {
	return Vec4i(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

float vec2_sqlength(vec2 v) {
	return v.x*v.x + v.y*v.y;
}
float vec3_sqlength(vec3 v) {
	return v.x*v.x + v.y*v.y + v.z*v.z;
}
float vec4_sqlength(vec4 v) {
	return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
}

vec2 vec2_normalize(vec2 v) {
	return vec2_scale(v, 1.0f / length(v));
}
vec3 vec3_normalize(vec3 v) {
	return vec3_scale(v, 1.0f / length(v));
}
vec4 vec4_normalize(vec4 v) {
	return vec4_scale(v, 1.0f / length(v));
}

vec2 vec2_abs(vec2 v) {
	return Vec2(fabsf(v.x), fabsf(v.y));
}
vec3 vec3_abs(vec3 v) {
	return Vec3(fabsf(v.x), fabsf(v.y), fabsf(v.z));
}
vec4 vec4_abs(vec4 v) {
	return Vec4(fabsf(v.x), fabsf(v.y), fabsf(v.z), fabsf(v.w));
}
vec2i vec2i_abs(vec2i v) {
	return Vec2i(abs(v.x), abs(v.y));
}
vec3i vec3i_abs(vec3i v) {
	return Vec3i(abs(v.x), abs(v.y), abs(v.z));
}
vec4i vec4i_abs(vec4i v) {
	return Vec4i(abs(v.x), abs(v.y), abs(v.z), abs(v.w));
}

int int_mod(int a, int b) {
	a %= b;
	if (a >= 0) return a;
	return a + b;
}

float float_mod(float a, float b) {
	a = fmodf(a, b);
	if (a >= 0) return a;
	return a + b;
}

vec2 vec2_mod(vec2 a, vec2 b) {
	return Vec2(float_mod(a.x, b.x), float_mod(a.y, b.y));
}

vec3 vec3_mod(vec3 a, vec3 b) {
	return Vec3(float_mod(a.x, b.x), float_mod(a.y, b.y), float_mod(a.z, b.z));
}

vec4 vec4_mod(vec4 a, vec4 b) {
	return Vec4(float_mod(a.x, b.x), float_mod(a.y, b.y), float_mod(a.z, b.z), float_mod(a.w, b.w));
}

vec2i vec2i_mod(vec2i a, vec2i b) {
	return Vec2i(int_mod(a.x, b.x), int_mod(a.y, b.y));
}

vec3i vec3i_mod(vec3i a, vec3i b) {
	return Vec3i(int_mod(a.x, b.x), int_mod(a.y, b.y), int_mod(a.z, b.z));
}

vec4i vec4i_mod(vec4i a, vec4i b) {
	return Vec4i(int_mod(a.x, b.x), int_mod(a.y, b.y), int_mod(a.z, b.z), int_mod(a.w, b.w));
}

float vec2_dot(vec2 a, vec2 b) {
	return a.x*b.x + a.y*b.y;
}
float vec3_dot(vec3 a, vec3 b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}
float vec4_dot(vec4 a, vec4 b) {
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}
int vec2i_dot(vec2i a, vec2i b) {
	return a.x*b.x + a.y*b.y;
}
int vec3i_dot(vec3i a, vec3i b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}
int vec4i_dot(vec4i a, vec4i b) {
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

vec3 vec3_cross(vec3 a, vec3 b) {
	return Vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

vec2 vec2_mul(vec2 a, vec2 b) {
	return Vec2(a.x*b.x, a.y*b.y);
}

vec3 vec3_mul(vec3 a, vec3 b) {
	return Vec3(a.x*b.x, a.y*b.y, a.z*b.z);
}

vec4 vec4_mul(vec4 a, vec4 b) {
	return Vec4(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);
}

vec2i vec2i_mul(vec2i a, vec2i b) {
	return Vec2i(a.x*b.x, a.y*b.y);
}

vec3i vec3i_mul(vec3i a, vec3i b) {
	return Vec3i(a.x*b.x, a.y*b.y, a.z*b.z);
}

vec4i vec4i_mul(vec4i a, vec4i b) {
	return Vec4i(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);
}

vec2 vec2_div(vec2 a, vec2 b) {
	return Vec2(a.x/b.x, a.y/b.y);
}

vec3 vec3_div(vec3 a, vec3 b) {
	return Vec3(a.x/b.x, a.y/b.y, a.z/b.z);
}

vec4 vec4_div(vec4 a, vec4 b) {
	return Vec4(a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w);
}

vec2i vec2i_div(vec2i a, vec2i b) {
	return Vec2i(a.x/b.x, a.y/b.y);
}

vec3i vec3i_div(vec3i a, vec3i b) {
	return Vec3i(a.x/b.x, a.y/b.y, a.z/b.z);
}

vec4i vec4i_div(vec4i a, vec4i b) {
	return Vec4i(a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w);
}

vec2 vec2_scale(vec2 v, float s) {
	return Vec2(v.x*s, v.y*s);
}

vec3 vec3_scale(vec3 v, float s) {
	return Vec3(v.x*s, v.y*s, v.z*s);
}

vec4 vec4_scale(vec4 v, float s) {
	return Vec4(v.x*s, v.y*s, v.z*s, v.w*s);
}

vec2i vec2i_scale(vec2i v, int s) {
	return Vec2i(v.x*s, v.y*s);
}

vec3i vec3i_scale(vec3i v, int s) {
	return Vec3i(v.x*s, v.y*s, v.z*s);
}

vec4i vec4i_scale(vec4i v, int s) {
	return Vec4i(v.x*s, v.y*s, v.z*s, v.w*s);
}

int imin(int a, int b) {
	return a < b ? a : b;
}

vec2 vec2_min(vec2 a, vec2 b) {
	return Vec2(fminf(a.x, b.x), fminf(a.y, b.y));
}

vec3 vec3_min(vec3 a, vec3 b) {
	return Vec3(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z));
}

vec4 vec4_min(vec4 a, vec4 b) {
	return Vec4(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z), fminf(a.w, b.w));
}

vec2i vec2i_min(vec2i a, vec2i b) {
	return Vec2i(imin(a.x, b.x), imin(a.y, b.y));
}

vec3i vec3i_min(vec3i a, vec3i b) {
	return Vec3i(imin(a.x, b.x), imin(a.y, b.y), imin(a.z, b.z));
}

vec4i vec4i_min(vec4i a, vec4i b) {
	return Vec4i(imin(a.x, b.x), imin(a.y, b.y), imin(a.z, b.z), imin(a.w, b.w));
}


int imax(int a, int b) {
	return a > b ? a : b;
}

vec2 vec2_max(vec2 a, vec2 b) {
	return Vec2(fmaxf(a.x, b.x), fmaxf(a.y, b.y));
}

vec3 vec3_max(vec3 a, vec3 b) {
	return Vec3(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z));
}

vec4 vec4_max(vec4 a, vec4 b) {
	return Vec4(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z), fmaxf(a.w, b.w));
}

vec2i vec2i_max(vec2i a, vec2i b) {
	return Vec2i(imax(a.x, b.x), imax(a.y, b.y));
}

vec3i vec3i_max(vec3i a, vec3i b) {
	return Vec3i(imax(a.x, b.x), imax(a.y, b.y), imax(a.z, b.z));
}

vec4i vec4i_max(vec4i a, vec4i b) {
	return Vec4i(imax(a.x, b.x), imax(a.y, b.y), imax(a.z, b.z), imax(a.w, b.w));
}

float flerp(float x, float a, float b) {
	return a + x * (b-a);
}
vec2 vec2_lerp(float x, vec2 a, vec2 b) {
	return Vec2(flerp(x, a.x, b.x), flerp(x, a.y, b.y));
}
vec3 vec3_lerp(float x, vec3 a, vec3 b) {
	return Vec3(flerp(x, a.x, b.x), flerp(x, a.y, b.y), flerp(x, a.z, b.z));
}
vec4 vec4_lerp(float x, vec4 a, vec4 b) {
	return Vec4(flerp(x, a.x, b.x), flerp(x, a.y, b.y), flerp(x, a.z, b.z), flerp(x, a.w, b.w));
}

float fclamp(float x, float a, float b) {
	if (x < a) return a;
	if (x > b) return b;
	return x;
}

int iclamp(int x, int a, int b) {
	if (x < a) return a;
	if (x > b) return b;
	return x;
}

vec2 vec2_clamp(vec2 x, vec2 a, vec2 b) {
	return Vec2(fclamp(x.x, a.x, b.x), fclamp(x.y, a.y, b.y));
}

vec3 vec3_clamp(vec3 x, vec3 a, vec3 b) {
	return Vec3(fclamp(x.x, a.x, b.x), fclamp(x.y, a.y, b.y), fclamp(x.z, a.z, b.z));
}

vec4 vec4_clamp(vec4 x, vec4 a, vec4 b) {
	return Vec4(fclamp(x.x, a.x, b.x), fclamp(x.y, a.y, b.y), fclamp(x.z, a.z, b.z), fclamp(x.w, a.w, b.w));
}

vec2i vec2i_clamp(vec2i x, vec2i a, vec2i b) {
	return Vec2i(iclamp(x.x, a.x, b.x), iclamp(x.y, a.y, b.y));
}

vec3i vec3i_clamp(vec3i x, vec3i a, vec3i b) {
	return Vec3i(iclamp(x.x, a.x, b.x), iclamp(x.y, a.y, b.y), iclamp(x.z, a.z, b.z));
}

vec4i vec4i_clamp(vec4i x, vec4i a, vec4i b) {
	return Vec4i(iclamp(x.x, a.x, b.x), iclamp(x.y, a.y, b.y), iclamp(x.z, a.z, b.z), iclamp(x.w, a.w, b.w));
}
