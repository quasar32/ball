#ifndef BALL_H
#define BALL_H

#define N_BALLS 20

struct vec2 {
	float x;
	float y;
};

struct ball {
	float radius;
	float mass;
	struct vec2 pos;
	struct vec2 vel;
};

extern struct ball balls[];

void init_balls(void); 
void simulate(void);

#endif
