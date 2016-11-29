NAME := simple-client
SRC := simple-client.c
DEPS := appindicator3-0.1

CFLAGS	?= -O3 -g

PREFIX ?= $(CURDIR)
ICON_PATH ?= ./icons

BIN     := $(NAME)
LIBS    += $(shell pkg-config --libs $(DEPS))
CFLAGS  += $(shell pkg-config --cflags $(DEPS)) -DPREFIX="\"$(PREFIX)\"" -DICON_PATH="\"$(ICON_PATH)\""

OBJS    = $(subst .c,.o, $(SRC))
CC 	?= $(CROSS)gcc
LD 	?= $(CROSS)gcc
CCLD 	?= $(CC)
STRIP 	= $(CROSS)strip

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN):	$(OBJS)
	$(CCLD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(BIN)

strip: $(BIN)
	$(STRIP) $(BIN)
