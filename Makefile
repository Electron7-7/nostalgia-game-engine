LINUX_CXX := clang++
LINUX_CC  := clang

ifneq ($(OS),Windows_NT)
	WINDOWS_CXX := x86_64-w64-mingw32-g++
	WINDOWS_CC  := x86_64-w64-mingw32-gcc
else
	WINDOWS_CXX := g++
	WINDOWS_CC  := gcc
endif

LINUX_AR := llvm-ar
# TODO: Figure out if I need a windows-specific 'ar' and what it would be
WINDOWS_AR := $(LAR)

FLAGS_DEBUG_COMMON    := -g -O0 -D DEBUGGING
FLAGS_DEBUG_LINUX     := -fsanitize=address
FLAGS_DEBUG_WINDOWS   := # Nothing yet
FLAGS_RELEASE_COMMON  := -O3
FLAGS_RELEASE_WINDOWS := # Nothing yet
FLAGS_RELEASE_LINUX   := # Nothing yet
FLAGS_DYNAMIC         := -shared -fPIC
FLAGS_STATIC          := -static
FLAGS_CXX_COMMON      := -std=c++23 -Wall -D GLFW_INCLUDE_NONE -MMD -MP
FLAGS_CC_COMMON       := -std=c11 -Wall -MMD -MP
FLAGS_WINDOWS         := -mwindows -D COMPILING_WINDOWS
FLAGS_LINUX           := # Nothing yet

# Archives & Libraries
ARCHIVES_DIR_LINUX       := src/lib/linux/static
ARCHIVES_DIR_WINDOWS     := src/lib/windows/static
DYNAMIC_LIBS_DIR_LINUX   := src/lib/linux/dynamic
DYNAMIC_LIBS_DIR_WINDOWS := src/lib/windows/dynamic
DYNAMIC_LIBRARIES        := -lglfw -lbrotlicommon -lbrotlidec -lbrotlienc -lbz2 -lz -lpng -lfreetype
DYNAMIC_LDFLAGS_LINUX    := -shared -L $(DYNAMIC_LIBS_DIR_LINUX) $(DYNAMIC_LIBRARIES)
DYNAMIC_LDFLAGS_WINDOWS  := -lstdc++exp -shared --out-implib -L $(DYNAMIC_LIBS_DIR_WINDOWS) $(DYNAMIC_LIBRARIES)

INCLUDE := -I src -I src/thirdparty -I src/common -I src/thirdparty/FreeType

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
	export LD_FLAGS      ?= $(DYNAMIC_LDFLAGS_LINUX)
	export APP_LD_FLAGS  ?= -lNostalgia
	export ARCHIVES_DIR  ?= $(ARCHIVES_DIR_LINUX)
	export CXX_COMPILER  ?= $(LINUX_CXX)
	export C_COMPILER    ?= $(LINUX_CC)
	export ARCHIVER      ?= $(LINUX_AR)
else # WINDOWS
	export APP_NAME      ?= $(APP_BASE).exe
	export NAME_STATIC   ?= $(NAME_STATIC_WINDOWS)
	export NAME_DYNAMIC  ?= $(NAME_DYNAMIC_WINDOWS)
	export BUILD_ARCH    ?= $(DIR_WINDOWS)
	export DEBUG_FLAGS   ?= $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_WINDOWS)
	export RELEASE_FLAGS ?= $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_WINDOWS)
	export CXX_FLAGS     ?= $(FLAGS_CXX_COMMON) $(FLAGS_WINDOWS)
	export CC_FLAGS      ?= $(FLAGS_CC_COMMON) $(FLAGS_WINDOWS)
	export LD_FLAGS      ?= $(DYNAMIC_LDFLAGS_WINDOWS) -fuse-ld=x86_64-w64-mingw32-ld
	export APP_LD_FLAGS  ?= -lstdc++exp -fuse-ld=x86_64-w64-mingw32-ld -lstdc++exp -l:libNostalgia.dll
	export ARCHIVES_DIR  ?= $(ARCHIVES_DIR_WINDOWS)
	export CXX_COMPILER  ?= $(WINDOWS_CXX)
	export C_COMPILER    ?= $(WINDOWS_CC)
	export ARCHIVER      ?= $(WINDOWS_AR)
endif

export BUILD_VERSION ?= $(DIR_RELEASE)
export VERSION_FLAGS ?= $(RELEASE_FLAGS)

export BUILD_DIR      ?= $(DIR_ROOT)/$(BUILD_ARCH)/$(BUILD_VERSION)
export DIR_OBJS_TYPE  ?= $(STRING_STATIC)
export BUILD_LIBRARY  ?= $(BUILD_DIR)/$(DIR_LIBRARY)
export BUILD_HEADERS  ?= $(BUILD_DIR)/$(DIR_HEADERS)/$(NAME_BASE)
export BUILD_OBJS     ?= $(BUILD_DIR)/$(DIR_OBJS_BASE)_$(DIR_OBJS_TYPE)
export BUILD_DEPS     ?= $(BUILD_OBJS)/$(DIR_DEPS)

