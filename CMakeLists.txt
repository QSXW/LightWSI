cmake_minimum_required(VERSION 3.16)

project(LightWSI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LAYER_SRCS
    DispatcheTable.h
    Instance.cpp
    Instance.h
    Layer.cpp
    Surface.cpp
    Surface.h
    Swapchain.cpp
    Swapchain.h)

list(TRANSFORM LAYER_SRCS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Layer/")

set(DIRECT2D_SRCS
    Surface.cpp
    Surface.h
    Swapchain.cpp
    Swapchain.h)

list(TRANSFORM DIRECT2D_SRCS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/WSI/Direct2D/")

set(SRC_FILES
    LightWSI.h)

source_group("//"                FILES ${SRC_FILES}    )
source_group("//Layer//"         FILES ${LAYER_SRCS}   )
source_group("//WSI//Direct2D//" FILES ${DIRECT2D_SRCS})

add_library(${PROJECT_NAME} SHARED
    ${SRC_FILES}
    ${LAYER_SRCS}
    ${DIRECT2D_SRCS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    vulkan)

set(LAYER_JSON_MANIFEST "${CMAKE_CURRENT_SOURCE_DIR}/Layer/VkLayer_LIGHTWSI_window_system_integration.json")
file(COPY ${LAYER_JSON_MANIFEST} DESTINATION ${CMAKE_BINARY_DIR})
message(STATUS "LightWSI install ${LAYER_JSON_MANIFEST} to ${CMAKE_BINARY_DIR}")

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}
)
