all:
	"$(MAKE)" -C game;
	"$(MAKE)" -C intro;
	"$(MAKE)" -C resident;
	"$(MAKE)" -C tunes;
	"$(MAKE)" -C cart;

clean:
	"$(MAKE)" -C cart clean;
	"$(MAKE)" -C game clean;
	"$(MAKE)" -C intro clean;
	"$(MAKE)" -C resident clean;
	"$(MAKE)" -C tunes clean;

