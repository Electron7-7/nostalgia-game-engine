LINUX_CXX := @clang++
LINUX_CC  := @clang

ifeq ($(OS),Windows_NT)
	WINDOWS_CXX := @clang++
	WINDOWS_CC  := @clang
else
	WINDOWS_CXX := @x86_64-w64-mingw32-g++
	WINDOWS_CC  := @x86_64-w64-mingw32-gcc
endif

CXX_SELECT ?= $(LINUX_CXX)
CC_SELECT  ?= $(LINUX_CC)

CXX = $(CXX_SELECT)
CC  = $(CC_SELECT)

# LSAN_OPTIONS=verbosity=1:log_threads=1 # Use this environment variable for more verbosity with address sanitizer
DEBUG_FLAGS  := -g -Wall -fsanitize=address -D NOSTALGIA_DEBUGGING
COMMON_FLAGS := -D COMPILER_FORWARD_DECLARATIONS

LINUX_FLAGS   = $(COMMON_FLAGS)
WINDOWS_FLAGS = $(COMMON_FLAGS) -static #-mwindows

PROGRAM_FLAGS ?= $(LINUX_FLAGS)
CXXFLAGS = -std=c++20 -frtti $(PROGRAM_FLAGS)
CCFLAGS  = $(PROGRAM_FLAGS)

INCLUDE_COMMON  := -I src/ -I src/common
INCLUDE_LINUX   := $(INCLUDE_COMMON) -I src/system/common/linux   # Not yet implemented
INCLUDE_WINDOWS := $(INCLUDE_COMMON) -I src/system/common/windows # Not yet implemented
INCLUDE_SELECT ?= $(INCLUDE_LINUX)
INCLUDE = $(INCLUDE_SELECT)

LIBRARIES_LINUX   := -L src/lib/public -l glfw3
LIBRARIES_WINDOWS := -L src/system/lib/windows -l:glfw-lib-mingw-w64/libglfw3.a -l gdi32
LIBRARIES_SELECT ?= $(LIBRARIES_LINUX)
LIBRARIES = $(LIBRARIES_SELECT)

RUN_INSTEAD_OF_BUILD=false

BUILD_PATH_LINUX   := linux
BUILD_PATH_WINDOWS := windows
BUILD_PATH_RELEASE := release
BUILD_PATH_DEBUG   := debug

BUILD_ROOT := build
BUILD_ARCH ?= $(BUILD_PATH_LINUX)
BUILD_VERSION ?= $(BUILD_PATH_RELEASE)

APP_NAME_RELEASE := Nostalgia
APP_NAME_DEBUG   := DEBUG__Nostalgia
APP_NAME_LINUX   := $(shell uname -s)_$(subst .,_,$(shell uname -r)).$(shell uname -m)
APP_NAME_WINDOWS := Win64.exe

APP_VERSION ?= $(APP_NAME_RELEASE)
ifeq ($(OS),Windows_NT)
	APP_ARCH ?= $(APP_NAME_WINDOWS)
else
	APP_ARCH ?= $(APP_NAME_LINUX)
endif


BUILD_OUT ?= $(BUILD_ROOT)/$(BUILD_ARCH)/$(BUILD_VERSION)
APP_OUT ?= $(APP_VERSION)_$(APP_ARCH)


SRC_DIRS :=                       \
	src/app                       \
	src/system                    \
	src/world                     \
	src/math                      \
	src/engine                    \
	src/engine/handlers           \
	src/engine/managers           \
	src/engine/rendering          \
	src/engine/backends           \
	src/engine/backends/graphics  \
	src/engine/backends/windowing

# Object files compiled from "DIRTY_SRC_DIRS" will not be cleaned during a dirty clean
DIRTY_SRC_DIRS :=        \
	src/common           \
	src/common/glad      \
	src/common/DearImGui

