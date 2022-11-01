build:
	gcc -o quadtree main.c -lm -Wall -std=c99 -g

clean:
	rm quadtree

memcheck:
	valgrind --leak-check=full --show-leak-kinds=all --log-file=val.out --track-origins=yes --verbose ./quadtree -d task2.out ceva.ppm