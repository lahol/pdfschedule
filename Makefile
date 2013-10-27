CC := gcc
PKG_CONFIG := pkg-config

CFLAGS += -Wall -g `$(PKG_CONFIG) --cflags glib-2.0 gio-2.0 poppler poppler-glib poppler-cairo gtk+-3.0 cairo`
LIBS += `$(PKG_CONFIG) --libs glib-2.0 gio-2.0 poppler poppler-glib poppler-cairo gtk+-3.0 cairo`

APPNAME := pdfschedule
PREFIX := /usr

SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
HDR := $(wildcard *.h)

all: $(APPNAME)

$(APPNAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c -o $@ $<

install: $(APPNAME)
	install $(APPNAME) $(PREFIX)/bin

clean:
	rm -f $(APPNAME) $(OBJ)

.PHONY: all clean install
