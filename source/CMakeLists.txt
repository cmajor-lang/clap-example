
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

set(CMAJ_CMAKE_PROJECT_NAME "Pro54")
set(CMAJ_TARGET_BUNDLE_ID "dev.cmajor.examples.pro54")
set(CMAJ_TARGET_PATCH_VERSION "1.0")
set(CMAJ_TARGET_NAME "Pro54")
set(CMAJ_TARGET_BUNDLE_NAME "${CMAJ_TARGET_NAME}")
set(CMAJ_TARGET_BUNDLE_VERSION "${CMAJ_TARGET_PATCH_VERSION}")
set(CMAJ_TARGET_SHORT_VERSION_STRING "${CMAJ_TARGET_PATCH_VERSION}")
set(CMAJ_TARGET_MANUFACTURER "Cmajor")
set(CMAJ_TARGET_MANUFACTURER_CODE "Cmaj")
set(CMAJ_TARGET_PLUGIN_CODE "Pr54")
set(CLAP_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/clap/include")
set(CLAP_WRAPPER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/clap-wrapper")

if(NOT CMAJ_INCLUDE_PATH)
    set(CMAJ_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")
endif()

if(NOT CMAJ_PLUGIN_HELPERS_PATH)
    set(CMAJ_PLUGIN_HELPERS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/helpers")
endif()

project("${CMAJ_CMAKE_PROJECT_NAME}" VERSION "${CMAJ_TARGET_PATCH_VERSION}" LANGUAGES CXX C)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
endif()

if(NOT CMAJ_PLUGIN_HELPERS_PATH)
    message (FATAL_ERROR "You must define the CMAJ_PLUGIN_HELPERS_PATH variable to point to your local plugin helpers folder")
endif()

add_subdirectory("${CMAJ_PLUGIN_HELPERS_PATH}/common" cmaj_plugin_helpers)
add_subdirectory("${CMAJ_PLUGIN_HELPERS_PATH}/clap" cmaj_clap_helpers)

if (CLAP_WRAPPER_PATH)
    set(CLAP_WRAPPER_DOWNLOAD_DEPENDENCIES TRUE)
    set(CLAP_WRAPPER_DONT_ADD_TARGETS TRUE)
    set(CLAP_WRAPPER_BUILD_AUV2 TRUE)
    add_subdirectory(${CLAP_WRAPPER_PATH} clap_wrapper)
endif()

add_library(${CMAJ_TARGET_NAME} MODULE entry.cpp)
target_link_libraries(${CMAJ_TARGET_NAME} PRIVATE cmaj_clap)
target_compile_options(${CMAJ_TARGET_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>)
target_compile_definitions(${CMAJ_TARGET_NAME} PRIVATE $<$<CONFIG:Debug>:CMAJ_ENABLE_WEBVIEW_DEV_TOOLS=1> CMAJOR_DLL=1)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_options(${CMAJ_TARGET_NAME} PRIVATE -exported_symbols_list "${CMAJ_PLUGIN_HELPERS_PATH}/clap/macos-symbols.txt")

    set_target_properties(${CMAJ_TARGET_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        BUNDLE True
        BUNDLE_EXTENSION "clap"
        MACOSX_BUNDLE_GUI_IDENTIFIER "${CMAJ_TARGET_BUNDLE_ID}"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "${CMAJ_TARGET_BUNDLE_ID}"
        MACOSX_BUNDLE_BUNDLE_NAME ${CMAJ_TARGET_BUNDLE_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION ${CMAJ_TARGET_BUNDLE_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${CMAJ_TARGET_SHORT_VERSION_STRING}
    )
else()
    set_target_properties(${CMAJ_TARGET_NAME} PROPERTIES
        SUFFIX ".clap"
        PREFIX ""
    )
endif()

if (CLAP_WRAPPER_PATH)
    if (APPLE)
        set(VST3_TARGET ${CMAJ_TARGET_NAME}_vst3)
        add_library(${VST3_TARGET} MODULE)
        target_link_libraries(${VST3_TARGET} PRIVATE cmaj_clap)
        target_sources(${VST3_TARGET} PRIVATE entry.cpp)
        target_add_vst3_wrapper(TARGET ${VST3_TARGET} OUTPUT_NAME ${CMAJ_CMAKE_PROJECT_NAME})
        target_compile_definitions(${VST3_TARGET} PRIVATE CMAJOR_DLL=1)

        set(STANDALONE_TARGET ${CMAJ_TARGET_NAME}_standalone)
        add_executable(${STANDALONE_TARGET})
        target_sources(${STANDALONE_TARGET} PRIVATE entry.cpp)
        target_link_libraries(${STANDALONE_TARGET} PRIVATE cmaj_clap)
        target_compile_definitions(${STANDALONE_TARGET} PRIVATE CMAJOR_DLL=1)
        target_add_standalone_wrapper(
            TARGET "${STANDALONE_TARGET}"
            OUTPUT_NAME "${CMAJ_TARGET_NAME}"
            STATICALLY_LINKED_CLAP_ENTRY True
            PLUGIN_ID "${CMAJ_TARGET_BUNDLE_ID}"
            BUNDLE_IDENTIFIER "${CMAJ_TARGET_NAME}_standalone"
            BUNDLE_VERSION "${CMAJ_TARGET_PATCH_VERSION}"
        )

        set(AUV2_TARGET ${CMAJ_TARGET_NAME}_auv2)
        add_library(${AUV2_TARGET} MODULE)
        target_link_libraries(${AUV2_TARGET} PRIVATE cmaj_clap)
        target_sources(${AUV2_TARGET} PRIVATE entry.cpp)
        target_add_auv2_wrapper(
            TARGET "${AUV2_TARGET}"
            OUTPUT_NAME "${CMAJ_CMAKE_PROJECT_NAME}"
            BUNDLE_IDENTIFIER "${CMAJ_TARGET_BUNDLE_ID}"
            BUNDLE_VERSION "${CMAJ_TARGET_PATCH_VERSION}"
            MANUFACTURER_NAME "${CMAJ_TARGET_MANUFACTURER}"
            MANUFACTURER_CODE "${CMAJ_TARGET_MANUFACTURER_CODE}"
            SUBTYPE_CODE "${CMAJ_TARGET_PLUGIN_CODE}"
            INSTRUMENT_TYPE "aumu"
        )
        target_compile_definitions(${AUV2_TARGET} PRIVATE CMAJOR_DLL=1)
    endif()
endif()
