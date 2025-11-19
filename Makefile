LINUX_CXX := clang++
LINUX_CC  := clang
LINUX_AR  := llvm-ar

ifneq ($(OS),Windows_NT)
	WINDOWS_CXX := x86_64-w64-mingw32-g++
	WINDOWS_CC  := x86_64-w64-mingw32-gcc
	WINDOWS_AR  := x86_64-w64-mingw32-ar
else
	WINDOWS_CXX := g++
	WINDOWS_CC  := gcc
	WINDOWS_AR  := ar
endif

FLAGS_DEBUG_COMMON    := -g -O0 -D DEBUGGING
FLAGS_DEBUG_LINUX     := -fsanitize=address
FLAGS_DEBUG_WINDOWS   := # Nothing yet
FLAGS_RELEASE_COMMON  := -O3
FLAGS_RELEASE_WINDOWS := # Nothing yet
FLAGS_RELEASE_LINUX   := # Nothing yet
FLAGS_DYNAMIC         := -shared -fPIC
FLAGS_STATIC          := -static
FLAGS_CXX_COMMON      := -std=c++26 -Wall -D JPH_PROFILE_ENABLED -D JPH_DEBUG_ENABLED -D JPH_DEBUG_RENDERER -D JPH_OBJECT_STREAM -D GLFW_INCLUDE_NONE -MMD -MP --embed-dir=src/embedded -Wno-c23-extensions
FLAGS_CC_COMMON       := -std=c23 -Wall -MMD -MP
FLAGS_WINDOWS         := -mwindows -lstdc++exp -D NOSTALGIA_WINDOWS
FLAGS_LINUX           := -D NOSTALGIA_LINUX

# These are only used when linking dynamically
EDITR_LD_FLAGS_LINUX    := -lglfw -lfreetype -l$(NAME_BASE)
EDITR_LD_FLAGS_WINDOWS  := -lstdc++exp -lglfw -lfreetype -l$(NAME_BASE)

# Archives & Libraries
ARCHIVES_DIR_LINUX       := Engine/src/static_libraries/linux
ARCHIVES_DIR_WINDOWS     := Engine/src/static_libraries/windows
DYNAMIC_LIBRARIES        := -lglfw -lfreetype
DYNAMIC_LDFLAGS_LINUX    := -shared $(DYNAMIC_LIBRARIES)
DYNAMIC_LDFLAGS_WINDOWS  := -shared --out-implib $(DYNAMIC_LIBRARIES)

INCLUDE := -I Engine/src -I Editor/src -I Engine/src/thirdparty -I Engine/src/thirdparty/FreeType

DIR_ROOT      := build
DIR_ENGINE    := Engine
DIR_EDITR     := Editor
DIR_LINUX     := Linux
DIR_WINDOWS   := Windows
DIR_DEBUG     := Debug
DIR_RELEASE   := Release
DIR_LIBRARY   := lib
DIR_HEADERS   := include
DIR_OBJS_BASE := .objs
DIR_DEPS      := .deps
DIR_ARCHIVES  := .archives

PRETTY_STRING_STATIC  := Static
PRETTY_STRING_DYNAMIC := Dynamic
STRING_STATIC  := static
STRING_DYNAMIC := dynamic
STRING_LIB := lib

NAME_BASE            := Nostalgia
NAME_STATIC_LINUX    := .a
NAME_DYNAMIC_LINUX   := .so
# NAME_STATIC_WINDOWS  := .lib
NAME_STATIC_WINDOWS  := .a
NAME_DYNAMIC_WINDOWS := .dll

EDITR_BASE := Nostalgia_Goggles

