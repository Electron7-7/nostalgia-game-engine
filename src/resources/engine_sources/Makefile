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
EMBED_DIR := ../engine
EMBED_DIR_ABSOLUTE := src/resources/engine

#-----------------#
# Print Functions #
#-----------------#
PRINT_GENERATING = printf "$(DEFAULT)::Generating Engine Resource: $(1) ($(CYAN)$(notdir $(2))$(DEFAULT), $(CYAN)$(notdir $(3))$(DEFAULT))$(RESET)\n"
CLEANED = if [ -e $(1) ]; then printf "$(RESET)::Cleaned $(RED)$(EMBED_DIR_ABSOLUTE)/$(1:$(EMBED_DIR)/%=%)$(RESET)\n"; fi

#-------------------------------#
# C++ Variable Naming Functions #
#-------------------------------#
UPPER_CASE = $(shell echo '$1' | tr '[:lower:]' '[:upper:]')
SNAKE_CASE = $(subst -,_,$(subst .,_,$(basename $(notdir $(1)))))
CAMEL_CASE = $(shell echo "$(call SNAKE_CASE,$(1))" | sed -re 's/(_|^)(\w)/\U\2/g')
FILENAME   = $(2)_$(call UPPER_CASE,$(subst .,,$(suffix $(1))))_$(call CAMEL_CASE,$(1))

#---------------------------#
# Code Generation Functions #
#---------------------------#------------#
# Embedding Files as Strings (via `cat`) #
#----------------------------------------#
CAT_FILE_TO_STRING  = R\"($(subst ^M,,$(subst $$,,$(subst $$ ,\n,$(shell cat --show-ends $(1))))))\";
MAKE_STR_SOURCE_VAR = $(shell printf "\nconst char* $(call FILENAME,$(1),$(2)) = $(call CAT_FILE_TO_STRING,$(1))\n" >> $(3))
MAKE_STR_HEADER_VAR = $(shell printf "\nextern const char* $(call FILENAME,$(1),$(2));" >> $(3))

#--------------------------------------------#
# Embedding Files as Data Arrays (via `xxd`) #
#--------------------------------------------#
XXD=xxd -ps -c 16 $(1)
SED = sed                       \
-re 's/([0-9a-z]{2})/0x\1, /gm' \
-re 's/^/\t/'                   \
-e '$$a};'                      \
-e '1i \\nconst std::vector<unsigned char> $(1) =\n{'
DECLARE_DATA_VARS   = extern const std::vector<unsigned char> $(1);\\n
MAKE_DAT_SOURCE_VAR = $(shell $(call XXD,$(1)) | $(call SED,$(call FILENAME,$(1),$(2))) >> $(3))
MAKE_DAT_HEADER_VAR = $(shell printf "$(call DECLARE_DATA_VARS,$(call FILENAME,$(1),$(2)))" >> $(3))

