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
DEBUG_FLAGS := -g -Wall -O0 -D NOSTALGIA_DEBUGGING
RELEASE_FLAGS := -O3
DYNAMIC_FLAGS := -fPIC
export LIBRARY_FLAGS ?=
COMMON_FLAGS := -frtti -D COMPILER_FORWARD_DECLARATIONS
COMMON_CXXFLAGS := -std=c++20
WINDOWS_FLAGS := -mwindows
LINUX_FLAGS :=

LINUX_INCLUDE := -I src/system/linux/common
WINDOWS_INCLUDE := -I src/system/windows/common
COMMON_INCLUDE := -I src -I src/thirdparty

LINUX_GLFW := src/lib/glfw-lib-linux/libglfw3.a
ifeq ($(OS),Windows_NT)
WINDOWS_GLFW := src/lib/glfw-lib-mingw-w64/libglfw3.dll src/lib/glfw-lib-mingw-w64/libglfw3dll.a # idk what I'm doing, lmfao
else
WINDOWS_GLFW := src/lib/glfw-lib-mingw-w64/libglfw3.a
endif

BUILD_ROOT         := build
BUILD_PATH_LINUX   := linux
BUILD_PATH_WINDOWS := windows
BUILD_PATH_RELEASE := release
BUILD_PATH_DEBUG   := debug
BUILD_PATH_DYNAMIC := dynamic
BUILD_PATH_STATIC  := static

export LIB_BUILD_ARCH    ?= $(BUILD_PATH_LINUX)
export LIB_BUILD_VERSION ?= $(BUILD_PATH_RELEASE)
export LIB_BUILD_TYPE    ?= $(BUILD_PATH_STATIC)

export LIB_BUILD_DIR ?= $(BUILD_ROOT)/$(LIB_BUILD_ARCH)_$(LIB_BUILD_TYPE)_$(LIB_BUILD_VERSION)
export LIB_BUILD_OBJS_DIR ?= $(LIB_BUILD_DIR)/.object_files

export CXX         ?= $(LINUX_CXX)
export CC          ?= $(LINUX_CC)
export CXXFLAGS    ?= $(COMMON_FLAGS) $(COMMON_CXXFLAGS)
export CCFLAGS     ?= $(COMMON_FLAGS)
export LIB_INCLUDE ?= $(COMMON_INCLUDE) $(LINUX_INCLUDE)
export GLFW_LIB    ?= $(LINUX_GLFW)
ifeq ($(OS),Windows_NT)
export CXX         ?= $(WINDOWS_CXX)
export CC          ?= $(WINDOWS_CC)
export LIB_INCLUDE ?= $(COMMON_INCLUDE) $(WINDOWS_INCLUDE)
export GLFW_LIB    ?= $(WINDOWS_GLFW)
endif

THIRDPARTY_SRC_DIRS :=       \
	src/thirdparty/DearImGui \
	src/thirdparty/glad

SRC_DIRS :=                         \
	src/backends                    \
	src/backends/graphics           \
	src/backends/windowing          \
	src/common                      \
	src/console                     \
	src/embedded                    \
	src/events                      \
	src/input                       \
	src/managers                    \
	src/rendering                   \
	src/rendering/shader_interfaces \
	src/things                      \
	src/things/actors               \
	$(THIRDPARTY_SRC_DIRS)          \
	src/ui                          \
	src/world                       \

TEST_APP_SRC_DIRS :=            \
	src/test_application/app    \
	src/test_application/system \
	src/test_application/ui     \

RESOURCES_DIR := src/resources

