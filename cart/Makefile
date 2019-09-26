include ../Common.mk

objects= \
	lnxhdr.o \
	directory.o

objlists = \
	../resident/objlist \
	../intro/objlist \
	../game/objlist \
	../tunes/objlist

others = \
	@../resident/objlist \
	@../intro/objlist \
	@../game/objlist \
	@../tunes/objlist

target = game.lnx

all: $(target)

$(target) : $(objects) $(objlists)
	$(CL) -t lynx -o $@ -m lynx.map -C lynx.cfg $(objects) $(others) lynx.lib

clean:
	$(RM) $(objects) $(target) lynx.map

