# LINUX_CXX := @clang++
# LINUX_CC  := @clang

# ifeq ($(OS),Windows_NT)
# WINDOWS_CXX := @g++
# WINDOWS_CC  := @gcc
# else
# WINDOWS_CXX := @x86_64-w64-mingw32-g++
# WINDOWS_CC  := @x86_64-w64-mingw32-gcc
# endif

# # LSAN_OPTIONS=verbosity=1:log_threads=1 # Use this environment variable for more verbosity with address sanitizer
# LINUX_DEBUG_FLAGS := -g -Wall -fsanitize=address -D NOSTALGIA_DEBUGGING
# WINDOWS_DEBUG_FLAGS := $(filter-out -fsanitize=address,$(LINUX_DEBUG_FLAGS))
# COMMON_FLAGS := -std=c++20 -frtti -D COMPILER_FORWARD_DECLARATIONS

# LINUX_INCLUDE := -I src/system/linux/common
# WINDOWS_INCLUDE := -I src/system/windows/common
# COMMON_INCLUDE := -I src/ -I src/common -I src/engine

# LINUX_LIBRARIES := -L src/system/linux/lib -l glfw3
# WINDOWS_LIBRARIES := -L src/system/windows/lib -l glfw-lib-mingw-w64/glfw3 -l gdi32


# BUILD_ROOT         := build
# BUILD_PATH_LINUX   := linux
# BUILD_PATH_WINDOWS := windows
# BUILD_PATH_RELEASE := release
# BUILD_PATH_DEBUG   := debug

# BUILD_ARCH    ?= $(BUILD_PATH_LINUX)
# BUILD_VERSION ?= $(BUILD_PATH_RELEASE)

# ifeq ($(OS),Windows_NT)
# APP_NAME_LINUX   := Linux_cross-compiled-on-windows.x86_64
# else
# APP_NAME_LINUX   := $(shell uname -s)_$(subst .,_,$(shell uname -r)).$(shell uname -m)
# endif
# APP_NAME_WINDOWS := Win64.exe
# APP_NAME_RELEASE := Nostalgia
# APP_NAME_DEBUG   := DEBUG__Nostalgia

# THE ABOVE STUFF IS BETTER, BUT NOT FOR THIS PROJECT SO I'M SAVING IT


LINUX_CXX := @clang++
LINUX_CC  := @clang

ifeq ($(OS),Windows_NT)
	WINDOWS_CXX := @g++
	WINDOWS_CC  := @gcc
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
WINDOWS_FLAGS = $(COMMON_FLAGS) -static -mwindows

PROGRAM_FLAGS ?= $(LINUX_FLAGS)
CXXFLAGS = -std=c++20 -frtti $(PROGRAM_FLAGS)
CCFLAGS  = $(PROGRAM_FLAGS)

INCLUDE_COMMON  := -I src/ -I src/common
INCLUDE_LINUX   := $(INCLUDE_COMMON) -I src/system/linux/common   # Not yet implemented
INCLUDE_WINDOWS := $(INCLUDE_COMMON) -I src/system/windows/common # Not yet implemented
INCLUDE_SELECT ?= $(INCLUDE_LINUX)
INCLUDE = $(INCLUDE_SELECT)

# LIBRARIES_LINUX   := -lz -lbz2 -lpng16 -lbrotlidec -lpthread -L src/system/linux/lib -l glfw3 -l:libfreetype.a
LIBRARIES_LINUX   := -L src/system/linux/lib -l glfw3
LIBRARIES_WINDOWS := -L src/system/windows/lib -l:glfw-lib-mingw-w64/libglfw3.a -l gdi32
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
APP_NAME_ENGINE  := Engine
APP_NAME_EDITOR  := Editor
APP_NAME_LINUX   := $(shell uname -s)_$(subst .,_,$(shell uname -r)).$(shell uname -m)
APP_NAME_WINDOWS := Win64.exe

APP_VERSION ?= $(APP_NAME_RELEASE)
APP_TYPE ?= $(APP_NAME_ENGINE)
ifeq ($(OS),Windows_NT)
	APP_ARCH ?= $(APP_NAME_WINDOWS)
else
	APP_ARCH ?= $(APP_NAME_LINUX)
endif


BUILD_OUT ?= $(BUILD_ROOT)/$(BUILD_ARCH)/$(BUILD_VERSION)
APP_OUT ?= $(APP_VERSION)_$(APP_TYPE)_$(APP_ARCH)