# LINUX
ifneq ($(OS),Windows_NT)
	export EDITR           ?= $(EDITR_BASE)
	export NAME_STATIC     ?= $(NAME_STATIC_LINUX)
	export NAME_DYNAMIC    ?= $(NAME_DYNAMIC_LINUX)
	export BUILD_ARCH      ?= $(DIR_LINUX)
	export DEBUG_FLAGS     ?= $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_LINUX)
	export RELEASE_FLAGS   ?= $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_LINUX)
	export CXX_FLAGS       ?= $(FLAGS_CXX_COMMON) $(FLAGS_LINUX)
	export CC_FLAGS        ?= $(FLAGS_CC_COMMON) $(FLAGS_LINUX)
	export LD_FLAGS        ?= $(DYNAMIC_LDFLAGS_LINUX)
	export EDITR_LD_FLAGS  ?= $(EDITR_LD_FLAGS_LINUX)
	export ARCHIVES_DIR    ?= $(ARCHIVES_DIR_LINUX)
	export CXX_COMPILER    ?= $(LINUX_CXX)
	export C_COMPILER      ?= $(LINUX_CC)
	export ARCHIVER        ?= $(LINUX_AR)
else # WINDOWS
	export EDITR           ?= $(EDITR_BASE).exe
	export NAME_STATIC     ?= $(NAME_STATIC_WINDOWS)
	export NAME_DYNAMIC    ?= $(NAME_DYNAMIC_WINDOWS)
	export BUILD_ARCH      ?= $(DIR_WINDOWS)
	export DEBUG_FLAGS     ?= $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_WINDOWS)
	export RELEASE_FLAGS   ?= $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_WINDOWS)
	export CXX_FLAGS       ?= $(FLAGS_CXX_COMMON) $(FLAGS_WINDOWS)
	export CC_FLAGS        ?= $(FLAGS_CC_COMMON) $(FLAGS_WINDOWS)
	export LD_FLAGS        ?= $(DYNAMIC_LDFLAGS_WINDOWS) -fuse-ld=x86_64-w64-mingw32-ld
	export EDITR_LD_FLAGS  ?= $(EDITR_LD_FLAGS_WINDOWS)
	export ARCHIVES_DIR    ?= $(ARCHIVES_DIR_WINDOWS)
	export CXX_COMPILER    ?= $(WINDOWS_CXX)
	export C_COMPILER      ?= $(WINDOWS_CC)
	export ARCHIVER        ?= $(WINDOWS_AR)
endif

export JOLT_DEBUG    ?= $(ARCHIVES_DIR)/Jolt_Debug/libJolt.a
export JOLT_RELEASE  ?= $(ARCHIVES_DIR)/Jolt_Distribution/libJolt.a
export JOLT_LIBRARY  ?= $(JOLT_RELEASE)
export BUILD_VERSION ?= $(DIR_RELEASE)
export VERSION_FLAGS ?= $(RELEASE_FLAGS)

export BUILD_ROOT     ?= $(DIR_ROOT)/$(BUILD_ARCH)/$(BUILD_VERSION)
export BUILD_DIR      ?= $(BUILD_ROOT)/$(DIR_ENGINE)
export DIR_OBJS_TYPE  ?= $(STRING_STATIC)
export BUILD_LIBRARY  ?= $(BUILD_DIR)/$(DIR_LIBRARY)
export BUILD_HEADERS  ?= $(BUILD_DIR)/$(DIR_HEADERS)/$(NAME_BASE)
export BUILD_OBJS     ?= $(BUILD_DIR)/$(DIR_OBJS_BASE)_$(DIR_OBJS_TYPE)
export BUILD_ARCHIVES ?= $(BUILD_OBJS)/$(DIR_ARCHIVES)
export BUILD_DEPS     ?= $(BUILD_OBJS)/$(DIR_DEPS)

export NAME  ?= $(STRING_LIB)$(NAME_BASE)$(NAME_STATIC)

export LIBRARY_FLAGS ?= $(FLAGS_STATIC)

export BUILDING_EDITR ?=
export BUILDING_DYNAMIC_LIBRARY ?=
export BUILDING_STATIC_LIBRARY  ?=

export CLEAN_ARCH    ?= .+
export CLEAN_VERSION ?= .+

VPATH := $(ENGINE_SRC_DIRS):$(EDITR_SRC_DIRS):$(DIR_ROOT)

ENGINE_SRC := Engine/src
EDITR_SRC  := Editor/src

