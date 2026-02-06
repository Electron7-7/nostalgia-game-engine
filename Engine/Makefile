# Installation directories. Change these variables in order to change where the `install` target
# chooses to install the library + headers.
INSTALL_DIR_LINUX   := /usr/local
INSTALL_DIR_MINGW   := /usr/x86_64-w64-mingw32
INSTALL_DIR_WINDOWS := # For the time being, compiling on Windows involves using MSYS2 & MinGW

LINUX_CXX := clang++
LINUX_CC  := clang
LINUX_AR  := llvm-ar

ifneq ($(OS),Windows_NT)
# Compiling for Windows on Linux
	WINDOWS_CXX := x86_64-w64-mingw32-g++
	WINDOWS_CC  := x86_64-w64-mingw32-gcc
	WINDOWS_AR  := x86_64-w64-mingw32-ar
else
# Compiling for Windows on Windows
	WINDOWS_CXX := g++
	WINDOWS_CC  := gcc
	WINDOWS_AR  := ar
endif

SRC := Nostalgia

FLAGS_DEBUG_COMMON    := -g -O0 -D NOSTALGIA_DEBUGGING -Winvalid-pch
FLAGS_DEBUG_LINUX     := -fsanitize=address,undefined
FLAGS_RELEASE_COMMON  := -O3 -D NOSTALGIA_RELEASE
FLAGS_DYNAMIC         := -fPIC
FLAGS_STATIC          := -static
FLAGS_CXX_COMMON      := -std=c++26 -Wall -MMD -MP --embed-dir=$(SRC)/embedded -D JPH_PROFILE_ENABLED -D JPH_DEBUG_ENABLED -D JPH_OBJECT_STREAM -D GLFW_INCLUDE_NONE
FLAGS_CC_COMMON       := -std=c23 -Wall -MMD -MP
FLAGS_WINDOWS         := -mwindows -lstdc++exp -include $(SRC)/core/types.hpp -D NOSTALGIA_WINDOWS -D NOSTALGIA_GCC_PCH
FLAGS_LINUX           := -D NOSTALGIA_LINUX -Wno-c23-extensions

# Archives & Libraries
ARCHIVES_DIR_LINUX        := $(SRC)/static_libraries/linux
ARCHIVES_DIR_WINDOWS      := $(SRC)/static_libraries/windows
DYNAMIC_LIB_FLAGS_LINUX   := -shared
DYNAMIC_LIB_FLAGS_WINDOWS := -shared --out-implib

INCLUDE := -I ./ -I $(SRC) -I $(SRC)/thirdparty -I $(SRC)/thirdparty/FreeType

DIR_ROOT      := build
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

# LINUX
ifneq ($(OS),Windows_NT)
	export INSTALL_DIR     ?= $(INSTALL_DIR_LINUX)
	export NAME_STATIC     ?= $(NAME_STATIC_LINUX)
	export NAME_DYNAMIC    ?= $(NAME_DYNAMIC_LINUX)
	export BUILD_ARCH      ?= $(DIR_LINUX)
	export DEBUG_FLAGS     ?= $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_LINUX)
	export RELEASE_FLAGS   ?= $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_LINUX)
	export LINKER_FLAGS    ?= $(DYNAMIC_LIB_FLAGS_LINUX)
	export CXX_FLAGS       ?= $(FLAGS_CXX_COMMON) $(FLAGS_LINUX)
	export PCH_FLAGS       ?= -include-pch $(BUILD_OBJS)
	export CC_FLAGS        ?= $(FLAGS_CC_COMMON) $(FLAGS_LINUX)
	export LD_FLAGS        ?= $(DYNAMIC_LIBRARIES)
	export ARCHIVES_DIR    ?= $(ARCHIVES_DIR_LINUX)
	export CXX_COMPILER    ?= $(LINUX_CXX)
	export C_COMPILER      ?= $(LINUX_CC)
	export ARCHIVER        ?= $(LINUX_AR)
