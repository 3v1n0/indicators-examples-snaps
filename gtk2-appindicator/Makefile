NAME := simple-client
SRC := simple-client.c
DEPS := appindicator-0.1

CFLAGS	?= -O3 -g

ICON_PATH ?= ./

BIN     := $(NAME)
LIBS    += $(shell pkg-config --libs $(DEPS))
CFLAGS  += $(shell pkg-config --cflags $(DEPS)) -DICON_PATH="\"$(ICON_PATH)\""

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
