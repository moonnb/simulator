#ifndef MMATH_H_
#define MMATH_H_

#include <tgmath.h>

typedef struct {
	float x, y;
} vec2;

typedef struct {
	float x, y, z;
} vec3;

typedef struct {
	float x, y, z, w;
} vec4;

typedef struct {
	int x, y;
} vec2i;

typedef struct {
	int x, y, z;
} vec3i;

typedef struct {
	int x, y, z, w;
} vec4i;

#define Vec2(x, y) ((vec2){x, y})
#define Vec3(x, y, z) ((vec3){x, y, z})
#define Vec4(x, y, z, w) ((vec4){x, y, z, w})
#define Vec2i(x, y) ((vec2i){x, y})
#define Vec3i(x, y, z) ((vec3i){x, y, z})
#define Vec4i(x, y, z, w) ((vec4i){x, y, z, w})

extern vec2 vec2_polar(float r, float theta);
extern vec3 vec3_spherical(float r, float alpha, float beta);


extern vec2 vec3_xy(vec3 v);

#define xy(v) vec3_xy(v)

extern void vec2_print(vec2 v);
extern void vec3_print(vec3 v);
extern void vec4_print(vec4 v);
extern void vec2i_print(vec2i v);
extern void vec3i_print(vec3i v);
extern void vec4i_print(vec4i v);
#define math_print(x) (_Generic((x),\
	vec2: vec2_print,\
	vec3: vec3_print,\
	vec4: vec4_print,\
	vec2i: vec2i_print,\
	vec3i: vec3i_print,\
	vec4i: vec4i_print))(x)

#define math_println(x) math_print(x), putchar('\n')

extern vec2 vec2_add(vec2 a, vec2 b);
extern vec3 vec3_add(vec3 a, vec3 b);
extern vec4 vec4_add(vec4 a, vec4 b);
extern vec2i vec2i_add(vec2i a, vec2i b);
extern vec3i vec3i_add(vec3i a, vec3i b);
extern vec4i vec4i_add(vec4i a, vec4i b);

#define add(a, b) (_Generic((a),\
	vec2: vec2_add,\
	vec3: vec3_add,\
	vec4: vec4_add,\
	vec2i: vec2i_add,\
	vec3i: vec3i_add,\
	vec4i: vec4i_add))(a, b)

extern vec2 vec2_sub(vec2 a, vec2 b);
extern vec3 vec3_sub(vec3 a, vec3 b);
extern vec4 vec4_sub(vec4 a, vec4 b);
extern vec2i vec2i_sub(vec2i a, vec2i b);
extern vec3i vec3i_sub(vec3i a, vec3i b);
extern vec4i vec4i_sub(vec4i a, vec4i b);

#define sub(a, b) (_Generic((a),\
	vec2: vec2_sub,\
	vec3: vec3_sub,\
	vec4: vec4_sub,\
	vec2i: vec2i_sub,\
	vec3i: vec3i_sub,\
	vec4i: vec4i_sub))(a, b)


extern float vec2_sqlength(vec2 v);
extern float vec3_sqlength(vec3 v);
extern float vec4_sqlength(vec4 v);
#define sqlength(x) (_Generic((x),\
	vec2: vec2_sqlength,\
	vec3: vec3_sqlength,\
	vec4: vec4_sqlength))(x)

extern vec2 vec2_normalize(vec2 v);
extern vec3 vec3_normalize(vec3 v);
extern vec4 vec4_normalize(vec4 v);

#define normalize(x) (_Generic((x),\
	vec2: vec2_normalize,\
	vec3: vec3_normalize,\
	vec4: vec4_normalize))(x)

extern vec2 vec2_abs(vec2 v);
extern vec3 vec3_abs(vec3 v);
extern vec4 vec4_abs(vec4 v);
extern vec2i vec2i_abs(vec2i v);
extern vec3i vec3i_abs(vec3i v);
extern vec4i vec4i_abs(vec4i v);

// can't use abs; that's in libc
#define absv(x) (_Generic((x), \
	float: fabsf,\
	int: abs,\
	vec2: vec2_abs,\
	vec3: vec3_abs,\
	vec4: vec4_abs,\
	vec2i: vec2i_abs,\
	vec3i: vec3i_abs,\
	vec4i: vec4i_abs))(x)

extern float float_mod(float a, float b);
extern int int_mod(int a, int b);
extern vec2 vec2_mod(vec2 a, vec2 b);
extern vec3 vec3_mod(vec3 a, vec3 b);
extern vec4 vec4_mod(vec4 a, vec4 b);
extern vec2i vec2i_mod(vec2i a, vec2i b);
extern vec3i vec3i_mod(vec3i a, vec3i b);
extern vec4i vec4i_mod(vec4i a, vec4i b);
#define mod(a, b) (_Generic((a), \
	float: float_mod,\
	int: int_mod,\
	vec2: vec2_mod,\
	vec3: vec3_mod,\
	vec4: vec4_mod,\
	vec2i: vec2i_mod,\
	vec3i: vec3i_mod,\
	vec4i: vec4i_mod))(a, b)

#define length(x) sqrtf(sqlength(x))
#define sqdistance(a, b) sqlength(sub(a, b))
#define distance(a, b) length(sub(a, b))

