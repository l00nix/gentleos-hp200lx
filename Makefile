CC      = tcc
AS      = vendor\nasm301\nasm
CFLAGS  = -mt -u- -g1 -c -Iinclude
ASFLAGS = -f obj

OBJS = &
	build\kernel\start.obj &
	build\apps\about.obj &
	build\apps\bjack.obj &
	build\apps\calc.obj &
	build\apps\calendar.obj &
	build\apps\clock.obj &
	build\apps\fonts.obj &
	build\apps\launcher.obj &
	build\apps\mines.obj &
	build\apps\pairs.obj &
	build\apps\snake.obj &
	build\apps\sounds.obj &
	build\apps\tetris.obj &
	build\data\data_bmp.obj &
	build\data\data_fnt.obj &
	build\gui\button.obj &
	build\gui\grid.obj &
	build\gui\main.obj &
	build\gui\rect.obj &
	build\gui\status.obj &
	build\gui\surface.obj &
	build\gui\timeout.obj &
	build\gui\vga.obj &
	build\gui\widget.obj &
	build\gui\window.obj &
	build\gui\wm.obj &
	build\kernel\debug.obj &
	build\kernel\event.obj &
	build\kernel\heap.obj &
	build\kernel\keyboard.obj &
	build\kernel\main.obj &
	build\kernel\rtc.obj &
	build\kernel\speaker.obj &
	build\kernel\system.obj &
	build\kernel\timer.obj &
	build\lib\bios.obj &
	build\lib\cpu.obj &
	build\lib\math.obj &
	build\lib\printf.obj &
	build\lib\rand.obj &
	build\lib\sleep.obj &
	build\lib\string.obj &
	build\lib\tests.obj &

INCLUDES = &
	include\config.h &
	include\gui.h &
	include\kernel.h &
	include\lib.h &
	include\p_apps.h &
	include\p_data.h &
	include\p_gui.h &
	include\p_kernel.h &
	include\p_lib.h &

all: build\fd1440.img .SYMBOLIC
    @echo All done!

boot: all .SYMBOLIC
    boot build\fd1440.img

build\kernel.com: $(OBJS)
	tlink @misc\tlink.rsp

build\boot.bin: boot\boot.s
    $(AS) -o build\boot.bin boot\boot.s

build\mkdisk.exe: misc\mkdisk.c
    $(CC) -nbuild misc\mkdisk.c

build\fd1440.img: build\kernel.com build\boot.bin build\mkdisk.exe
    build\mkdisk

clean: .SYMBOLIC
	-del build\data\*.obj
	-del build\gui\*.obj
	-del build\lib\*.obj
	-del build\kernel\*.obj
	-del build\*.*

build\kernel\start.obj: kernel\start.s
	$(AS) $(ASFLAGS) -o build\kernel\start.obj kernel\start.s

build\apps\about.obj: apps\about.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\about.c

build\apps\bjack.obj: apps\bjack.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\bjack.c

build\apps\calc.obj: apps\calc.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\calc.c

build\apps\calendar.obj: apps\calendar.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\calendar.c

build\apps\clock.obj: apps\clock.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\clock.c

build\apps\fonts.obj: apps\fonts.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\fonts.c

build\apps\launcher.obj: apps\launcher.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\launcher.c

build\apps\mines.obj: apps\mines.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\mines.c

build\apps\pairs.obj: apps\pairs.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\pairs.c

build\apps\snake.obj: apps\snake.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\snake.c

build\apps\sounds.obj: apps\sounds.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\sounds.c

build\apps\tetris.obj: apps\tetris.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\tetris.c

build\data\data_bmp.obj: data\data_bmp.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\data data\data_bmp.c

build\data\data_fnt.obj: data\data_fnt.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\data data\data_fnt.c

build\gui\button.obj: gui\button.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\gui gui\button.c

build\gui\grid.obj: gui\grid.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\gui gui\grid.c

build\gui\main.obj: gui\main.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\gui gui\main.c

build\gui\rect.obj: gui\rect.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\gui gui\rect.c

build\gui\status.obj: gui\status.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\gui gui\status.c

build\gui\surface.obj: gui\surface.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\gui gui\surface.c

build\gui\timeout.obj: gui\timeout.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\gui gui\timeout.c

build\gui\vga.obj: gui\vga.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\gui gui\vga.c

build\gui\widget.obj: gui\widget.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\gui gui\widget.c

build\gui\window.obj: gui\window.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\gui gui\window.c

build\gui\wm.obj: gui\wm.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\gui gui\wm.c

build\kernel\debug.obj: kernel\debug.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\kernel kernel\debug.c

build\kernel\event.obj: kernel\event.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\kernel kernel\event.c

build\kernel\heap.obj: kernel\heap.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\kernel kernel\heap.c

build\kernel\keyboard.obj: kernel\keyboard.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\kernel kernel\keyboard.c

build\kernel\main.obj: kernel\main.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\kernel kernel\main.c

build\kernel\rtc.obj: kernel\rtc.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\kernel kernel\rtc.c

build\kernel\speaker.obj: kernel\speaker.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\kernel kernel\speaker.c

build\kernel\system.obj: kernel\system.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\kernel kernel\system.c

build\kernel\timer.obj: kernel\timer.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\kernel kernel\timer.c

build\lib\bios.obj: lib\bios.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\lib lib\bios.c

build\lib\cpu.obj: lib\cpu.s
	$(AS) $(ASFLAGS) -o build\lib\cpu.obj lib\cpu.s

build\lib\math.obj: lib\math.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\lib lib\math.c

build\lib\printf.obj: lib\printf.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\lib lib\printf.c

build\lib\rand.obj: lib\rand.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\lib lib\rand.c

build\lib\sleep.obj: lib\sleep.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\lib lib\sleep.c

build\lib\string.obj: lib\string.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\lib lib\string.c

build\lib\tests.obj: lib\tests.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\lib lib\tests.c
