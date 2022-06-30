CC=gcc
CFLAGS+=
LIBS =

TARGET=UDP_socket_C
HDRS=
SRCS=UDP_socket_C.c comms.c
OBJS=$(SRCS:.c=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
