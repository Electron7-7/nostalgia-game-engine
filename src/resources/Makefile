LINUX := linux
WINDOWS := windows
RELEASE := release
DEBUG := debug

ARCHITECTURE ?= $(LINUX)
VERSION ?= $(RELEASE)

EMBEDDED_RESOURCES_OUTPUT_DIR := ../engine/embedded

GL_SHADERS_DIR   := shaders/GLSL
GL_SHADERS       := $(wildcard $(GL_SHADERS_DIR)/*.vert) $(wildcard $(GL_SHADERS_DIR)/*.frag)
GL_SHADERS_CPP   := $(EMBEDDED_RESOURCES_OUTPUT_DIR)/opengl_shaders_test.cpp
GL_SHADERS_HPP   := $(EMBEDDED_RESOURCES_OUTPUT_DIR)/opengl_shaders.hpp
GL_SHADERS_HEADER_GUARD := OPENGL_SHADERS_H

define GL_SHADER_NAME
	glsl_$(subst .,_,$(notdir $(1)))
endef
define MAKE_GL_SHADER_VAR
	$(shell echo -e "extern std::string glsl_$(subst .,_,$(notdir $(1)));\n" >> $(2))
endef

.PHONY: all glsl test

test:
	@ -rm -f $(GL_SHADERS_HPP) $(GL_SHADERS_CPP)

all: glsl

glsl: $(GL_SHADERS_CPP)
	@ echo -e $(GL_SHADERS_NAMES)

$(GL_SHADERS_CPP): $(GL_SHADERS_HPP)
	$(shell echo -e "#include <string>" > $(GL_SHADERS_CPP))
# 	$(foreach shader_file,$(GL_SHADERS),$(shell printf))

$(GL_SHADERS_HPP):
	$(shell echo -e "#ifndef $(GL_SHADERS_HEADER_GUARD)\n#define $(GL_SHADERS_HEADER_GUARD)\n#include <string>" > $(GL_SHADERS_HPP))
# 	$(foreach shader_file,$(GL_SHADERS), $(shell printf "extern std::string $(call GL_SHADER_NAME,$(shader_file));\n" >> $(GL_SHADERS_HPP)))
	$(foreach shader_file,$(GL_SHADERS),$(call MAKE_GL_SHADER_VAR,$(shader_file),$(GL_SHADERS_HPP)))
	$(shell echo -e "#endif // $(GL_SHADERS_HEADER_GUARD)" >> $(GL_SHADERS_HPP))
