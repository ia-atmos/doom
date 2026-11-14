# Builds the engine (a copy of Chocolate Doom, in engine/):
#
#   out/doom        the game, with a window
#   out/doom-tst    the same engine over the test platform (tst/): no
#                   window, no real time, input from a script
#
# `make test` compares the runs of tst/ with their expected outputs.

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

# the test platform takes the place of these files of the engine
TST_OUT  := engine/src/i_video.c engine/src/i_timer.c engine/src/z_zone.c
TST_SRCS := $(filter-out $(TST_OUT), $(SRCS)) tst/i_video.c tst/i_timer.c tst/z_zone.c
TST_OBJS := $(TST_SRCS:%.c=$(OUT)/%.o)

all: $(OUT)/doom $(OUT)/doom-tst

$(OUT)/doom: $(OBJS)
	$(CC) -o $@ $(OBJS) $(SDL_L) -lm

$(OUT)/doom-tst: $(TST_OBJS)
	$(CC) -o $@ $(TST_OBJS) $(SDL_L) -lm

test: $(OUT)/doom-tst
	tst/run.sh

$(OUT)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

clean:
	rm -rf $(OUT)

.PHONY: all test clean

-include $(OBJS:.o=.d) $(TST_OBJS:.o=.d)
