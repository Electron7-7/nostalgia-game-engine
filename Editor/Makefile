LINUX_CXX := clang++
LINUX_CC  := clang

ifneq ($(OS),Windows_NT)
	WINDOWS_CXX := x86_64-w64-mingw32-g++
	WINDOWS_CC  := x86_64-w64-mingw32-gcc
else
	WINDOWS_CXX := g++
	WINDOWS_CC  := gcc
endif

FLAGS_DEBUG_COMMON    := -g -O0 -D NOSTALGIA_DEBUGGING
FLAGS_DEBUG_LINUX     := -D_GLIBCXX_DEBUG -fsanitize=address,undefined
FLAGS_DEBUG_WINDOWS   := # Nothing yet
FLAGS_RELEASE_COMMON  := -O3 -D NOSTALGIA_RELEASE
FLAGS_RELEASE_WINDOWS := # Nothing yet
FLAGS_RELEASE_LINUX   := # Nothing yet
FLAGS_CXX_COMMON      := -std=c++26 -Wall -MMD -MP -Wno-c23-extensions
FLAGS_CC_COMMON       := -std=c23 -Wall -MMD -MP
FLAGS_WINDOWS         := -mwindows -lstdc++exp
FLAGS_LINUX           := # Nothing yet

DYNAMIC_LIBRARIES       := -lglfw -lfreetype -lgetargs -l:libNostalgia.so
DYNAMIC_LDFLAGS_LINUX   := $(DYNAMIC_LIBRARIES)
DYNAMIC_LDFLAGS_WINDOWS := -fuse-ld=x86_64-w64-mingw32-ld -lstdc++exp $(DYNAMIC_LIBRARIES)

STATIC_LDFLAGS_LINUX   := -l:libNostalgia.a
STATIC_LDFLAGS_WINDOWS := -fuse-ld=x86_64-w64-mingw32-ld -l:libNostalgia.a

INCLUDE := -I src -I src/thirdparty -I Nostalgia/thirdparty

DIR_ROOT      := build
DIR_LINUX     := Linux
DIR_WINDOWS   := Windows
DIR_DEBUG     := Debug
DIR_RELEASE   := Release
DIR_OBJS_BASE := .objs
DIR_DEPS      := .deps

PRETTY_STRING_STATIC  := Static
PRETTY_STRING_DYNAMIC := Dynamic
STRING_STATIC  := static
STRING_DYNAMIC := dynamic

NAME_BASE := Nostalgia_Goggles

# LINUX
ifneq ($(OS),Windows_NT)
	export NAME_DYNAMIC    ?= $(PRETTY_STRING_DYNAMIC)_$(NAME_BASE)
	export NAME_STATIC     ?= $(PRETTY_STRING_STATIC)_$(NAME_BASE)
	export BUILD_ARCH      ?= $(DIR_LINUX)
	export DEBUG_FLAGS     ?= $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_LINUX)
	export RELEASE_FLAGS   ?= $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_LINUX)
	export DYNAMIC_LDFLAGS ?= $(DYNAMIC_LDFLAGS_LINUX)
	export STATIC_LDFLAGS  ?= $(STATIC_LDFLAGS_LINUX)
	export CXX_FLAGS       ?= $(FLAGS_CXX_COMMON) $(FLAGS_LINUX)
	export PCH_FLAGS       ?= $(PCH:$(PCH_OUT)/%=-include-pch $(PCH_OUT)/%)
	export CC_FLAGS        ?= $(FLAGS_CC_COMMON) $(FLAGS_LINUX)
	export CXX_COMPILER    ?= $(LINUX_CXX)
	export C_COMPILER      ?= $(LINUX_CC)
else # WINDOWS
	export NAME_DYNAMIC    ?= $(PRETTY_STRING_DYNAMIC)_$(NAME_BASE).exe
	export NAME_STATIC     ?= $(PRETTY_STRING_STATIC)_$(NAME_BASE).exe
	export BUILD_ARCH      ?= $(DIR_WINDOWS)
	export DEBUG_FLAGS     ?= $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_WINDOWS)
	export RELEASE_FLAGS   ?= $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_WINDOWS)
	export DYNAMIC_LDFLAGS ?= $(DYNAMIC_LDFLAGS_WINDOWS)
	export STATIC_LDFLAGS  ?= $(STATIC_LDFLAGS_WINDOWS)
	export CXX_FLAGS       ?= $(FLAGS_CXX_COMMON) $(FLAGS_WINDOWS)
	export PCH_FLAGS       ?= -I $(dir $(PCH_OUT))
	export CC_FLAGS        ?= $(FLAGS_CC_COMMON) $(FLAGS_WINDOWS)
	export CXX_COMPILER    ?= $(WINDOWS_CXX)
	export C_COMPILER      ?= $(WINDOWS_CC)