else # WINDOWS
# Again, installing on Windows is a manual process for the time being, so the installation
# target assumes you're compiling for Windows on Linux, using MinGW
	export INSTALL_DIR     ?= $(INSTALL_DIR_MINGW)
	export NAME_STATIC     ?= $(NAME_STATIC_WINDOWS)
	export NAME_DYNAMIC    ?= $(NAME_DYNAMIC_WINDOWS)
	export BUILD_ARCH      ?= $(DIR_WINDOWS)
	export DEBUG_FLAGS     ?= $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_WINDOWS)
	export RELEASE_FLAGS   ?= $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_WINDOWS)
	export LINKER_FLAGS    ?= $(DYNAMIC_LIB_FLAGS_WINDOWS)
	export CXX_FLAGS       ?= $(FLAGS_CXX_COMMON) $(FLAGS_WINDOWS)
	export PCH_FLAGS       ?= -I $(dir $(BUILD_OBJS))
	export CC_FLAGS        ?= $(FLAGS_CC_COMMON) $(FLAGS_WINDOWS)
	export LD_FLAGS        ?= -fuse-ld=x86_64-w64-mingw32-ld $(DYNAMIC_LIBRARIES)
	export ARCHIVES_DIR    ?= $(ARCHIVES_DIR_WINDOWS)
	export CXX_COMPILER    ?= $(WINDOWS_CXX)
	export C_COMPILER      ?= $(WINDOWS_CC)
	export ARCHIVER        ?= $(WINDOWS_AR)
endif

export INSTALL_LIB_DIR  ?= $(INSTALL_DIR)/lib
export INSTALL_INCL_DIR ?= $(INSTALL_DIR)/include

export BUILD_VERSION ?= $(DIR_RELEASE)
export VERSION_FLAGS ?= $(RELEASE_FLAGS)
export PRINTOUT_TYPE ?= $(PRETTY_STRING_STATIC)

export BUILD_ROOT     ?= $(DIR_ROOT)/$(BUILD_ARCH)/$(BUILD_VERSION)
export DIR_OBJS_TYPE  ?= $(STRING_STATIC)
export BUILD_LIBRARY  ?= $(BUILD_ROOT)/$(DIR_LIBRARY)
export BUILD_HEADERS  ?= $(BUILD_ROOT)/$(DIR_HEADERS)/$(NAME_BASE)
export BUILD_OBJS     ?= $(BUILD_ROOT)/$(DIR_OBJS_BASE)_$(DIR_OBJS_TYPE)
export BUILD_ARCHIVES ?= $(BUILD_ROOT)/$(DIR_OBJS_BASE)_$(DIR_OBJS_TYPE)/$(DIR_ARCHIVES)
export BUILD_DEPS     ?= $(BUILD_OBJS)/$(DIR_DEPS)

export NAME  ?= $(STRING_LIB)$(NAME_BASE)$(NAME_STATIC)

export LIBRARY_FLAGS ?= $(FLAGS_STATIC)

export BUILDING_DYNAMIC_LIBRARY ?=
export BUILDING_STATIC_LIBRARY  ?=

export CLEAN_ARCH    ?= .+
export CLEAN_VERSION ?= .+

VPATH := $(SRC_DIRS):$(DIR_ROOT)

BACKEND_DIRS := \
	$(SRC)/backends \
	$(SRC)/backends/glfw \
	$(SRC)/backends/imgui \
	$(SRC)/backends/opengl

SRC_DIRS := \
	$(SRC) \
    $(SRC)/application \
    $(BACKEND_DIRS) \
    $(SRC)/common \
    $(SRC)/components \
    $(SRC)/console \
    $(SRC)/core \
    $(SRC)/embedded \
    $(SRC)/events \
    $(SRC)/filesystem \
    $(SRC)/fwd \
    $(SRC)/managers \
    $(SRC)/math \
    $(SRC)/physics \
    $(SRC)/rendering \
    $(SRC)/settings \
    $(SRC)/theatre \
    $(SRC)/theatre/parser \
    $(SRC)/theatre/things \
    $(SRC)/theatre/things/resources \
    $(SRC)/theatre/things/thinkers \
    $(SRC)/theatre/things/thinkers/3d \
    $(SRC)/theatre/things/thinkers/2d \
	$(SRC)/ui \

THIRDPARTY_SRC_DIRS := \
	$(SRC)/thirdparty/DearImGui \
	$(SRC)/thirdparty/frozen \
	$(SRC)/thirdparty/frozen/bits \
	$(SRC)/thirdparty/glm/detail \
	$(SRC)/thirdparty/glad

DIRTY_SRC_DIRS := \
	thirdparty

