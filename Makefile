all: disks .SYMBOLIC
    @echo All done!

disks: build/kernel.com build/boot.bin .SYMBOLIC
    perl misc/mkdisks.pl

boot: all .SYMBOLIC
    boot build\fd1440.img

genmake: .SYMBOLIC
    perl misc/genmake.pl

procbmp: .SYMBOLIC
    perl misc/procbmp.pl

clean: .SYMBOLIC
	-del build\apps\*.*
	-del build\data\*.*
	-del build\gui\*.*
	-del build\lib\*.*
	-del build\kernel\*.*
	-del build\*.*

OBJS = &
	build/kernel/start.obj &
	build/apps/about.obj &
	build/apps/bjack.obj &
	build/apps/calc.obj &
	build/apps/calendar.obj &
	build/apps/clock.obj &
	build/apps/fonts.obj &
	build/apps/keys.obj &
	build/apps/launcher.obj &
	build/apps/mines.obj &
	build/apps/pairs.obj &
	build/apps/snake.obj &
	build/apps/sounds.obj &
	build/apps/tetris.obj &
	build/data/data_bmp.obj &
	build/data/data_fnt.obj &
	build/gui/button.obj &
	build/gui/grid.obj &
	build/gui/main.obj &
	build/gui/rect.obj &
	build/gui/status.obj &
	build/gui/surface.obj &
	build/gui/widget.obj &
	build/gui/window.obj &
	build/gui/wm.obj &
	build/kernel/debug.obj &
	build/kernel/event.obj &
	build/kernel/heap.obj &
	build/kernel/isr.obj &
	build/kernel/keyboard.obj &
	build/kernel/main.obj &
	build/kernel/rtc.obj &
	build/kernel/speaker.obj &
	build/kernel/system.obj &
	build/kernel/timer.obj &
	build/kernel/vga.obj &
	build/lib/bios.obj &
	build/lib/cpu.obj &
	build/lib/math.obj &
	build/lib/printf.obj &
	build/lib/rand.obj &
	build/lib/sleep.obj &
	build/lib/string.obj &
	build/lib/tests.obj &

INCLUDES = &
	include/config.h &
	include/gui.h &
	include/kernel.h &
	include/lib.h &
	include/p_apps.h &
	include/p_data.h &
	include/p_gui.h &
	include/p_kernel.h &
	include/p_lib.h &

build/boot.bin: boot/boot.s
    nasm -o build/boot.bin boot/boot.s

build\kernel.com: $(OBJS)
	wlink @misc/kernel.lnk

build\kernel\start.obj: kernel\start.s
	nasm -f obj -o build\kernel\start.obj kernel\start.s

build\apps\about.obj: apps\about.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\apps apps\about.c
!else
	wcc -fo=build\apps\about.obj apps\about.c @misc\wcc.occ
!endif

build\apps\bjack.obj: apps\bjack.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\apps apps\bjack.c
!else
	wcc -fo=build\apps\bjack.obj apps\bjack.c @misc\wcc.occ
!endif

build\apps\calc.obj: apps\calc.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\apps apps\calc.c
!else
	wcc -fo=build\apps\calc.obj apps\calc.c @misc\wcc.occ
!endif

build\apps\calendar.obj: apps\calendar.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\apps apps\calendar.c
!else
	wcc -fo=build\apps\calendar.obj apps\calendar.c @misc\wcc.occ
!endif

build\apps\clock.obj: apps\clock.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\apps apps\clock.c
!else
	wcc -fo=build\apps\clock.obj apps\clock.c @misc\wcc.occ
!endif

build\apps\fonts.obj: apps\fonts.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\apps apps\fonts.c
!else
	wcc -fo=build\apps\fonts.obj apps\fonts.c @misc\wcc.occ
!endif

build\apps\keys.obj: apps\keys.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\apps apps\keys.c
!else
	wcc -fo=build\apps\keys.obj apps\keys.c @misc\wcc.occ
!endif

build\apps\launcher.obj: apps\launcher.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\apps apps\launcher.c
!else
	wcc -fo=build\apps\launcher.obj apps\launcher.c @misc\wcc.occ
!endif

build\apps\mines.obj: apps\mines.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\apps apps\mines.c
!else
	wcc -fo=build\apps\mines.obj apps\mines.c @misc\wcc.occ
!endif

build\apps\pairs.obj: apps\pairs.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\apps apps\pairs.c
!else
	wcc -fo=build\apps\pairs.obj apps\pairs.c @misc\wcc.occ
!endif

build\apps\snake.obj: apps\snake.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\apps apps\snake.c
!else
	wcc -fo=build\apps\snake.obj apps\snake.c @misc\wcc.occ
!endif

build\apps\sounds.obj: apps\sounds.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\apps apps\sounds.c
!else
	wcc -fo=build\apps\sounds.obj apps\sounds.c @misc\wcc.occ
!endif

build\apps\tetris.obj: apps\tetris.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\apps apps\tetris.c
!else
	wcc -fo=build\apps\tetris.obj apps\tetris.c @misc\wcc.occ
!endif

build\data\data_bmp.obj: data\data_bmp.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\data data\data_bmp.c
!else
	wcc -fo=build\data\data_bmp.obj data\data_bmp.c @misc\wcc.occ
!endif

