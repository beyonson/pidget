CC=gcc
CFLAGS=-Wall `pkg-config --cflags --libs xcb-icccm`
ODIR=build

CLANG_FORMAT := clang-format -i --style=GNU

SRC = $(shell find . -name *.c)
OBJ = $(patsubst %.c,$(ODIR)/%.o, $(SRC))

$(ODIR)/%.o: $(SRC)
	@mkdir -p build
	$(CC) -o $@ -c $< $(CFLAGS)

$(ODIR)/main: $(OBJ)
	@mkdir -p build
	$(CC) -o $@ $< $(CFLAGS)

.PHONY: format
format:
	$(CLANG_FORMAT) $(SRC)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o
