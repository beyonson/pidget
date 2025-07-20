CC=gcc
IDIR=include
SDIR=src
ODIR=build

CFLAGS=-Wall `pkg-config --cflags --libs xcb-icccm libpng` -I$(IDIR)
CLANG_FORMAT := clang-format -i --style=GNU

SRC = $(shell find ./src -name *.c)

_OBJ = main.o logger.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_DEPS = logger.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	@mkdir -p build
	$(CC) -o $@ -c $< $(CFLAGS)


$(ODIR)/main: $(OBJ)
	@mkdir -p build
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: format
format:
	$(CLANG_FORMAT) $(SRC)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o