export NAME           ?= $(STRING_LIB)$(NAME_BASE)$(NAME_STATIC)
export APP_TYPE       ?= $(PRETTY_STRING_STATIC)
export APP            ?= $(APP_TYPE)$(APP_NAME)

export LIBRARY_FLAGS ?= $(FLAGS_STATIC)

export BUILDING_APP ?=
export BUILDING_DYNAMIC_LIBRARY ?=
export BUILDING_STATIC_LIBRARY  ?=

export CLEAN_ARCH    ?= .+
export CLEAN_VERSION ?= .+

VPATH := $(SRC_DIRS):$(DIR_ROOT)

SRC := src

SRC_DIRS :=                             \
    $(SRC)/commands                     \
    $(SRC)/common                       \
    $(SRC)/demo                         \
    $(SRC)/filesystem                   \
    $(SRC)/input                        \
    $(SRC)/managers                     \
    $(SRC)/rendering                    \
    $(SRC)/rendering/backends/graphics  \
    $(SRC)/rendering/backends/windowing \
    $(SRC)/rendering/shader_interfaces  \
    $(SRC)/resources                    \
    $(SRC)/resources/basic              \
    $(SRC)/resources/complex            \
    $(SRC)/resources/engine             \
    $(SRC)/settings                     \
    $(SRC)/types                        \
    $(SRC)/theatre                      \
    $(SRC)/things/actors

THIRDPARTY_SRC_DIRS :=                        \
	$(SRC)/thirdparty/DearImGui               \
	$(SRC)/thirdparty/DearImGui/misc/freetype \
	$(SRC)/thirdparty/glm/detail              \
	$(SRC)/thirdparty/glad

APP_SRC_DIRS :=               \
    $(SRC)/testing_app/app    \
    $(SRC)/testing_app/system \
    $(SRC)/testing_app/ui

DIRTY_SRC_DIRS := \
	thirdparty

RESOURCES_DIR := $(SRC)/resources/engine_sources

get_source_files = $(foreach directory,$(1),$(wildcard $(directory)/$(2)))

CC_SRCS  ?= $(call get_source_files,$(THIRDPARTY_SRC_DIRS),*.c)   $(call get_source_files,$(SRC_DIRS),*.c)
CXX_SRCS ?= $(call get_source_files,$(THIRDPARTY_SRC_DIRS),*.cpp) $(call get_source_files,$(SRC_DIRS),*.cpp)

APP_SRCS := $(call get_source_files,$(APP_SRC_DIRS),*.cpp)

HEADER_FILES := $(call get_source_files,$(SRC_DIRS),*.h) $(call get_source_files,$(SRC_DIRS),*.hpp)

export CC_OBJS  ?= $(addprefix $(BUILD_OBJS)/,$(subst .c,.o,$(CC_SRCS:$(SRC)/%=%)))
export CXX_OBJS ?= $(addprefix $(BUILD_OBJS)/,$(subst .cpp,.obj,$(CXX_SRCS:$(SRC)/%=%)))
export DEPS_OUT ?= $(notdir $(CC_SRCS:.c=.d)) $(notdir $(CXX_SRCS:.cpp=.d))

