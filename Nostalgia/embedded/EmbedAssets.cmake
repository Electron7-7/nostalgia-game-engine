if(NOT SOURCE_DIR)
    set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# https://stackoverflow.com/a/27206982
function(create_resources dir output header_guard namespace)
    # Create output file
    file(WRITE ${output} "#ifndef ${header_guard}\n#define ${header_guard}\n\n#pragma clang diagostic push\n#pragma clang diagnostic ignored \"-Wc23-extensions\"\n\nnamespace ${namespace}\n{")
    # Collect input files
    file(GLOB bins ${dir}/*)
    # Iterate through input files
    foreach(bin ${bins})
        # Get short filename
        string(REGEX MATCH "([^/]+)$" file_name ${bin})
        string(REGEX REPLACE "\\..+" "" filename ${file_name})
        # Replace filename spaces & extension separator for C compatibility
        string(REGEX REPLACE "\\.| |-" "_" filename ${filename})
        if(WIN32)
            # Read hex data from file
            file(READ ${bin} filedata HEX)
            # Convert hex data for C compatibility
            string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
            # Append data to output file
            file(APPEND ${output} "\n\tconstinit const uchar ${filename}[]{${filedata}};\n")
        else()
            file(APPEND ${output} "\n\tconstinit const uchar ${filename}[]{\n\t\t#embed \"${dir}/${file_name}\"\n\t};\n")
        endif()
    endforeach()
    file(APPEND ${output} "}\n\n#endif // ${header_guard}\n")
endfunction()

function(create_shaders dir output)
    # Create empty output file
    file(WRITE ${output} "#ifndef SHADERS_H\n#define SHADERS_H\n\n#pragma clang diagostic push\n#pragma clang diagnostic ignored \"-Wc23-extensions\"\n")
    # Get short directory name
    string(REGEX MATCH "([^/]+)$" dir_name ${dir})
    # Collect input files
    file(GLOB bins ${dir}/*)
    # Iterate through input files
    foreach(bin ${bins})
        # Get file extension
        string(REGEX MATCH "([^\\.]+)$" file_ext_raw ${bin})
        string(SUBSTRING ${file_ext_raw} 0 1 file_ext_begin)
        string(TOUPPER ${file_ext_begin} file_ext_begin)
        string(SUBSTRING ${file_ext_raw} 1 -1 file_ext_end)
        string(CONCAT file_ext _${file_ext_begin}${file_ext_end})
        # Get short filename
        string(REGEX MATCH "([^/]+)$" file_name ${bin})
        string(REGEX REPLACE "\\..+" "" filename ${file_name})
        # Replace filename spaces & extension separator for C compatibility
        string(REGEX REPLACE "\\.| |-" "_" filename ${filename})
        string(CONCAT filename ${dir_name}_${filename}${file_ext})
        if(WIN32)
            file(READ ${bin} filedata)
            # Append data to output file
            file(APPEND ${output} "\nconstinit const char ${filename}[]{R\"(${filedata})\"};\n")
        else()
            file(APPEND ${output} "\nconstinit const char ${filename}[]{\n\t#embed \"${dir}/${file_name}\" \\\n\t\tsuffix(,)\n\t\t0\n};\n")
        endif()
    endforeach()
    file(APPEND ${output} "\n\n#endif // SHADERS_H\n")
endfunction()

create_resources(${SOURCE_DIR}/fonts fonts.hpp FONTS_H Fonts)
create_resources(${SOURCE_DIR}/images images.hpp IMAGES_H Images)
create_resources(${SOURCE_DIR}/models models.hpp MODELS_H Models)
create_shaders(${SOURCE_DIR}/shaders/GLSL shaders.hpp)
