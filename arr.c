#include <stdio.h>
#include <stdlib.h>
#include "ball.h"

static int time;
	
static void print_balls(void) {
	int i;

	printf("\t/* time %d */ {\n", time);
	for (i = 0; i < N_BALLS; i++) {
		printf("\t\t/* ball %d */ {\n", i);
		printf("\t\t\t.mass = %f,\n", balls[i].mass);
		printf("\t\t\t.radius = %f,\n", balls[i].radius);
		printf("\t\t\t.pos = {%f, %f},\n", 
				balls[i].pos.x, balls[i].pos.y);
		printf("\t\t\t.vel = {%f, %f},\n", 
				balls[i].vel.x, balls[i].vel.y);
		printf("\t\t},\n");
	}
	printf("\t},\n");
}

int main(void) {
	init_balls();
	printf("struct ball steps[201][%d] = {\n", N_BALLS);
	while (time < 200) {
		print_balls();
		simulate();
		time++;
	}
	print_balls();
	printf("};\n");
}
