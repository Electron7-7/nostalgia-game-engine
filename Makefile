LINUX_CXX := clang++
LINUX_CC  := clang

ifneq ($(OS),Windows_NT)
	WINDOWS_CXX := x86_64-w64-mingw32-g++
	WINDOWS_CC  := x86_64-w64-mingw32-gcc
else
	WINDOWS_CXX := g++
	WINDOWS_CC  := gcc
endif

LAR := llvm-ar
# TODO: Figure out if I need a windows-specific 'ar' and what it would be
WAR := $(LAR)

FLAGS_DEBUG_COMMON    := -g -Wall -O0 -D DEBUGGING
FLAGS_DEBUG_LINUX     := -fsanitize=address
FLAGS_DEBUG_WINDOWS   := # Nothing yet
FLAGS_RELEASE_COMMON  := -O3
FLAGS_RELEASE_WINDOWS := # Nothing yet
FLAGS_RELEASE_LINUX   := # Nothing yet
FLAGS_LIB_DYNAMIC     := -fPIC
FLAGS_LIB_STATIC      := # Nothing yet
FLAGS_CXX_COMMON      := -std=c++23
FLAGS_CC_COMMON       := -std=c11
FLAGS_WINDOWS         := -mwindows
FLAGS_LINUX           := # Nothing yet
GLFW_ARCHIVE          := libglfw3.a
GLFW_LINUX            := src/lib/glfw-lib-linux
GLFW_WINDOWS          := src/lib/glfw-lib-mingw-w64
LDFLAGS_LINUX         := -shared -L $(GLFW_LINUX) -l glfw
LDFLAGS_WINDOWS       := -shared --out-implib -L $(GLFW_WINDOWS) -l glfw

INCLUDE := -I src -I src/thirdparty -I src/common

DIR_ROOT      := build
DIR_LINUX     := Linux
DIR_WINDOWS   := Windows
DIR_DEBUG     := Debug
DIR_RELEASE   := Release
DIR_LIBRARY   := lib
DIR_HEADERS   := include
DIR_OBJS_BASE := .objs
DIR_DEPS      := .deps

PRETTY_STRING_STATIC  := Static
PRETTY_STRING_DYNAMIC := Dynamic
STRING_STATIC  := static
STRING_DYNAMIC := dynamic
STRING_LIB := lib

NAME_BASE            := Nostalgia
NAME_STATIC_LINUX    := .a
NAME_DYNAMIC_LINUX   := .so
NAME_STATIC_WINDOWS  := .lib
NAME_DYNAMIC_WINDOWS := .dll

APP_BASE := LibraryTestingApp

# LINUX
ifneq ($(OS),Windows_NT)
	export APP_NAME      ?= $(APP_BASE)
	export NAME_STATIC   ?= $(NAME_STATIC_LINUX)
	export NAME_DYNAMIC  ?= $(NAME_DYNAMIC_LINUX)
	export BUILD_ARCH    ?= $(DIR_LINUX)
	export DEBUG_FLAGS   ?= $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_LINUX)
	export RELEASE_FLAGS ?= $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_LINUX)
	export CXX_FLAGS     ?= $(FLAGS_CXX_COMMON) $(FLAGS_LINUX)
	export CC_FLAGS      ?= $(FLAGS_CC_COMMON) $(FLAGS_LINUX)
	export LD_FLAGS      ?= $(LDFLAGS_LINUX)
	export CXX_COMPILER  ?= $(LINUX_CXX)
	export C_COMPILER    ?= $(LINUX_CC)
	export GLFW_LIBRARY  ?= $(GLFW_LINUX)
else # WINDOWS
	export APP_NAME      ?= $(APP_BASE).exe
	export NAME_STATIC   ?= $(NAME_STATIC_WINDOWS)
	export NAME_DYNAMIC  ?= $(NAME_DYNAMIC_WINDOWS)
	export BUILD_ARCH    ?= $(DIR_WINDOWS)
	export DEBUG_FLAGS   ?= $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_WINDOWS)
	export RELEASE_FLAGS ?= $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_WINDOWS)
	export CXX_FLAGS     ?= $(FLAGS_CXX_COMMON) $(FLAGS_WINDOWS)
	export CC_FLAGS      ?= $(FLAGS_CC_COMMON) $(FLAGS_WINDOWS)
	export LD_FLAGS      ?= $(LDFLAGS_WINDOWS)
	export CXX_COMPILER  ?= $(WINDOWS_CXX)
	export C_COMPILER    ?= $(WINDOWS_CC)
	export GLFW_LIBRARY  ?= $(GLFW_WINDOWS)