ENGINE_DIRS :=                           \
	src/engine                             \
	src/engine/embedded                    \
	src/engine/input                       \
	src/engine/managers                    \
	src/engine/rendering                   \
	src/engine/rendering/shader_interfaces \
	src/engine/backends                    \
	src/engine/backends/graphics           \
	src/engine/backends/windowing

SRC_DIRS :=        \
	src/app        \
	src/system     \
	src/world

# Object files compiled from "DIRTY_SRC_DIRS" will not be cleaned during a dirty clean
DIRTY_SRC_DIRS :=        \
	src/common           \
	src/common/glad      \
	src/common/DearImGui


RESOURCES_DIR := src/resources

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


LIBRARY_OUT  := src/lib
LIBRARY_NAME := libNostalgiaEngine.a
LIBCOMPILER  := @llvm-ar rc
LIBRARY_SRCS := $(foreach directory,$(ENGINE_DIRS),$(wildcard $(directory)/*.cpp))
LIBRARY_OBJS ?= $(addprefix $(BUILD_OUT)/,$(subst .cpp,.obj,$(LIBRARY_SRCS:src/%=%)))


VPATH := $(SRC_DIRS) $(DIRTY_SRC_DIRS)

export RESET   = \\033[0m
export BLACK   = \\033[30m
export RED     = \\033[31m
export GREEN   = \\033[32m
export YELLOW  = \\033[33m
export BLUE    = \\033[34m
export MAGENTA = \\033[35m
export CYAN    = \\033[36m
export WHITE   = \\033[37m
export DEFAULT = \\033[39m


.PHONY: default sublime linux windows debug release resources build clean clean_debug clean_release clean_linux clean_windows clean_dirty

default:
	@ $(MAKE) -s ARCHITECTURE="$(BUILD_ARCH)" VERSION="$(BUILD_VERSION)" -C $(RESOURCES_DIR) # Embedded Resources

	@ echo -e "$(DEFAULT)::Compiling library objects$(RESET)"
	@ echo -e "$(DEFAULT)::Compile command: $(CXX:@%=%) $(YELLOW)(CXXFLAGS) (INCLUDE)$(DEFAULT) -c <source file> -o <object file>$(RESET)"
	@ echo -e "$(DEFAULT)::Variable Definitions:$(RESET)"
	@ echo -e "\t$(YELLOW)CXXFLAGS: $(DEFAULT)$(CXXFLAGS)$(RESET)"
	@ echo -e "\t$(YELLOW)INCLUDE: $(DEFAULT)$(INCLUDE)$(RESET)\n"

	@ $(MAKE) -s CXXFLAGS="$(CXXFLAGS)" INCLUDE="$(INCLUDE)" APP_VERSION="$(APP_VERSION)" BUILD_OUT="$(BUILD_OUT)" $(LIBRARY_OBJS)

	@ echo -e "$(DEFAULT)::Compiling library$(RESET)"
	@ $(MAKE) -s BUILD_OUT="$(BUILD_OUT)" library

	@ echo -e "$(DEFAULT)::Compiling application objects$(RESET)"
	@ echo -e "$(DEFAULT)::Compile command: $(CXX:@%=%) $(YELLOW)(CXXFLAGS) (INCLUDE)$(DEFAULT) -c <source file> -o <object file>$(RESET)"
	@ echo -e "$(DEFAULT)::Variable Definitions:$(RESET)"
	@ echo -e "\t$(YELLOW)CXXFLAGS: $(DEFAULT)$(CXXFLAGS)$(RESET)"
	@ echo -e "\t$(YELLOW)INCLUDE: $(DEFAULT)$(INCLUDE)$(RESET)\n"

	@ $(MAKE) -s CXXFLAGS="$(CXXFLAGS)" INCLUDE="$(INCLUDE)" APP_VERSION="$(APP_VERSION)" BUILD_OUT="$(BUILD_OUT)" $(OBJS)

	@ echo -e "$(DEFAULT)::Linking command: $(CXX:@%=%) (CXXFLAGS) (INCLUDE) $(DEFAULT)-o$(YELLOW) (BUILD_OUT)$(DEFAULT)/$(YELLOW)(APP_OUT) (LIBRARIES)$(RESET)"
	@ echo -e "$(DEFAULT)::Variables:$(RESET)"
	@ echo -e "\t$(YELLOW)LIBRARIES: $(DEFAULT)$(LIBRARIES)$(RESET)"
	@ echo -e "\t$(YELLOW)OBJS: $(DEFAULT)all the object files previously compiled$(RESET)"
	@ echo -e "\t$(YELLOW)BUILD_OUT: $(DEFAULT)$(BUILD_OUT)$(RESET)"
	@ echo -e "\t$(YELLOW)APP_OUT: $(DEFAULT)$(APP_OUT)$(RESET)\n"

	@ -rm -f $(BUILD_OUT)/$(APP_OUT) # in case it already exists
	@ $(MAKE) -s CXXFLAGS="$(CXXFLAGS)" INCLUDE="$(INCLUDE)" LIBRARIES="$(LIBRARIES) src/lib/libNostalgiaEngine.a" APP_VERSION="$(APP_VERSION)" BUILD_OUT="$(BUILD_OUT)" $(BUILD_OUT)/$(APP_OUT)

library:
	@ echo -e "$(DEFAULT)Building: $(CYAN)$(LIBRARY_OUT)/$(LIBRARY_NAME)$(RESET)"
	$(LIBCOMPILER) $(LIBRARY_OUT)/$(LIBRARY_NAME) $(LIBRARY_OBJS)
	@ echo -e "$(DEFAULT)Finished Building: $(GREEN)$(LIBRARY_OUT)/$(LIBRARY_NAME)$(RESET)"

# This target is for disabling the ANSI colors. The reason it's called 'sublime' (and an example use-case) is because
# Sublime Text's output panel doesn't support ANSI colors natively, so I call this target in every build system that's
# in my Sublime Text project file for Nostalgia.
sublime: ;@:
	$(eval export RESET="")
	$(eval export BLACK="")
	$(eval export RED="")
	$(eval export GREEN="")
	$(eval export YELLOW="")
	$(eval export BLUE="")
	$(eval export MAGENTA="")
	$(eval export CYAN="")
	$(eval export WHITE="")
	$(eval export DEFAULT="")

linux: ;@:
	@ echo -e "$(DEFAULT)::Building for Linux$(RESET)"
	$(eval INCLUDE_SELECT = $(INCLUDE_LINUX))
	$(eval LIBRARIES_SELECT = $(LIBRARIES_LINUX))
	$(eval PROGRAM_FLAGS = $(LINUX_FLAGS))
	$(eval CXX_SELECT = $(LINUX_CXX))
	$(eval CC_SELECT = $(LINUX_CC))
	$(eval APP_ARCH = $(APP_NAME_LINUX))
	$(eval BUILD_ARCH = $(BUILD_PATH_LINUX))

windows: ;@:
	@ echo -e "$(DEFAULT)::Building for Windows$(RESET)"
	$(eval INCLUDE_SELECT = $(INCLUDE_WINDOWS))
	$(eval LIBRARIES_SELECT = $(LIBRARIES_WINDOWS))
	$(eval PROGRAM_FLAGS = $(WINDOWS_FLAGS))
	$(eval CXX_SELECT = $(WINDOWS_CXX))
	$(eval CC_SELECT = $(WINDOWS_CC))
	$(eval APP_ARCH = $(APP_NAME_WINDOWS))
	$(eval BUILD_ARCH = $(BUILD_PATH_WINDOWS))
	$(eval DEBUG_FLAGS = $(filter-out -fsanitize=address,$(DEBUG_FLAGS)))

debug: PROGRAM_FLAGS += $(DEBUG_FLAGS)
debug: APP_VERSION = $(APP_NAME_DEBUG)
debug: BUILD_VERSION = $(BUILD_PATH_DEBUG)
debug:
	@ echo -e "$(DEFAULT)::Building Debug$(RESET)"
	$(call recursive_make)

release: APP_VERSION = $(APP_NAME_RELEASE)
release: BUILD_VERSION = $(BUILD_PATH_RELEASE)
release:
	@ echo -e "$(DEFAULT)::Building Release$(RESET)"
	$(call recursive_make)

resources:
	@ $(MAKE) -s ARCHITECTURE="$(BUILD_ARCH)" VERSION="$(BUILD_VERSION)" -C $(RESOURCES_DIR)


define recursive_make
	@ $(MAKE) -s OBJS="$(OBJS)" CXX_SELECT="$(CXX_SELECT)" CC_SELECT="$(CC_SELECT)" PROGRAM_FLAGS="$(PROGRAM_FLAGS)" CXX_FLAGS="$(CXX_FLAGS)" INCLUDE="$(INCLUDE)" LIBRARIES="$(LIBRARIES)" APP_VERSION="$(APP_VERSION)" BUILD_VERSION="$(BUILD_VERSION)" BUILD_ARCH="$(BUILD_ARCH)" BUILD_OUT="$(BUILD_OUT)" APP_OUT="$(APP_OUT)" RESET="$(RESET)" BLACK="$(BLACK)" RED="$(RED)" GREEN="$(GREEN)" YELLOW="$(YELLOW)" BLUE="$(BLUE)" MAGENTA="$(MAGENTA)" CYAN="$(CYAN)" WHITE="$(WHITE)" DEFAULT="$(DEFAULT)"
endef

build:
	@ -mkdir -p $(BUILD_OUT)

$(BUILD_OUT)/$(APP_OUT):
	@ echo -e "$(DEFAULT)Linking: $(CYAN)$@$(RESET)"
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(OBJS) -o $@ $(LIBRARIES)
	@ echo -e "$(DEFAULT)Finished Linking: $(GREEN)$(BUILD_OUT)/$(APP_OUT)$(RESET)"

$(BUILD_OUT)/%.obj: src/%.cpp | build
	@ echo -e "$(DEFAULT)Compiling: $(DEFAULT)$<$(RESET) -> $(CYAN)$@$(RESET)"
	@ -mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@
	@ echo -e "$(DEFAULT)Finished Compiling: $(GREEN)$@$(RESET)"

$(BUILD_OUT)/%.o: src/%.c | build
	@ echo -e "$(DEFAULT)Compiling: $(DEFAULT)$<$(RESET) -> $(CYAN)$@$(RESET)"
	@ -mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) $(INCLUDE) -c $< -o $@
	@ echo -e "$(DEFAULT)Finished Compiling: $(GREEN)$@$(RESET)"

#
# Clean Targets
#
define clean_with_message
	@ if [ -d $(1) ]; then echo -e "\n$(DEFAULT)Cleaned: $(RED)$(1)$(RESET)"; fi
	@ -rm -rf $(1)
endef

clean:
	$(call clean_with_message,$(BUILD_ROOT))

clean_debug:
	$(call clean_with_message,$(BUILD_LINUX_DEBUG))
	$(call clean_with_message,$(BUILD_WINDOWS_DEBUG))

clean_release:
	$(call clean_with_message,$(BUILD_LINUX_RELEASE))
	$(call clean_with_message,$(BUILD_WINDOWS_RELEASE))

clean_linux:
	$(call clean_with_message,$(BUILD_ROOT)/$(BUILD_PATH_LINUX))

clean_windows:
	$(call clean_with_message,$(BUILD_ROOT)/$(BUILD_PATH_WINDOWS))

# These variables help make 'clean_dirty' less awful
BUILD_LINUX_RELEASE   := $(BUILD_ROOT)/$(BUILD_PATH_LINUX)/$(BUILD_PATH_RELEASE)
BUILD_LINUX_DEBUG     := $(BUILD_ROOT)/$(BUILD_PATH_LINUX)/$(BUILD_PATH_DEBUG)
BUILD_WINDOWS_RELEASE := $(BUILD_ROOT)/$(BUILD_PATH_WINDOWS)/$(BUILD_PATH_RELEASE)
BUILD_WINDOWS_DEBUG   := $(BUILD_ROOT)/$(BUILD_PATH_WINDOWS)/$(BUILD_PATH_DEBUG)

define _dirty_clean
	$(shell if [ -d $(1) ]; then echo -e "Cleaned:\t$(DEFAULT)$(RED)$(1)$(RESET)"; rm -rf $(1); fi;)
endef

clean_dirty: ;@:
	@ echo -e $(foreach dir,$(SRC_DIRS:src/%=$(BUILD_LINUX_RELEASE)/%),$(call _dirty_clean,$(dir)))
	@ echo -e $(foreach dir,$(SRC_DIRS:src/%=$(BUILD_LINUX_DEBUG)/%),$(call _dirty_clean,$(dir)))
	@ echo -e $(foreach dir,$(SRC_DIRS:src/%=$(BUILD_WINDOWS_RELEASE)/%),$(call _dirty_clean,$(dir)))
	@ echo -e $(foreach dir,$(SRC_DIRS:src/%=$(BUILD_WINDOWS_DEBUG)/%),$(call _dirty_clean,$(dir)))
