ifneq ($(OS),Windows_NT)
# Compiling for Windows on Linux
	CXX := x86_64-w64-mingw32-clang++
	CC  := x86_64-w64-mingw32-clang
	AR  := x86_64-w64-mingw32-ar
else
# Compiling for Windows on Windows
	CXX := clang++
	CC  := clang
	AR  := llvm-ar
endif

NAME := libNostalgia
ROOT := ../../..
SRC  := $(ROOT)/Nostalgia

BUILD_LIBRARY  := lib
BUILD_HEADERS  := include
BUILD_ARCHIVES := .static_libraries
BUILD_DYNAMIC  := .objs_dynamic
BUILD_STATIC   := .objs_static
BUILD_DEPENDS  := .dependencies

LIBRARY_STATIC  := $(BUILD_LIBRARY)/$(NAME).lib
LIBRARY_DYNAMIC := $(BUILD_LIBRARY)/$(NAME).dll

PCH_HPP := $(SRC)/Nostalgia.hpp
PCH     := Nostalgia.pch

STATIC_PCH_INCLUDE  := -include-pch $(BUILD_STATIC)/$(PCH)
DYNAMIC_PCH_INCLUDE := -include-pch $(BUILD_DYNAMIC)/$(PCH)

COMMONFLAGS := -D NOSTALGIA_DEBUGGING -D NOSTALGIA_WINDOWS -g -O0 -MMD -MP -Wall -fsanitize=address,undefined
CXXFLAGS := -std=c++26 $(COMMONFLAGS) -include $(SRC)/core/types.hpp --embed-dir=$(SRC)/embedded -Wno-c23-extensions -D JPH_PROFILE_ENABLED -D JPH_DEBUG_ENABLED -D JPH_OBJECT_STREAM -D GLFW_INCLUDE_NONE
CCFLAGS  := -std=c23 $(COMMONFLAGS)
LDFLAGS  := -lglfw -lfreetype -lstdc++exp

# Set by the `compile_commands` target (line 157)
export COMPILE_COMMANDS ?=

INCLUDE := -I $(ROOT) -I $(SRC) -I $(SRC)/thirdparty -I $(SRC)/thirdparty/FreeType

BACKEND_DIRS := \
	$(SRC)/backends \
	$(SRC)/backends/glfw \
	$(SRC)/backends/imgui \
	$(SRC)/backends/opengl

SRC_DIRS := \
    $(BACKEND_DIRS) \
    $(SRC)/application \
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

STATIC_LIBRARIES := $(SRC)/static_libraries/linux
JOLT := $(STATIC_LIBRARIES)/Jolt_Debug/libJolt.a

