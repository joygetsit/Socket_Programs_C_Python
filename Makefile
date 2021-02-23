CC=clang
CFLAGS+=
LIBS =

TARGET=socket_udp_server_C
HDRS=
SRCS=socket_udp_server_C.c conio.c Socket_Communication_C.c
OBJS=$(SRCS:.c=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
