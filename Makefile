csv: csv.o ball.o 
	gcc $^ -o $@ -lm 

arr: arr.o ball.o 
	gcc $^ -o $@ -lm 

wnd: glad/src/gl.o wnd.o ball.o draw.o
	gcc $^ -o $@ -lSDL2main -lSDL2 -lm 

vid: glad/src/gl.o vid.o ball.o draw.o
	gcc $^ -o $@ -lSDL2main -lSDL2 -lm -lswscale \
		-lavcodec -lavformat -lavutil -lx264

vid.o: vid.c ball.h draw.h
	gcc $< -o $@ -c -Iglad/include 

wnd.o: wnd.c ball.h draw.h
	gcc $< -o $@ -c -Iglad/include 

arr.o: arr.c ball.h
	gcc $< -o $@ -c 

csv.o: csv.c ball.h
	gcc $< -o $@ -c 

ball.o: ball.c ball.h
	gcc $< -o $@ -c

draw.o: draw.c draw.h ball.h
	gcc $< -o $@ -c -Iglad/include

glad/src/gl.o: glad/src/gl.c
	gcc $< -o $@ -c -Iglad/include 

clean:
	rm glad/src/gl.o *.o csv arr wnd vid