CXX_SRCS := $(foreach directory,$(SRC_DIRS),$(wildcard $(directory)/*.cpp))
CC_SRCS  := $(foreach directory,$(SRC_DIRS),$(wildcard $(directory)/*.c))
CXX_OBJS ?= $(addprefix $(BUILD_OUT)/,$(subst .cpp,.obj,$(CXX_SRCS:src/%=%)))
CC_OBJS  ?= $(addprefix $(BUILD_OUT)/,$(subst .c,.o,$(CC_SRCS:src/%=%)))

DIRTY_CXX_SRCS := $(foreach directory,$(DIRTY_SRC_DIRS),$(wildcard $(directory)/*.cpp))
DIRTY_CC_SRCS  := $(foreach directory,$(DIRTY_SRC_DIRS),$(wildcard $(directory)/*.c))
DIRTY_CXX_OBJS ?= $(addprefix $(BUILD_OUT)/,$(subst .cpp,.obj,$(DIRTY_CXX_SRCS:src/%=%)))
DIRTY_CC_OBJS  ?= $(addprefix $(BUILD_OUT)/,$(subst .c,.o,$(DIRTY_CC_SRCS:src/%=%)))

SRCS  = $(CXX_SRCS) $(DIRTY_CXX_SRCS) $(CC_SRCS) $(DIRTY_CC_SRCS)
OBJS ?= $(CXX_OBJS) $(DIRTY_CXX_OBJS) $(CC_OBJS) $(DIRTY_CC_OBJS)

VPATH := $(SRC_DIRS) $(DIRTY_SRC_DIRS)

RESET   = \\033[0m
RED     = \\033[31m
GREEN   = \\033[32m
YELLOW  = \\033[33m
BLUE    = \\033[34m
MAGENTA = \\033[35m
CYAN    = \\033[36m
WHITE   = \\033[37m
DEFAULT = \\033[39m
GREY    = \\033[90m

.PHONY: default sublime linux windows debug release build clean dirty_clean

default:
	@ echo -e "::Compile command: $(WHITE)$(CXX:@%=%) $(YELLOW)(CXXFLAGS) (INCLUDE)$(WHITE) -c <source file> -o <object file>$(RESET)"
	@ echo -e "::Variable Definitions:"
	@ echo -e "\t$(YELLOW)CXXFLAGS: $(WHITE)$(CXXFLAGS)$(RESET)"
	@ echo -e "\t$(YELLOW)INCLUDE: $(WHITE)$(INCLUDE)$(RESET)\n"

	@ $(MAKE) -s CXXFLAGS="$(CXXFLAGS)" INCLUDE="$(INCLUDE)" APP_VERSION="$(APP_VERSION)" BUILD_OUT="$(BUILD_OUT)" $(OBJS)

	@ echo -e "::Linking command: $(WHITE)$(CXX:@%=%) (CXXFLAGS) (INCLUDE) $(YELLOW)(OBJS) $(WHITE)-o$(YELLOW) (BUILD_OUT)$(WHITE)/$(YELLOW)(APP_OUT) (LIBRARIES)$(RESET)"
	@ echo -e "::Variables:"
	@ echo -e "\t$(YELLOW)LIBRARIES: $(WHITE)$(LIBRARIES)$(RESET)"
	@ echo -e "\t$(YELLOW)OBJS: $(WHITE)all the object files previously compiled$(RESET)"
	@ echo -e "\t$(YELLOW)BUILD_OUT: $(WHITE)$(BUILD_OUT)$(RESET)"
	@ echo -e "\t$(YELLOW)APP_OUT: $(WHITE)$(APP_OUT)$(RESET)\n"

	$(MAKE) -s CXXFLAGS="$(CXXFLAGS)" INCLUDE="$(INCLUDE)" LIBRARIES="$(LIBRARIES)" APP_VERSION="$(APP_VERSION)" BUILD_OUT="$(BUILD_OUT)" $(BUILD_OUT)/$(APP_OUT)

	$(eval BUILD_OUT=$(BUILD_OUT))
	$(eval APP_OUT=$(APP_OUT))

sublime: ;@:
	$(eval RESET="")
	$(eval RED="")
	$(eval GREEN="")
	$(eval YELLOW="")
	$(eval BLUE="")
	$(eval MAGENTA="")
	$(eval CYAN="")
	$(eval WHITE="")
	$(eval DEFAULT="")
	$(eval GREY="")

linux: ;@:
	$(eval INCLUDE_SELECT = $(INCLUDE_LINUX))
	$(eval LIBRARIES_SELECT = $(LIBRARIES_LINUX))
	$(eval PROGRAM_FLAGS = $(LINUX_FLAGS))
	$(eval CXX_SELECT = $(LINUX_CXX))
	$(eval CC_SELECT = $(LINUX_CC))
	$(eval APP_ARCH = $(APP_NAME_LINUX))
	$(eval BUILD_ARCH = $(BUILD_PATH_LINUX))

windows: ;@:
	$(eval INCLUDE_SELECT = $(INCLUDE_WINDOWS))
	$(eval LIBRARIES_SELECT = $(LIBRARIES_WINDOWS))
	$(eval PROGRAM_FLAGS = $(WINDOWS_FLAGS))
	$(eval CXX_SELECT = $(WINDOWS_CXX))
	$(eval CC_SELECT = $(WINDOWS_CC))
	$(eval APP_ARCH = $(APP_NAME_WINDOWS))
	$(eval BUILD_ARCH = $(BUILD_PATH_WINDOWS))

debug: PROGRAM_FLAGS += $(DEBUG_FLAGS)
debug: APP_VERSION = $(APP_NAME_DEBUG)
debug: BUILD_VERSION = $(BUILD_PATH_DEBUG)
debug:
	@ if [ $(CXX_SELECT:@%=%) == x86_64-w64-mingw32-g++ ]; then echo 'Removing address sanitizer for mingw build' $(eval DEBUG_FLAGS = $(filter-out -fsanitize=address,$(DEBUG_FLAGS))); fi
	$(call recursive_make)

release: APP_VERSION = $(APP_NAME_RELEASE)
release: BUILD_VERSION = $(BUILD_PATH_RELEASE)
release:
	$(call recursive_make)

define recursive_make
	@ $(MAKE) -s CXX_SELECT="$(CXX_SELECT)" CC_SELECT="$(CC_SELECT)" CXXFLAGS="$(CXXFLAGS)" INCLUDE="$(INCLUDE)" LIBRARIES="$(LIBRARIES)" APP_VERSION="$(APP_VERSION)" BUILD_VERSION="$(BUILD_VERSION)" BUILD_ARCH="$(BUILD_ARCH)" BUILD_OUT="$(BUILD_OUT)" APP_OUT="$(APP_OUT)"
endef


build:
	@ -mkdir -p $(BUILD_OUT)


$(BUILD_OUT)/$(APP_OUT):
	@ echo -e "Linking: $(CYAN)$@$(RESET)"
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(OBJS) -o $@ $(LIBRARIES)
	@ echo -e "Finished Linking: $(GREEN)$(BUILD_OUT)/$(APP_OUT)$(RESET)"

$(BUILD_OUT)/%.obj: src/%.cpp | build
	@ echo -e "Compiling: $(WHITE)$<$(RESET) -> $(CYAN)$@$(RESET)"
	@ -mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@
	@ echo -e "Finished Compiling: $(GREEN)$@$(RESET)"

$(BUILD_OUT)/%.o: src/%.c | build
	@ echo -e "Compiling: $(WHITE)$<$(RESET) -> $(CYAN)$@$(RESET)"
	@ -mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) $(INCLUDE) -c $< -o $@
	@ echo -e "Finished Compiling: $(GREEN)$@$(RESET)"

IS_CLEAN = $(shell [ -d $(OUT) ]; echo $$? )

clean:
ifeq ($(IS_CLEAN), 1)
	@ echo "Build is already clean"
else
	@ -rm -rf $(BUILD_ROOT)
	@ echo -e "Cleaned: $(RED)$(BUILD_ROOT)/$(RESET)"
endif

define selective_clean
	@ if [ -d $(1) ]; then echo -e "Cleaned: $(RED)$(1)/$(RESET)"; fi
	@ -rm -rf $(1)
endef

# These variables help make 'dirty_clean' less awful
BUILD_LINUX_RELEASE   := $(BUILD_ROOT)/$(BUILD_PATH_LINUX)/$(BUILD_PATH_RELEASE)
BUILD_LINUX_DEBUG     := $(BUILD_ROOT)/$(BUILD_PATH_LINUX)/$(BUILD_PATH_DEBUG)
BUILD_WINDOWS_RELEASE := $(BUILD_ROOT)/$(BUILD_PATH_WINDOWS)/$(BUILD_PATH_RELEASE)
BUILD_WINDOWS_DEBUG   := $(BUILD_ROOT)/$(BUILD_PATH_WINDOWS)/$(BUILD_PATH_DEBUG)

dirty_clean:
	$(call selective_clean,$(BUILD_LINUX_RELEASE))
	$(call selective_clean,$(BUILD_LINUX_DEBUG))
	$(call selective_clean,$(BUILD_WINDOWS_RELEASE))
	$(call selective_clean,$(BUILD_WINDOWS_DEBUG))