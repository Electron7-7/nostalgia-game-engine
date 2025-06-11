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
export DEBUG_FLAGS := -g -Wall -O0 -fsanitize=address -D NOSTALGIA_DEBUGGING
export RELEASE_FLAGS := -O3
export COMMON_FLAGS := -std=c++20 -frtti -D COMPILER_FORWARD_DECLARATIONS

export LINUX_INCLUDE := -I src/system/linux/common
export WINDOWS_INCLUDE := -I src/system/windows/common
export COMMON_INCLUDE := -I src -I src/thirdparty

export LINUX_LIBRARIES := -L src/system/linux/lib
export WINDOWS_LIBRARIES := -L src/system/windows/lib


export BUILD_ROOT         := build
export BUILD_PATH_LINUX   := linux
export BUILD_PATH_WINDOWS := windows
export BUILD_PATH_RELEASE := release
export BUILD_PATH_DEBUG   := debug

export BUILD_ARCH    ?= $(BUILD_PATH_LINUX)
export BUILD_VERSION ?= $(BUILD_PATH_RELEASE)

export BUILD_DIR ?= $(BUILD_ROOT)/$(BUILD_ARCH)_$(BUILD_VERSION)
export BUILD_OBJS_DIR ?= $(BUILD_DIR)/object_files

CXX         ?= $(LINUX_CXX)
CC          ?= $(LINUX_CC)
CXXFLAGS    ?= $(COMMON_FLAGS)
INCLUDE     ?= $(COMMON_INCLUDE) $(LINUX_INCLUDE)
LIBRARIES   ?= $(LINUX_LIBRARIES)
ifeq ($(OS),Windows_NT)
CXX         ?= $(WINDOWS_CXX)
CC          ?= $(WINDOWS_CC)
INCLUDE     ?= $(COMMON_INCLUDE) $(WINDOWS_INCLUDE)
LIBRARIES   ?= $(WINDOWS_LIBRARIES)
endif

SRC_DIRS :=                         \
	src/input                       \
	src/common                      \
	src/managers                    \
	src/backends                    \
	src/embedded                    \
	src/rendering                   \
	src/backends/graphics           \
	src/backends/windowing          \
	src/rendering/shader_interfaces \
	$(THIRDPARTY_SRC_DIRS)

THIRDPARTY_SRC_DIRS :=       \
	src/thirdparty/DearImGui \
	src/thirdparty/glad

RESOURCES_DIR := src/resources

export CXX_SRCS := $(foreach directory,$(SRC_DIRS),$(wildcard $(directory)/*.cpp))
export CC_SRCS  := $(foreach directory,$(SRC_DIRS),$(wildcard $(directory)/*.c))
export CXX_OBJS ?= $(addprefix $(BUILD_OBJS_DIR)/,$(subst .cpp,.obj,$(CXX_SRCS:src/%=%)))
export CC_OBJS  ?= $(addprefix $(BUILD_OBJS_DIR)/,$(subst .c,.o,$(CC_SRCS:src/%=%)))

export HEADER_FILES := $(foreach directory,$(filter-out $(THIRDPARTY_SRC_DIRS),$(SRC_DIRS)),$(wildcard $(directory)/*.hpp) $(wildcard $(directory)/*.h))
export HEADERS_OUT  ?= $(addprefix $(BUILD_DIR)/include/,$(HEADER_FILES:src/%=%))


LIBRARY_COMPILER := @llvm-ar
LIBRARY_COMPILER_OPTIONS := rc

LIBRARY_STATIC  := .a
LIBRARY_DYNAMIC := .lib
export LIBRARY_TYPE    ?= $(LIBRARY_STATIC)

LIBRARY_NAME := libNostalgiaEngine

.PHONY: install build resources linux windows release debug clean

install: resources build
	@ $(MAKE) -s $(HEADERS_OUT) $(CC_OBJS) $(CXX_OBJS) $(BUILD_DIR)/$(LIBRARY_NAME)$(LIBRARY_TYPE)
	@ echo -e "Successfully compiled: $(BUILD_DIR)/$(LIBRARY_NAME)$(LIBRARY_TYPE)"
	@ echo -e "To use Nostalgia for your project, you need the library file and the headers located in the \"include\" directory."

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
	$(eval CXXFLAGS := $(filter-out $(RELEASE_FLAGS),$(CXXFLAGS)) $(COMMON_FLAGS) $(DEBUG_FLAGS))
	@ echo -e "::Version - Debug"

release:
	$(eval BUILD_VERSION := $(BUILD_PATH_RELEASE))
	$(eval CXXFLAGS := $(filter-out $(DEBUG_FLAGS),$(CXXFLAGS)) $(COMMON_FLAGS) $(RELEASE_FLAGS))
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

$(BUILD_DIR)/include/%.hpp: src/%.hpp | build
	@ echo -e "Including Header File: $@"
	@ -mkdir -p $(dir $@)
	@ cp $< $@

$(BUILD_DIR)/include/%.h: src/%.h | build
	@ echo -e "Including Header File: $@"
	@ -mkdir -p $(dir $@)
	@ cp $< $@

clean:
	@ -rm -rf $(BUILD_ROOT)
