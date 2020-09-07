CC = gcc
PKGCONFIG = $(shell which pkg-config)
CFLAGS = $(shell $(PKGCONFIG) --cflags gtk+-3.0)
LIBS = $(shell $(PKGCONFIG) --libs gtk+-3.0) -export-dynamic
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)

SRCDIR = src
SRC = converter.c
BUILD_SRC = resource.c

OBJS = $(BUILD_SRC:.c=.o) $(SRC:.c=.o)

all: baseconverter

resource.c: $(SRCDIR)/resource.xml $(SRCDIR)/converter.glade
	$(GLIB_COMPILE_RESOURCES) resource.xml --target=$@ --sourcedir=. --generate-source

%.o: $(SRCDIR)/%.c
	$(CC) -c -o $(@F) $(CFLAGS) $<

baseconverter: $(OBJS)
	$(CC) -o $(@F) $(OBJS) $(LIBS)   

clean:
	rm -f $(BUILD_SRC)
	rm -f $(OBJS)
	rm -f baseconverter