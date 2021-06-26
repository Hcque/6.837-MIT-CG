CC = g++
SRCS = $(wildcard *.cpp)
SRCS += $(wildcard vecmath/src/*.cpp)
OBJS = $(SRCS:.cpp=.o)
PROG = a5
CFLAGS = -O0 -Wall -Wextra -g
INCFLAGS = -Ivecmath/include

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LINKFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -c -o $@ $(INCFLAGS)

runn:
	./a5 -input scene01_plane.txt -size 200 200 -output 1.bmp

	./a5 -input scene02_cube.txt -size 200 200 -output 2.bmp

	./a5 -input scene03_sphere.txt -size 200 200 -output 3.bmp

	./a5 -input scene04_axes.txt -size 200 200 -output 4.bmp

	./a5 -input scene05_bunny_200.txt -size 200 200 -output 5.bmp

	./a5 -input scene06_bunny_1k.txt -size 200 200 -output 6.bmp

	./a5 -input scene07_shine.txt -size 200 200 -output 7.bmp

	./a5 -input scene08_c.txt -size 200 200 -output 8.bmp

	./a5 -input scene09_s.txt -size 200 200 -output 9.bmp

make runn2:
	./a5 -input scene06_bunny_1k.txt -size 300 300 -output 6.bmp -shadows -bounces 4

	./a5 -input scene10_sphere.txt -size 300 300 -output 10.bmp -shadows -bounces 4 -jitter -filter

	./a5 -input scene11_cube.txt -size 300 300 -output 11.bmp -shadows -bounces 4 -jitter -filter

	./a5 -input scene12_vase.txt -size 300 300 -output 12.bmp -shadows -bounces 4 -jitter -filter

	./a5 -input scene13_diamond.txt -size 300 300 -output 13.bmp -shadows -bounces 4 -jitter -filter

runn3:
	./a5 -input scene01_plane.txt -size 200 200 -output 1.bmp -shadows

	./a5 -input scene02_cube.txt -size 200 200 -output 2.bmp -shadows

	./a5 -input scene03_sphere.txt -size 200 200 -output 3.bmp -shadows

	./a5 -input scene04_axes.txt -size 200 200 -output 4.bmp -shadows

	./a5 -input scene05_bunny_200.txt -size 200 200 -output 5.bmp -shadows

	./a5 -input scene06_bunny_1k.txt -size 200 200 -output 6.bmp -shadows

	./a5 -input scene07_shine.txt -size 200 200 -output 7.bmp -shadows

	./a5 -input scene08_c.txt -size 200 200 -output 8.bmp -shadows

	./a5 -input scene09_s.txt -size 200 200 -output 9.bmp -shadows
clean:
	rm -f *.bak vecmath/src/*.o *.o core.* $(PROG) 
