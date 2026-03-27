CC      = tcc
AS      = nasm
CFLAGS  = -mt -u- -g1 -c -Iinclude
ASFLAGS = -f obj

OBJS = \
	build\kernel\start.obj \
	build\apps\about.obj \
	build\apps\clock.obj \
	build\apps\fonts.obj \
	build\apps\launcher.obj \
	build\apps\snake.obj \
	build\apps\sounds.obj \
	build\data\data_bmp.obj \
	build\data\data_fnt.obj \
	build\gui\button.obj \
	build\gui\grid.obj \
	build\gui\main.obj \
	build\gui\rect.obj \
	build\gui\status.obj \
	build\gui\surface.obj \
	build\gui\timeout.obj \
	build\gui\vga.obj \
	build\gui\widget.obj \
	build\gui\window.obj \
	build\gui\wm.obj \
	build\kernel\debug.obj \
	build\kernel\event.obj \
	build\kernel\keyboard.obj \
	build\kernel\main.obj \
	build\kernel\rtc.obj \
	build\kernel\speaker.obj \
	build\kernel\system.obj \
	build\kernel\timer.obj \
	build\lib\cpu.obj \
	build\lib\math.obj \
	build\lib\printf.obj \
	build\lib\rand.obj \
	build\lib\sleep.obj \
	build\lib\string.obj \

INCLUDES = \
	include\config.h \
	include\gui.h \
	include\kernel.h \
	include\lib.h \
	include\p_apps.h \
	include\p_data.h \
	include\p_gui.h \
	include\p_kernel.h \
	include\p_lib.h \

all: build\kernel.com

run: all
    build\kernel.com

build\kernel.com: $(OBJS)
	tlink @misc\tlink.rsp

build\kernel\start.obj: kernel\start.s $(INCLUDES)
	$(AS) $(ASFLAGS) -o build\kernel\start.obj kernel\start.s

build\apps\about.obj: apps\about.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\about.c

build\apps\clock.obj: apps\clock.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\clock.c

build\apps\fonts.obj: apps\fonts.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\fonts.c

build\apps\launcher.obj: apps\launcher.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\launcher.c

build\apps\snake.obj: apps\snake.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\snake.c

build\apps\sounds.obj: apps\sounds.c $(INCLUDES)
	$(CC) $(CFLAGS) -nbuild\apps apps\sounds.c

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

build\lib\cpu.obj: lib\cpu.s $(INCLUDES)
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