ENGINE_SRC_DIRS :=                             \
    $(ENGINE_SRC)/application                  \
    $(ENGINE_SRC)/backends                     \
    $(ENGINE_SRC)/backends/glfw                \
    $(ENGINE_SRC)/backends/opengl              \
    $(ENGINE_SRC)/common                       \
    $(ENGINE_SRC)/core                         \
    $(ENGINE_SRC)/embedded                     \
    $(ENGINE_SRC)/filesystem                   \
    $(ENGINE_SRC)/input                        \
    $(ENGINE_SRC)/managers                     \
    $(ENGINE_SRC)/math                         \
    $(ENGINE_SRC)/physics                      \
    $(ENGINE_SRC)/rendering                    \
    $(ENGINE_SRC)/settings                     \
    $(ENGINE_SRC)/theatre                      \
    $(ENGINE_SRC)/theatre/parser               \
    $(ENGINE_SRC)/things                       \
    $(ENGINE_SRC)/things/actors                \
    $(ENGINE_SRC)/things/resources             \
    $(ENGINE_SRC)/things/devices               \
    $(ENGINE_SRC)/things/thinkers              \
	$(ENGINE_SRC)/ui                           \

THIRDPARTY_SRC_DIRS :=                  \
	$(ENGINE_SRC)/thirdparty/DearImGui  \
	$(ENGINE_SRC)/thirdparty/glm/detail \
	$(ENGINE_SRC)/thirdparty/glad

EDITR_SRC_DIRS :=               \
    $(EDITR_SRC)/app    \
    $(EDITR_SRC)/system \
    $(EDITR_SRC)/tools  \
    $(EDITR_SRC)/gui

DIRTY_SRC_DIRS := \
	thirdparty

getfiles = $(foreach directory,$(1),$(wildcard $(directory)/$(2)))

CC_SRCS  ?= $(call getfiles,$(ENGINE_SRC_DIRS),*.c)   $(call getfiles,$(THIRDPARTY_SRC_DIRS),*.c)
CXX_SRCS ?= $(call getfiles,$(ENGINE_SRC_DIRS),*.cpp) $(call getfiles,$(THIRDPARTY_SRC_DIRS),*.cpp)

EDITR_SRCS := $(call getfiles,$(EDITR_SRC_DIRS),*.cpp)

# TODO: Make better API headers
HEADER_FILES := $(call getfiles,$(ENGINE_SRC_DIRS),*.hpp)

export CC_OBJS  ?= $(addprefix $(BUILD_OBJS)/,$(subst .c,.o,$(CC_SRCS:$(ENGINE_SRC)/%=%)))
export CXX_OBJS ?= $(addprefix $(BUILD_OBJS)/,$(subst .cpp,.obj,$(CXX_SRCS:$(ENGINE_SRC)/%=%)))
export DEPS_OUT ?= $(notdir $(CC_SRCS:.c=.d)) $(notdir $(CXX_SRCS:.cpp=.d))

