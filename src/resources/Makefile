EMBEDDED_RESOURCES_OUTPUT_DIR := ../embedded

GL_SHADERS_DIR   := shaders/GLSL
GL_SHADERS       := $(wildcard $(GL_SHADERS_DIR)/*.vert) $(wildcard $(GL_SHADERS_DIR)/*.frag)
GL_SHADERS_CPP   := $(EMBEDDED_RESOURCES_OUTPUT_DIR)/opengl_shaders.cpp
GL_SHADERS_HPP   := $(EMBEDDED_RESOURCES_OUTPUT_DIR)/opengl_shaders.hpp
GL_SHADERS_HEADER_GUARD := OPENGL_SHADERS_H

define MAKE_GL_SHADER_HEADER_VAR
	$(shell echo -e "extern std::string glsl_$(subst .,_,$(notdir $(1)));" >> $(2))
endef
define MAKE_GL_SHADER_SOURCE_VAR
	$(shell printf "std::string glsl_$(subst .,_,$(notdir $(1))) = R\"~(\n" >> $(2) && cat $(1) >> $(2) && printf ")~\";\n" >> $(2))
endef

MESHES_DIR        := meshes
GRAPHX_MESHES     := $(wildcard $(MESHES_DIR)/*.graphxmesh)
GRAPHX_MESHES_HPP := $(EMBEDDED_RESOURCES_OUTPUT_DIR)/graphx_meshes.hpp
GRAPHX_MESHES_HEADER_GUARD := GRAPHX_MESHES_H

.PHONY: all clean glsl

# FIXME: Don't clean build every time
all: clean glsl graphxmodels

# FIXME: when implementing different APIs and, therefore, shaders, have a better 'clean' target
clean:
	@ -rm -f $(GL_SHADERS_HPP) $(GL_SHADERS_CPP)

glsl: $(GL_SHADERS_CPP)
	@ echo -e "Embedded: $(GL_SHADERS)\n"

$(GL_SHADERS_CPP): $(GL_SHADERS_HPP)
	$(shell printf "#include <string>\n" > $(GL_SHADERS_CPP))
	$(foreach shader_file,$(GL_SHADERS),$(call MAKE_GL_SHADER_SOURCE_VAR,$(shader_file),$(GL_SHADERS_CPP)))

$(GL_SHADERS_HPP):
	$(shell echo -e "#ifndef $(GL_SHADERS_HEADER_GUARD)\n#define $(GL_SHADERS_HEADER_GUARD)\n#include <string>" > $(GL_SHADERS_HPP))
	$(foreach shader_file,$(GL_SHADERS),$(call MAKE_GL_SHADER_HEADER_VAR,$(shader_file),$(GL_SHADERS_HPP)))
	$(shell echo -e "#endif // $(GL_SHADERS_HEADER_GUARD)" >> $(GL_SHADERS_HPP))

graphxmodels: $(GRAPHX_MESHES_HPP)
	@ echo -e "Embedded: $(GRAPHX_MESHES_HPP)\n"

$(GRAPHX_MESHES_HPP):
	$(shell echo -e "#ifndef $(GRAPHX_MESHES_HEADER_GUARD)\n#define $(GRAPHX_MESHES_HEADER_GUARD)\n" > $(GRAPHX_MESHES_HPP))
	$(foreach graphx_mesh,$(GRAPHX_MESHES),$(shell echo -e "#include $(graphx_mesh)" >> $(GRAPHX_MESHES_HPP)))
	$(shell echo -e "#endif // $(GL_SHADERS_HEADER_GUARD)" >> $(GL_SHADERS_HPP))
