# Output Colors
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

# Common Variables
EMBED_DIR := ../data
EMBED_DIR_ABSOLUTE := src/things/resources/data
SOURCES_DIR_ABSOLUTE := $(EMBED_DIR_ABSOLUTE)_sources

#-----------------#
# Print Functions #
#-----------------#
PRINT_GENERATING = printf "$(DEFAULT)::Generating Resource Data: $(1) ($(CYAN)$(notdir $(2))$(DEFAULT), $(CYAN)$(notdir $(3))$(DEFAULT))$(RESET)\n"
CLEANED = if [ -e $(1) ]; then printf "$(RESET)::Cleaned $(RED)$(EMBED_DIR_ABSOLUTE)/$(1:$(EMBED_DIR)/%=%)$(RESET)\n"; fi

#-------------------------------#
# C++ Variable Naming Functions #
#-------------------------------#
UPPER_CASE = $(shell echo '$1' | tr '[:lower:]' '[:upper:]')
SNAKE_CASE = $(subst -,_,$(subst .,_,$(basename $(notdir $(1)))))
NAMESPACE  = \nnamespace $(1)\n{
FILE_TYPE  = $(2)$(call UPPER_CASE,$(subst .,_,$(suffix $(1))))

#---------------------------#
# Code Generation Functions #
#---------------------------#
STAT = stat -c%s $(1)
XXD  = xxd -ps -c0 $(1)
SED  = sed                     \
-re 's/([0-9a-z]{2})/0x\1,/gm' \
-re 's/,$$/,/gm'               \
-re '$$s/,$$/};/'
DATA = $(call SNAKE_CASE,$(1))_data
SIZE = $(call SNAKE_CASE,$(1))_size

DATA_VAR  = printf "\n\tinline constinit unsigned char $(call DATA,$(1))[] = {" >> $(2)
SIZE_VAR  = printf   "\tinline constexpr long          $(call SIZE,$(1))   = $$($(call STAT,$(2)));" >> $(2)
FILE_DATA = printf "\n\tinline constinit FileData      $(call SNAKE_CASE,$(1))        = FileData(FileType::$(call FILE_TYPE,$(1),$(2)),$(call DATA,$(1)),$(call SIZE,$(1)));\n" >> $(3)
DATA_SIZE = $(call DATA_VAR,$(1),$(2));$(call XXD,$(1))|$(SED) >> $(2);$(call SIZE_VAR,$(1),$(2))
MAKE_VARS = $(shell $(call DATA_SIZE,$(1),$(3));$(call FILE_DATA,$(1),$(2),$(3)))
SIZE = \n\tconstexpr long $(call SNAKE_CASE,$(1))_len = std::extent_v<decltype($(call SNAKE_CASE,$(1)))>;\n
EMBED_DAT = $(shell printf "\n\tconstexpr unsigned char $(call SNAKE_CASE,$(1))[] =\n\t{\n\t\t#embed <$(1)>\n\t};$(call SIZE,$(1))" >> $(2))
EMBED_STR = $(shell printf "\n\tconstexpr char $(call SNAKE_CASE,$(1))[] =\n\t{\n\t\t#embed <$(1)> \\\\\n\t\tsuffix(,)\n\t\t0\n\t};$(call SIZE,$(1))" >> $(2))

# DATA_VAR = $(call SNAKE_CASE,$(1))_data
# MAKE_DATA_VARS  = $(shell printf "$(call XXD,$(1))" >> $(2))
# FILE_DATA_VAR   = constexpr FileData $(1)::$(call SNAKE_CASE,$(2))(FileType::$(call FILE_TYPE,$(2),$(3)),$(call DATA_VAR,$(1)),$(call DATA_VAR,$(1))_len)
# MAKE_SOURCE_VAR = $(shell printf "$(call VAR_BEGIN,$(1),$(2),$(3))" >> $(4); $(call VAR_END,$(2),$(3)) >> $(4))
# MAKE_HEADER_VAR = $(shell printf "\n\textern const FileData $(call SNAKE_CASE,$(1));" >> $(2))

# Font Variables
FONT        := font
NAMESP_FONT := Fonts
FONTS_GUARD := FONTS_H
FONTS_DIR   := fonts
FONTS       := $(wildcard $(FONTS_DIR)/*)
FONTS_CPP   := $(EMBED_DIR)/fonts.cpp
FONTS_HPP   := $(EMBED_DIR)/fonts.hpp

# GLSL Variables
GLSL             := glsl
NAMESP_GLSL      := GLSL
GL_SHADERS_GUARD := OPENGL_SHADERS_H
GL_SHADERS_DIR   := shaders/GLSL
GL_SHADERS       := $(wildcard $(GL_SHADERS_DIR)/*)
GL_SHADERS_CPP   := $(EMBED_DIR)/opengl_shaders.cpp
GL_SHADERS_HPP   := $(EMBED_DIR)/opengl_shaders.hpp

# Model Variables
MODEL        := model
NAMESP_MODEL := Models
MODELS_GUARD := MODELS_H
MODELS_DIR   := models
MODELS       := $(wildcard $(MODELS_DIR)/*.obj)
MODELS_HPP   := $(EMBED_DIR)/models.hpp
MODELS_CPP   := $(EMBED_DIR)/models.cpp

# Image Variables
IMAGE        := image
NAMESP_IMAGE := Images
IMAGES_GUARD := IMAGES_H
IMAGES_DIR   := images
IMAGES       := $(wildcard $(IMAGES_DIR)/*)
IMAGES_HPP   := $(EMBED_DIR)/images.hpp
IMAGES_CPP   := $(EMBED_DIR)/images.cpp

.PHONY: all build clean shaders fonts glsl models images

all: build shaders models images fonts
	@ printf "$(DEFAULT)::Engine Resource Data Directory - $(GREEN)$(EMBED_DIR_ABSOLUTE)$(RESET)\n"

build: ;@:
	@ -mkdir -p $(EMBED_DIR)

clean: clean_shaders clean_fonts clean_models clean_images ;@:

clean_shaders: clean_glsl ;@:

clean_glsl:
	@ $(call CLEANED,$(GL_SHADERS_HPP))
	@ $(RM) $(GL_SHADERS_HPP)

clean_fonts:
	@ $(call CLEANED,$(FONTS_HPP))
	@ $(RM) $(FONTS_HPP)

clean_models:
	@ $(call CLEANED,$(MODELS_HPP))
	@ $(RM) $(MODELS_HPP)

clean_images:
	@ $(call CLEANED,$(IMAGES_HPP))
	@ $(RM) $(IMAGES_HPP)

# Fonts
fonts: $(FONTS_HPP) $(FONTS)

$(FONTS_HPP): $(FONTS)
	@ $(call PRINT_GENERATING,Fonts,$@,$<)
	@ $(MAKE) -s $(FONTS:%=%.print)
	$(shell printf "#ifndef $(FONTS_GUARD)\n#define $(FONTS_GUARD)\n\n#include <type_traits>\n" > $@)
	$(shell printf "$(call NAMESPACE,$(NAMESP_FONT))" >> $@)
	$(foreach file,$^,$(call EMBED_DAT,$(file),$@))
	$(shell printf "}\n\n#endif // $(FONTS_GUARD)\n" >> $@)

# Images
images: $(IMAGES_HPP) $(IMAGES)

$(IMAGES_HPP): $(IMAGES)
	@ $(call PRINT_GENERATING,Images,$@,$<)
	@ $(MAKE) -s $(IMAGES:%=%.print)
	$(shell printf "#ifndef $(IMAGES_GUARD)\n#define $(IMAGES_GUARD)\n\n#include <type_traits>\n" > $@)
	$(shell printf "$(call NAMESPACE,$(NAMESP_IMAGE))" >> $@)
	$(foreach file,$^,$(call EMBED_DAT,$(file),$@))
	$(shell printf "}\n\n#endif // $(IMAGES_GUARD)\n" >> $@)

# Shaders (all this does is build every shader type; so far, I've only implemented GLSL)
shaders: glsl ;@:

# GLSL
glsl: $(GL_SHADERS_HPP) $(GL_SHADERS)

$(GL_SHADERS_HPP): $(GL_SHADERS)
	@ $(call PRINT_GENERATING,GLSL Shaders,$@,$<)
	@ $(MAKE) -s $(GL_SHADERS:%=%.print)
	$(shell printf "#ifndef $(GL_SHADERS_GUARD)\n#define $(GL_SHADERS_GUARD)\n" > $@)
	$(shell printf "$(call NAMESPACE,$(NAMESP_GLSL))" >> $@)
	$(foreach file,$^,$(call EMBED_STR,$(file),$@))
	$(shell printf "}\n\n#endif // $(GL_SHADERS_GUARD)\n" >> $@)

# Models
models: $(MODELS_HPP) $(MODELS)

$(MODELS_HPP): $(MODELS)
	@ $(call PRINT_GENERATING,Models,$@,$<)
	@ $(MAKE) -s $(MODELS:%=%.print)
	$(shell printf "#ifndef $(MODELS_GUARD)\n#define $(MODELS_GUARD)\n\n#include <type_traits>\n" > $@)
	$(shell printf "$(call NAMESPACE,$(NAMESP_MODEL))" >> $@)
	$(foreach file,$^,$(call EMBED_DAT,$(file),$@))
	$(shell printf "}\n\n#endif // $(MODELS_GUARD)\n" >> $@)

# Printouts
%.print: %
	@ printf "$(RESET)::Embedding: $(YELLOW)$(SOURCES_DIR_ABSOLUTE)/$<$(RESET)\n"