endif

export BUILD_VERSION ?= $(DIR_RELEASE)
export VERSION_FLAGS ?= $(RELEASE_FLAGS)

export BUILD_DIR      ?= $(DIR_ROOT)/$(BUILD_ARCH)/$(BUILD_VERSION)
export DIR_OBJS_TYPE  ?= $(STRING_STATIC)
export BUILD_LIBRARY  ?= $(BUILD_DIR)/$(DIR_LIBRARY)
export BUILD_HEADERS  ?= $(BUILD_DIR)/$(DIR_HEADERS)/$(NAME_BASE)
export BUILD_OBJS     ?= $(BUILD_DIR)/$(DIR_OBJS_BASE)_$(DIR_OBJS_TYPE)
export BUILD_DEPENDS  ?= $(BUILD_OBJS)/$(DIR_DEPS)
export BUILD_GLFW     ?= $(BUILD_OBJS)/thirdparty/libglfw3

export NAME           ?= $(STRING_LIB)$(NAME_BASE)$(NAME_STATIC)
export APP_LD_FLAGS   ?= $(BUILD_LIBRARY)/$(NAME)
export APP_TYPE       ?= $(PRETTY_STRING_STATIC)
export APP            ?= $(APP_TYPE)$(APP_NAME)

export FLAGS_LIBRARY ?= $(FLAGS_LIB_STATIC)

export BUILDING_APP ?=

export CLEAN_TARGET ?= 0
export CLEAN_ARCH ?= .+
export CLEAN_VERSION ?= .+

VPATH := $(SRC_DIRS)

SRC := src

SRC_DIRS :=                             \
    $(SRC)/commands                     \
    $(SRC)/common                       \
    $(SRC)/embedded                     \
    $(SRC)/events                       \
    $(SRC)/input                        \
    $(SRC)/managers                     \
    $(SRC)/rendering                    \
    $(SRC)/rendering/backends/graphics  \
    $(SRC)/rendering/backends/windowing \
    $(SRC)/rendering/shader_interfaces  \
    $(SRC)/world                        \


THIRDPARTY_SRC_DIRS :=          \
	$(SRC)/thirdparty/DearImGui \
	$(SRC)/thirdparty/glad

APP_SRC_DIRS :=               \
    $(SRC)/testing_app/app    \
    $(SRC)/testing_app/system \
    $(SRC)/testing_app/ui

DIRTY_SRC_DIRS := \
	thirdparty    \

RESOURCES_DIR := $(SRC)/resources

get_source_files = $(foreach directory,$(1),$(wildcard $(directory)/$(2)))

CC_SRCS  := $(call get_source_files,$(SRC_DIRS),*.c)   $(call get_source_files,$(THIRDPARTY_SRC_DIRS),*.c)
CXX_SRCS := $(call get_source_files,$(SRC_DIRS),*.cpp) $(call get_source_files,$(THIRDPARTY_SRC_DIRS),*.cpp)

APP_SRCS := $(call get_source_files,$(APP_SRC_DIRS),*.cpp)

HEADER_FILES := $(call get_source_files,$(SRC_DIRS),*.h) $(call get_source_files,$(SRC_DIRS),*.hpp)

export CC_OBJS  ?= $(addprefix $(BUILD_OBJS)/,$(subst .c,.o,$(CC_SRCS:$(SRC)/%=%)))
export CXX_OBJS ?= $(addprefix $(BUILD_OBJS)/,$(subst .cpp,.obj,$(CXX_SRCS:$(SRC)/%=%)))

export DEP_FILES ?= $(CXX_SRCS:$(SRC)/%.cpp=$(BUILD_DEPENDS)/%.d)

export APP_OBJS ?= $(addprefix $(BUILD_OBJS)/,$(subst .cpp,.obj,$(APP_SRCS:$(SRC)/%=%)))

