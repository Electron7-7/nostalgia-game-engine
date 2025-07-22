LCXX := clang++
LCC  := clang

ifeq ($(OS),Windows_NT)
	WCXX := g++
	WCC  := gcc
else
	WCXX := x86_64-w64-mingw32-g++
	WCC  := x86_64-w64-mingw32-gcc
endif

LAR := llvm-ar
# TODO: Figure out if I need a windows-specific 'ar' and what it would be
WAR := $(LAR)

COMMON_DEBUG_FLAGS := -g -Wall -O0 -D DEBUGGING
COMMON_CXX_FLAGS   := -std=c++20 -frtti
LINUX_DEBUG_FLAGS  := -fsanitize=address
WINDOWS_FLAGS      := -mwindows
LINUX_FLAGS        := # Nothing yet
GLFW_LINUX         := src/lib/glfw-lib-linux
GLFW_WINDOWS       := src/lib/glfw-lib-mingw-w64

INCLUDE := -I src -I src/thirdparty

BUILD_ROOT      := build
BUILD_LINUX     := Linux
BUILD_WINDOWS   := Windows
BUILD_DEBUG     := Debug
BUILD_RELEASE   := Release

NAME_BASE            := libNostalgia
NAME_STATIC_LINUX    := .a
NAME_DYNAMIC_LINUX   := .so
NAME_STATIC_WINDOWS  := .lib
NAME_DYNAMIC_WINDOWS := .dll

APP_BASE := LibraryTestingApp

INSTALL_LIB_LINUX       := /usr/lib/
INSTALL_HEADERS_LINUX   := /usr/include/

LINUX_DYNAMIC_LDFLAGS   := -shared
WINDOWS_DYNAMIC_LDFLAGS := -shared --out-implib
export COMMON_DYNAMIC_LDFLAGS ?= -L $(GLFW_LIB) -l glfw

export APP         ?= $(APP_BASE)
export BUILD_DIR   ?= $(BUILD_ROOT)/$(BUILD_LINUX)/$(BUILD_VERSION)
export DEBUG_FLAGS ?= $(COMMON_DEBUG_FLAGS) $(LINUX_DEBUG_FLAGS)
export GLFW_LIB    ?= $(GLFW_LINUX)
export CXX         ?= $(LCXX)
export CC          ?= $(LCC)
export AR          ?= $(LAR)
export NAME_STATIC     ?= $(NAME_STATIC_LINUX)
export NAME_DYNAMIC    ?= $(NAME_DYNAMIC_LINUX)
export DYNAMIC_LDFLAGS ?= $(COMMON_DYNAMIC_LDFLAGS) $(LINUX_DYNAMIC_LDFLAGS)
ifeq ($(OS),Windows_NT)
	export APP         ?= $(APP_BASE).exe
	export BUILD_DIR   ?= $(BUILD_ROOT)/$(BUILD_WINDOWS)/$(BUILD_VERSION)
	export DEBUG_FLAGS ?= $(COMMON_DEBUG_FLAGS)
	export GLFW_LIB    ?= $(GLFW_WINDOWS)
	export CXX         ?= $(WCXX)
	export CC          ?= $(WCC)
	export AR          ?= $(WAR)
	export NAME_STATIC     ?= $(NAME_STATIC_WINDOWS)
	export NAME_DYNAMIC    ?= $(NAME_DYNAMIC_WINDOWS)
	export DYNAMIC_LDFLAGS ?= $(COMMON_DYNAMIC_LDFLAGS) $(WINDOWS_DYNAMIC_LDFLAGS)
endif

export VERSION_FLAGS  ?=
export NAME_EXTENSION ?= $(NAME_STATIC)
export APP_LDFLAGS    ?= $(BUILD_DIR)/lib/libNostalgia$(NAME_EXTENSION)

