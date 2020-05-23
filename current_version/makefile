TARGET=cannon

all: $(TARGET)

%.o:%.c
	gcc -g -I.. -Wall -pedantic `sdl2-config --cflags` -c  $< -o $@

$(TARGET): ../primlib.o $(TARGET).o game.o
	gcc -g -I.. -Wall -pedantic `sdl2-config --cflags` ../primlib.o $(TARGET).o game.o -o $@ -lSDL2_gfx `sdl2-config --libs` -lm

../primlib.o: ../primlib.c ../primlib.h
$(TARGET).o: $(TARGET).c
game.o: game.c game.h

$(TARGET): $(TARGET).c ../primlib.h game.h

clean:
	-rm $(TARGET)
