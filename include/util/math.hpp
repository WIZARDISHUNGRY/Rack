#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <cmath>


namespace rack {

////////////////////
// basic integer functions (suffixed with "i")
////////////////////

inline int mini(int a, int b) {
	return a < b ? a : b;
}

inline int maxi(int a, int b) {
	return a > b ? a : b;
}

/** Limits a value between a minimum and maximum
Assumes min <= max
*/
inline int clampi(int x, int min, int max) {
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}

inline int absi(int a) {
	return (a >= 0) ? a : -a;
}

/** Euclidean modulus, always returns 0 <= mod < base for positive base.
*/
inline int eucmodi(int a, int base) {
	int mod = a % base;
	return (mod >= 0) ? mod : mod + base;
}

/** Returns floor(log_2(n)), or 0 if n == 1.
*/
inline int log2i(int n) {
	int i = 0;
	while (n >>= 1) {
		i++;
	}
	return i;
}

inline bool ispow2i(int n) {
	return n > 0 && (n & (n - 1)) == 0;
}

////////////////////
// basic float functions (suffixed with "f")
////////////////////

inline float absf(float x) {
	return (x < 0.f) ? -x : x;
}

/** Returns 1.f for positive numbers and -1.f for negative numbers (including positive/negative zero) */
inline float sgnf(float x) {
	return copysignf(1.f, x);
}

inline float eucmodf(float a, float base) {
	float mod = fmodf(a, base);
	return (mod >= 0.f) ? mod : mod + base;
}

inline float nearf(float a, float b, float epsilon = 1e-6) {
	return fabsf(a - b) <= epsilon;
}

/** Limits a value between a minimum and maximum
Assumes min <= max
*/
inline float clampf(float x, float min, float max) {
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}

/** Limits a value between a minimum and maximum
If min > max, switches the two values
*/
inline float clamp2f(float x, float min, float max) {
	return clampf(x, fminf(min, max), fmaxf(min, max));
}

/** If the magnitude of x if less than eps, return 0 */
inline float chopf(float x, float eps) {
	return (-eps < x && x < eps) ? 0.f : x;
}

inline float rescalef(float x, float xMin, float xMax, float yMin, float yMax) {
	return yMin + (x - xMin) / (xMax - xMin) * (yMax - yMin);
}

inline float crossf(float a, float b, float frac) {
	return a + frac * (b - a);
}

/** Linearly interpolate an array `p` with index `x`
Assumes that the array at `p` is of length at least floor(x)+1.
*/
inline float interpf(const float *p, float x) {
	int xi = x;
	float xf = x - xi;
	return crossf(p[xi], p[xi+1], xf);
}

/** Complex multiply c = a * b
Arguments may be the same pointers
i.e. cmultf(&ar, &ai, ar, ai, br, bi)
*/
inline void cmultf(float *cr, float *ci, float ar, float ai, float br, float bi) {
	*cr = ar * br - ai * bi;
	*ci = ar * bi + ai * br;
}

////////////////////
// 2D vector and rectangle
////////////////////

struct Rect;

struct Vec {
	float x, y;

	Vec() : x(0.f), y(0.f) {}
	Vec(float x, float y) : x(x), y(y) {}

	Vec neg() {
		return Vec(-x, -y);
	}
	Vec plus(Vec b) {
		return Vec(x + b.x, y + b.y);
	}
	Vec minus(Vec b) {
		return Vec(x - b.x, y - b.y);
	}
	Vec mult(float s) {
		return Vec(x * s, y * s);
	}
	Vec mult(Vec b) {
		return Vec(x * b.x, y * b.y);
	}
	Vec div(float s) {
		return Vec(x / s, y / s);
	}
	Vec div(Vec b) {
		return Vec(x / b.x, y / b.y);
	}
	float dot(Vec b) {
		return x * b.x + y * b.y;
	}
	float norm() {
		return hypotf(x, y);
	}
	Vec min(Vec b) {
		return Vec(fminf(x, b.x), fminf(y, b.y));
	}
	Vec max(Vec b) {
		return Vec(fmaxf(x, b.x), fmaxf(y, b.y));
	}
	Vec round() {
		return Vec(roundf(x), roundf(y));
	}
	Vec floor() {
		return Vec(floorf(x), floorf(y));
	}
	Vec ceil() {
		return Vec(ceilf(x), ceilf(y));
	}
	bool isEqual(Vec b) {
		return x == b.x && y == b.y;
	}
	bool isZero() {
		return x == 0.f && y == 0.f;
	}
	bool isFinite() {
		return std::isfinite(x) && std::isfinite(y);
	}
	Vec clamp(Rect bound);
};


struct Rect {
	Vec pos;
	Vec size;

