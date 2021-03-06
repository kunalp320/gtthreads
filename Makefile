#### GTThread Library Makefile

CFLAGS  = -Wall -pedantic -g
LFLAGS  =
CC      = gcc
RM      = /bin/rm -rf
AR      = ar rc
RANLIB  = ranlib
LD	= gcc
PROG_SRC = DiningPhilosophers.c
PROG_OBJ = $(patsubst %.c,%.o,$(PROG_SRC))
PROG_NAME = DiningPhilosophers
LIBRARY = gtthread.a

LIB_SRC = gtthread.c

LIB_OBJ = $(patsubst %.c,%.o,$(LIB_SRC))

# pattern rule for object files
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

all: $(LIBRARY)
	$(LD) $(LFLAGS) -o DiningPhilosophers DiningPhilosophers.c $(LIBRARY)

$(LIBRARY): $(LIB_OBJ)
	$(AR) $(LIBRARY) $(LIB_OBJ)
	$(RANLIB) $(LIBRARY)

clean:
	$(RM) $(LIBRARY) $(LIB_OBJ)

.PHONY: depend
depend:
	$(CFLAGS) -- $(LIB_SRC)  2>/dev/null