build\data\data_fnt.obj: data\data_fnt.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\data data\data_fnt.c
!else
	wcc -fo=build\data\data_fnt.obj data\data_fnt.c @misc\wcc.occ
!endif

build\gui\button.obj: gui\button.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\gui gui\button.c
!else
	wcc -fo=build\gui\button.obj gui\button.c @misc\wcc.occ
!endif

build\gui\grid.obj: gui\grid.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\gui gui\grid.c
!else
	wcc -fo=build\gui\grid.obj gui\grid.c @misc\wcc.occ
!endif

build\gui\main.obj: gui\main.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\gui gui\main.c
!else
	wcc -fo=build\gui\main.obj gui\main.c @misc\wcc.occ
!endif

build\gui\rect.obj: gui\rect.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\gui gui\rect.c
!else
	wcc -fo=build\gui\rect.obj gui\rect.c @misc\wcc.occ
!endif

build\gui\status.obj: gui\status.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\gui gui\status.c
!else
	wcc -fo=build\gui\status.obj gui\status.c @misc\wcc.occ
!endif

build\gui\surface.obj: gui\surface.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\gui gui\surface.c
!else
	wcc -fo=build\gui\surface.obj gui\surface.c @misc\wcc.occ
!endif

build\gui\widget.obj: gui\widget.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\gui gui\widget.c
!else
	wcc -fo=build\gui\widget.obj gui\widget.c @misc\wcc.occ
!endif

build\gui\window.obj: gui\window.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\gui gui\window.c
!else
	wcc -fo=build\gui\window.obj gui\window.c @misc\wcc.occ
!endif

build\gui\wm.obj: gui\wm.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\gui gui\wm.c
!else
	wcc -fo=build\gui\wm.obj gui\wm.c @misc\wcc.occ
!endif

build\kernel\debug.obj: kernel\debug.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\kernel kernel\debug.c
!else
	wcc -fo=build\kernel\debug.obj kernel\debug.c @misc\wcc.occ
!endif

build\kernel\event.obj: kernel\event.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\kernel kernel\event.c
!else
	wcc -fo=build\kernel\event.obj kernel\event.c @misc\wcc.occ
!endif

build\kernel\heap.obj: kernel\heap.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\kernel kernel\heap.c
!else
	wcc -fo=build\kernel\heap.obj kernel\heap.c @misc\wcc.occ
!endif

build\kernel\isr.obj: kernel\isr.s
	nasm -f obj -o build\kernel\isr.obj kernel\isr.s

build\kernel\keyboard.obj: kernel\keyboard.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\kernel kernel\keyboard.c
!else
	wcc -fo=build\kernel\keyboard.obj kernel\keyboard.c @misc\wcc.occ
!endif

build\kernel\main.obj: kernel\main.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\kernel kernel\main.c
!else
	wcc -fo=build\kernel\main.obj kernel\main.c @misc\wcc.occ
!endif

build\kernel\rtc.obj: kernel\rtc.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\kernel kernel\rtc.c
!else
	wcc -fo=build\kernel\rtc.obj kernel\rtc.c @misc\wcc.occ
!endif

build\kernel\speaker.obj: kernel\speaker.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\kernel kernel\speaker.c
!else
	wcc -fo=build\kernel\speaker.obj kernel\speaker.c @misc\wcc.occ
!endif

build\kernel\system.obj: kernel\system.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\kernel kernel\system.c
!else
	wcc -fo=build\kernel\system.obj kernel\system.c @misc\wcc.occ
!endif

build\kernel\timer.obj: kernel\timer.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\kernel kernel\timer.c
!else
	wcc -fo=build\kernel\timer.obj kernel\timer.c @misc\wcc.occ
!endif

build\kernel\vga.obj: kernel\vga.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\kernel kernel\vga.c
!else
	wcc -fo=build\kernel\vga.obj kernel\vga.c @misc\wcc.occ
!endif

build\lib\bios.obj: lib\bios.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\lib lib\bios.c
!else
	wcc -fo=build\lib\bios.obj lib\bios.c @misc\wcc.occ
!endif

build\lib\cpu.obj: lib\cpu.s
	nasm -f obj -o build\lib\cpu.obj lib\cpu.s

build\lib\math.obj: lib\math.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\lib lib\math.c
!else
	wcc -fo=build\lib\math.obj lib\math.c @misc\wcc.occ
!endif

build\lib\printf.obj: lib\printf.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\lib lib\printf.c
!else
	wcc -fo=build\lib\printf.obj lib\printf.c @misc\wcc.occ
!endif

build\lib\rand.obj: lib\rand.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\lib lib\rand.c
!else
	wcc -fo=build\lib\rand.obj lib\rand.c @misc\wcc.occ
!endif

build\lib\sleep.obj: lib\sleep.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\lib lib\sleep.c
!else
	wcc -fo=build\lib\sleep.obj lib\sleep.c @misc\wcc.occ
!endif

build\lib\string.obj: lib\string.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\lib lib\string.c
!else
	wcc -fo=build\lib\string.obj lib\string.c @misc\wcc.occ
!endif

build\lib\tests.obj: lib\tests.c $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -nbuild\lib lib\tests.c
!else
	wcc -fo=build\lib\tests.obj lib\tests.c @misc\wcc.occ
!endif
