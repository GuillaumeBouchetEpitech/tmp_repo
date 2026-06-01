cmake_minimum_required(VERSION 3.24)

project(custom-container-unit-tests)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES
    ./basic_double_linked_list/add_link_to_list.tests.cpp
    ./basic_double_linked_list/list_is_empty_initially.tests.cpp
    ./basic_double_linked_list/loop_list_links_and_reset.tests.cpp
    ./basic_double_linked_list/loop_list_links.tests.cpp
    ./basic_double_linked_list/remove_link_from_list.tests.cpp
    ./basic_double_linked_list/reset_link.tests.cpp
    ./basic_double_linked_list/reset_list.tests.cpp
    ./basic_double_linked_list/swap_two_links_from_same_list.tests.cpp

    ./utils/generic_array_container_commons/common.tests.cpp

    ./static_array_tests/allocations.cpp
    ./static_array_tests/data_access.cpp
    ./static_array_tests/iterators.cpp
    ./static_array_tests/sort.cpp

    ./dynamic_heap_array/allocations.cpp
    ./dynamic_heap_array/emplace_back.cpp
    ./dynamic_heap_array/push_back__by_rvalue.cpp
    ./dynamic_heap_array/push_back__by_ref.cpp

    ./weak_ref_data_pool/acquire_weak_ref.cpp
    ./weak_ref_data_pool/filter.cpp
    ./weak_ref_data_pool/for_each.cpp
    ./weak_ref_data_pool/miscellaneous.cpp
    ./weak_ref_data_pool/multiple_weak_ref.cpp
    ./weak_ref_data_pool/release_weak_ref.cpp
    ./weak_ref_data_pool/remove_unreferenced_items.cpp

    ./weak_ref_data_pool/usecase1.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ../src
)

# set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-O3")
# set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-O3")
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-g3")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-g3")


find_package(PkgConfig REQUIRED)

pkg_check_modules(UNIT_TESTS REQUIRED IMPORTED_TARGET GLOBAL gtest_main)
# pkg_check_modules(YOUR_PKG REQUIRED IMPORTED_TARGET GLOBAL ya-package)

target_link_libraries(${PROJECT_NAME} PUBLIC
    PkgConfig::UNIT_TESTS
    # PkgConfig::YOUR_PKG
)

# if (DEFINED EMSCRIPTEN)

#     set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS " -O3 -s WASM=1 -s BINARYEN_IGNORE_IMPLICIT_TRAPS=1 ")

#     set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${GERONIMO_DIR}/lib/wasm")

# else()

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/_bin")

# endif()

