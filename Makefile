
SRCS_ALL = $(wildcard src/*.c)

# all: library.cpp main.cpp
# $@ evaluates to all
# $< evaluates to library.cpp
# $^ evaluates to library.cpp main.cpp



# Zone éditable

# fichiers explus de la compilation
SRCS_EXCL   = 
# fichier principal
SRCS_MAIN   = $(wildcard src/chariot*.c)
# fichiers à compiler
SRCS_OTH    = $(filter-out ${SRCS_MAIN} ${SRCS_EXCL}, ${SRCS_ALL})

CFLAGS		= -DDEBUG
LDFLAGS		= -lm

# Zone normalement non modifiable

EXECS	    = $(patsubst %.c,%,$(SRCS_MAIN))
OBJS_MAIN   = $(SRCS_MAIN:.c=.o)
HEADS_OTH   = $(SRCS_OTH:.c=.h)
OBJS_OTH    = $(SRCS_OTH:.c=.o)

.PHONY: clean

all: $(EXECS)

$(OBJS_MAIN) : %.o : %.c $(HEADS_OTH) Makefile
	gcc -c -o $@ $(CFLAGS) $<

$(EXECS) : % : %.o $(OBJS_OTH)
	gcc -o $@ $^ $(LDFLAGS)

%.o : %.c
	gcc -c -o build/$@ $(CFLAGS) $<

# clean:
# 	rm -f *.o $(EXECS)

# clean sans supprimer les exécutables
# TODO: prendre en compte les paths
clean:
	rm -f *.o

# Pour afficher les variables du makefile
test_makefile:
	$(info    SRCS_ALL  is $(SRCS_ALL))
	$(info    SRCS_MAIN is $(SRCS_MAIN))