API_HEADER_DIRS := \
	$(filter-out $(BACKEND_DIRS),$(SRC_DIRS)) \
	$(SRC)/backends/imgui \
    $(SRC)/thirdparty/DearImGui \
    $(SRC)/thirdparty/frozen \
    $(SRC)/thirdparty/frozen/bits \
    $(SRC)/thirdparty/glm \
    $(SRC)/thirdparty/glm/gtc \
    $(SRC)/thirdparty/glm/gtx \
    $(SRC)/thirdparty/glm/ext \
    $(SRC)/thirdparty/glm/detail \
    $(SRC)/thirdparty/glm/simd

getfiles = $(foreach directory,$(1),$(wildcard $(directory)/$(2)))

CC_SRCS  ?= $(call getfiles,$(SRC_DIRS),*.c)   $(call getfiles,$(THIRDPARTY_SRC_DIRS),*.c)
CXX_SRCS ?= $(call getfiles,$(SRC_DIRS),*.cpp) $(call getfiles,$(THIRDPARTY_SRC_DIRS),*.cpp)

# TODO: Make better API headers
HEADER_FILES := $(call getfiles,$(API_HEADER_DIRS),*.hpp) $(call getfiles,$(API_HEADER_DIRS),*.h) $(call getfiles,$(API_HEADER_DIRS),*.inl)

PCH   := Nostalgia.pch
PCH_H := Nostalgia.hpp

export JOLT_DEBUG    ?= $(ARCHIVES_DIR)/Jolt_Debug/libJolt.a
export JOLT_RELEASE  ?= $(ARCHIVES_DIR)/Jolt_Debug/libJolt.a
# export JOLT_RELEASE  ?= $(ARCHIVES_DIR)/Jolt_Distribution/libJolt.a
export JOLT_LIBRARY  ?= $(JOLT_RELEASE)
export DYNAMIC_LIBRARIES  ?= -lglfw -lfreetype

export CC_OBJS  ?= $(addprefix $(BUILD_OBJS)/,$(subst .c,.o,$(CC_SRCS:$(SRC)/%=%)))
export CXX_OBJS ?= $(addprefix $(BUILD_OBJS)/,$(subst .cpp,.obj,$(CXX_SRCS:$(SRC)/%=%)))
export DEPS_OUT ?= $(notdir $(CC_SRCS:.c=.d)) $(notdir $(CXX_SRCS:.cpp=.d))

