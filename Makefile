# Sélection de tout les fichiers source c.
SRCS_ALL	= $(wildcard *.c)


###############################################################################
# Zone éditable


# Fichier à exclure de la compilation.
SRCS_EXCL	= 
# Fichier source principal.
SRCS_MAIN	= $(wildcard chariot*.c)
# Fichiers à compiler autre que le source principal.
SRCS_OTH 	= $(filter-out ${SRCS_MAIN} ${SRCS_EXCL}, ${SRCS_ALL})

# Options passées au compilateur.
# -DDEBUG active la librairie de débogage.
CFLAGS		= -DDEBUG
# Options passées au linker (éditeur de liens).
# -lm permet de lier (-l) la bibliothèque mathématique (libm) qui contient \
<math.h>.
LDFLAGS		= -lm


###############################################################################
# Zone normalement non modifiable

# Nom de l'exécutable.
EXECS		= $(patsubst %.c,%,$(SRCS_MAIN))
# Nom du fichier objet principal, obtenu par substitution de %.c par %.o.
OBJS_MAIN	= $(SRCS_MAIN:.c=.o)
# Nom des fichiers headers, obtenu par substitution de %.c par %.h.
HEADS_OTH	= $(SRCS_OTH:.c=.h)
# Nom des fichiers objets, obtenu par substitution de %.c par %.o.
OBJS_OTH	= $(SRCS_OTH:.c=.o)


# Tâche par défaut.
# Nécessite la présence de l'éxécutable.
all: $(EXECS)

# Tâche associé au fichier objet principal.
# Nécessite la présence du fichier source principal.
$(OBJS_MAIN) : %.o : %.c $(HEADS_OTH) Makefile
	gcc -c -o $@ $(CFLAGS) $<

# Tâche associée à l'éxécutable.
# Nécessite la présence du fichier objet principal.
$(EXECS) : % : %.o $(OBJS_OTH)
	gcc -o $@ $^ $(LDFLAGS)

# Tâche associée aux fichiers objets.
# Nécessite le fichier source associé.
%.o : %.c
	gcc -c -o $@ $(CFLAGS) $<


# Nettoie les fichiers objets.
# .PHONY indique que cette tâche ne craie pas de fichier.
.PHONY: clean
clean:
	rm -f *.o
	
.PHONY: mov
mov:
	cp chariot19.c /mnt/c/e_Tous\ mes\ dossiers/a\ sauvegarder\ provisoir/Cours/ENSISA/1A/S_6/projet\ maths/export
