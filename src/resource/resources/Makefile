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
TOP       := ../../
EMBED_DIR := $(TOP)embedded

#-----------------#
# Print Functions #
#-----------------#
PRINT_GENERATING = printf "$(DEFAULT)::Generating Embedded $(1) ($(CYAN)$(notdir $(2))$(DEFAULT), $(CYAN)$(notdir $(3))$(DEFAULT))$(RESET)\n"
PRINT_CLEANED    = @ printf "$(RESET)::Cleaned $(RED)$(subst $(TOP),src/,$(1))$(RESET)\n"

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
MAKE_STR_SOURCE_VAR = $(shell printf "\nconst char* $(call FILENAME,$(1),$(2)) = $(call CAT_FILE_TO_STRING,$(1))" >> $(3))
MAKE_STR_HEADER_VAR = $(shell printf "\nextern const char* $(call FILENAME,$(1),$(2));" >> $(3))

#--------------------------------------------#
# Embedding Files as Data Arrays (via `xxd`) #
#--------------------------------------------#
XXD=HEX_LEN="$$(stat -c %s $(1))"; xxd -ps -c 16 $(1)
SED = sed                       \
-re 's/([0-9a-z]{2})/0x\1, /gm' \
-re 's/^/\t/'                   \
-e '$$a};'                      \
-e '1i \\nunsigned int  $(1)_len = '$$HEX_LEN';\nunsigned char $(1)[] =\n{'
DECLARE_DATA_VARS   = extern unsigned char $(1)[];\\nextern unsigned int  $(1)_len;\\n
MAKE_DAT_SOURCE_VAR = $(shell $(call XXD,$(1)) | $(call SED,$(call FILENAME,$(1),$(2))) >> $(3))
MAKE_DAT_HEADER_VAR = $(shell printf "\n$(call DECLARE_DATA_VARS,$(call FILENAME,$(1),$(2)))" >> $(3))

# Font Variables
FONTS_GUARD := FONTS_H
FONTS_DIR   := fonts
FONTS       := $(wildcard $(FONTS_DIR)/*.ttf) $(wildcard $(FONTS_DIR)/*.otf)
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

# Mesh Variables
MESHES_GUARD  := MESHES_H
MESHES_DIR    := meshes
OBJ_MESHES    := $(wildcard $(MESHES_DIR)/*.obj)
GRAPHX_MESHES := $(wildcard $(MESHES_DIR)/*.graphxmesh)
MESHES_HPP    := $(EMBED_DIR)/meshes.hpp
MESHES_CPP    := $(EMBED_DIR)/meshes.cpp

.PHONY: all build clean shaders glsl meshes fonts

all:
	@ $(MAKE) -s shaders meshes fonts
	@ printf "$(DEFAULT)::Embedded Resources Directory - $(GREEN)$(EMBED_DIR:$(TOP)%=src/%)$(RESET)\n"

build:
	@ -mkdir -p $(EMBED_DIR)

clean:
	@ -rm -rf $(OUT_DIR)
	$(call cleaned,$(OUT_DIR))

clean_glsl:
	@ -rm -f $(GL_SHADERS_CPP)
	$(call cleaned,$(GL_SHADERS_CPP))
	@ -rm -f $(GL_SHADERS_HPP)
	$(call cleaned,$(GL_SHADERS_HPP))

clean_fonts:
	@ -rm -f $(FONTS_CPP)
	$(call cleaned,$(FONTS_CPP))
	@ -rm -f $(FONTS_HPP)
	$(call cleaned,$(FONTS_HPP))

# Fonts
fonts: $(FONTS_CPP) $(FONTS_HPP) | build

$(FONTS_CPP): $(FONTS_HPP) | build
	@ $(call PRINT_GENERATING,Fonts,$@,$<)
	@ $(MAKE) -s $(FONTS:%=%.print)
	$(shell printf "#include \"$(notdir $<)\"\n" > $@)
	$(foreach font_file,$(FONTS),$(call MAKE_DAT_SOURCE_VAR,$(font_file),font,$@))

$(FONTS_HPP): $(FONTS) | build
	$(shell printf "#ifndef $(FONTS_GUARD)\n#define $(FONTS_GUARD)\n" > $(FONTS_HPP))
	$(foreach font_file,$(FONTS),$(call MAKE_DAT_HEADER_VAR,$(font_file),font,$@))
	$(shell printf "\n\n#endif // $(FONTS_GUARD)\n" >> $(FONTS_HPP))

# Shaders (all this does is build every shader type; so far, I've only implemented GLSL)
shaders: glsl ;@:

# GLSL
glsl: $(GL_SHADERS_CPP) $(GL_SHADERS_HPP) | build

$(GL_SHADERS_CPP): $(GL_SHADERS_HPP) | build
	@ $(call PRINT_GENERATING,GLSL Shaders,$@,$<)
	@ $(MAKE) -s $(GL_SHADERS:%=%.print)
	$(shell printf "#include \"$(notdir $<)\"\n" > $@)
	$(foreach shader_file,$(GL_SHADERS),$(call MAKE_STR_SOURCE_VAR,$(shader_file),glsl,$@))

$(GL_SHADERS_HPP): $(GL_SHADERS) | build
	$(shell printf "#ifndef $(GL_SHADERS_GUARD)\n#define $(GL_SHADERS_GUARD)\n" > $(GL_SHADERS_HPP))
	$(foreach shader_stem,$(GL_SHADERS),$(call GLSL_HEADER_VAR,$(shader_stem),glsl,$@))
	$(shell printf "\n#endif // $(GL_SHADERS_GUARD)\n" >> $(GL_SHADERS_HPP))

# Meshes
meshes: $(MESHES_CPP) $(MESHES_HPP) | build

$(MESHES_CPP): $(MESHES_HPP) | build
	@ $(call PRINT_GENERATING,Meshes,$@,$<)
	@ $(MAKE) -s $(GRAPHX_MESHES:%=%.print) $(OBJ_MESHES:%=%.print)
	$(shell printf "#include \"$(notdir $<)\"\n\n" > $@)
	$(foreach graphx_mesh,$(GRAPHX_MESHES),$(shell printf "#include \"resource/resources/$(graphx_mesh)\" // IWYU pragma: keep\n" >> $(MESHES_CPP)))
	$(foreach obj_mesh,$(OBJ_MESHES),$(call MAKE_STR_SOURCE_VAR,$(obj_mesh),mesh,$@))

$(MESHES_HPP): $(GRAPHX_MESHES) $(OBJ_MESHES) | build
	$(shell printf "#ifndef $(MESHES_GUARD)\n#define $(MESHES_GUARD)\n\n#include \"rendering/mesh.hpp\"\n\n" > $(MESHES_HPP))
	$(foreach graphx_mesh,$(GRAPHX_MESHES),$(shell printf "extern Mesh mesh_GRAPHX_$(notdir $(graphx_mesh:%.graphxmesh=%));\n" >> $(MESHES_HPP)))
	$(foreach obj_mesh,$(OBJ_MESHES),$(call MAKE_STR_HEADER_VAR,$(obj_mesh),mesh,$@))
	$(shell printf "\n\n#endif // $(MESHES_GUARD)" >> $(MESHES_HPP))


# Printouts
%.print: %
	@ printf "$(RESET)::Embedding: $(YELLOW)src/resources/$<$(RESET)\n"