# Font Variables
FONTS_GUARD := FONTS_H
FONTS_DIR   := fonts
FONTS       := $(wildcard $(FONTS_DIR)/*)
FONTS_CPP   := $(EMBED_DIR)/fonts.cpp
FONTS_HPP   := $(EMBED_DIR)/fonts.hpp

# GLSL Variables
GL_SHADERS_GUARD := OPENGL_SHADERS_H
GL_SHADERS_DIR   := shaders/GLSL
GL_SHADERS       := $(wildcard $(GL_SHADERS_DIR)/*)
GL_SHADERS_CPP   := $(EMBED_DIR)/opengl_shaders.cpp
GL_SHADERS_HPP   := $(EMBED_DIR)/opengl_shaders.hpp

# GLSL Naming Function
GLSL_HEADER_VAR = \
	$(call MAKE_STR_HEADER_VAR,$(1),$(2),$(3)) \
	$(shell printf "\n" >> $(3))

# Model Variables
MODELS_GUARD := MODELS_H
MODELS_DIR   := models
MODELS       := $(wildcard $(MODELS_DIR)/*.obj)
MODELS_HPP   := $(EMBED_DIR)/models.hpp
MODELS_CPP   := $(EMBED_DIR)/models.cpp

# Image Variables
IMAGES_GUARD := IMAGES_H
IMAGES_DIR   := images
IMAGES       := $(wildcard $(IMAGES_DIR)/*)
IMAGES_HPP   := $(EMBED_DIR)/images.hpp
IMAGES_CPP   := $(EMBED_DIR)/images.cpp

.PHONY: all build clean shaders fonts glsl models images

all: shaders models images fonts
	@ printf "$(DEFAULT)::Engine Resources Directory - $(GREEN)$(EMBED_DIR_ABSOLUTE)$(RESET)\n"

clean: clean_glsl clean_fonts clean_models clean_images ;@:

clean_glsl:
	@ $(call CLEANED,$(GL_SHADERS_CPP))
	@ $(call CLEANED,$(GL_SHADERS_HPP))
	@ $(RM) $(GL_SHADERS_CPP)
	@ $(RM) $(GL_SHADERS_HPP)

clean_fonts:
	@ $(call CLEANED,$(FONTS_CPP))
	@ $(call CLEANED,$(FONTS_HPP))
	@ $(RM) $(FONTS_CPP)
	@ $(RM) $(FONTS_HPP)

clean_models:
	@ $(call CLEANED,$(MODELS_CPP))
	@ $(call CLEANED,$(MODELS_HPP))
	@ $(RM) $(MODELS_CPP)
	@ $(RM) $(MODELS_HPP)

clean_images:
	@ $(call CLEANED,$(IMAGES_CPP))
	@ $(call CLEANED,$(IMAGES_HPP))
	@ $(RM) $(IMAGES_CPP)
	@ $(RM) $(IMAGES_HPP)

# Fonts
fonts: $(FONTS_CPP) $(FONTS_HPP)

$(FONTS_CPP): $(FONTS_HPP)
	@ $(call PRINT_GENERATING,Fonts,$@,$<)
	@ $(MAKE) -s $(FONTS:%=%.print)
	$(shell printf "#include \"$(notdir $<)\"\n" > $@)
	$(foreach font_file,$(FONTS),$(call MAKE_DAT_SOURCE_VAR,$(font_file),font,$@))

$(FONTS_HPP): $(FONTS)
	$(shell printf "#ifndef $(FONTS_GUARD)\n#define $(FONTS_GUARD)\n" > $@)
	$(shell printf "\n#include <vector>\n\n" >> $@)
	$(foreach font_file,$(FONTS),$(call MAKE_DAT_HEADER_VAR,$(font_file),font,$@))
	$(shell printf "\n#endif // $(FONTS_GUARD)\n" >> $@)

# Shaders (all this does is build every shader type; so far, I've only implemented GLSL)
shaders: glsl ;@:

# GLSL
glsl: $(GL_SHADERS_CPP) $(GL_SHADERS_HPP)

$(GL_SHADERS_CPP): $(GL_SHADERS_HPP)
	@ $(call PRINT_GENERATING,GLSL Shaders,$@,$<)
	@ $(MAKE) -s $(GL_SHADERS:%=%.print)
	$(shell printf "#include \"$(notdir $<)\"\n" > $@)
	$(foreach shader_file,$(GL_SHADERS),$(call MAKE_STR_SOURCE_VAR,$(shader_file),glsl,$@))

$(GL_SHADERS_HPP): $(GL_SHADERS)
	$(shell printf "#ifndef $(GL_SHADERS_GUARD)\n#define $(GL_SHADERS_GUARD)\n" > $@)
	$(foreach shader_stem,$(GL_SHADERS),$(call GLSL_HEADER_VAR,$(shader_stem),glsl,$@))
	$(shell printf "\n#endif // $(GL_SHADERS_GUARD)\n" >> $@)

# Models
models: $(MODELS_CPP) $(MODELS_HPP)

$(MODELS_CPP): $(MODELS_HPP)
	@ $(call PRINT_GENERATING,Models,$@,$<)
	@ $(MAKE) -s $(MODELS:%=%.print)
	$(shell printf "#include \"$(notdir $<)\"\n" > $@)
	$(foreach model,$(MODELS),$(call MAKE_STR_SOURCE_VAR,$(model),model,$@))

$(MODELS_HPP): $(MODELS)
	$(shell printf "#ifndef $(MODELS_GUARD)\n#define $(MODELS_GUARD)\n" > $@)
	$(foreach model,$(MODELS),$(call MAKE_STR_HEADER_VAR,$(model),model,$@))
	$(shell printf "\n\n#endif // $(MODELS_GUARD)" >> $@)

# Images
images: $(IMAGES_CPP) $(IMAGES_HPP)

$(IMAGES_CPP): $(IMAGES_HPP)
	@ $(call PRINT_GENERATING,Images,$@,$<)
	@ $(MAKE) -s $(IMAGES:%=%.print)
	$(shell printf "#include \"$(notdir $<)\"\n" > $@)
	$(foreach image,$(IMAGES),$(call MAKE_DAT_SOURCE_VAR,$(image),image,$@))

$(IMAGES_HPP): $(IMAGES)
	$(shell printf "#ifndef $(IMAGES_GUARD)\n#define $(IMAGES_GUARD)\n" > $@)
	$(shell printf "\n#include <vector>\n\n" >> $@)
	$(foreach image,$(IMAGES),$(call MAKE_DAT_HEADER_VAR,$(image),image,$@))
	$(shell printf "\n#endif // $(IMAGES_GUARD)\n" >> $@)

# Printouts
%.print: %
	@ printf "$(RESET)::Embedding: $(YELLOW)$(EMBED_DIR_ABSOLUTE)$<$(RESET)\n"
