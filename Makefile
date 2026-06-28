TARGET = paper

OBJS   = obj/area.o \
		obj/bot.o \
		obj/colors.o \
		obj/draw.o \
		obj/game.o \
		obj/input.o \
		obj/main.o \
		obj/render.o \
		obj/screens.o \
		obj/entity.o \

BUILD_PRX       = 1
EXTRA_TARGETS   = EBOOT.PBP
PSP_EBOOT_TITLE = Paper
PSP_EBOOT_ICON  = assets/ICON0.PNG

VPATH    = src
INCDIR   = include

PSPDEV  := $(shell psp-config --pspdev-path)
SDL2_CFG = $(PSPDEV)/psp/bin/sdl2-config

CFLAGS   = -Wall -Wextra -O2 -G0 $(shell $(SDL2_CFG) --cflags)
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS  = $(CFLAGS)

LIBDIR  =
LDFLAGS =
LIBS    = $(shell $(SDL2_CFG) --libs)

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

obj/%.o: %.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p $@

re: clean all

.PHONY: re
