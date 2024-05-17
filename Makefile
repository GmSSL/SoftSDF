
CC=gcc
CFLAGS=-fPIC -Wall
LDFLAGS=-shared
LIBS=-lgmssl -framework Security

TARGET=libsoftsdf.so
OBJS=softsdf.o

all: $(TARGET)

$(OBJS): softsdf.c
	$(CC) $(CFLAGS) -c softsdf.c -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) -Wl,-exported_symbols_list,softsdf.exp

clean:
	rm -f $(OBJS) $(TARGET)

install:
	cp $(TARGET) /usr/local/lib
	ldconfig

uninstall:
	rm /usr/local/lib/$(TARGET)
	ldconfig

