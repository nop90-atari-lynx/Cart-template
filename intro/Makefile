include ../Common.mk

thisdir=intro

objects= \
	intro.o \
	bg.o

temporaryobjects= \
	bg.c

SEGMENTS= \
	--code-name INTRO_CODE \
	--rodata-name INTRO_RODATA \
	--data-name INTRO_DATA \
	--bss-name INTRO_BSS

all: objlist

objlist: $(objects)
	$(RM) objlist
	for obj in $(objects); do $(ECHO) ../$(thisdir)/$$obj >> objlist; done

clean:
	$(RM) objlist $(temporaryobjects) $(objects)

