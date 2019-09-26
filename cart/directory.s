;
; Karri Kaksonen, 2015
;

	.import __BLOCKSIZE__
	.import __STARTOFDIRECTORY__

	.export _MAIN_FILENR : absolute
	.import __STARTUP_LOAD__
	.import __STARTUP_SIZE__
	.import __INIT_SIZE__
	.import __CODE_SIZE__
	.import __DATA_SIZE__
	.import __RODATA_SIZE__

	.export _INTRO_FILENR : absolute
	.import __INTRO_CODE_LOAD__
	.import __INTRO_CODE_SIZE__
	.import __INTRO_DATA_SIZE__
	.import __INTRO_RODATA_SIZE__

	.export _GAME_FILENR : absolute
	.import __GAME_CODE_LOAD__
	.import __GAME_CODE_SIZE__
	.import __GAME_DATA_SIZE__
	.import __GAME_RODATA_SIZE__

	.export _TUNE0_FILENR : absolute
	.import __TUNE_START__
	.import __TUNE0_RODATA_SIZE__

	.segment "DIRECTORY"

__DIRECTORY_START__:

.macro entry old_off, old_len, new_off, new_block, new_len, new_size, new_addr
new_off=old_off+old_len
new_block=new_off/__BLOCKSIZE__
new_len=new_size
	.byte	<new_block
	.word	(new_off & (__BLOCKSIZE__ - 1))
	.byte	$88
	.word	new_addr
	.word	new_len
.endmacro

; Entry 0 - first executable
_MAIN_FILENR=0
entry __STARTOFDIRECTORY__+(__DIRECTORY_END__-__DIRECTORY_START__), 0, mainoff, mainblock, mainlen, __STARTUP_SIZE__+__INIT_SIZE__+__CODE_SIZE__+__RODATA_SIZE__+__DATA_SIZE__, __STARTUP_LOAD__

_INTRO_FILENR=_MAIN_FILENR+1
entry mainoff, mainlen, introoff, introblock, introlen,__INTRO_CODE_SIZE__+__INTRO_RODATA_SIZE__+__INTRO_DATA_SIZE__, __INTRO_CODE_LOAD__

_GAME_FILENR=_INTRO_FILENR+1
entry introoff, introlen, gameoff, gameblock, gamelen,__GAME_CODE_SIZE__+__GAME_RODATA_SIZE__+__GAME_DATA_SIZE__, __GAME_CODE_LOAD__

_TUNE0_FILENR=_GAME_FILENR+1
entry gameoff, gamelen, tune0off, tune0block, tune0len,__TUNE0_RODATA_SIZE__, __TUNE_START__

__DIRECTORY_END__:

