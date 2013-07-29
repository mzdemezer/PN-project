IDIR =.
CC=gcc
CFLAGS=-I$(IDIR) -Wall -g

SRCDIR=src
ODIR=obj
LIBS=-lallegro -lallegro_audio -lallegro_acodec -lallegro_image -lallegro_font -lallegro_main -lallegro_ttf -lallegro_primitives -lm

_OBJ = collisions.o draw.o eventthread.o game.o loading.o main.o mathematics.o menu.o stringprocessing.o structures/collstructures.o structures/fastreadset.o structures/gameobjects.o structures/gameshareddata.o structures/level.o structures/RBtree.o structures/zone.o Tests/AllTests.o Tests/collisionsSuite.o Tests/CuTest.o Tests/fastreadsetSuite.o Tests/heapSuite.o Tests/levelSuite.o Tests/mathematicsSuite.o Tests/RBtreeSuite.o Tests/stringprocessingSuite.o
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
