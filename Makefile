CFLAGS = -I ./include
##LIB    = ./libggfonts.so
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: graveyardSurvival

graveyardSurvival: graveyardSurvival.cpp log.cpp timers.cpp djimenezgarc.cpp jcornejo.cpp jsweeny.cpp mgarcia6.cpp
	g++ $(CFLAGS) djimenezgarc.cpp jcornejo.cpp jsweeny.cpp mgarcia6.cpp graveyardSurvival.cpp log.cpp timers.cpp libggfonts.a -Wall -Wextra $(LFLAGS) -o graveyardSurvival

clean:
	rm -f graveyardSurvival
	rm -f *.o

