// Copyright (c) 2014-2015 James Lowenthal - All rights reserved

#pragma once

template<typename T>
struct vec2d
{
	T x, y;

	vec2d<T>() { x = 0; y = 0; };
	vec2d<T>(T _x, T _y) { x = _x; y = _y; };

	void normalise();
	double magnitude();
	double magnitudeSq();
	vec2d<T> tangent();
};

template<typename T>
inline void vec2d<T>::normalise()
{
	double magnitude = (x * x) + (y * y);
	if (magnitude == 0) return;
	magnitude = sqrt(magnitude);
	x /= magnitude;
	y /= magnitude;
}

template<typename T>
inline double vec2d<T>::magnitudeSq()
{
	return (x * x) + (y * y);
}

template<typename T>
inline double vec2d<T>::magnitude()
{
	return sqrt(magnitudeSq());
}

template<typename T>
inline vec2d<T> vec2d<T>::tangent()
{
	return vec2d<T>(-y, x);
}

template<typename T>
inline double dot(const vec2d<T>& a, const vec2d<T>& b)
{
	return (a.x * b.x) + (a.y * b.y);
}

template<typename T>
inline double cross(const vec2d<T>& a, const vec2d<T>& b)
{
	return (a.x * b.y) - (a.y * b.x);
}

template<typename T>
inline vec2d<T> cross(const vec2d<T>& v, float a)
{
	return vec2d<T>(a * v.y, -a * v.x);
}

template<typename T>
inline vec2d<T> cross(float a, const vec2d<T>& v)
{
	return vec2d<T>(-a * v.y, a * v.x);
}

typedef vec2d<int> ivec2d;
typedef vec2d<float> fvec2d;
typedef vec2d<double> dvec2d;