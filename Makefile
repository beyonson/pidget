CC=gcc
CFLAGS=-Wall `pkg-config --cflags --libs xcb`
ODIR=build

CLANG_FORMAT := clang-format -i --style=GNU

SRC = $(shell find . -name *.c)
OBJ = $(patsubst %.c,$(ODIR)/%.o, $(SRC))

$(ODIR)%.o: $(SRC)
	$(CC) $(CFLAGS) -o $@ -c $<

$(ODIR)/main: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: format
format:
	$(CLANG_FORMAT) $(SRC)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o
