#include <math.h>
#include <stdlib.h>
#include "ball.h"

struct ball balls[N_BALLS];

static void ball_col(struct ball *a, struct ball *b) {
	float dx, dy, dv;
	float corr;
	float ma, mb;
	float v0a, v1a, v0b, v1b;

	dx = b->pos.x - a->pos.x;
	dy = b->pos.y - a->pos.y;
	dv = sqrtf(dx * dx + dy * dy);
	if (dv == 0.0F || dv > a->radius + b->radius)
		return;
	dx /= dv;
	dy /= dv;
	corr = (a->radius + b->radius - dv) / 2.0F;
	a->pos.x -= dx * corr;
	a->pos.y -= dy * corr;
	b->pos.x += dx * corr;
	b->pos.y += dy * corr;
	v0a = a->vel.x * dx + a->vel.y * dy;
	v0b = b->vel.x * dx + b->vel.y * dy;
	ma = a->mass;
	mb = b->mass;
	v1a = (ma * v0a + mb * v0b - mb * (v0a - v0b)) / (ma + mb);
	v1b = (ma * v0a + mb * v0b - ma * (v0b - v0a)) / (ma + mb);
	a->vel.x += dx * (v1a - v0a);
	a->vel.y += dy * (v1a - v0a);
	b->vel.x += dx * (v1b - v0b);
	b->vel.y += dy * (v1b - v0b);
}

static void wall_col(struct ball *ball) {
	float neg, pos;
	neg = -1.0F + ball->radius;
	pos = 1.0F - ball->radius;
	if (ball->pos.x < neg) {
		ball->pos.x = neg; 
		ball->vel.x = -ball->vel.x; 
	} else if (ball->pos.x > pos) { 
		ball->pos.x = pos; 
		ball->vel.x = -ball->vel.x;
	}
	if (ball->pos.y < neg) {
		ball->pos.y = neg; 
		ball->vel.y = -ball->vel.y; 
	} else if (ball->pos.y > pos) { 
		ball->pos.y = pos; 
		ball->vel.y = -ball->vel.y;
	}
}

void init_balls(void) { 
	int i;
	for (i = 0; i < N_BALLS; i++) {
		float radius = rand() % 10 / 100.0F + 0.01F; 
		balls[i].radius = radius; 
		balls[i].mass = (float) M_PI * radius * radius;
		balls[i].pos.x = drand48() * 2.0F - 1.0F;
		balls[i].pos.y = drand48() * 2.0F - 1.0F;
		balls[i].vel.x = drand48() * 0.04F - 0.02F;
		balls[i].vel.y = drand48() * 0.04F - 0.02F;
	}
}

void simulate(void) {
	int i, j;
	for (i = 0; i < N_BALLS; i++) {
		balls[i].pos.x += balls[i].vel.x;
		balls[i].pos.y += balls[i].vel.y;
		for (j = i + 1; j < N_BALLS; j++) 
			ball_col(&balls[i], &balls[j]);
		wall_col(&balls[i]);
	}
}

