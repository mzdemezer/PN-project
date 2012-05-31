IDIR =.
CC=gcc
CFLAGS=-I$(IDIR) -Wall -g

SRCDIR=src
ODIR=obj
LIBS=-lallegro -lallegro_audio -lallegro_acodec -lallegro_image -lallegro_font -lallegro_main -lallegro_ttf -lallegro_primitives -lm

_OBJ = game.o main.o loading.o menu.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

OUTPUTDIR = bin

$(ODIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUTPUTDIR)/project: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

all: $(OUTPUTDIR)/project

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(OUTPUTDIR)/*