endif

export NAME ?= $(NAME_STATIC)
export LD_FLAGS ?= $(DYNAMIC_LDFLAGS)
export BUILD_VERSION ?= $(DIR_RELEASE)
export VERSION_FLAGS ?= $(RELEASE_FLAGS)

export BUILD_ROOT     ?= $(DIR_ROOT)/$(BUILD_ARCH)/$(BUILD_VERSION)
export BUILD_DIR      ?= $(BUILD_ROOT)/Editor
export DIR_OBJS_TYPE  ?= $(STRING_STATIC)
export BUILD_OBJS     ?= $(BUILD_DIR)/$(DIR_OBJS_BASE)_$(DIR_OBJS_TYPE)
export BUILD_DEPS     ?= $(BUILD_OBJS)/$(DIR_DEPS)

export CLEAN_ARCH    ?= .+
export CLEAN_VERSION ?= .+

VPATH := $(SRC_DIRS):$(DIR_ROOT)

SRC := src

SRC_DIRS := \
    $(SRC)/app \
    $(SRC)/system \
    $(SRC)/tools \
    $(SRC)/gui

THIRDPARTY_SRC_DIRS := \
	$(SRC)/thirdparty/getargs

DIRTY_SRC_DIRS := \
	thirdparty

getfiles = $(foreach directory,$(1),$(wildcard $(directory)/$(2)))

CC_SRCS  ?= $(call getfiles,$(SRC_DIRS),*.c)   $(call getfiles,$(THIRDPARTY_SRC_DIRS),*.c)
CXX_SRCS ?= $(call getfiles,$(SRC_DIRS),*.cpp) $(call getfiles,$(THIRDPARTY_SRC_DIRS),*.cpp)

