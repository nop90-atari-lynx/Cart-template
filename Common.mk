CC=cc65
AS=ca65
CL=cl65
SP=sp65
CP=cp
RM=rm -f
ECHO=echo
TOUCH=touch
CFLAGS=-I . --add-source -O -Or -Cl -Os

ifndef PREFIX
	PREFIX=/usr/local
endif
CC65_HOME=$(PREFIX)/lib/cc65
CC65_INC=$(CC65_HOME)/include
CC65_ASMINC=$(CC65_HOME)/asminc
CC65_LIB=$(CC65_HOME)/lib

# Rule for making a *.o file out of a *.c file
%.o: %.c
	$(CC) -t lynx -I $(CC65_INC) $(CFLAGS) $(SEGMENTS) -o $(patsubst %c, %s, $(notdir $<)) $<
	$(AS) -t lynx -I $(CC65_ASMINC) -o $@ $(AFLAGS) $(*).s
	$(RM) $*.s

# Rule for making a *.o file out of a *.s file
%.o: %.s
	$(AS) -t lynx -I $(CC65_ASMINC) -o $@ $(AFLAGS) $<

# Rule for making a *.c file out of a *.pcx file
%.c : %.pcx
	$(SP) -r $< -c lynx-sprite,mode=packed -w $*.c,ident=$*,bytesperline=8

