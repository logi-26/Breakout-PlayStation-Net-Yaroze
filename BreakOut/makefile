CFLAGS	= -O3 -g
LINKER	= -Xlinker -Ttext -Xlinker 80140000

# Target executable
PROG = main.exe

# Object files
OBJS = main.o init.o logic.o render.o collision.o game.o utils.o

all: $(PROG)


# Link the object files to create the executable
$(PROG): $(OBJS) 
	$(CC) $(LINKER) -o $@ $?
	
# Compile each source file into an object file
main.o: main.c game.h
	$(CC) $(CFLAGS) -c main.c

init.o: init.c init.h game.h
	$(CC) $(CFLAGS) -c init.c

logic.o: logic.c logic.h game.h
	$(CC) $(CFLAGS) -c logic.c

render.o: render.c render.h game.h
	$(CC) $(CFLAGS) -c render.c

collision.o: collision.c collision.h game.h
	$(CC) $(CFLAGS) -c collision.c

game.o: game.c game.h
	$(CC) $(CFLAGS) -c game.c

utils.o: utils.c utils.h game.h
	$(CC) $(CFLAGS) -c utils.c

# Clean up build artifacts
clean:
	$(RM) $(PROG) $(OBJS)