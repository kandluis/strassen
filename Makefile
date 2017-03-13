CC = clang
CFLAGS  = -g -Wall -O3

INCLUDES = -I./include
LFLAGS = -L./lib
LIBS = -lm

SRCS = cs124_2.c ./src/matrix.c
OBJS = $(SRCS:.c=.o)

MAIN = cs124_2

.PHONY: depend clean

all: $(MAIN)
	@echo  Program $(MAIN) compiled successfully. Execute with ./$(MAIN).

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^