export PCH_DIR ?= ../../Engine/src/precompiled_headers
export PCH_H   ?= $(wildcard $(PCH_DIR)/*.hpp)

export CC_OBJS  ?= $(addprefix $(BUILD_OBJS)/,$(subst .c,.o,$(CC_SRCS:$(SRC)/%=%)))
export CXX_OBJS ?= $(addprefix $(BUILD_OBJS)/,$(subst .cpp,.obj,$(CXX_SRCS:$(SRC)/%=%)))
export DEPS_OUT ?= $(notdir $(CC_SRCS:.c=.d)) $(notdir $(CXX_SRCS:.cpp=.d))

# Printout Styles
export RESET   ?= \\x1b[0m
export NORM    ?= \\x1b[22m
export BOLD    ?= \\x1b[1m
export DIM     ?= \\x1b[2m
# Printout Colors
export BLACK   ?= \\x1b[30m
export RED     ?= \\x1b[31m
export GREEN   ?= \\x1b[32m
export YELLOW  ?= \\x1b[33m
export BLUE    ?= \\x1b[34m
export MAGENTA ?= \\x1b[35m
export CYAN    ?= \\x1b[36m
export WHITE   ?= \\x1b[37m
export DEFAULT ?= \\x1b[39m

.PHONY: all compile_commands printout build_dir static dynamic linux windows release debug clean mostlyclean disable_colors

all: editor ;@:

compile_commands:
	$(eval PCH_FLAGS = $(PCH_H:$(PCH_DIR)%=-include $(PCH_DIR)%))

printout:
	@ printf "$(BOLD)$(DEFAULT)::Architecture - $(BOLD)$(BLUE)$(BUILD_ARCH)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::Version - $(BOLD)$(BLUE)$(BUILD_VERSION)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::C++ Compile Command - $(BOLD)$(YELLOW)$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(INCLUDE) -c $(NORM)<source>$(BOLD)$(YELLOW) -o $(NORM)<object>$(YELLOW)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::C Compile Command - $(BOLD)$(YELLOW)$(C_COMPILER) $(CC_FLAGS) $(VERSION_FLAGS) $(INCLUDE) -c $(NORM)<source>$(BOLD)$(YELLOW) -o $(NORM)<object>$(YELLOW)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::Linking Command - $(BOLD)$(YELLOW)$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(INCLUDE) $(NORM)<objects> $(BOLD)-o $(BUILD_DIR)/$(NAME) $(LD_FLAGS)$(RESET)\n"

# TODO: Remove this
build_dir:
	@ -mkdir -p $(BUILD_DIR)/$(DIR_OBJS_BASE)_$(DIR_OBJS_TYPE)/$(DIR_DEPS)

static:
	$(eval DIR_OBJS_TYPE = $(STRING_STATIC))
	$(eval NAME = $(NAME_STATIC))
	@ -mkdir -p $(BUILD_DIR)/$(DIR_OBJS_BASE)_$(DIR_OBJS_TYPE)/$(DIR_DEPS)
	@ -rm -f $(BUILD_DIR)/$(NAME)
	$(MAKE) -s printout $(BUILD_DIR)/$(NAME)
	@ printf "$(BOLD)$(DEFAULT)::Static Library Built Successfully$(RESET)\n"

dynamic:
	$(eval DIR_OBJS_TYPE = $(STRING_DYNAMIC))
	$(eval NAME = $(NAME_DYNAMIC))
	@ -mkdir -p $(BUILD_DIR)/$(DIR_OBJS_BASE)_$(DIR_OBJS_TYPE)/$(DIR_DEPS)
	@ -rm -f $(BUILD_DIR)/$(NAME)
	$(MAKE) -s printout $(BUILD_DIR)/$(NAME)
	@ printf "$(BOLD)$(DEFAULT)::Dynamic Editor Built Successfully$(RESET)\n"

linux: ;@:
	$(eval NAME_DYNAMIC    = $(PRETTY_STRING_DYNAMIC)_$(NAME_BASE))
	$(eval NAME_STATIC     = $(PRETTY_STRING_STATIC)_$(NAME_BASE))
	$(eval DYNAMIC_LDFLAGS = $(DYNAMIC_LDFLAGS_LINUX))
	$(eval BUILD_ARCH      = $(DIR_LINUX))
	$(eval DEBUG_FLAGS     = $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_LINUX))
	$(eval RELEASE_FLAGS   = $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_LINUX))
	$(eval CXX_FLAGS       = $(FLAGS_CXX_COMMON) $(FLAGS_LINUX))
	$(eval CC_FLAGS        = $(FLAGS_CC_COMMON) $(FLAGS_LINUX))
	$(eval CXX_COMPILER    = $(LINUX_CXX))
	$(eval C_COMPILER      = $(LINUX_CC))
	$(eval TARGET_CALLED   = 1)
	$(eval CLEAN_ARCH      = $(DIR_LINUX))

windows: ;@:
	$(eval NAME_DYNAMIC    = $(PRETTY_STRING_DYNAMIC)_$(NAME_BASE).exe)
	$(eval NAME_STATIC     = $(PRETTY_STRING_STATIC)_$(NAME_BASE).exe)
	$(eval BUILD_ARCH      = $(DIR_WINDOWS))
	$(eval DEBUG_FLAGS     = $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_WINDOWS))
	$(eval RELEASE_FLAGS   = $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_WINDOWS))
	$(eval CXX_FLAGS       = $(FLAGS_CXX_COMMON) $(FLAGS_WINDOWS))
	$(eval CC_FLAGS        = $(FLAGS_CC_COMMON) $(FLAGS_WINDOWS))
	$(eval DYNAMIC_LDFLAGS = $(DYNAMIC_LDFLAGS_WINDOWS))
	$(eval CXX_COMPILER    = $(WINDOWS_CXX))
	$(eval C_COMPILER      = $(WINDOWS_CC))
	$(eval TARGET_CALLED   = 1)
	$(eval CLEAN_ARCH      = $(DIR_WINDOWS))

release: ;@:
	$(eval VERSION_FLAGS = $(RELEASE_FLAGS))
	$(eval BUILD_VERSION = $(DIR_RELEASE))
	$(eval TARGET_CALLED = 1)
	$(eval CLEAN_VERSION = $(DIR_RELEASE))

debug: ;@:
	$(eval VERSION_FLAGS = $(DEBUG_FLAGS))
	$(eval BUILD_VERSION = $(DIR_DEBUG))
	$(eval TARGET_CALLED = 1)
	$(eval CLEAN_VERSION = $(DIR_DEBUG))

# Cleaning Functions
CLEAN       = find $(DIR_ROOT) -type $(1) $(2) -not -path "*.git/*" -print 2>/dev/null
DIRTY       = $(foreach dir,$(1),-not -path "*$(dir)*")
CLEAN_PRINT = \
$(eval MAKE_TARGET != if [[ -n "$(1)" && "$(1)" != " " ]]; then printf "$(1)"; else printf "NOTHING_TO_CLEAN"; fi) \
$(MAKE) -s $(foreach cleaned,$(MAKE_TARGET),$(cleaned).clean)

# Cleans directories based on previous targets
# (e.g: 'make windows clean' will clean 'build/Windows/', but 'make windows debug clean' will only clean 'build/Windows/Debug/')
.__clean_target: ;@:
	$(eval CLEAN_FILES != $(call CLEAN,f,-regex '$(DIR_ROOT)/$(CLEAN_ARCH)/$(CLEAN_VERSION)/.*'))
	$(eval CLEAN_DIRS  != $(call CLEAN,d,-regex '$(DIR_ROOT)/$(CLEAN_ARCH)/$(CLEAN_VERSION)/.*'))
	$(RM) $(CLEAN_FILES)
	$(RM) -r $(CLEAN_DIRS)
	@ $(call CLEAN_PRINT,$(CLEAN_DIRS))

# Cleans the entire 'build/' directory (default behaviour for 'clean')
.__clean_all: ;@:
	$(eval CLEAN_FILES != $(call CLEAN,f,-regex '$(DIR_ROOT)/.*'))
	$(eval CLEAN_DIRS  != $(call CLEAN,d,-regex '$(DIR_ROOT)/.*'))
	$(RM) $(CLEAN_FILES)
	$(RM) -r $(CLEAN_DIRS)
	@ $(call CLEAN_PRINT,$(CLEAN_DIRS))

# What 'clean' does depends on if it's called by itself, or after other targets
clean:
	@ if [ "$(TARGET_CALLED)" == "1" ]; then \
		$(MAKE) -s .__clean_target;          \
	else                                     \
		$(MAKE) -s .__clean_all;             \
	fi

# 'mostlyclean' doesn't clean files from 'DIRTY_SRC_DIRS'
.__mostlyclean_target:
	$(eval CLEAN_FILES != $(call CLEAN,f,-regex '$(DIR_ROOT)/$(CLEAN_ARCH)/$(CLEAN_VERSION)/.*' $(call DIRTY,$(DIRTY_SRC_DIRS))))
	$(eval CLEAN_DIRS  != $(call CLEAN,d,-empty -regex '$(DIR_ROOT)/$(CLEAN_ARCH)/.*' $(call DIRTY,$(DIRTY_SRC_DIRS))))
	$(RM) $(CLEAN_FILES)
	$(RM) -r $(CLEAN_DIRS)
	@ $(call CLEAN_PRINT,$(CLEAN_DIRS))

.__mostlyclean_all: ;@:
	$(eval CLEAN_FILES != $(call CLEAN,f,$(call DIRTY,$(DIRTY_SRC_DIRS))))
	$(eval CLEAN_DIRS  != $(call CLEAN,d,-empty $(call DIRTY,$(DIRTY_SRC_DIRS))))
	$(RM) $(CLEAN_FILES)
	$(RM) -r $(CLEAN_DIRS)
	@ $(call CLEAN_PRINT,$(CLEAN_DIRS))

mostlyclean:
	@ if [ "$(TARGET_CALLED)" == "1" ]; then \
		$(MAKE) -s .__mostlyclean_target;    \
	else                                     \
		$(MAKE) -s .__mostlyclean_all;       \
	fi

disable_colors:
	$(eval RESET   = "")
	$(eval NORM    = "")
	$(eval BOLD    = "")
	$(eval DIM     = "")
	$(eval BLACK   = "")
	$(eval RED     = "")
	$(eval GREEN   = "")
	$(eval YELLOW  = "")
	$(eval BLUE    = "")
	$(eval MAGENTA = "")
	$(eval CYAN    = "")
	$(eval WHITE   = "")
	$(eval DEFAULT = "")
	@ printf "::Output styles disabled\n"

# C++ Object Files
$(BUILD_OBJS)/%.obj: $(SRC)/%.cpp
	@ printf "::Compiling $(BOLD)$(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(INCLUDE) -c $< -o $@
	@ -mv $(@:.obj=.d) $(BUILD_DEPS)/$(notdir $(@:.obj=.d))

# C Object Files
$(BUILD_OBJS)/%.o: $(SRC)/%.c
	@ printf "::Compiling $(BOLD)$(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(C_COMPILER) $(CC_FLAGS) $(VERSION_FLAGS) $(INCLUDE) -c $< -o $@
	@ -mv $(@:.o=.d) $(BUILD_DEPS)/$(notdir $(@:.o=.d))

# Editor
$(BUILD_DIR)/$(NAME): $(CXX_OBJS) $(CC_OBJS)
	@ printf "::Building $(BOLD)$(GREEN)$@$(RESET)\n"
	$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(INCLUDE) $^ -o $@ $(LD_FLAGS)

# Prints a unique cleanup message
NOTHING_TO_CLEAN.clean:
	@ printf "::Nothing left to clean\n"

# Prints a cleanup message
%.clean:
	@ printf "::Cleaned $(BOLD)$(RED)$*$(RESET)\n"

include $(wildcard $(BUILD_DEPS)/*.d)
