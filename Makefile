CC=gcc
IDIR=include
SDIR=src
ODIR=build

CFLAGS=-Wall -g -lm -lev `pkg-config --cflags --libs xcb-icccm xcb-image \
			 libpng xcb-aux xcb-errors libcyaml` -I$(IDIR)
CLANG_FORMAT := clang-format -i --style=GNU

SRC = $(shell find ./src -name *.c)

_OBJ = main.o logger.o xcb.o config_parser.o image_proc.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_DEPS = logger.h xcb.h config_parser.h image_proc.h common.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	@mkdir -p build
	$(CC) -o $@ -c $< $(CFLAGS)

$(ODIR)/pidget: $(OBJ)
	@mkdir -p build
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: format
format:
	$(CLANG_FORMAT) $(SRC)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o