export ARCHIVES ?= $(wildcard $(ARCHIVES_DIR)/*.a)
export AR_OBJS  ?= $(addprefix $(BUILD_ARCHIVES)/,$(subst .a,.o,$(ARCHIVES:$(ARCHIVES_DIR)/%=%)))
export APP_OBJS ?= $(addprefix $(BUILD_OBJS)/,$(subst .cpp,.obj,$(APP_SRCS:$(SRC)/%=%)))

export HEADERS_OUT ?= $(addprefix $(BUILD_HEADERS)/,$(HEADER_FILES:$(SRC)/%=%))

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

.PHONY: all run printout build_dir static dynamic libraries testapp_static testapp_dynamic testapps headers rebuild_resources resources linux windows release debug clean_target clean_all clean mostlyclean disable_colors

all: headers resources static dynamic testapp_static testapp_dynamic ;@:

run: ;@:
	@ $(BUILD_DIR)/$(APP)

printout:
	@ printf "$(BOLD)$(DEFAULT)::Architecture - $(BOLD)$(BLUE)$(BUILD_ARCH)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::Version - $(BOLD)$(BLUE)$(BUILD_VERSION)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::C Compile Command - $(BOLD)$(YELLOW)$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) -c $(NORM)<source file>$(BOLD)$(YELLOW) -o $(NORM)<object file>$(YELLOW)$(LD_FLAGS)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::C++ Compile Command - $(BOLD)$(YELLOW)$(C_COMPILER) $(CC_FLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) -c $(NORM)<source file>$(BOLD)$(YELLOW) -o $(NORM)<object file>$(YELLOW)$(LD_FLAGS)$(RESET)\n"
	@ if [ -n "$(BUILDING_APP)" ]; then printf "$(BOLD)$(DEFAULT)::Linking Command - $(BOLD)$(YELLOW)$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(INCLUDE) $(NORM)<object files> $(BOLD)-o $(BUILD_DIR)/$(APP) $(APP_LD_FLAGS)$(RESET)\n"; fi
	@ if [ -n "$(BUILDING_DYNAMIC_LIBRARY)" ]; then \
		printf "$(BOLD)$(DEFAULT)::Linker Flags - $(BOLD)$(YELLOW)$(LD_FLAGS)$(RESET)\n"\
	; fi

	@ if [ -n "$(BUILDING_STATIC_LIBRARY)" ]; then \
		printf "$(BOLD)$(DEFAULT)::Archives Compile Command - $(BOLD)$(YELLOW)$(CXX_COMPILER) -r -o $(NORM)<archive object file>$(BOLD) -Xlinker --whole-archive $(NORM)<archive>$(RESET)\n"; \
		printf "$(BOLD)$(DEFAULT)::Library Command (Static) - $(BOLD)$(YELLOW)$(ARCHIVER) rcs $(BUILD_LIBRARY)/$(STRING_LIB)$(NAME_BASE)$(NAME_STATIC) $(NORM)<object files> <archive object files>$(RESET)\n" \
	; fi

build_dir:
	@ -mkdir -p $(BUILD_DIR)/$(DIR_OBJS_BASE)_$(DIR_OBJS_TYPE)/$(DIR_DEPS)

static: resources
	$(eval LD_FLAGS =)
	$(eval DIR_OBJS_TYPE = $(STRING_STATIC))
	$(eval LIBRARY_FLAGS = $(FLAGS_STATIC))
	$(eval NAME = $(STRING_LIB)$(NAME_BASE)$(NAME_STATIC))
	$(eval BUILDING_STATIC_LIBRARY = 1)
	@ -rm -f $(BUILD_LIBRARY)/$(NAME)
	@ -mkdir -p $(BUILD_ARCHIVES)
	@ $(MAKE) -s printout $(BUILD_LIBRARY)/$(NAME)
	@ printf "$(BOLD)$(DEFAULT)::Static Library Built Successfully$(RESET)\n"

dynamic: resources
	$(eval DIR_OBJS_TYPE = $(STRING_DYNAMIC))
	$(eval LIBRARY_FLAGS = $(FLAGS_DYNAMIC))
	$(eval NAME = $(STRING_LIB)$(NAME_BASE)$(NAME_DYNAMIC))
	$(eval BUILDING_DYNAMIC_LIBRARY = 1)
	@ -rm -f $(BUILD_LIBRARY)/$(NAME)
	@ $(MAKE) -s printout $(BUILD_LIBRARY)/$(NAME)
	@ printf "$(BOLD)$(DEFAULT)::Dynamic Library Built Successfully$(RESET)\n"

libraries: static dynamic ;@:

testapp_static:
	$(eval LD_FLAGS =)
	$(eval APP_TYPE = $(PRETTY_STRING_STATIC))
	$(eval NAME = $(STRING_LIB)$(NAME_BASE)$(NAME_STATIC))
	$(eval DIR_OBJS_TYPE = $(STRING_STATIC))
	$(eval BUILDING_STATIC_LIBRARY =)
	$(eval BUILDING_DYNAMIC_LIBRARY =)
	$(eval BUILDING_APP = 1)
	@ -rm -f $(BUILD_DIR)/$(APP)
	@ $(MAKE) -s printout $(BUILD_DIR)/$(APP)
	@ printf "$(BOLD)$(DEFAULT)::Static Application Built Successfully$(RESET)\n"

testapp_dynamic:
	$(eval APP_TYPE = $(PRETTY_STRING_DYNAMIC))
	$(eval NAME = $(STRING_LIB)$(NAME_BASE)$(NAME_DYNAMIC))
	$(eval DIR_OBJS_TYPE = $(STRING_DYNAMIC))
	$(eval BUILDING_STATIC_LIBRARY =)
	$(eval BUILDING_DYNAMIC_LIBRARY =)
	$(eval BUILDING_APP = 1)
	@ -rm -f $(BUILD_DIR)/$(APP)
	@ $(MAKE) -s printout $(BUILD_DIR)/$(APP)
	@ printf "$(BOLD)$(DEFAULT)::Dynamic Application Built Successfully$(RESET)\n"

testapps: testapp_static testapp_dynamic ;@:

headers:
	@ printf "$(BOLD)$(RED)[TODO][TODO][TODO] Make header files that are designed for API use and only copy those into '$(BUILD_HEADERS)/'$(RESET)\n"
	@ $(MAKE) -s $(HEADERS_OUT)

rebuild_resources:
	@ $(MAKE) -s -C $(RESOURCES_DIR) clean all

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
	$(eval LD_FLAGS      = $(DYNAMIC_LDFLAGS_LINUX))
	$(eval APP_LD_FLAGS  = -L /home/thelegend27/git/nostalgia-game-engine/build/Linux/Release/lib -l$(NAME_BASE))
	$(eval ARCHIVES_DIR  = $(ARCHIVES_DIR_LINUX))
	$(eval CXX_COMPILER  = $(LINUX_CXX))
	$(eval C_COMPILER    = $(LINUX_CC))
	$(eval ARCHIVER      = $(LINUX_AR))
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
	$(eval LD_FLAGS      = $(DYNAMIC_LDFLAGS_WINDOWS) -fuse-ld=x86_64-w64-mingw32-ld)
	$(eval APP_LD_FLAGS  = -L /home/thelegend27/git/nostalgia-game-engine/build/Linux/Release/lib -l$(NAME_BASE) -lstdc++exp -fuse-ld=x86_64-w64-mingw32-ld -lstdc++exp)
	$(eval ARCHIVES_DIR  = $(ARCHIVES_DIR_WINDOWS))
	$(eval CXX_COMPILER  = $(WINDOWS_CXX))
	$(eval C_COMPILER    = $(WINDOWS_CC))
	$(eval ARCHIVER      = $(WINDOWS_AR))
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
	@ $(MAKE) -s -C $(RESOURCES_DIR) clean

# Cleans the entire 'build/' directory (default behaviour for 'clean')
.__clean_all: ;@:
	$(eval CLEAN_FILES != $(call CLEAN,f,-regex '$(DIR_ROOT)/.*'))
	$(eval CLEAN_DIRS  != $(call CLEAN,d,-regex '$(DIR_ROOT)/.*'))
	$(RM) $(CLEAN_FILES)
	$(RM) -r $(CLEAN_DIRS)
	@ $(call CLEAN_PRINT,$(CLEAN_DIRS))
	@ $(MAKE) -s -C $(RESOURCES_DIR) clean

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
	$(eval CLEAN_DIRS  != $(call CLEAN,d,-regex '$(DIR_ROOT)/$(CLEAN_ARCH)/.*' $(call DIRTY,$(DIRTY_SRC_DIRS))))
	$(RM) $(CLEAN_FILES)
	$(RM) -r $(CLEAN_DIRS)
	@ $(call CLEAN_PRINT,$(CLEAN_DIRS))

.__mostlyclean_all: ;@:
	$(eval CLEAN_FILES != $(call CLEAN,f,$(call DIRTY,$(DIRTY_SRC_DIRS))))
	$(eval CLEAN_DIRS  != $(call CLEAN,d,$(call DIRTY,$(DIRTY_SRC_DIRS))))
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
$(BUILD_OBJS)/%.obj: $(SRC)/%.cpp | build_dir
	@ printf "::Compiling $(BOLD)$(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) -c $< -o $@
	@ -mv $(@:.obj=.d) $(BUILD_DEPS)/$(notdir $(@:.obj=.d))

# C Object Files
$(BUILD_OBJS)/%.o: $(SRC)/%.c | build_dir
	@ printf "::Compiling $(BOLD)$(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(C_COMPILER) $(CC_FLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) -c $< -o $@
	@ -mv $(@:.o=.d) $(BUILD_DEPS)/$(notdir $(@:.o=.d))

# Header Files
$(BUILD_HEADERS)/%.hpp: $(SRC)/%.hpp | build_dir
	@ printf "::Copying Header $(BOLD)$(CYAN)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	@ cp $< $@

# Archive Object Files
$(BUILD_ARCHIVES)/%.o: $(ARCHIVES_DIR)/%.a | build_dir
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

# Library Testing Applications
$(BUILD_DIR)/$(APP): $(APP_OBJS)
	@ printf "::Linking $(BOLD)$(GREEN)$@$(RESET)\n"
	$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(INCLUDE) $^ -o $@ $(APP_LD_FLAGS)


# Prints a unique cleanup message
NOTHING_TO_CLEAN.clean:
	@ printf "::Nothing left to clean\n"

# Prints a cleanup message
%.clean:
	@ printf "::Cleaned $(BOLD)$(RED)$*$(RESET)\n"

include $(wildcard $(BUILD_DEPS)/*.d)
