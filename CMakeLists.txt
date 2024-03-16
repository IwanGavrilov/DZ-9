cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

project(bulk VERSION ${PROJECT_VESRION})

configure_file(version.h.in version.h)

add_executable(bulk_cli cmd_processing.cpp)
add_library(bulk lib.cpp)
add_library(loger loger.cpp)
add_library(observer observer.cpp)

set_target_properties(bulk_cli loger PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(bulk PRIVATE "${CMAKE_BINARY_DIR}")

target_link_libraries(bulk_cli PRIVATE bulk)
target_link_libraries(bulk_cli PRIVATE loger)
target_link_libraries(bulk_cli PRIVATE observer)

if (MSVC)
    target_compile_options(bulk_cli PRIVATE
        /W4
    )
    target_compile_options(bulk PRIVATE
        /W4
    )
else ()
    target_compile_options(bulk_cli PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    # target_compile_options(bulk PRIVATE
    #   -Wall -Wextra -pedantic -Werror
    #)
endif()

install(TARGETS bulk_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