CC_SRCS  := $(call getfiles,$(SRC_DIRS),*.c)   $(call getfiles,$(THIRDPARTY_SRC_DIRS),*.c)
CXX_SRCS := $(call getfiles,$(SRC_DIRS),*.cpp) $(call getfiles,$(THIRDPARTY_SRC_DIRS),*.cpp)
ARCHIVES := $(wildcard $(STATIC_LIBRARIES)/Common/*.a)

STATIC_CC_OBJS  := $(addprefix $(BUILD_STATIC)/,$(subst .c,.o,$(CC_SRCS:$(SRC)/%=%)))
STATIC_CXX_OBJS := $(addprefix $(BUILD_STATIC)/,$(subst .cpp,.obj,$(CXX_SRCS:$(SRC)/%=%)))

DYNAMIC_CC_OBJS  := $(addprefix $(BUILD_DYNAMIC)/,$(subst .c,.o,$(CC_SRCS:$(SRC)/%=%)))
DYNAMIC_CXX_OBJS := $(addprefix $(BUILD_DYNAMIC)/,$(subst .cpp,.obj,$(CXX_SRCS:$(SRC)/%=%)))

AR_OBJS  := $(addprefix $(BUILD_ARCHIVES)/,$(ARCHIVES:$(STATIC_LIBRARIES)/%.a=%.o))
JOLT_OBJ := $(BUILD_ARCHIVES)/Jolt_Debug/libJolt.o

# TODO: Make better API headers
HEADER_FILES := $(PCH_HPP) $(call getfiles,$(API_HEADER_DIRS),*.hpp) $(call getfiles,$(API_HEADER_DIRS),*.h) $(call getfiles,$(API_HEADER_DIRS),*.inl)
HEADERS_OUT := $(addprefix $(BUILD_HEADERS)/,$(HEADER_FILES:$(SRC)/%=%))

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

.PHONY: all ._static ._dynamic static dynamic install headers compile_commands printout clean disable_colors

all: static dynamic headers

._static:
	@ -mkdir -p $(BUILD_STATIC) $(BUILD_DEPENDS) $(BUILD_ARCHIVES)

._dynamic:
	@ -mkdir -p $(BUILD_DYNAMIC) $(BUILD_DEPENDS) $(BUILD_ARCHIVES)

static: ._static $(LIBRARY_STATIC)
	@ printf "$(BOLD)$(DEFAULT)::Successfully Built Nostalgia$(RESET)\n"

dynamic: ._dynamic $(LIBRARY_DYNAMIC)
	@ printf "$(BOLD)$(DEFAULT)::Successfully Built Nostalgia$(RESET)\n"

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

headers:
	@ printf "$(BOLD)$(RED)[TODO] Make header files that are designed for API use and only copy those into '$(BUILD_HEADERS)'$(RESET)\n"
	@ -mkdir -p $(BUILD_HEADERS)
	@ $(MAKE) -s $(HEADERS_OUT)
	@ printf "$(BOLD)$(GREEN)::Header files copied into '$(BUILD_HEADERS)'$(RESET)\n"

compile_commands:
	$(eval COMPILE_COMMANDS = -U NOSTALGIA_GCC_PCH -D CLANGD_KEEPS_CRASHING_HERE -include $(SRC)/$(PCH_H)) # Fix a problem with LSP-clangd that's (most likely) due to the massive #embed for shader files

printout:
	@ printf "$(BOLD)$(DEFAULT)::Architecture - $(BOLD)$(BLUE)$(BUILD_ARCH)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::Version - $(BOLD)$(BLUE)$(BUILD_VERSION)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::C++ Compile Command - $(BOLD)$(YELLOW)$(CXX) $(CXXFLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) $(PCH_FLAGS) -c $(NORM)<source_file>$(BOLD)$(YELLOW) -o $(NORM)<object_file>$(YELLOW)$(RESET)\n"
	@ printf "$(BOLD)$(DEFAULT)::C Compile Command - $(BOLD)$(YELLOW)$(C_COMPILER) $(CC_FLAGS) $(VERSION_FLAGS) $(LIBRARY_FLAGS) $(INCLUDE) -c $(NORM)<source_file>$(BOLD)$(YELLOW) -o $(NORM)<object_file>$(YELLOW)$(RESET)\n"
	@ if [ -n "$(BUILDING_DYNAMIC_LIBRARY)" ]; then \
		printf "$(BOLD)$(DEFAULT)::Dynamic Compile Flags: $(BOLD)$(YELLOW)$(LINKER_FLAGS)$(RESET)\n"; \
		printf "$(BOLD)$(DEFAULT)::Linker Flags: $(BOLD)$(YELLOW)$(LD_FLAGS)$(RESET)\n" \
	; fi

	@ if [ -n "$(BUILDING_STATIC_LIBRARY)" ]; then \
		printf "$(BOLD)$(DEFAULT)::Archives Compile Command - $(BOLD)$(YELLOW)$(CXX) -r -o $(NORM)<compiled_archive_file>$(BOLD) -Xlinker --whole-archive $(NORM)<archive_file>$(RESET)\n"; \
		printf "$(BOLD)$(DEFAULT)::Library Command (Static) - $(BOLD)$(YELLOW)$(AR) rcs $(BUILD_LIBRARY)/$(STRING_LIB)$(NAME_BASE)$(NAME_STATIC) $(NORM)<object_files> <compiled_archive_file>$(RESET)\n" \
	; fi

clean:
	@ $(RM) -r \
		$(BUILD_STATIC) \
		$(BUILD_DYNAMIC) \
		$(BUILD_LIBRARY) \
		$(BUILD_HEADERS) \
		$(BUILD_DEPENDS) \
		$(BUILD_ARCHIVES)
	@ printf "$(BOLD)$(RED)::Cleaned Build$(RESET)\n"

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
$(BUILD_STATIC)/$(PCH): $(PCH_HPP)
	@ printf "::Building Pre-Compiled Header: $(BOLD)$(BLUE)$@$(RESET)\n"
	$(CXX) $(CXXFLAGS) $(INCLUDE) -static -xc++-header -c $< -o $@
	@ -mv $(@:.pch=.d) $(BUILD_DEPENDS)/$(notdir $(@:.pch=.d))

# Pre-Compiled Header
$(BUILD_DYNAMIC)/$(PCH): $(PCH_HPP)
	@ printf "::Building Pre-Compiled Header: $(BOLD)$(BLUE)$@$(RESET)\n"
	$(CXX) $(CXXFLAGS) $(INCLUDE) -fPIC -xc++-header -c $< -o $@
	@ -mv $(@:.pch=.d) $(BUILD_DEPENDS)/$(notdir $(@:.pch=.d))

# C++ Object Files (Static Library)
$(BUILD_STATIC)/%.obj: $(SRC)/%.cpp
	@ printf "::Compiling $(BOLD)$(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(STATIC_PCH_INCLUDE) -static -c $< -o $@
	@ -mv $(@:.obj=.d) $(BUILD_DEPENDS)/$(notdir $(@:.obj=.d))

# C Object Files (Static Library)
$(BUILD_STATIC)/%.o: $(SRC)/%.c
	@ printf "::Compiling $(BOLD)$(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) $(INCLUDE) -static -c $< -o $@
	@ -mv $(@:.o=.d) $(BUILD_DEPENDS)/$(notdir $(@:.o=.d))

# C++ Object Files (Dynamic Library)
$(BUILD_DYNAMIC)/%.obj: $(SRC)/%.cpp
	@ printf "::Compiling $(BOLD)$(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(DYNAMIC_PCH_INCLUDE) -fPIC -c $< -o $@
	@ -mv $(@:.obj=.d) $(BUILD_DEPENDS)/$(notdir $(@:.obj=.d))

# C Object Files (Dynamic Library)
$(BUILD_DYNAMIC)/%.o: $(SRC)/%.c
	@ printf "::Compiling $(BOLD)$(BLUE)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) $(INCLUDE) -fPIC -c $< -o $@
	@ -mv $(@:.o=.d) $(BUILD_DEPENDS)/$(notdir $(@:.o=.d))

# Header Files
$(BUILD_HEADERS)/%: $(SRC)/%
	@ printf "::Copying Header $(BOLD)$(CYAN)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	@ cp $< $@

# Archive Object Files
$(BUILD_ARCHIVES)/%.o: $(STATIC_LIBRARIES)/%.a
	@ printf "::Compiling $(BOLD)$(BLUE)$@$(NORM)$(GREEN) (Static Library)$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(CXX) -r -o $@ -Xlinker --whole-archive $^

# Static Library
$(LIBRARY_STATIC): $(BUILD_STATIC)/$(PCH) $(STATIC_CC_OBJS) $(STATIC_CXX_OBJS) $(AR_OBJS)
	@ printf "::Building $(BOLD)$(GREEN)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(AR) rcs $@ $^

# Dynamic Library
$(LIBRARY_DYNAMIC): $(BUILD_DYNAMIC)/$(PCH) $(DYNAMIC_CC_OBJS) $(DYNAMIC_CXX_OBJS) $(JOLT_OBJ)
	@ printf "::Building $(BOLD)$(GREEN)$@$(RESET)\n"
	@ -mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -fPIC -s -shared -Wl,--out-implib -Wl,--subsytem,windows $^ -o $@ $(LDFLAGS)

# Prints a unique cleanup message
NOTHING_TO_CLEAN.clean:
	@ printf "::Nothing left to clean\n"

# Prints a cleanup message
%.clean:
	@ printf "::Cleaned $(BOLD)$(RED)$*$(RESET)\n"

include $(wildcard $(BUILD_DEPENDS)/*.d)