	Rect() {}
	Rect(Vec pos, Vec size) : pos(pos), size(size) {}
	static Rect fromMinMax(Vec min, Vec max) {
		return Rect(min, max.minus(min));
	}

	/** Returns whether this Rect contains an entire point, inclusive on the top/left, non-inclusive on the bottom/right */
	bool contains(Vec v) {
		return pos.x <= v.x && v.x < pos.x + size.x
			&& pos.y <= v.y && v.y < pos.y + size.y;
	}
	/** Returns whether this Rect contains an entire Rect */
	bool contains(Rect r) {
		return pos.x <= r.pos.x && r.pos.x + r.size.x <= pos.x + size.x
			&& pos.y <= r.pos.y && r.pos.y + r.size.y <= pos.y + size.y;
	}
	/** Returns whether this Rect overlaps with another Rect */
	bool intersects(Rect r) {
		return (pos.x + size.x > r.pos.x && r.pos.x + r.size.x > pos.x)
			&& (pos.y + size.y > r.pos.y && r.pos.y + r.size.y > pos.y);
	}
	bool isEqual(Rect r) {
		return pos.isEqual(r.pos) && size.isEqual(r.size);
	}
	Vec getCenter() {
		return pos.plus(size.mult(0.5f));
	}
	Vec getTopRight() {
		return pos.plus(Vec(size.x, 0.f));
	}
	Vec getBottomLeft() {
		return pos.plus(Vec(0.f, size.y));
	}
	Vec getBottomRight() {
		return pos.plus(size);
	}
	/** Clamps the edges of the rectangle to fit within a bound */
	Rect clamp(Rect bound) {
		Rect r;
		r.pos.x = clamp2f(pos.x, bound.pos.x, bound.pos.x + bound.size.x);
		r.pos.y = clamp2f(pos.y, bound.pos.y, bound.pos.y + bound.size.y);
		r.size.x = clampf(pos.x + size.x, bound.pos.x, bound.pos.x + bound.size.x) - r.pos.x;
		r.size.y = clampf(pos.y + size.y, bound.pos.y, bound.pos.y + bound.size.y) - r.pos.y;
		return r;
	}
	/** Nudges the position to fix inside a bounding box */
	Rect nudge(Rect bound) {
		Rect r;
		r.size = size;
		r.pos.x = clamp2f(pos.x, bound.pos.x, bound.pos.x + bound.size.x - size.x);
		r.pos.y = clamp2f(pos.y, bound.pos.y, bound.pos.y + bound.size.y - size.y);
		return r;
	}
	/** Expands this Rect to contain `other` */
	Rect expand(Rect other) {
		Rect r;
		r.pos.x = fminf(pos.x, other.pos.x);
		r.pos.y = fminf(pos.y, other.pos.y);
		r.size.x = fmaxf(pos.x + size.x, other.pos.x + other.size.x) - r.pos.x;
		r.size.y = fmaxf(pos.y + size.y, other.pos.y + other.size.y) - r.pos.y;
		return r;
	}
	/** Returns a Rect with its position set to zero */
	Rect zeroPos() {
		Rect r;
		r.size = size;
		return r;
	}
};


inline Vec Vec::clamp(Rect bound) {
	return Vec(
		clamp2f(x, bound.pos.x, bound.pos.x + bound.size.x),
		clamp2f(y, bound.pos.y, bound.pos.y + bound.size.y));
}


} // namespace rack