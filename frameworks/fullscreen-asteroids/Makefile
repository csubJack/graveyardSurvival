CFLAGS = -I ./include
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: fasteroids

fasteroids: fullscreen-asteroids.cpp log.cpp timers.cpp
	g++ $(CFLAGS) fullscreen-asteroids.cpp log.cpp timers.cpp libggfonts.a -Wall -Wextra $(LFLAGS) -ofasteroids

clean:
	rm -f fasteroids