export ARCHIVES ?= $(wildcard $(ARCHIVES_DIR)/Common/*.a) $(JOLT_LIBRARY)
export AR_OBJS  ?= $(addprefix $(BUILD_ARCHIVES)/,$(ARCHIVES:$(ARCHIVES_DIR)/%.a=%.o))

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

.PHONY: __none install compile_commands printout build static dynamic headers linux windows release debug clean mostlyclean disable_colors .__clean_target .__clean_all .__mostlyclean_target .__mostlyclean_all

__none: ;@:
	$(info Nothing done; you need to include the 'build' target, lol)

install: headers
	@ if [ -e $(BUILD_LIBRARY)/$(NAME) ]; then \
		printf "$(BOLD)$(MAGENTA)::Requesting root permissions in order to access \\x1b[4m$(INSTALL_DIR)\\x1b[24m"; \
		printf "$(BLUE) (see lines \\x1b[4m224\\x1b[24m, \\x1b[4m228\\x1b[24m, and \\x1b[4m229\\x1b[24m in the Makefile if you wish to verify for yourself)$(RESET)\n"; \
		sudo cp $(BUILD_LIBRARY)/$(NAME) $(INSTALL_LIB_DIR)/; \
		printf "$(BOLD)$(DEFAULT)::Installed $(GREEN)$(INSTALL_LIB_DIR)/$(NAME)$(RESET)\n" \
	; fi
	@ if [ -d $(BUILD_HEADERS) ]; then \
		sudo $(RM) -r $(INSTALL_INCL_DIR)/$(BUILD_HEADERS); \
		sudo cp -r $(BUILD_HEADERS) $(INSTALL_INCL_DIR)/; \
		printf "$(BOLD)$(DEFAULT)::Installed $(GREEN)$(INSTALL_INCL_DIR)/$(NAME_BASE)/$(RESET)\n" \
	; fi
	@ if [ ! -d $(INSTALL_INCL_DIR)/Jolt ]; then \
		printf "$(BOLD)$(YELLOW)::IMPORTANT NOTE - In order to use the Jolt headers properly, you'll need to install them to \\x1b[4m$(INSTALL_INCL_DIR)/Jolt\\x1b[24m. An easy shortcut you could use is just copying/linking \\x1b[4mEngine/Nostalgia/thirdparty/Jolt\\x1b[24m from the source code.$(RESET)\n" \
	; fi

compile_commands:
	$(eval CXX_FLAGS += -D CLANGD_KEEPS_CRASHING_HERE) # Fix a problem with LSP-clangd that's (most likely) due to the massive #embed for shader files
	$(eval PCH_FLAGS = -U NOSTALGIA_GCC_PCH -include $(SRC)/$(PCH_H))

printout:
	@ printf "$(BOLD)$(DEFAULT)::Architecture - $(BOLD)$(BLUE)$(BUILD_ARCH)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::Version - $(BOLD)$(BLUE)$(BUILD_VERSION)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::C++ Compile Command - $(BOLD)$(YELLOW)$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) $(PCH_FLAGS) -c $(NORM)<source_file>$(BOLD)$(YELLOW) -o $(NORM)<object_file>$(YELLOW)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::C Compile Command - $(BOLD)$(YELLOW)$(C_COMPILER) $(CC_FLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) -c $(NORM)<source_file>$(BOLD)$(YELLOW) -o $(NORM)<object_file>$(YELLOW)$(RESET)\n"
	@ if [ -n "$(BUILDING_DYNAMIC_LIBRARY)" ]; then \
		printf "$(BOLD)$(DEFAULT)::Linker Flags - $(BOLD)$(YELLOW)$(LD_FLAGS)$(RESET)\n"\
	; fi

	@ if [ -n "$(BUILDING_STATIC_LIBRARY)" ]; then \
		printf "$(BOLD)$(DEFAULT)::Archives Compile Command - $(BOLD)$(YELLOW)$(CXX_COMPILER) -r -o $(NORM)<compiled_archive_file>$(BOLD) -Xlinker --whole-archive $(NORM)<archive_file>$(RESET)\n"; \
		printf "$(BOLD)$(DEFAULT)::Library Command (Static) - $(BOLD)$(YELLOW)$(ARCHIVER) rcs $(BUILD_LIBRARY)/$(STRING_LIB)$(NAME_BASE)$(NAME_STATIC) $(NORM)<object_files> <compiled_archive_file>$(RESET)\n" \
	; fi

build:
	@ -mkdir -p $(BUILD_ROOT)/$(DIR_OBJS_BASE)_$(DIR_OBJS_TYPE)/$(DIR_DEPS)
	@ $(MAKE) -s printout $(BUILD_OBJS)/$(PCH) $(BUILD_LIBRARY)/$(NAME)
	@ printf "$(BOLD)$(DEFAULT)::$(PRINTOUT_TYPE) Library Built Successfully$(RESET)\n"

static: ;@:
	$(eval PRINTOUT_TYPE = $(PRETTY_STRING_STATIC))
	$(eval DIR_OBJS_TYPE = $(STRING_STATIC))
	$(eval LIBRARY_FLAGS = $(FLAGS_STATIC))
	$(eval NAME = $(STRING_LIB)$(NAME_BASE)$(NAME_STATIC))
	$(eval CURRENT_SRC = $(SRC))
	$(eval BUILDING_DYNAMIC_LIBRARY =)
	$(eval BUILDING_STATIC_LIBRARY = 1)

dynamic: ;@:
	$(eval PRINTOUT_TYPE = $(PRETTY_STRING_DYNAMIC))
	$(eval DIR_OBJS_TYPE = $(STRING_DYNAMIC))
	$(eval LIBRARY_FLAGS = $(FLAGS_DYNAMIC))
	$(eval NAME = $(STRING_LIB)$(NAME_BASE)$(NAME_DYNAMIC))
	$(eval CURRENT_SRC = $(SRC))
	$(eval BUILDING_DYNAMIC_LIBRARY = 1)
	$(eval BUILDING_STATIC_LIBRARY =)
	$(eval BUILD_ARCHIVES =)

headers:
	@ printf "$(BOLD)$(RED)[TODO] Make header files that are designed for API use and only copy those into '$(BUILD_HEADERS)'$(RESET)\n"
	@ -mkdir -p $(BUILD_HEADERS)
	@ $(MAKE) -s $(HEADERS_OUT)
	@ printf "$(BOLD)$(GREEN)::Header files copied into '$(BUILD_HEADERS)'$(RESET)\n"

linux: ;@:
	$(eval INSTALL_DIR     = $(INSTALL_DIR_LINUX))
	$(eval NAME_STATIC     = $(NAME_STATIC_LINUX))
	$(eval NAME_DYNAMIC    = $(NAME_DYNAMIC_LINUX))
	$(eval BUILD_ARCH      = $(DIR_LINUX))
	$(eval DEBUG_FLAGS     = $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_LINUX))
	$(eval RELEASE_FLAGS   = $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_LINUX))
	$(eval CXX_FLAGS       = $(FLAGS_CXX_COMMON) $(FLAGS_LINUX))
	$(eval PCH_FLAGS       = -include-pch $(BUILD_OBJS))
	$(eval CC_FLAGS        = $(FLAGS_CC_COMMON) $(FLAGS_LINUX))
	$(eval LD_FLAGS        = $(DYNAMIC_LIBRARIES))
	$(eval ARCHIVES_DIR    = $(ARCHIVES_DIR_LINUX))
	$(eval CXX_COMPILER    = $(LINUX_CXX))
	$(eval C_COMPILER      = $(LINUX_CC))
	$(eval ARCHIVER        = $(LINUX_AR))
	$(eval TARGET_CALLED   = 1)
	$(eval CLEAN_ARCH      = $(DIR_LINUX))

windows: ;@:
	$(eval INSTALL_DIR     = $(INSTALL_DIR_MINGW))
	$(eval NAME_STATIC     = $(NAME_STATIC_WINDOWS))
	$(eval NAME_DYNAMIC    = $(NAME_DYNAMIC_WINDOWS))
	$(eval BUILD_ARCH      = $(DIR_WINDOWS))
	$(eval DEBUG_FLAGS     = $(FLAGS_DEBUG_COMMON) $(FLAGS_DEBUG_WINDOWS))
	$(eval RELEASE_FLAGS   = $(FLAGS_RELEASE_COMMON) $(FLAGS_RELEASE_WINDOWS))
	$(eval CXX_FLAGS       = $(FLAGS_CXX_COMMON) $(FLAGS_WINDOWS))
	$(eval PCH_FLAGS       = -D NOSTALGIA_GCC_PCH -I $(BUILD_OBJS))
	$(eval CC_FLAGS        = $(FLAGS_CC_COMMON) $(FLAGS_WINDOWS))
	$(eval LD_FLAGS        = -fuse-ld=x86_64-w64-mingw32-ld $(DYNAMIC_LIBRARIES))
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

# Pre-Compiled Header
$(BUILD_OBJS)/$(PCH): $(SRC)/$(PCH_H)
	@ printf "::Building Pre-Compiled Header: $(BOLD)$(BLUE)$@$(RESET)\n"
	$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) -xc++-header -c $< -o $@
	@ -mv $(@:.pch=.d) $(BUILD_DEPS)/$(notdir $(@:.pch=.d))

# C++ Object Files
$(BUILD_OBJS)/%.obj: $(SRC)/%.cpp
	@ printf "::Compiling $(BOLD)$(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) $(PCH_FLAGS) -c $< -o $@
	@ -mv $(@:.obj=.d) $(BUILD_DEPS)/$(notdir $(@:.obj=.d))

# C Object Files
$(BUILD_OBJS)/%.o: $(SRC)/%.c
	@ printf "::Compiling $(BOLD)$(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(C_COMPILER) $(CC_FLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) -c $< -o $@
	@ -mv $(@:.o=.d) $(BUILD_DEPS)/$(notdir $(@:.o=.d))

# Header Files
$(BUILD_HEADERS)/%: $(SRC)/%
	@ printf "::Copying Header $(BOLD)$(CYAN)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	@ cp $< $@

# Main Header File (becomes pch)
$(BUILD_HEADERS)/%.hpp: %.hpp
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
$(BUILD_LIBRARY)/$(STRING_LIB)$(NAME_BASE)$(NAME_DYNAMIC): $(CC_OBJS) $(CXX_OBJS) $(JOLT_LIBRARY)
	@ printf "::Building $(BOLD)$(GREEN)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(CXX_COMPILER) $(CXX_FLAGS) $(VERSION_FLAGS) $(INCLUDE) $(LINKER_FLAGS) $^ -o $@ $(LD_FLAGS)

# Prints a unique cleanup message
NOTHING_TO_CLEAN.clean:
	@ printf "::Nothing left to clean\n"

# Prints a cleanup message
%.clean:
	@ printf "::Cleaned $(BOLD)$(RED)$*$(RESET)\n"

include $(wildcard $(BUILD_DEPS)/*.d)
