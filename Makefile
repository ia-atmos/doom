# Builds the engine (a copy of Chocolate Doom, in engine/) into out/doom.

CC     ?= cc
OUT    := out
SDL_C  := $(shell sdl2-config --cflags)
SDL_L  := $(shell sdl2-config --libs)

CFLAGS := -std=gnu99 -O2 -g -w \
          -DDISABLE_SDL2MIXER=1 -DDISABLE_SDL2NET=1 \
          -Icfg -Iengine/src -Iengine/textscreen -Iengine/pcsound -Iengine/opl \
          $(SDL_C)

# doom_icon.c is data, included by i_video.c
SRCS := $(filter-out engine/src/doom/doom_icon.c, \
          $(wildcard engine/src/*.c engine/src/doom/*.c \
                     engine/textscreen/*.c engine/pcsound/*.c engine/opl/*.c))
OBJS := $(SRCS:%.c=$(OUT)/%.o)

$(OUT)/doom: $(OBJS)
	$(CC) -o $@ $(OBJS) $(SDL_L) -lm

$(OUT)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

clean:
	rm -rf $(OUT)

.PHONY: clean

-include $(OBJS:.o=.d)