export BUILD_LIB     ?= $(BUILD_DIR)/lib
export BUILD_HEADERS ?= $(BUILD_DIR)/include/Nostalgia
export OBJS_DIR      ?= .objs_static
export BUILD_OBJS    ?= $(BUILD_DIR)/$(OBJS_DIR)
export BUILD_GLFW    ?= $(BUILD_OBJS)/thirdparty/libglfw3
export BUILD_VERSION ?= $(BUILD_RELEASE)
export CXXFLAGS      ?= $(COMMON_CXX_FLAGS) $(CXX_FLAGS) $(VERSION_FLAGS)
export CCFLAGS       ?= $(COMMON_CC_FLAGS)

VPATH := $(SRC_DIRS)

THIRDPARTY_SRC_DIRS :=       \
	src/thirdparty/DearImGui \
	src/thirdparty/glad

SRC_DIRS :=                          \
	src/common                       \
	src/console                      \
	src/embedded                     \
	src/events                       \
	src/input                        \
	src/managers                     \
	src/math                         \
	src/rendering                    \
	src/rendering/backends           \
	src/rendering/backends/graphics  \
	src/rendering/backends/windowing \
	src/rendering/shader_interfaces  \
	src/things                       \
	src/things/actors                \
	src/ui                           \
	src/world                        \

TEST_APP_SRC_DIRS :=       \
	src/testing_app/app    \
	src/testing_app/system \
	src/testing_app/ui     \

# Only one for now
DIRTY_DIR := thirdparty

RESOURCES_DIR := src/resources

get_source_files = $(foreach directory,$(1),$(wildcard $(directory)/$(2)))

CC_SRCS  := $(call get_source_files,$(SRC_DIRS),*.c)   $(call get_source_files,$(THIRDPARTY_SRC_DIRS),*.c)
CXX_SRCS := $(call get_source_files,$(SRC_DIRS),*.cpp) $(call get_source_files,$(THIRDPARTY_SRC_DIRS),*.cpp)

export CC_OBJS  ?= $(addprefix $(BUILD_OBJS)/,$(subst .c,.o,$(CC_SRCS:src/%=%)))
export CXX_OBJS ?= $(addprefix $(BUILD_OBJS)/,$(subst .cpp,.obj,$(CXX_SRCS:src/%=%)))

