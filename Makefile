NAME			= ls
EXT				=
CC				= clang
RM				= rm -f
MKDIR			= mkdir -p
DEBUG			= 0

ifeq ($(DEBUG), 1)
	DEBUG_FLAGS	= -fsanitize=address -g
else
	MAKEFLAGS	= -j --output-sync=recurse --no-print-directory
	DEBUG_FLAGS	= -O2 -flto -fPIE
endif

WARNING_FLAGS   = -Wall -Wextra -Werror -Wpointer-arith -Wfloat-equal -fpermissive
PROTECT_FLAGS	= -fno-exceptions -fstack-protector-all
COMMON_FLAGS	= -std=c99 -MMD -pipe
CFLAGS			= $(DEBUG_FLAGS) $(COMMON_FLAGS) $(PROTECT_FLAGS) $(WARNING_FLAGS)

BIN_DIR			= bin
SRC_DIR			= src
TESTS_DIR		= tests
BUILD_DIR		= build
INC_DIR			= include

SRCS			= src/main.c\
					src/parser.c\
					src/browsing.c\
					src/directory.c\
					src/entry.c\
					src/print.c\
					src/utils.c\

BIN_PATH		= $(BIN_DIR)/$(NAME)
OBJS			= $(subst $(SRC_DIR), $(BUILD_DIR), $(SRCS:%.c=%.o))
DEPS			= $(OBJS:.o=.d)

VPATH			= $(SRC_DIR):$(INC_DIR):$(BUILD_DIR)

CURRENT_DIR		= $(shell pwd)

all:				$(BIN_PATH)

$(BIN_PATH):		$(OBJS)
					@if [ ! -d $(dir $@) ] ; then $(MKDIR) $(dir $@); fi
					$(CC) $(CFLAGS) -I $(INC_DIR) $(OBJS) -o $(BIN_PATH)

$(BUILD_DIR)/%.o:  $(SRC_DIR)/%.c
					@if [ ! -d $(dir $@) ] ; then $(MKDIR) $(dir $@); fi
					$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

clean:
					$(RM) $(OBJS)
					$(RM) $(DEPS)
					$(RM) -rd $(BUILD_DIR)

fclean:				clean
					$(RM) $(BIN_PATH)
					$(RM) -rd $(BIN_DIR)

re:
					$(MAKE) fclean
					$(MAKE) all

-include $(DEPS)
-include $(TEST_DEPS)
.PHONY:				all, clean, fclean, re