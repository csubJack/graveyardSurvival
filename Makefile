CFLAGS = -I ./include
##LIB    = ./libggfonts.so
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: asteroids

asteroids: asteroids.cpp log.cpp timers.cpp djimenezgarc.cpp jcornejo.cpp jsweeny.cpp mgarcia6.cpp
	g++ $(CFLAGS) djimenezgarc.cpp jcornejo.cpp jsweeny.cpp mgarcia6.cpp asteroids.cpp log.cpp timers.cpp libggfonts.a -Wall -Wextra $(LFLAGS) -oasteroids

clean:
	rm -f asteroids
	rm -f *.o