extern float vec2_dot(vec2 a, vec2 b);
extern float vec3_dot(vec3 a, vec3 b);
extern float vec4_dot(vec4 a, vec4 b);
extern int vec2i_dot(vec2i a, vec2i b);
extern int vec3i_dot(vec3i a, vec3i b);
extern int vec4i_dot(vec4i a, vec4i b);
#define dot(a, b) (_Generic((a),\
	vec2: vec2_dot,\
	vec3: vec3_dot,\
	vec4: vec4_dot,\
	vec2i: vec2i_dot,\
	vec3i: vec3i_dot,\
	vec4i: vec4i_dot))(a, b)

extern vec3 vec3_cross(vec3 a, vec3 b);
#define cross(a, b) vec3_cross(a, b)

extern vec2 vec2_mul(vec2 a, vec2 b);
extern vec3 vec3_mul(vec3 a, vec3 b);
extern vec4 vec4_mul(vec4 a, vec4 b);
extern vec2i vec2i_mul(vec2i a, vec2i b);
extern vec3i vec3i_mul(vec3i a, vec3i b);
extern vec4i vec4i_mul(vec4i a, vec4i b);
#define mul(a, b) (_Generic((a),\
	vec2: vec2_mul,\
	vec3: vec3_mul,\
	vec4: vec4_mul,\
	vec2i: vec2i_mul,\
	vec3i: vec3i_mul,\
	vec4i: vec4i_mul))(a, b)

extern vec2 vec2_div(vec2 a, vec2 b);
extern vec3 vec3_div(vec3 a, vec3 b);
extern vec4 vec4_div(vec4 a, vec4 b);
extern vec2i vec2i_div(vec2i a, vec2i b);
extern vec3i vec3i_div(vec3i a, vec3i b);
extern vec4i vec4i_div(vec4i a, vec4i b);
// can't use div; that's a libc function
#define divide(a, b) (_Generic((a),\
	vec2: vec2_div,\
	vec3: vec3_div,\
	vec4: vec4_div,\
	vec2i: vec2i_div,\
	vec3i: vec3i_div,\
	vec4i: vec4i_div))(a, b)

extern vec2 vec2_scale(vec2 v, float s);
extern vec3 vec3_scale(vec3 v, float s);
extern vec4 vec4_scale(vec4 v, float s);
extern vec2i vec2i_scale(vec2i v, int s);
extern vec3i vec3i_scale(vec3i v, int s);
extern vec4i vec4i_scale(vec4i v, int s);
#define scale(a, x) (_Generic((a),\
	vec2: vec2_scale,\
	vec3: vec3_scale,\
	vec4: vec4_scale,\
	vec2i: vec2i_scale,\
	vec3i: vec3i_scale,\
	vec4i: vec4i_scale))(a, x)

extern int imin(int a, int b);
extern vec2 vec2_min(vec2 a, vec2 b);
extern vec3 vec3_min(vec3 a, vec3 b);
extern vec4 vec4_min(vec4 a, vec4 b);
extern vec2i vec2i_min(vec2i a, vec2i b);
extern vec3i vec3i_min(vec3i a, vec3i b);
extern vec4i vec4i_min(vec4i a, vec4i b);
#define min(a, b) (_Generic((a),\
	float: fminf,\
	int: imin,\
	vec2: vec2_min,\
	vec3: vec3_min,\
	vec4: vec4_min,\
	vec2i: vec2i_min,\
	vec3i: vec3i_min,\
	vec4i: vec4i_min))(a, b)

extern int imax(int a, int b);
extern vec2 vec2_max(vec2 a, vec2 b);
extern vec3 vec3_max(vec3 a, vec3 b);
extern vec4 vec4_max(vec4 a, vec4 b);
extern vec2i vec2i_max(vec2i a, vec2i b);
extern vec3i vec3i_max(vec3i a, vec3i b);
extern vec4i vec4i_max(vec4i a, vec4i b);
#define max(a, b) (_Generic((a),\
	float: fmaxf,\
	int: imax,\
	vec2: vec2_max,\
	vec3: vec3_max,\
	vec4: vec4_max,\
	vec2i: vec2i_max,\
	vec3i: vec3i_max,\
	vec4i: vec4i_max))(a, b)

extern float flerp(float x, float a, float b);
extern vec2 vec2_lerp(float x, vec2 a, vec2 b);
extern vec3 vec3_lerp(float x, vec3 a, vec3 b);
extern vec4 vec4_lerp(float x, vec4 a, vec4 b);
#define lerp(x, a, b) (_Generic((a),\
	float: flerp,\
	vec2: vec2_lerp,\
	vec3: vec3_lerp,\
	vec4: vec4_lerp\
	))(x, a, b)

extern float fclamp(float x, float a, float b);
extern int iclamp(int x, int a, int b);
extern vec2 vec2_clamp(vec2 x, vec2 a, vec2 b);
extern vec3 vec3_clamp(vec3 x, vec3 a, vec3 b);
extern vec4 vec4_clamp(vec4 x, vec4 a, vec4 b);
extern vec2i vec2i_clamp(vec2i x, vec2i a, vec2i b);
extern vec3i vec3i_clamp(vec3i x, vec3i a, vec3i b);
extern vec4i vec4i_clamp(vec4i x, vec4i a, vec4i b);
#define clamp(x, a, b) (_Generic(x,\
	float: fclamp,\
	int: iclamp,\
	vec2: vec2_clamp,\
	vec3: vec3_clamp,\
	vec4: vec4_clamp\
	))(x, a, b)
	

#endif // MMATH_H_
