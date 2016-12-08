#include "Agent.h"

#include <SDL.h>
#include <random>
#include <vector>
#include <chrono>
#include <thread>

using namespace std::chrono;

#define X 100
#define Y 100
#define FACTOR 5
#define PHERMONE_STRENGTH 128
/* Interesting strengths : 128 - nice long term structures, community; 1 - very random, individual*/

int main(int argc, char* argv[]) {
	SDL_Window* window = SDL_CreateWindow("Hive Mind", 50, 50, X * FACTOR, Y * FACTOR, 0);
	SDL_Surface* surface = SDL_GetWindowSurface(window);
	if (!window) return -1;

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) return -2;

	srand(time(0));

	unsigned short map[X][Y];
	std::vector<Agent> agents;
	
	// Initialise map to all 0
	for (int x = 0; x < X; ++x) {
		for (int y = 0; y < Y; ++y) {
			map[x][y] = 0;
		}
	}

	for (int i = 0; i < X * Y / 100.0; ++i) {
		map[rand() % X][rand() % Y] = -2;
	}

	// Setup agents
	ivec2d home(X / 2, Y / 2);
	for (int i = 0; i < X * Y / 1000; ++i) {
		int vx = 0, vy = 0, tmp = pow(-1, rand() % 2);
		if (rand() % 2 == 0) vx = tmp;
		else vy = tmp;

		agents.push_back(Agent(home.x, home.y, vx, vy));
	}

	bool quit = false;
	while (!quit) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}

		// Iterate map and decrement to baseline of 0
		for (int x = 0; x < X; ++x) {
			for (int y = 0; y < Y; ++y) {
				if (map[x][y] > 0 && map[x][y] < 128) --map[x][y];
			}
		}

		// Iterate agents and let them move
		for (Agent& a : agents) {
			if (a.pos.x == home.x && a.pos.y == home.y){
				a.homesick = false;
				a.awaytime = 0;
			}
			else if (map[a.pos.x][a.pos.y] < 128) {
				++a.awaytime;
			}

			if (a.awaytime > 1000) {
				a.homesick = true;
			}

			if (a.homesick) {
				if (abs(a.pos.x - home.x) > abs(a.pos.y - home.y)) {
					a.vel.y = 0;
					if (a.pos.x - home.x < 0) a.vel.x = 1;
					else a.vel.x = -1;
				}
				else {
					a.vel.x = 0;
					if (a.pos.y - home.y < 0) a.vel.y = 1;
					else a.vel.y = -1;
				}
			}
			else {
				// Found food? -> go home
				if (map[a.pos.x][a.pos.y] < 0) {
					a.homesick = true;
					continue;
				}

				ivec2d vf = a.vel;
				ivec2d vl = ivec2d(a.vel.y, -a.vel.x);
				ivec2d vr = ivec2d(-a.vel.y, a.vel.x);

				// If move is in bounds then set bias to pheremone + 1 (pheremone can be 1)
				int f = 0, l = 0, r = 0;
				if (0 <= a.pos.x + vf.x && a.pos.x + vf.x < X &&	// Forward
					0 <= a.pos.y + vf.y && a.pos.y + vf.y < Y)
					f = map[a.pos.x + vf.x][a.pos.y + vf.y] + 1;
				if (0 <= a.pos.x + vl.x && a.pos.x + vl.x < X &&	// Left
					0 <= a.pos.y + vl.y && a.pos.y + vl.y < Y)
					l = map[a.pos.x + vl.x][a.pos.y + vl.y] + 1;
				if (0 <= a.pos.x + vr.x && a.pos.x + vr.x < X &&	// Right
					0 <= a.pos.y + vr.y && a.pos.y + vr.y < Y)
					r = map[a.pos.x + vr.x][a.pos.y + vr.y] + 1;

				f *= 4; // Bias forward motion
				int sum = f + r + l;
				if (sum == 0)
					continue;

				// Check for food
				if (f < 0) {			// Food ahead
					// a.vel = vf = a.vel
				}
				else if (r < 0)		// Food right
					a.vel = vr;
				else if (l < 0)		// Food left
					a.vel = vl;
				else {
					int choice = rand() % sum;
					if (!(choice < f)) {
						if (choice - f < r) a.vel = vr;
						else a.vel = vl;
					}
				}
			}

			// Add pheremone tracer
			if (a.pos.x >= 0 && a.pos.x < X && a.pos.y >= 0 && a.pos.y < Y) {
				if (map[a.pos.x][a.pos.y] + PHERMONE_STRENGTH < PHERMONE_STRENGTH)
				if (map[a.pos.x][a.pos.y] < 255) {
					map[a.pos.x][a.pos.y] += PHERMONE_STRENGTH;
					if (a.homesick) map[a.pos.x][a.pos.y] += PHERMONE_STRENGTH;
				}
			}

			// Move
			a.pos.x += a.vel.x;
			a.pos.y += a.vel.y;
		}

		// Clear
		SDL_FillRect(surface, NULL, 0xffffffff);

		// Draw map
		for (int x = 0; x < X; ++x) {
			for (int y = 0; y < Y; ++y) {
				if (map[x][y] != 0) {
					SDL_Rect rect;
					rect.x = x * FACTOR; rect.y = y * FACTOR;
					rect.w = FACTOR; rect.h = FACTOR;
					if (map[x][y] > 0) {
						SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 255 - map[x][y], 255 - map[x][y], 255));
					}
					else if (map[x][y] < 0) {
						SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 0, 255, 0));
					}
				}
			}
		}

		for (Agent& a : agents) {
			SDL_Rect rect;
			rect.x = a.pos.x * FACTOR; rect.y = a.pos.y * FACTOR;
			rect.w = FACTOR; rect.h = FACTOR;
			SDL_FillRect(surface, &rect, 0);
		}

		SDL_UpdateWindowSurface(window);
		//std::this_thread::sleep_for(milliseconds(10));
	}

	return 0;
}