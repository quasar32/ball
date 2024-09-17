#include <stdio.h>
#include <stdlib.h>
#include "ball.h"

static int time;
	
static void print_balls(void) {
	int i;
	struct ball *b;

	for (i = 0; i < N_BALLS; i++) {
		b = &balls[i];
		printf("%d,%f,%f,%f,%f,%f,%f\n", time, b->mass, b->radius, 
				b->pos.x, b->pos.y, b->vel.x, b->vel.y);
	}
}

int main(int argc, char **argv) {
	init_balls();
	printf("time,mass,radius,pos.x,pos.y,vel.x,vel.y\n");
	while (time < 200) {
		print_balls();
		simulate();
		time++;
	}
	print_balls();
	return 0;
}
