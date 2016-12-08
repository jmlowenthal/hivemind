#pragma once

#include "vec2D.h"

struct Agent {
	ivec2d pos;
	ivec2d vel;

	int awaytime = 0;
	bool homesick = false;
	//Aim aim

	Agent() : vel(0, 0), pos(0, 0) {};
	Agent(int x, int y) : pos(x, y), vel(0, 0) {};
	Agent(int x, int y, int vx, int vy) : pos(x, y), vel(vx, vy) {};
};