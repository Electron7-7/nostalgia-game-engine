export
LINUX_CXX := @clang++
LINUX_CC  := @clang

ifeq ($(OS),Windows_NT)
WINDOWS_CXX := @g++
WINDOWS_CC  := @gcc
else
WINDOWS_CXX := @x86_64-w64-mingw32-g++
WINDOWS_CC  := @x86_64-w64-mingw32-gcc
endif

# LSAN_OPTIONS=verbosity=1:log_threads=1 # Use this environment variable for more verbosity with address sanitizer
DEBUG_FLAGS := -g -Wall -fsanitize=address -D NOSTALGIA_DEBUGGING
COMMON_FLAGS := -std=c++20 -frtti -D COMPILER_FORWARD_DECLARATIONS

LINUX_INCLUDE := -I src/system/linux/common
WINDOWS_INCLUDE := -I src/system/windows/common
COMMON_INCLUDE := -I src/ -I src/common -I src/engine

LINUX_LIBRARIES := -L src/system/linux/lib
WINDOWS_LIBRARIES := -L src/system/windows/lib


BUILD_ROOT         := build
BUILD_PATH_LINUX   := linux
BUILD_PATH_WINDOWS := windows
BUILD_PATH_RELEASE := release
BUILD_PATH_DEBUG   := debug

BUILD_ARCH    ?= $(BUILD_PATH_LINUX)
BUILD_VERSION ?= $(BUILD_PATH_RELEASE)

BUILD_DIR ?= $(BUILD_ROOT)/$(BUILD_ARCH)_$(BUILD_VERSION)
BUILD_OBJS_DIR ?= $(BUILD_DIR)/object_files

ifeq ($(OS),Windows_NT)
CXX         ?= $(WINDOWS_CXX)
CC          ?= $(WINDOWS_CC)
INCLUDE     ?= $(COMMON_INCLUDE) $(WINDOWS_INCLUDE)
LIBRARIES   ?= $(WINDOWS_LIBRARIES)
else
CXX         ?= $(LINUX_CXX)
CC          ?= $(LINUX_CC)
INCLUDE     ?= $(COMMON_INCLUDE) $(LINUX_INCLUDE)
LIBRARIES   ?= $(LINUX_LIBRARIES)
endif
CXXFLAGS    ?= $(COMMON_FLAGS)


SRC_DIRS :=                                \
	src/engine                             \
	src/engine/embedded                    \
	src/engine/input                       \
	src/engine/managers                    \
	src/engine/rendering                   \
	src/engine/rendering/shader_interfaces \
	src/engine/backends                    \
	src/engine/backends/graphics           \
	src/engine/backends/windowing

RESOURCES_DIR := src/resources

CXX_SRCS := $(foreach directory,$(SRC_DIRS),$(wildcard $(directory)/*.cpp))
CC_SRCS  := $(foreach directory,$(SRC_DIRS),$(wildcard $(directory)/*.c))
CXX_OBJS ?= $(addprefix $(BUILD_OBJS_DIR)/,$(subst .cpp,.obj,$(CXX_SRCS:src/%=%)))
CC_OBJS  ?= $(addprefix $(BUILD_OBJS_DIR)/,$(subst .c,.o,$(CC_SRCS:src/%=%)))


LIBRARY_COMPILER := @llvm-ar
LIBRARY_COMPILER_OPTIONS := rc

LIBRARY_STATIC  := .a
LIBRARY_DYNAMIC := .lib
LIBRARY_TYPE    ?= $(LIBRARY_STATIC)

LIBRARY_NAME := libNostalgiaEngine

.PHONY: install build resources linux windows release debug clean

install: resources build $(CC_OBJS) $(CXX_OBJS) $(BUILD_DIR)/$(LIBRARY_NAME)$(LIBRARY_TYPE)
	@ echo -e "Successfully compiled: $(BUILD_DIR)/$(LIBRARY_NAME)$(LIBRARY_TYPE)"

build:
	@ -mkdir -p $(BUILD_OBJS_DIR)

resources:
	@ $(MAKE) -s -C $(RESOURCES_DIR)

linux:
ifeq ($(OS),Windows_NT)
	$(eval BUILD_ARCH := $(BUILD_PATH_LINUX))
	$(eval INCLUDE    := $(COMMON_INCLUDE) $(LINUX_INCLUDE))
	$(eval LIBRARIES  := $(LINUX_LIBRARIES))
endif
	@ echo -e "::Architecture - Linux"

windows:
ifneq ($(OS),Windows_NT)
	$(eval BUILD_ARCH := $(BUILD_PATH_WINDOWS))
	$(eval INCLUDE    := $(COMMON_INCLUDE) $(WINDOWS_INCLUDE))
	$(eval LIBRARIES  := $(WINDOWS_LIBRARIES))
endif
	$(eval CXXFLAGS := $(filter-out -fsanitize=address,$(CXXFLAGS)))
	@ echo -e "::Architecture - Windows"

debug:
	$(eval BUILD_VERSION := $(BUILD_PATH_DEBUG))
	$(eval CXXFLAGS := $(COMMON_FLAGS) $(DEBUG_FLAGS))
	@ echo -e "::Version - Debug"

release:
	$(eval BUILD_VERSION := $(BUILD_PATH_RELEASE))
	$(eval CXXFLAGS := $(filter-out $(DEBUG_FLAGS),$(CXXFLAGS)))
	@ echo -e "::Version - Release"


$(BUILD_DIR)/$(LIBRARY_NAME)$(LIBRARY_TYPE): $(CC_OBJS) $(CXX_OBJS) | build
	@ echo -e "Linking: $@"
	$(LIBRARY_COMPILER) $(LIBRARY_COMPILER_OPTIONS) $@ $^

$(BUILD_OBJS_DIR)/%.o: src/%.c | build
	@ echo -e "Compiling: $< -> $@"
	@ -mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) $(INCLUDE) -c $< -o $@

$(BUILD_OBJS_DIR)/%.obj: src/%.cpp | build
	@ echo -e "Compiling: $< -> $@"
	@ -mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean:
	@ -rm -rf $(BUILD_ROOT)
