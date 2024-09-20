LIBS = 
SOURCES = hostd.c utility.c queue.c sigtrap.c
OBJECTS = $(subst .c,.o,$(SOURCES))
EXE = process hostd
.PHONY: clean help

.PHONY: debug
debug: CFLAGS += -O0 -g3
debug: $(EXE)

process : sigtrap.o
	gcc -Wall -Wextra -std=gnu99 $^ $(LIBS) -o $@ 

hostd : hostd.o utility.o queue.o
	gcc -Wall -Wextra -std=gnu99 $^ $(LIBS) -o $@ 

%.o : %.c
	gcc -Wall -Wextra -std=gnu99 -c $< 

all : $(EXE)

clean:
	rm -f $(OBJECTS) $(EXE) *~

