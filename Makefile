CXX := @ clang++
CC  := @ clang

# LSAN_OPTIONS=verbosity=1:log_threads=1 # Use this environment variable for more verbosity with address sanitizer
CXXFLAGS := -g -Wall -fsanitize=address -frtti -std=c++20 -D COMPILER_FORWARD_DECLARATIONS
CCFLAGS  := -g -Wall -fsanitize=address -D COMPILER_FORWARD_DECLARATIONS

INCLUDE   := -I src/ -I src/common
LIBRARIES := -L src/lib/public -l glfw3

OUT := build
APP := $(OUT)/Nostalgia.x86_64
DEBUG_APP := $(OUT)/Nostalgia_Debug.x86_64

SRC_DIRS :=                          \
	src/app                          \
	src/system                       \
	src/world                        \
	src/rendering                    \
	src/rendering/backends           \
	src/rendering/backends/graphics  \
	src/rendering/backends/windowing

DIRTY_SRC_DIRS :=        \
	src/common           \
	src/common/glad      \
	src/common/DearImGui

CXX_SRCS = $(foreach directory,$(SRC_DIRS),$(wildcard $(directory)/*.cpp))
CC_SRCS = $(foreach directory,$(SRC_DIRS),$(wildcard $(directory)/*.c))
CXX_OBJS = $(addprefix $(OUT)/,$(notdir $(CXX_SRCS:.cpp=.obj)))
CC_OBJS = $(addprefix $(OUT)/,$(notdir $(CC_SRCS:.c=.o)))

DIRTY_CXX_SRCS = $(foreach directory,$(DIRTY_SRC_DIRS),$(wildcard $(directory)/*.cpp))
DIRTY_CC_SRCS = $(foreach directory,$(DIRTY_SRC_DIRS),$(wildcard $(directory)/*.c))
DIRTY_CXX_OBJS = $(addprefix $(OUT)/,$(notdir $(DIRTY_CXX_SRCS:.cpp=.obj)))
DIRTY_CC_OBJS = $(addprefix $(OUT)/,$(notdir $(DIRTY_CC_SRCS:.c=.o)))

SRCS = $(CXX_SRCS)$(DIRTY_CXX_SRCS)$(CC_SRCS)$(DIRTY_CC_SRCS)
OBJS = $(CXX_OBJS)$(DIRTY_CXX_OBJS)$(CC_OBJS)$(DIRTY_CC_OBJS)

VPATH := $(SRC_DIRS) $(DIRTY_SRC_DIRS)

RESET = \\033[0m
RED   = \\033[31m
GREEN = \\033[32m
BLUE  = \\033[34m

.PHONY: all clean dirty_clean build run run_debug run_release eval_debug debug release sublime
sublime: ;@:
	$(eval RESET="")
	$(eval RED="")
	$(eval GREEN="")
	$(eval BLUE="")

all: $(APP) $(DEBUG_APP)
	@echo -e "Finished compiling: $(BLUE)$(APP)$(RESET) & $(BLUE)$(DEBUG_APP)$(RESET)"

eval_debug:
	$(eval CXXFLAGS += -D NOSTALGIA_DEBUGGING)

debug: eval_debug $(DEBUG_APP)
	@echo -e "Finished compiling: $(BLUE)$(DEBUG_APP)$(RESET)"

release: $(APP)
	@echo -e "Finished compiling: $(BLUE)$(APP)$(RESET)"

$(APP): $(CXX_OBJS) $(DIRTY_CXX_OBJS) $(CC_OBJS) $(DIRTY_CC_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@ $(LIBRARIES)

$(DEBUG_APP): $(CXX_OBJS) $(DIRTY_CXX_OBJS) $(CC_OBJS) $(DIRTY_CC_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@ $(LIBRARIES)

$(OUT)/%.obj: %.cpp | build
	@echo -e "Compiling: $(GREEN)$<$(RESET)"
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(OUT)/%.o: %.c | build
	@echo -e "Compiling: $(GREEN)$<$(RESET)"
	$(CC) $(CCFLAGS) $(INCLUDE) -c $< -o $@

build:
	@ -mkdir -p $(OUT)

IS_CLEAN = $(shell [ -d $(OUT) ]; echo $$? )

clean:
ifeq ($(IS_CLEAN), 1)
	@ echo "Build is already clean"
else
	@ -rm -rf $(OUT)
	@ echo -e "[In directory: $(OUT)/]$(shell find $(OUT) -type f | sed -re 's/build\/(.+)/\\n$(RESET)Cleaning: $(RED)\1/g')$(RESET)"
endif


dirty_clean:
	@-rm -f $(CXX_OBJS) $(CC_OBJS) $(APP) $(DEBUG_APP)

run: run_debug run_release

run_debug:
	@ if [ -f $(DEBUG_APP) ]; then $(DEBUG_APP); fi

run_release:
	@ if [ -f $(APP) ]; then $(APP); fi