APP_SRCS := $(foreach directory,$(TEST_APP_SRC_DIRS),$(wildcard $(directory)/*.cpp))
export APP_OBJS ?= $(addprefix $(BUILD_OBJS)/,$(subst .cpp,.obj,$(APP_SRCS:src/%=%)))

HEADER_FILES := $(call get_source_files,$(SRC_DIRS),*.hpp) $(call get_source_files,$(SRC_DIRS),*.h)
export HEADERS_OUT ?= $(addprefix $(BUILD_HEADERS)/,$(HEADER_FILES:src/%=%))

export RESET   ?= \\033[0m
export BLACK   ?= \\033[30m
export RED     ?= \\033[31m
export GREEN   ?= \\033[32m
export YELLOW  ?= \\033[33m
export BLUE    ?= \\033[34m
export MAGENTA ?= \\033[35m
export CYAN    ?= \\033[36m
export WHITE   ?= \\033[37m
export DEFAULT ?= \\033[39m


.PHONY: all install uninstall static dynamic headers statictestapp dynamictestapp sublime linux windows debug release build clean clean_dirty

all: build headers static dynamic statictestapp dynamictestapp ;@:

install:
ifndef ($(OS),Windows_NT)
	@ printf "[Note: if you want to install a specific version, specify it when you run make (i.e: make debug install). The Makefile defaults to the Release version, which I recommend installing over the Debug version]\n"
	cp -r $(BUILD_HEADERS) $(INSTALL_HEADERS_LINUX)
	-cp $(BUILD_LIB)/$(NAME_BASE)$(NAME_STATIC)  $(INSTALL_LIB_LINUX)
	-cp $(BUILD_LIB)/$(NAME_BASE)$(NAME_DYNAMIC) $(INSTALL_LIB_LINUX)
else
	@ printf "Because Windows is stupid you have to manually place the library and headers into whichever directories you want/need to... which is super messy and annoying but that's why you use Linux.\n"
endif

uninstall:
ifndef ($(OS),Windows_NT)
	@ printf "[Note: if you installed the debug version, you'll have to delete the library files yourself (they should be in $(INSTALL_LIB_LINUX))]\n"
	rm -f  $(INSTALL_LIB_LINUX)$(NAME_BASE)$(NAME_STATIC)
	rm -f  $(INSTALL_LIB_LINUX)$(NAME_BASE)$(NAME_DYNAMIC)
	rm -rf $(INSTALL_HEADERS_LINUX)Nostalgia
else
	@ printf "Uh... I have no clue where you installed the library & headers. Yet another reason why Windows is stinky.\n"
endif

static: resources
	$(eval OBJS_DIR := .objs_static)
	@ -rm -f $(BUILD_LIB)/$(NAME_BASE)$(NAME_STATIC)
	@ $(MAKE) -s $(CC_OBJS) $(CXX_OBJS) $(BUILD_LIB)/$(NAME_BASE)$(NAME_STATIC)

dynamic: CXXFLAGS += -fPIC
dynamic: CCFLAGS  += -fPIC
dynamic: resources
	$(eval OBJS_DIR := .objs_dynamic)
	@ -rm -f $(BUILD_LIB)/$(NAME_BASE)$(NAME_DYNAMIC)
	@ $(MAKE) -s $(CC_OBJS) $(CXX_OBJS) $(BUILD_LIB)/$(NAME_BASE)$(NAME_DYNAMIC)

headers:
	@ $(MAKE) -s $(HEADERS_OUT)

statictestapp:
	$(eval APP := Static$(APP))
	$(eval NAME_EXTENSION := $(NAME_STATIC))
	$(eval OBJS_DIR := .objs_static)
	@ -rm -f $(BUILD_DIR)/$(APP)
	@ $(MAKE) -s $(APP_OBJS)
	@ $(MAKE) -s $(BUILD_DIR)/$(APP)

dynamictestapp:
	$(eval APP := Dynamic$(APP))
	$(eval NAME_EXTENSION := $(NAME_DYNAMIC))
	$(eval OBJS_DIR := .objs_dynamic)
	@ -rm -f $(BUILD_DIR)/$(APP)
	@ $(MAKE) -s $(APP_OBJS)
	@ $(MAKE) -s $(BUILD_DIR)/$(APP)

# This target is for disabling the ANSI colors. The reason it's called 'sublime' (and an example use-case) is because
# Sublime Text's output panel doesn't support ANSI colors natively, so I call this target in every build system that's
# in my Sublime Text project file for Nostalgia.
sublime:
	$(eval RESET   := "")
	$(eval BLACK   := "")
	$(eval RED     := "")
	$(eval GREEN   := "")
	$(eval YELLOW  := "")
	$(eval BLUE    := "")
	$(eval MAGENTA := "")
	$(eval CYAN    := "")
	$(eval WHITE   := "")
	$(eval DEFAULT := "")
	@ printf "::Output colors disabled\n"

linux:
	@ printf "$(DEFAULT)::Setting Architecture to Linux$(RESET)\n"
ifeq ($(OS),Windows_NT)
	$(eval APP         := $(APP_BASE))
	$(eval BUILD_DIR   := $(BUILD_ROOT)/$(BUILD_LINUX)/$(BUILD_VERSION))
	$(eval DEBUG_FLAGS := $(COMMON_DEBUG_FLAGS) $(LINUX_DEBUG_FLAGS))
	$(eval GLFW_LIB    := $(GLFW_LINUX))
	$(eval CXX_FLAGS   := $(LINUX_FLAGS))
	$(eval NAME_STATIC     := $(NAME_STATIC_LINUX))
	$(eval NAME_DYNAMIC    := $(NAME_DYNAMIC_LINUX))
	$(eval DYNAMIC_LDFLAGS := $(COMMON_DYNAMIC_LDFLAGS) $(LINUX_DYNAMIC_LDFLAGS))
endif
	$(eval CXX := $(LCXX))
	$(eval CC  := $(LCC))
	$(eval AR  := $(LAR))

windows:
	@ printf "$(DEFAULT)::Setting Architecture to Windows$(RESET)\n"
ifneq ($(OS),Windows_NT)
	$(eval APP         := $(APP_BASE).exe)
	$(eval BUILD_DIR   := $(BUILD_ROOT)/$(BUILD_WINDOWS)/$(BUILD_VERSION))
	$(eval DEBUG_FLAGS := $(COMMON_DEBUG_FLAGS))
	$(eval GLFW_LIB    := $(GLFW_WINDOWS))
	$(eval CXX_FLAGS   := $(WINDOWS_FLAGS))
	$(eval NAME_STATIC     := $(NAME_STATIC_WINDOWS))
	$(eval NAME_DYNAMIC    := $(NAME_DYNAMIC_WINDOWS))
	$(eval DYNAMIC_LDFLAGS := $(COMMON_DYNAMIC_LDFLAGS) $(WINDOWS_DYNAMIC_LDFLAGS))
endif
	$(eval CXX := $(WCXX))
	$(eval CC  := $(WCC))
	$(eval AR  := $(WAR))

debug:
	@ printf "$(DEFAULT)::Setting Version to Debug$(RESET)\n"
	$(eval BUILD_VERSION := $(BUILD_DEBUG))
	$(eval VERSION_FLAGS := $(DEBUG_FLAGS))

release:
	@ printf "$(DEFAULT)::Setting Version to Release$(RESET)\n"
	$(eval BUILD_VERSION := $(BUILD_RELEASE))
	$(eval VERSION_FLAGS :=)

build:
	@ -mkdir -p $(BUILD_GLFW)
	@ -mkdir -p $(BUILD_HEADERS)
	@ -mkdir -p $(BUILD_LIB)

resources:
	@ $(MAKE) -s -C $(RESOURCES_DIR)

clean:
	@ -rm -rf $(BUILD_ROOT)

clean_dirty:
	@ printf " $(foreach architecture,$(wildcard $(BUILD_ROOT)/*),$(foreach version,$(wildcard $(architecture)/*),$(foreach clean_dir,$(shell ls --ignore='$(DIRTY_DIR)' $(version)/.objs*),$(shell rm -rf $(version)/.objs_static/$(clean_dir) $(version)/.objs_dynamic/$(clean_dir) &>/dev/null)Cleaned: $(DEFAULT)$(RED)$(version)/.objs/$(clean_dir)$(RESET)\\n)))\n"

# Static Library
$(BUILD_LIB)/$(NAME_BASE)$(NAME_STATIC_LINUX):
	@ $(shell cd $(BUILD_GLFW) && $(AR) x ../../../../../../$(GLFW_LIB)/libglfw3.a)
	@ printf "::Building $(GREEN)$@$(RESET)\n"
	$(AR) cr $@ $(wildcard $(BUILD_GLFW)/*.o) $(CC_OBJS) $(CXX_OBJS)

# Dynamic Library
$(BUILD_LIB)/$(NAME_BASE)$(NAME_DYNAMIC_LINUX):
	@ printf "::Building $(GREEN)$@$(RESET)\n"
	$(CXX) $(CXXFLAGS) $(CXX_OBJS) $(CC_OBJS) -o $@ $(DYNAMIC_LDFLAGS)

# Testing Application
$(BUILD_DIR)/$(APP): $(APP_OBJS)
	@ printf "::Linking $(GREEN)$@$(RESET)\n"
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@ $(APP_LDFLAGS)

$(BUILD_OBJS)/%.obj: src/%.cpp | build
	@ printf "::Compiling $(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(BUILD_OBJS)/%.o: src/%.c | build
	@ printf "::Compiling $(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) $(INCLUDE) -c $< -o $@

$(BUILD_HEADERS)/%.hpp: src/%.hpp | build
	@ printf "::Copying Header $(CYAN)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	@ cp $< $@

$(BUILD_HEADERS)/%.h: src/%.h | build
	@ printf "::Copying Header $(CYAN)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	@ cp $< $@
