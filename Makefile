CXX := clang++
CC  := clang

# LSAN_OPTIONS=verbosity=1:log_threads=1 # Use this environment variable for more verbosity with address sanitizer
CXXFLAGS := -g -Wall -fsanitize=address -frtti -std=c++20 -D COMPILER_FORWARD_DECLARATIONS
CCFLAGS  := -g -Wall -fsanitize=address -D COMPILER_FORWARD_DECLARATIONS

INCLUDE   := -I src/ -I src/common
LIBRARIES := -L src/lib/public -l glfw3

OUT := build
APP := $(OUT)/Nostalgia.x86_64
DEBUG_APP := $(OUT)/Nostalgia_Debug.x86_64

SRC_DIRS       := src/app src/dedicated src/api_abstraction src/app_systems
DIRTY_SRC_DIRS := src/common src/common/glad src/common/DearImGui

CXX_SRCS = $(foreach directory,$(SRC_DIRS),$(wildcard $(directory)/*.cpp))
CC_SRCS = $(foreach directory,$(SRC_DIRS),$(wildcard $(directory)/*.c))
CXX_OBJS = $(addprefix $(OUT)/,$(notdir $(CXX_SRCS:.cpp=.obj)))
CC_OBJS = $(addprefix $(OUT)/,$(notdir $(CC_SRCS:.c=.o)))

DIRTY_CXX_SRCS = $(foreach directory,$(DIRTY_SRC_DIRS),$(wildcard $(directory)/*.cpp))
DIRTY_CC_SRCS = $(foreach directory,$(DIRTY_SRC_DIRS),$(wildcard $(directory)/*.c))
DIRTY_CXX_OBJS = $(addprefix $(OUT)/,$(notdir $(DIRTY_CXX_SRCS:.cpp=.obj)))
DIRTY_CC_OBJS = $(addprefix $(OUT)/,$(notdir $(DIRTY_CC_SRCS:.c=.o)))

VPATH := $(SRC_DIRS) $(DIRTY_SRC_DIRS)

.PHONY: all clean dirty_clean build run run_debug run_release debug release
all: $(APP) $(DEBUG_APP)
	$(info Finished compiling: "$(APP)" & "$(DEBUG_APP)")

debug: $(DEBUG_APP)
	$(info Finished compiling: "$(DEBUG_APP)")

release: $(APP)
	$(info Finished compiling: "$(APP)")

$(APP): $(CXX_OBJS) $(DIRTY_CXX_OBJS) $(CC_OBJS) $(DIRTY_CC_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@ $(LIBRARIES)

$(DEBUG_APP): $(CXX_OBJS) $(DIRTY_CXX_OBJS) $(CC_OBJS) $(DIRTY_CC_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@ $(LIBRARIES)

$(OUT)/%.obj: %.cpp | build
	$(info Compiling: "$<")
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(OUT)/%.o: %.c | build
	$(info Compiling: "$<")
	$(CC) $(CCFLAGS) $(INCLUDE) -c $< -o $@

build:
	-mkdir -p $(OUT)

clean:
	-rm -r $(OUT)

# "dirty_clean" won't remove object files built from source code in DIRTY_SRC_DIRS
dirty_clean:
	-rm -r $(CXX_OBJS) $(CC_OBJS) $(APP) $(DEBUG_APP)

run: run_debug run_release

run_debug:
	@ if [ -f $(DEBUG_APP) ]; then $(DEBUG_APP); fi

run_release:
	@ if [ -f $(APP) ]; then $(APP); fi