export HEADERS_OUT ?= $(addprefix $(BUILD_HEADERS)/,$(HEADER_FILES:$(SRC)/%=%))

export RESET   ?= \\x1b[0m
export BLACK   ?= \\x1b[1;30m
export RED     ?= \\x1b[1;31m
export GREEN   ?= \\x1b[1;32m
export YELLOW  ?= \\x1b[1;33m
export BLUE    ?= \\x1b[1;34m
export MAGENTA ?= \\x1b[1;35m
export CYAN    ?= \\x1b[1;36m
export WHITE   ?= \\x1b[1;37m
export DEFAULT ?= \\x1b[1;39m

.PHONY: all printout static dynamic libraries testapp_static testapp_dynamic testapps headers resources build linux windows release debug build_dirs clean cleanDirty disable_colors

all: build_dirs headers resources static dynamic testapp_static testapp_dynamic ;@:

printout:
	@ printf "$(DEFAULT)::Architecture - $(BLUE)$(BUILD_ARCH)$(RESET)\n"
	@ printf "$(DEFAULT)::Version - $(BLUE)$(BUILD_VERSION)$(RESET)\n"
	@ printf "$(DEFAULT)::C Compile Command - $(YELLOW)$(C_COMPILER) $(CC_FLAGS) $(VERSION_FLAGS) $(INCLUDE)$(RESET)\n"
	@ printf "$(DEFAULT)::C++ Compile Command - $(YELLOW)$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(INCLUDE)$(RESET)\n"
	@ if [ -n "$(BUILDING_APP)" ]; then printf "$(DEFAULT)::Linker Flags - $(YELLOW)$(APP_LD_FLAGS)$(RESET)\n"; fi

# FIXME: Sometimes it takes two builds for the app to link correctly (due to undefined errors related to GLFW)
# Clues: the only operation that actually happens both times is linking the library/app & extracting the glfw archive
# so I think maybe it doesn't link correctly the first time but it does the second time? Maybe the glfw objects have to
# be added to the library last/after it's created?
static: resources
	$(eval DIR_OBJS_TYPE = $(STRING_STATIC))
	$(eval FLAGS_LIBRARY = $(FLAGS_LIB_STATIC))
	$(eval NAME = $(STRING_LIB)$(NAME_BASE)$(NAME_STATIC))
	@ -rm -f $(BUILD_LIBRARY)/$(NAME)
	@ $(MAKE) -s printout $(CC_OBJS) $(CXX_OBJS) $(BUILD_LIBRARY)/$(NAME)
	@ printf "$(DEFAULT)::Static Library Built Successfully$(RESET)\n"

dynamic: resources
	$(eval DIR_OBJS_TYPE = $(STRING_DYNAMIC))
	$(eval FLAGS_LIBRARY = $(FLAGS_LIB_DYNAMIC))
	$(eval NAME = $(STRING_LIB)$(NAME_BASE)$(NAME_DYNAMIC))
	@ -rm -f $(BUILD_LIBRARY)/$(NAME)
	@ $(MAKE) -s printout $(CC_OBJS) $(CXX_OBJS) $(BUILD_LIBRARY)/$(NAME)
	@ printf "$(DEFAULT)::Dynamic Library Built Successfully$(RESET)\n"

libraries: static dynamic ;@:

testapp_static:
	$(eval APP_TYPE = $(PRETTY_STRING_STATIC))
	$(eval NAME = $(STRING_LIB)$(NAME_BASE)$(NAME_STATIC))
	$(eval DIR_OBJS_TYPE = $(STRING_STATIC))
	$(eval BUILDING_APP = 1)
	@ -rm -f $(BUILD_DIR)/$(APP)
	@ $(MAKE) -s printout $(BUILD_DIR)/$(APP)
	@ printf "$(DEFAULT)::Static Application Built Successfully$(RESET)\n"

testapp_dynamic:
	$(eval APP_TYPE = $(PRETTY_STRING_DYNAMIC))
	$(eval NAME = $(STRING_LIB)$(NAME_BASE)$(NAME_DYNAMIC))
	$(eval DIR_OBJS_TYPE = $(STRING_DYNAMIC))
	$(eval BUILDING_APP = 1)
	@ -rm -f $(BUILD_DIR)/$(APP)
	@ $(MAKE) -s printout $(BUILD_DIR)/$(APP)
	@ printf "$(DEFAULT)::Dynamic Application Built Successfully$(RESET)\n"

testapps: testapp_static testapp_dynamic ;@:

headers:
	@ $(MAKE) -s $(HEADERS_OUT)

resources:
	@ $(MAKE) -s -C $(RESOURCES_DIR)

linux: ;@:
	$(eval APP_NAME      = $(APP_BASE))
	$(eval NAME_STATIC   = $(NAME_STATIC_LINUX))
	$(eval NAME_DYNAMIC  = $(NAME_DYNAMIC_LINUX))
	$(eval BUILD_ARCH    = $(DIR_LINUX))
	$(eval DEBUG_FLAGS   = $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_LINUX))
	$(eval RELEASE_FLAGS = $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_LINUX))
	$(eval CXX_FLAGS     = $(FLAGS_CXX_COMMON) $(FLAGS_LINUX))
	$(eval CC_FLAGS      = $(FLAGS_CC_COMMON) $(FLAGS_LINUX))
	$(eval LD_FLAGS      = $(LDFLAGS_LINUX))
	$(eval CXX_COMPILER  = $(LINUX_CXX))
	$(eval C_COMPILER    = $(LINUX_CC))
	$(eval GLFW_LIBRARY  = $(GLFW_LINUX))
	$(eval TARGET_CALLED = 1)
	$(eval CLEAN_ARCH    = $(DIR_LINUX))

windows: ;@:
	$(eval APP_NAME      = $(APP_BASE).exe)
	$(eval NAME_STATIC   = $(NAME_STATIC_WINDOWS))
	$(eval NAME_DYNAMIC  = $(NAME_DYNAMIC_WINDOWS))
	$(eval BUILD_ARCH    = $(DIR_WINDOWS))
	$(eval DEBUG_FLAGS   = $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_WINDOWS))
	$(eval RELEASE_FLAGS = $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_WINDOWS))
	$(eval CXX_FLAGS     = $(FLAGS_CXX_COMMON) $(FLAGS_WINDOWS))
	$(eval CC_FLAGS      = $(FLAGS_CC_COMMON) $(FLAGS_WINDOWS))
	$(eval LD_FLAGS      = $(LDFLAGS_WINDOWS))
	$(eval CXX_COMPILER  = $(WINDOWS_CXX))
	$(eval C_COMPILER    = $(WINDOWS_CC))
	$(eval GLFW_LIBRARY  = $(GLFW_WINDOWS))
	$(eval TARGET_CALLED = 1)
	$(eval CLEAN_ARCH    = $(DIR_WINDOWS))

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

build_dirs:
	@ -mkdir -p $(BUILD_DIR) $(BUILD_OBJS) $(BUILD_GLFW) $(BUILD_HEADERS) $(BUILD_LIBRARY) $(BUILD_DEPENDS)

# Cleaning Functions
CLEAN = find $(DIR_ROOT) -type $(1) $(2) -not -path "*.git/*" -delete -print 2>/dev/null
CLEAN_OBJS = $(call CLEAN,$(1),-regex '.+\.objs_.+/.+' $(2))
CLEAN_DIRTY = $(call CLEAN_OBJS,$(1),$(DIRTY_SRC_DIRS:%=-not -path "*%*"))
CLEAN_PRINTOUT = \
$(eval MAKE_TARGET != if [[ -n "$(1)" && "$(1)" != " " ]]; then printf "$(1)"; else printf "NOTHING_TO_CLEAN"; fi) \
$(MAKE) -s $(foreach cleaned,$(MAKE_TARGET),$(cleaned).clean)

clean_target: ;@:
	$(eval CLEAN_FILES != $(call CLEAN,f,-regex '$(DIR_ROOT)/$(CLEAN_ARCH)/$(CLEAN_VERSION)/.*'))
	$(eval CLEAN_DIRS  != $(call CLEAN,d,-regex '$(DIR_ROOT)/$(CLEAN_ARCH)/$(CLEAN_VERSION)'))
	$(eval EMPTY_DIRS  != $(call CLEAN,d,-empty -regex '$(DIR_ROOT)/$(CLEAN_ARCH)'))
	@ $(call CLEAN_PRINTOUT,$(EMPTY_DIRS))

clean_all: ;@:
	$(eval CLEAN_FILES != $(call CLEAN,f,-regex '$(DIR_ROOT)/.*'))
	$(eval CLEAN_DIRS  != $(call CLEAN,d,-regex '$(DIR_ROOT)/.*'))
	$(eval EMPTY_DIR   != find ./ -type d -not -path "*.git*" -empty -delete -print 2>/dev/null)
	@ $(call CLEAN_PRINTOUT,$(EMPTY_DIR))

clean:
	@ if [ "$(TARGET_CALLED)" == "1" ]; then \
		$(MAKE) -s clean_target;           \
	else                                   \
		$(MAKE) -s clean_all;              \
	fi

# Dirty Clean: Clean all obj files+dirs that *aren't* `DIRTY_SRC_DIRS`
cleanDirty:
	$(eval CLEAN_FILES != $(call CLEAN_DIRTY,f))
	$(eval CLEAN_DIRS  != $(call CLEAN_DIRTY,d -empty))
	@ printf "FILES: '$(CLEAN_FILES)'\nDIRS: '$(CLEAN_DIRS)'\n"
	@ $(call CLEAN_PRINTOUT, $(CLEAN_FILES)$(CLEAN_DIRS))

disable_colors:
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

# C++ Object Files
$(BUILD_OBJS)/%.obj: $(SRC)/%.cpp | build_dirs
	@ printf "::Compiling $(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(FLAGS_LIBRARY) $(INCLUDE) -c $< -o $@

# C Object Files
$(BUILD_OBJS)/%.o: $(SRC)/%.c | build_dirs
	@ printf "::Compiling $(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(C_COMPILER) $(CC_FLAGS) $(VERSION_FLAGS) $(FLAGS_LIBRARY) $(INCLUDE) -c $< -o $@

# Header Files
$(BUILD_HEADERS)/%.hpp: $(SRC)/%.hpp | build
	@ printf "::Copying Header $(CYAN)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	@ cp $< $@

# Static Library
$(BUILD_LIBRARY)/$(STRING_LIB)$(NAME_BASE)$(NAME_STATIC): $(CC_OBJS) $(CXX_OBJS)
	@ printf "::Building $(GREEN)$@$(RESET)\n"
	@ $(AR) cr $@ $^
	@ printf "$(DEFAULT)::Extracting $(CYAN)GLFW Object Files$(RESET)\n"
	@ $(MAKE) -s $(shell $(AR) t $(GLFW_LIBRARY)/libglfw3.a | sed -re 's/(.+)/\1.print-ar-x/g')
	@ $(AR) x --output $(BUILD_GLFW) $(GLFW_LIBRARY)/libglfw3.a
	@ printf "$(DEFAULT)::Injecting $(CYAN)GLFW Object Files$(RESET)\n"
	@ $(AR) q $@ $(wildcard $(BUILD_GLFW)/*.o)

# Dynamic Library
$(BUILD_LIBRARY)/$(STRING_LIB)$(NAME_BASE)$(NAME_DYNAMIC): $(CC_OBJS) $(CXX_OBJS)
	@ printf "::Building $(GREEN)$@$(RESET)\n"
	$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(FLAGS_LIBRARY) $(INCLUDE) $^ -o $@ $(LD_FLAGS)

# Library Testing Applications
$(BUILD_DIR)/$(APP): $(APP_OBJS)
	@ printf "::Linking $(GREEN)$@$(RESET)\n"
	$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(FLAGS_LIBRARY) $(INCLUDE) $^ -o $@ $(APP_LD_FLAGS)


# GLFW Library Extraction Printouts
%.print-ar-x:
	@ printf "::Extracting $(BLUE)$(BUILD_GLFW)/$*$(RESET)\n"

NOTHING_TO_CLEAN.clean:
	@ printf "::Nothing left to clean\n"

%.clean:
	@ printf "::Cleaned $(RED)$*$(RESET)\n"
