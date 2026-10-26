CC := gcc

SRC_DIR := src
BUILD_DIR := build

MODE ?= release

# -Werror 
CFLAGS_COMMON := -Wall -Wextra -Werror -std=c11 -MMD -MP
CFLAGS_DEBUG := -g -O0
CFLAGS_RELEASE := -O2
CFLAGS_GCOV := --coverage $(CFLAGS_DEBUG)

ifeq ($(MODE), debug)
	CFLAGS := $(CFLAGS_COMMON) $(CFLAGS_DEBUG)
endif

ifeq ($(MODE), release)
	CFLAGS := $(CFLAGS_COMMON) $(CFLAGS_RELEASE)
endif

ifeq ($(MODE), gcov)
	CFLAGS := $(CFLAGS_COMMON) $(CFLAGS_GCOV)
endif

TETRIS_LIB := $(BUILD_DIR)/libtetris.a
TETRIS_APP := $(BUILD_DIR)/tetris
TETRIS_TEST := $(BUILD_DIR)/tetris_test

all: $(TETRIS_APP) $(TETRIS_TEST) $(TETRIS_LIB)

run: ${TETRIS_APP}
	./${TETRIS_APP}

test: ${TETRIS_TEST} 
	./${TETRIS_TEST} 

gcov_report:
	$(MAKE) MODE=gcov clean $(TETRIS_TEST)
	./$(TETRIS_TEST)
	lcov -t "tetris_test" -o $(BUILD_DIR)/coverage.info -c -d $(BUILD_DIR)
	genhtml -o $(BUILD_DIR)/report $(BUILD_DIR)/coverage.info

linter:
	bear --output $(BUILD_DIR)/compile_commands.json -- $(MAKE) -B all $(TETRIS_TEST)

format:
	find $(SRC_DIR)/brick_game $(SRC_DIR)/gui -name "*.c" -o -name "*.h" | xargs clang-format -i --style=file

PREFIX ?= /usr/local
INSTALL_DIR ?= $(PREFIX)/bin

install: $(TETRIS_APP)
	@mkdir -p $(INSTALL_DIR)
	@cp $(TETRIS_APP) $(INSTALL_DIR)/tetris
	@echo "Installed to $(INSTALL_DIR)/tetris"

uninstall:
	@rm -f $(INSTALL_DIR)/tetris
	@echo "Uninstalled $(INSTALL_DIR)/tetris"

DIST_NAME := BrickGame-v1.0
DIST_ARCHIVE := $(DIST_NAME).tar.gz

dist:
	@mkdir -p $(BUILD_DIR)/$(DIST_NAME)
	@cp -r $(SRC_DIR) docs Makefile README.md $(BUILD_DIR)/$(DIST_NAME)/
	@rm -f $(BUILD_DIR)/$(DIST_NAME)/$(SRC_DIR)/*.tar.gz $(BUILD_DIR)/$(DIST_NAME)/$(SRC_DIR)/highscore.dat
	@tar -czf $(DIST_ARCHIVE) -C $(BUILD_DIR) $(DIST_NAME)
	@rm -rf $(BUILD_DIR)/$(DIST_NAME)
	@echo "Created $(DIST_ARCHIVE)"

dvi:
	@mkdir -p $(BUILD_DIR)/docs
	@( \
		echo "PROJECT_NAME = BrickGame Tetris"; \
		echo "OUTPUT_DIRECTORY = $(BUILD_DIR)/docs"; \
		echo "INPUT = $(SRC_DIR)/brick_game/tetris/tetris.h $(SRC_DIR)/gui/cli/draw.h"; \
		echo "RECURSIVE = NO"; \
		echo "GENERATE_HTML = YES"; \
		echo "GENERATE_LATEX = NO"; \
		echo "EXTRACT_ALL = YES"; \
		echo "HAVE_DOT = NO"; \
		echo "QUIET = YES"; \
	) | doxygen -
	@echo "Documentation generated in $(BUILD_DIR)/docs/html/index.html"

clean:
	@rm -rf ${BUILD_DIR}

.PHONY: all run test gcov_report clean install uninstall dist linter dvi format


# --- Tetris library ---
TETRIS_LIB_DIR := $(SRC_DIR)/brick_game/tetris

TETRIS_LIB_SRCS := $(addprefix $(TETRIS_LIB_DIR)/, \
	tetris.c 	\
	tetris_fsm.c \
	tetris_figures.c	\
	tetris_field.c \
	tetris_statistics.c \
)

TETRIS_LIB_OBJS := $(patsubst \
	$(TETRIS_LIB_DIR)/%.c, \
	$(BUILD_DIR)/$(TETRIS_LIB_DIR)/%.o, \
	$(TETRIS_LIB_SRCS) \
)

# - Objs compile
$(BUILD_DIR)/$(TETRIS_LIB_DIR)/%.o : $(TETRIS_LIB_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# - Static libary packing
$(TETRIS_LIB) : $(TETRIS_LIB_OBJS) | $(BUILD_DIR)
	ar rcs $@ $?

TETRIS_LIB_INCLUDE := -I$(TETRIS_LIB_DIR)
# --- *** ---


# --- Tetris app ---
TETRIS_APP_DIR := $(SRC_DIR)/gui/cli

TETRIS_APP_SRCS := $(addprefix $(TETRIS_APP_DIR)/, \
	draw.c		\
	main.c 		\
)

TETRIS_APP_OBJS := $(patsubst \
	$(TETRIS_APP_DIR)/%.c, \
	$(BUILD_DIR)/$(TETRIS_APP_DIR)/%.o, \
	$(TETRIS_APP_SRCS) \
)

# - Objs compile
$(BUILD_DIR)/$(TETRIS_APP_DIR)/%.o : $(TETRIS_APP_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(TETRIS_LIB_INCLUDE) -c $< -o $@

# - Executable linking
$(TETRIS_APP) : $(TETRIS_APP_OBJS) $(TETRIS_LIB)| $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TETRIS_APP_OBJS) $(TETRIS_LIB) -o $@ -lncursesw

# --- *** ---


# --- Tests ---
TETRIS_TEST_DIR := $(TETRIS_LIB_DIR)/tests

TETRIS_TEST_SRCS := $(addprefix $(TETRIS_TEST_DIR)/, \
	tetris_test.c \
	tetris_test_field.c \
	tetris_test_figure.c \
	tetris_test_statistics.c \
	tetris_test_fsm.c \
	tetris_test_tetris.c \
)

TETRIS_TEST_OBJS := $(patsubst \
	$(TETRIS_TEST_DIR)/%.c, \
	$(BUILD_DIR)/$(TETRIS_TEST_DIR)/%.o, \
	$(TETRIS_TEST_SRCS) \
)

# - Objs compile
$(BUILD_DIR)/$(TETRIS_TEST_DIR)/%.o : $(TETRIS_TEST_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(TETRIS_LIB_INCLUDE) -c $< -o $@

# - Executable linking
$(TETRIS_TEST) : $(TETRIS_TEST_OBJS) $(TETRIS_LIB)| $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TETRIS_TEST_OBJS) $(TETRIS_LIB) -o $@ -lcheck -lsubunit -lpthread -lrt -lm

-include $(TETRIS_LIB_OBJS:.o=.d)
-include $(TETRIS_APP_OBJS:.o=.d)
-include $(TETRIS_TEST_OBJS:.o=.d)

$(BUILD_DIR) :
	@mkdir -p $@