export ARCHIVES ?= $(wildcard $(ARCHIVES_DIR)/Common/*.a) $(JOLT_LIBRARY)
export AR_OBJS  ?= $(addprefix $(BUILD_ARCHIVES)/,$(ARCHIVES:$(ARCHIVES_DIR)/%.a=%.o))
export EDITR_OBJS ?= $(addprefix $(BUILD_OBJS)/,$(subst .cpp,.obj,$(EDITR_SRCS:$(EDITR_SRC)/%=%)))

export HEADERS_OUT ?= $(addprefix $(BUILD_HEADERS)/,$(HEADER_FILES:$(ENGINE_SRC)/%=%))

export CURRENT_SRC ?= $(ENGINE_SRC)

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

.PHONY: all run printout build_dir clangd static dynamic libraries editor headers linux windows release debug clean mostlyclean disable_colors

all: editor ;@:

run: editor
	@ $(BUILD_DIR)/$(EDITR)

printout:
	@ printf "$(BOLD)$(DEFAULT)::Architecture - $(BOLD)$(BLUE)$(BUILD_ARCH)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::Version - $(BOLD)$(BLUE)$(BUILD_VERSION)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::C Compile Command - $(BOLD)$(YELLOW)$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) -c $(NORM)<source file>$(BOLD)$(YELLOW) -o $(NORM)<object file>$(YELLOW)$(LD_FLAGS)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::C++ Compile Command - $(BOLD)$(YELLOW)$(C_COMPILER) $(CC_FLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) -c $(NORM)<source file>$(BOLD)$(YELLOW) -o $(NORM)<object file>$(YELLOW)$(LD_FLAGS)$(RESET)\n"
	@ if [ -n "$(BUILDING_EDITR)" ]; then printf "$(BOLD)$(DEFAULT)::Linking Command - $(BOLD)$(YELLOW)$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(INCLUDE) $(NORM)<object files> $(BOLD)-o $(BUILD_DIR)/$(EDITR) $(EDITR_LD_FLAGS)$(RESET)\n"; fi
	@ if [ -n "$(BUILDING_DYNAMIC_LIBRARY)" ]; then \
		printf "$(BOLD)$(DEFAULT)::Linker Flags - $(BOLD)$(YELLOW)$(LD_FLAGS)$(RESET)\n"\
	; fi

	@ if [ -n "$(BUILDING_STATIC_LIBRARY)" ]; then \
		printf "$(BOLD)$(DEFAULT)::Archives Compile Command - $(BOLD)$(YELLOW)$(CXX_COMPILER) -r -o $(NORM)<archive object file>$(BOLD) -Xlinker --whole-archive $(NORM)<archive>$(RESET)\n"; \
		printf "$(BOLD)$(DEFAULT)::Library Command (Static) - $(BOLD)$(YELLOW)$(ARCHIVER) rcs $(BUILD_LIBRARY)/$(STRING_LIB)$(NAME_BASE)$(NAME_STATIC) $(NORM)<object files> <archive object files>$(RESET)\n" \
	; fi

# TODO: Remove this
build_dir:
	@ -mkdir -p $(BUILD_DIR)/$(DIR_OBJS_BASE)_$(DIR_OBJS_TYPE)/$(DIR_DEPS)

# Fix a problem with LSP-clangd that's (most likely) due to the massive #embed for shader files
clangd: ;@:
	$(eval CXX_FLAGS += -D CLANGD_KEEPS_CRASHING_HERE)

static: headers
	$(eval LD_FLAGS =)
	$(eval DIR_OBJS_TYPE = $(STRING_STATIC))
	$(eval LIBRARY_FLAGS = $(FLAGS_STATIC))
	$(eval NAME = $(STRING_LIB)$(NAME_BASE)$(NAME_STATIC))
	$(eval BUILD_DIR = $(BUILD_ROOT)/$(DIR_ENGINE))
	$(eval CURRENT_SRC = $(ENGINE_SRC))
	$(eval BUILDING_STATIC_LIBRARY = 1)
	@ -mkdir -p $(BUILD_DIR)/$(DIR_OBJS_BASE)_$(DIR_OBJS_TYPE)/$(DIR_DEPS)
	@ -mkdir -p $(BUILD_ARCHIVES)
	@ -rm -f $(BUILD_LIBRARY)/$(NAME)
	@ $(MAKE) -s printout $(BUILD_LIBRARY)/$(NAME)
	@ printf "$(BOLD)$(DEFAULT)::Static Library Built Successfully$(RESET)\n"

dynamic: headers
	$(eval DIR_OBJS_TYPE = $(STRING_DYNAMIC))
	$(eval LIBRARY_FLAGS = $(FLAGS_DYNAMIC))
	$(eval NAME = $(STRING_LIB)$(NAME_BASE)$(NAME_DYNAMIC))
	$(eval BUILD_DIR = $(BUILD_ROOT)/$(DIR_ENGINE))
	$(eval CURRENT_SRC = $(ENGINE_SRC))
	$(eval BUILDING_DYNAMIC_LIBRARY = 1)
	@ -mkdir -p $(BUILD_DIR)/$(DIR_OBJS_BASE)_$(DIR_OBJS_TYPE)/$(DIR_DEPS)
	@ -rm -f $(BUILD_LIBRARY)/$(NAME)
	@ $(MAKE) -s printout $(BUILD_LIBRARY)/$(NAME)
	@ printf "$(BOLD)$(DEFAULT)::Dynamic Library Built Successfully$(RESET)\n"

libraries: static dynamic ;@:

editor: static
	$(eval LD_FLAGS =)
	$(eval EDITR_TYPE = $(PRETTY_STRING_STATIC))
	$(eval NAME = $(STRING_LIB)$(NAME_BASE)$(NAME_STATIC))
	$(eval EDITR_LD_FLAGS = $(BUILD_LIBRARY)/$(NAME) -lstdc++exp)
	$(eval DIR_OBJS_TYPE = $(STRING_STATIC))
	$(eval BUILD_DIR = $(BUILD_ROOT)/$(DIR_EDITR))
	$(eval CURRENT_SRC = $(EDITR_SRC))
	$(eval BUILDING_STATIC_LIBRARY =)
	$(eval BUILDING_DYNAMIC_LIBRARY =)
	$(eval BUILDING_EDITR = 1)
	@ -mkdir -p $(BUILD_DIR)/$(DIR_OBJS_BASE)_$(DIR_OBJS_TYPE)/$(DIR_DEPS)
	@ -rm -f $(BUILD_DIR)/$(EDITR)
	@ $(MAKE) -s printout $(BUILD_DIR)/$(EDITR)
	@ printf "$(BOLD)$(DEFAULT)::Nostalgia Editor Built Successfully$(RESET)\n"

headers:
	@ printf "$(BOLD)$(RED)[TODO] Make header files that are designed for API use and only copy those into '$(BUILD_HEADERS)'$(RESET)\n"
	@ -mkdir -p $(BUILD_HEADERS)
	@ $(MAKE) -s $(HEADERS_OUT)
	@ printf "$(BOLD)$(GREEN)::Header files copied into '$(BUILD_HEADERS)'$(RESET)\n"

linux: ;@:
	$(eval EDITR           = $(EDITR_BASE))
	$(eval NAME_STATIC     = $(NAME_STATIC_LINUX))
	$(eval NAME_DYNAMIC    = $(NAME_DYNAMIC_LINUX))
	$(eval BUILD_ARCH      = $(DIR_LINUX))
	$(eval DEBUG_FLAGS     = $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_LINUX))
	$(eval RELEASE_FLAGS   = $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_LINUX))
	$(eval CXX_FLAGS       = $(FLAGS_CXX_COMMON) $(FLAGS_LINUX))
	$(eval CC_FLAGS        = $(FLAGS_CC_COMMON) $(FLAGS_LINUX))
	$(eval LD_FLAGS        = $(DYNAMIC_LDFLAGS_LINUX))
	$(eval EDITR_LD_FLAGS  = $(EDITR_LD_FLAGS_LINUX))
	$(eval ARCHIVES_DIR    = $(ARCHIVES_DIR_LINUX))
	$(eval CXX_COMPILER    = $(LINUX_CXX))
	$(eval C_COMPILER      = $(LINUX_CC))
	$(eval ARCHIVER        = $(LINUX_AR))
	$(eval TARGET_CALLED   = 1)
	$(eval CLEAN_ARCH      = $(DIR_LINUX))

windows: ;@:
	$(eval EDITR           = $(EDITR_BASE).exe)
	$(eval NAME_STATIC     = $(NAME_STATIC_WINDOWS))
	$(eval NAME_DYNAMIC    = $(NAME_DYNAMIC_WINDOWS))
	$(eval BUILD_ARCH      = $(DIR_WINDOWS))
	$(eval DEBUG_FLAGS     = $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_WINDOWS))
	$(eval RELEASE_FLAGS   = $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_WINDOWS))
	$(eval CXX_FLAGS       = $(FLAGS_CXX_COMMON) $(FLAGS_WINDOWS))
	$(eval CC_FLAGS        = $(FLAGS_CC_COMMON) $(FLAGS_WINDOWS))
	$(eval LD_FLAGS        = -fuse-ld=x86_64-w64-mingw32-ld $(DYNAMIC_LDFLAGS_WINDOWS))
	$(eval EDITR_LD_FLAGS  = $(EDITR_LD_FLAGS_WINDOWS))
	$(eval ARCHIVES_DIR    = $(ARCHIVES_DIR_WINDOWS))
	$(eval CXX_COMPILER    = $(WINDOWS_CXX))
	$(eval C_COMPILER      = $(WINDOWS_CC))
	$(eval ARCHIVER        = $(WINDOWS_AR))
	$(eval TARGET_CALLED   = 1)
	$(eval CLEAN_ARCH      = $(DIR_WINDOWS))

release: ;@:
	$(eval VERSION_FLAGS = $(RELEASE_FLAGS))
	$(eval BUILD_VERSION = $(DIR_RELEASE))
	$(eval JOLT_LIBRARY  = $(JOLT_RELEASE))
	$(eval TARGET_CALLED = 1)
	$(eval CLEAN_VERSION = $(DIR_RELEASE))

debug: ;@:
	$(eval VERSION_FLAGS = $(DEBUG_FLAGS))
	$(eval BUILD_VERSION = $(DIR_DEBUG))
	$(eval JOLT_LIBRARY  = $(JOLT_DEBUG))
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
$(BUILD_OBJS)/%.obj: $(CURRENT_SRC)/%.cpp
	@ printf "::Compiling $(BOLD)$(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) -c $< -o $@
	@ -mv $(@:.obj=.d) $(BUILD_DEPS)/$(notdir $(@:.obj=.d))

# C Object Files
$(BUILD_OBJS)/%.o: $(CURRENT_SRC)/%.c
	@ printf "::Compiling $(BOLD)$(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(C_COMPILER) $(CC_FLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) -c $< -o $@
	@ -mv $(@:.o=.d) $(BUILD_DEPS)/$(notdir $(@:.o=.d))

# Header Files
$(BUILD_HEADERS)/%.hpp: $(CURRENT_SRC)/%.hpp
	@ printf "::Copying Header $(BOLD)$(CYAN)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	@ cp $< $@

# Archive Object Files
$(BUILD_ARCHIVES)/%.o: $(ARCHIVES_DIR)/%.a
	@ printf "::Compiling $(BOLD)$(BLUE)$@$(NORM)$(GREEN) (Static Library)$(RESET)\n"
	@ -mkdir -p $(dir $@)
	@ $(CXX_COMPILER) -r -o $@ -Xlinker --whole-archive $^

# Static Library
$(BUILD_LIBRARY)/$(STRING_LIB)$(NAME_BASE)$(NAME_STATIC): $(CC_OBJS) $(CXX_OBJS) $(AR_OBJS)
	@ printf "::Building $(BOLD)$(GREEN)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	@ $(ARCHIVER) rcs $@ $^

# Dynamic Library
$(BUILD_LIBRARY)/$(STRING_LIB)$(NAME_BASE)$(NAME_DYNAMIC): $(CC_OBJS) $(CXX_OBJS)
	@ printf "::Building $(BOLD)$(GREEN)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) $^ -o $@ $(LD_FLAGS)

# Editor
$(BUILD_DIR)/$(EDITR): $(EDITR_OBJS)
	@ printf "::Building $(BOLD)$(GREEN)$@$(RESET)\n"
	$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(INCLUDE) $^ -o $@ $(EDITR_LD_FLAGS)

# Prints a unique cleanup message
NOTHING_TO_CLEAN.clean:
	@ printf "::Nothing left to clean\n"

# Prints a cleanup message
%.clean:
	@ printf "::Cleaned $(BOLD)$(RED)$*$(RESET)\n"

include $(wildcard $(BUILD_DEPS)/*.d)
