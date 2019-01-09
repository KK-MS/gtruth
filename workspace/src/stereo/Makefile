IDIR =./
CC=gcc
CFLAGS+=-I$(IDIR)

ODIR=obj
LDIR =../lib

LIBS=-lm

_DEPS = stereo_input.h stereo_output.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = stereo_main.o stereo_input.o stereo_output.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

stereo: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
