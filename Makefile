
CC=gcc
CFLAGS=-fPIC -Wall
LDFLAGS=-shared
LIBS=-lgmssl -framework Security

TARGET=libsoftsdf.so
OBJS=softsdf.o

BIN=softsdfinit
BIN_OBJS=softsdfinit.o



all: $(BIN) $(TARGET)

$(OBJS): softsdf.c
	$(CC) $(CFLAGS) -c softsdf.c -o $@

$(BIN_OBJS): softsdfinit.c
	$(CC) $(CFLAGS) -c softsdfinit.c -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) -Wl,-exported_symbols_list,softsdf.exp

$(BIN): $(BIN_OBJS)
	$(CC) -o $@ $(BIN_OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET)

install:
	cp $(TARGET) /usr/local/lib
	ldconfig

uninstall:
	rm /usr/local/lib/$(TARGET)
	ldconfig

