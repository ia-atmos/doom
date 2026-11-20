# Builds the game: the program in Atmos (doom.atm) over the engine (a
# copy of Chocolate Doom, in engine/).
#
#   out/doom        the game, with a window
#   out/doom-tst    the game over the test platform (tst/): no window,
#                   no real time, input from a script
#
#   out/doom-c      the engine all in C (c/main.c in the place of the
#   out/doom-c-tst  program in Atmos), to compare
#
# `make test` compares the runs of tst/ with their expected outputs, and
# `make test-c` does the same for the engine all in C.
#
# ATMOS is a checkout of the compiler (ia-atmos/lang), already built.

ATMOS  ?= ../lang
CC     ?= cc
OUT    := out
SDL_C  := $(shell sdl2-config --cflags)
SDL_L  := $(shell sdl2-config --libs)

CFLAGS := -std=gnu99 -O2 -g -w \
          -DDISABLE_SDL2MIXER=1 -DDISABLE_SDL2NET=1 \
          -Icfg -Iengine/src -Iengine/src/doom -Iengine/textscreen -Iengine/pcsound -Iengine/opl \
          $(SDL_C)

# doom_icon.c is data, included by i_video.c
ENG_SRCS := $(filter-out engine/src/doom/doom_icon.c, \
              $(wildcard engine/src/*.c engine/src/doom/*.c \
                         engine/textscreen/*.c engine/pcsound/*.c engine/opl/*.c))

# the test platform takes the place of these files of the engine
TST_OUT  := engine/src/i_video.c engine/src/i_timer.c engine/src/z_zone.c
TST_SRCS := $(filter-out $(TST_OUT), $(ENG_SRCS)) $(wildcard tst/*.c)

ENG_OBJS := $(ENG_SRCS:%.c=$(OUT)/%.o)
TST_OBJS := $(TST_SRCS:%.c=$(OUT)/%.o)

# these files of the engine are in the program in Atmos
ATM_OUT  := $(OUT)/engine/src/doom/f_wipe.o $(OUT)/engine/src/doom/wi_stuff.o \
            $(OUT)/engine/src/doom/f_finale.o
ATM_OBJS := $(OUT)/doom.atm.o
C_OBJS   := $(OUT)/c/main.o
ATM_LIB  := $(ATMOS)/build/libatmos.a

all: $(OUT)/doom $(OUT)/doom-tst $(OUT)/doom-c $(OUT)/doom-c-tst

$(OUT)/doom: $(ATM_OBJS) $(filter-out $(ATM_OUT), $(ENG_OBJS))
	$(CC) -o $@ $^ $(ATM_LIB) $(SDL_L) -lm

$(OUT)/doom-tst: $(ATM_OBJS) $(filter-out $(ATM_OUT), $(TST_OBJS))
	$(CC) -o $@ $^ $(ATM_LIB) $(SDL_L) -lm

$(OUT)/doom-c: $(C_OBJS) $(ENG_OBJS)
	$(CC) -o $@ $^ $(SDL_L) -lm

$(OUT)/doom-c-tst: $(C_OBJS) $(TST_OBJS)
	$(CC) -o $@ $^ $(SDL_L) -lm

# the program in Atmos, through C
$(OUT)/doom.atm.c: $(wildcard *.atm) $(ATMOS)/atmos
	@mkdir -p $(dir $@)
	$(ATMOS)/atmos -c game.atm > $@

$(OUT)/doom.atm.o: $(OUT)/doom.atm.c
	$(CC) $(CFLAGS) -I$(ATMOS)/rt -c $< -o $@

# THE VARIANT of the study (var.atm): a door as a task; no saved games
VAR_OUT  := $(ATM_OUT) $(OUT)/engine/src/doom/p_doors.o

var: $(OUT)/doom-var-tst

$(OUT)/doom-var-tst: $(OUT)/var.atm.o $(filter-out $(VAR_OUT), $(TST_OBJS))
	$(CC) -o $@ $^ $(ATM_LIB) $(SDL_L) -lm

$(OUT)/var.atm.c: $(wildcard *.atm var/*.atm) $(ATMOS)/atmos
	@mkdir -p $(dir $@)
	$(ATMOS)/atmos -c var.atm > $@

$(OUT)/var.atm.o: $(OUT)/var.atm.c
	$(CC) $(CFLAGS) -DDOOM_VARIANT -I$(ATMOS)/rt -c $< -o $@

test: $(OUT)/doom-tst
	tst/run.sh

# all the tests but the one that saves, against the outputs of the main
# build
test-var: $(OUT)/doom-var-tst
	SKIP=save BIN=$(abspath $(OUT)/doom-var-tst) tst/run.sh

test-c: $(OUT)/doom-c-tst
	BIN=$(abspath $(OUT)/doom-c-tst) tst/run.sh

$(OUT)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

clean:
	rm -rf $(OUT)

.PHONY: all var test test-c test-var clean

-include $(ENG_OBJS:.o=.d) $(TST_OBJS:.o=.d) $(C_OBJS:.o=.d)
