include ../Common.mk

thisdir=game

objects= \
	game.o \
	tree.o

temporaryobjects= \
	tree.c

SEGMENTS= \
	--code-name GAME_CODE \
	--rodata-name GAME_RODATA \
	--data-name GAME_DATA \
	--bss-name GAME_BSS

all: objlist

objlist: $(objects)
	$(RM) objlist
	for obj in $(objects); do $(ECHO) ../$(thisdir)/$$obj >> objlist; done

tree.c : tree.pcx
	$(SP) -r $< -c lynx-sprite,mode=packed,ax=94,ay=168 -w $*.c,ident=$*,bytesperline=8

clean:
	$(RM) objlist $(temporaryobjects) $(objects)