TEST_APP_SRCS := $(foreach directory,$(TEST_APP_SRC_DIRS),$(wildcard $(directory)/*.cpp))
export TEST_APP_OBJS ?= $(addprefix $(LIB_BUILD_OBJS_DIR)/,$(subst .cpp,.obj,$(TEST_APP_SRCS:src/%=%)))

CXX_SRCS := $(foreach directory,$(SRC_DIRS),$(wildcard $(directory)/*.cpp))
CC_SRCS  := $(foreach directory,$(SRC_DIRS),$(wildcard $(directory)/*.c))
export LIB_CXX_OBJS ?= $(addprefix $(LIB_BUILD_OBJS_DIR)/,$(subst .cpp,.obj,$(CXX_SRCS:src/%=%)))
export LIB_CC_OBJS  ?= $(addprefix $(LIB_BUILD_OBJS_DIR)/,$(subst .c,.o,$(CC_SRCS:src/%=%)))

export GLFW_OBJS ?= $(LIB_BUILD_OBJS_DIR)/glfw_extracted_object_files

HEADER_FILES := $(foreach directory,$(SRC_DIRS),$(wildcard $(directory)/*.hpp) $(wildcard $(directory)/*.h))
export HEADERS_OUT  ?= $(addprefix $(LIB_BUILD_DIR)/include/,$(HEADER_FILES:src/%=%))

AR := @llvm-ar
DYNAMIC_LIBRARY_LINUX_FLAGS := -fvisibility=hidden
export DYNAMIC_LIBRARY_FLAGS ?= -fPIC -shared

LINUX_LIBRARY_DYNAMIC   := .so
WINDOWS_LIBRARY_DYNAMIC := .dll
export LIBRARY_DYNAMIC ?= $(LINUX_LIBRARY_DYNAMIC)
LIBRARY_STATIC  := .a

export LIBRARY_TYPE ?= $(LIBRARY_STATIC)

WINDOWS_DYNAMIC_LIBRARY_LD_FLAGS := -shared --out-implib $(LIB_BUILD_DIR)/$(LIBRARY_NAME).dll.a
WINDOWS_DYNAMIC_LIBRARY_COMPILE_FLAGS := -DMYLIB_EXPORT
LINUX_DYNAMIC_LIBRARY_LD_FLAGS := -shared
LINUX_DYNAMIC_LIBRARY_COMPILE_FLAGS := -fvisibility=hidden -fvisibility-inlines-hidden

export DYNAMIC_LIBRARY_LD_FLAGS ?= $(LINUX_DYNAMIC_LIBRARY_LD_FLAGS)
export DYNAMIC_LIBRARY_COMPILE_FLAGS ?= $(LINUX_DYNAMIC_LIBRARY_COMPILE_FLAGS)

LIBRARY_NAME_BASE := NostalgiaEngine
export LIBRARY_NAME ?= lib$(LIBRARY_NAME_BASE)

.PHONY: install build resources linux windows release debug static dynamic clean

install: resources build
	@ $(MAKE) -s $(HEADERS_OUT) $(LIB_CC_OBJS) $(LIB_CXX_OBJS) $(LIB_BUILD_DIR)/$(LIBRARY_NAME)$(LIBRARY_TYPE)
	@ echo -e "Successfully made: $(LIB_BUILD_DIR)/$(LIBRARY_NAME)$(LIBRARY_TYPE)"
	@ echo -e "To use Nostalgia for your project, you need the library file and the headers located in the \"include\" directory."

build:
	@ -mkdir -p $(LIB_BUILD_OBJS_DIR)

resources:
	@ $(MAKE) -s -C $(RESOURCES_DIR)

linux:
ifeq ($(OS),Windows_NT)
	$(eval LIB_BUILD_ARCH := $(BUILD_PATH_LINUX))
	$(eval LIB_INCLUDE    := $(COMMON_INCLUDE) $(LINUX_INCLUDE))
	$(eval DYNAMIC_LIBRARY_FLAGS += $(DYNAMIC_LIBRARY_LINUX_FLAGS))
	$(eval GLFW_LIB := $(LINUX_GLFW))
endif
	$(eval LIBRARY_DYNAMIC := $(LINUX_LIBRARY_DYNAMIC))
	$(eval DYNAMIC_LIBRARY_LD_FLAGS := $(LINUX_DYNAMIC_LIBRARY_LD_FLAGS))
	$(eval DYNAMIC_LIBRARY_COMPILE_FLAGS := $(LINUX_DYNAMIC_LIBRARY_COMPILE_FLAGS))
	$(eval CXXFLAGS += $(COMMON_FLAGS) $(COMMON_CXXFLAGS) $(LINUX_FLAGS))
	$(eval CXX := $(LINUX_CXX))
	$(eval CC := $(LINUX_CC))
	@ echo -e "::Architecture - Linux"

windows:
ifneq ($(OS),Windows_NT)
	$(eval LIB_BUILD_ARCH := $(BUILD_PATH_WINDOWS))
	$(eval LIB_INCLUDE    := $(COMMON_INCLUDE) $(WINDOWS_INCLUDE))
	$(eval GLFW_LIB   := $(WINDOWS_GLFW))
endif
	$(eval LIBRARY_DYNAMIC := $(WINDOWS_LIBRARY_DYNAMIC))
	$(eval DYNAMIC_LIBRARY_LD_FLAGS := $(WINDOWS_DYNAMIC_LIBRARY_LD_FLAGS))
	$(eval DYNAMIC_LIBRARY_COMPILE_FLAGS := $(WINDOWS_DYNAMIC_LIBRARY_COMPILE_FLAGS))
	$(eval CXXFLAGS := $(filter-out -fsanitize=address,$(CXXFLAGS)) $(COMMON_FLAGS) $(COMMON_CXXFLAGS) $(WINDOWS_FLAGS))
	$(eval CXX := $(WINDOWS_CXX))
	$(eval CC := $(WINDOWS_CC))
	@ echo -e "::Architecture - Windows"

debug:
	$(eval LIB_BUILD_VERSION := $(BUILD_PATH_DEBUG))
	$(eval CXXFLAGS += $(DEBUG_FLAGS) -I src/test_application)
	$(eval LIB_CXX_OBJS := $(LIB_CXX_OBJS) $(TEST_APP_OBJS))
	$(eval LIBRARY_NAME := $(LIBRARY_NAME_BASE))
	$(eval LIBRARY_TYPE := .x86_64)
	@ echo -e "::Version - Debug"

release:
	$(eval LIBRARY_NAME := lib$(LIBRARY_NAME_BASE))
	$(eval LIB_BUILD_VERSION := $(BUILD_PATH_RELEASE))
	$(eval CXXFLAGS += $(RELEASE_FLAGS))
	@ echo -e "::Version - Release"

static:
	$(eval LIB_BUILD_TYPE := $(BUILD_PATH_STATIC))
	$(eval LIBRARY_TYPE := $(LIBRARY_STATIC))
	$(eval COMPILE_LIBRARY := $(COMPILE_STATIC_LIBRARY))
	$(eval LIBRARY_FLAGS := )
	@ echo -e "::Library Type - Static"

dynamic:
	$(eval LIB_BUILD_TYPE := $(BUILD_PATH_DYNAMIC))
	$(eval LIBRARY_TYPE := $(LIBRARY_DYNAMIC))
	$(eval COMPILE_LIBRARY := $(COMPILE_DYNAMIC_LIBRARY))
	$(eval LIBRARY_FLAGS := $(DYNAMIC_FLAGS) $(DYNAMIC_LIBRARY_COMPILE_FLAGS))
	@ echo -e "::Library Type - Dynamic"

clean:
	@ -rm -rf $(BUILD_ROOT)

# Test Application
$(LIB_BUILD_DIR)/$(LIBRARY_NAME_BASE).x86_64:
	@ echo -e "Linking Test Application: $@"
	$(CXX) $(CXXFLAGS) $^ $(GLFW_LIB) -o $@

# Static Library
$(LIB_BUILD_DIR)/$(LIBRARY_NAME)$(LIBRARY_STATIC):
	@ $(shell mkdir -p $(GLFW_OBJS))
	@ $(shell cd $(GLFW_OBJS) && $(AR:@%=%) x ../../../../$(GLFW_LIB))
	@ echo -e "Building: $@"
	$(AR) cr $@ $(wildcard $(GLFW_OBJS)/*.o) $^

# Dynamic Library
$(LIB_BUILD_DIR)/$(LIBRARY_NAME)$(LIBRARY_DYNAMIC):
	@ echo -e "Building: $@"
	$(CXX) $(CXXFLAGS) $(DYNAMIC_LIBRARY_LD_FLAGS) $(GLFW_LIB) $^ -o $@

$(LIB_BUILD_OBJS_DIR)/%.o: src/%.c | build
	@ echo -e "Compiling: $< -> $@"
	@ -mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) $(LIB_INCLUDE) -c $< -o $@

$(LIB_BUILD_OBJS_DIR)/%.obj: src/%.cpp | build
	@ echo -e "Compiling: $< -> $@"
	@ -mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(LIB_INCLUDE) -c $< -o $@

$(LIB_BUILD_DIR)/include/%.hpp: src/%.hpp | build
	@ echo -e "Including Header File: $@"
	@ -mkdir -p $(dir $@)
	@ cp $< $@

$(LIB_BUILD_DIR)/include/%.h: src/%.h | build
	@ echo -e "Including Header File: $@"
	@ -mkdir -p $(dir $@)
	@ cp $< $@
