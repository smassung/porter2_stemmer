cmake_minimum_required(VERSION 3.0.0)
project(porter2-stemmer)

option(USE_LIBCXX "Use libc++ for the C++ standard library" ON)

include(CheckCXXCompilerFlag)

if(UNIX OR MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

    # if we don't already set the standard for the compiler, detect the
    # best one available and use it
    if(NOT "${CMAKE_CXX_FLAGS}" MATCHES "std=c\\+\\+(0x|11|1y|14)")
        check_cxx_compiler_flag(-std=c++14 HAS_CXX14)
        if(HAS_CXX14)
            message("-- Compiler supports C++14 (using it)")
            set(STDOPT "-std=c++14")
        endif()

        if(NOT STDOPT)
            check_cxx_compiler_flag(-std=c++1y HAS_CXX1Y)
            if(HAS_CXX1Y)
                message("-- Compiler supports C++1y (using it)")
                set(STDOPT "-std=c++1y")
            endif()
        endif()

        if(NOT STDOPT)
            check_cxx_compiler_flag(-std=c++11 HAS_CXX11)
            if(HAS_CXX11)
                message("-- Compiler supports C++11 (using it)")
                set(STDOPT "-std=c++11")
            endif()
        endif()

        if(NOT STDOPT)
            check_cxx_compiler_flag(-std=c++0x HAS_CXX0X)
            if(HAS_CXXOX)
                message("-- Compiler supports C++0x (using it)")
                set(STDOPT "-std=c++0x")
            endif()
        endif()

        if(NOT STDOPT)
            message(FATAL_ERROR
                    "meta requires a compiler with at least C++0x support")
        endif()

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STDOPT}")
    endif()

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        if(CMAKE_GENERATOR STREQUAL "Ninja")
          set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
        endif()
        if(USE_LIBCXX)
            message("-- Locating libc++...")
            find_library(LIBCXX_LIBRARY NAMES c++ cxx)
            if(LIBCXX_LIBRARY)
                message("-- Located libc++: ${LIBCXX_LIBRARY}")
                set(LIBCXX_OPTIONS "-stdlib=libc++")
                get_filename_component(LIBCXX_LIB_PATH ${LIBCXX_LIBRARY}
                                       DIRECTORY)
                find_path(LIBCXX_PREFIX c++/v1/algorithm
                          PATHS ${LIBCXX_LIB_PATH}/../include
                          ${CMAKE_SYSTEM_PREFIX_PATH})
                set(LIBCXX_INCLUDE_DIR ${LIBCXX_PREFIX}/c++/v1/)
                message("-- Located libc++ include path: ${LIBCXX_INCLUDE_DIR}")

                message("--     Locating libc++'s abi...")
                find_library(LIBCXXABI_LIBRARY NAMES c++abi)
                find_library(LIBCXXRT_LIBRARY NAMES cxxrt)
                if(LIBCXXABI_LIBRARY)
                    message("--     Found libc++abi: ${LIBCXXABI_LIBRARY}")
                    set(CXXABI_LIBRARY ${LIBCXXABI_LIBRARY})
                elseif(LIBCXXRT_LIBRARY)
                    message("--     Found libcxxrt: ${LIBCXXRT_LIBRARY}")
                    set(CXXABI_LIBRARY ${LIBCXXRT_LIBRARY})
                else()
                    message("--     No abi library found. "
                            "Attempting to continue without one...")
                endif()
            else()
                message("-- Could not find libc++, will not use it.")
            endif()
        endif()
    endif()

    find_library(LIBDL_LIBRARY NAMES dl ldl)
    if(LIBDL_LIBRARY)
      set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${LIBDL_LIBRARY}")
    endif()

    if(LIBCXX_OPTIONS)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBCXX_OPTIONS}")
    endif()

    if(CXXABI_LIBRARY)
      set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${LIBCXX_OPTIONS} ${CXXABI_LIBRARY} -L${LIBCXX_LIB_PATH}")
    endif()
    if(LIBCXX_INCLUDE_DIR)
      set(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES} ${LIBCXX_INCLUDE_DIR}")
    endif()
endif()



add_library(porter2-stemmer porter2_stemmer.cpp)
target_include_directories(porter2-stemmer PUBLIC ${PROJECT_SOURCE_DIR})
target_compile_options(porter2-stemmer PUBLIC ${STDOPT})

check_cxx_source_compiles("
#include <experimental/string_view>
int main() {
    std::experimental::string_view sv = \"hello world\";
    return 0;
}" META_HAS_EXPERIMENTAL_STRING_VIEW)

if (META_HAS_EXPERIMENTAL_STRING_VIEW)
  target_compile_definitions(porter2-stemmer PUBLIC
                             -DMETA_HAS_EXPERIMENTAL_STRING_VIEW)
endif()

# work around a bug in libstdc++ provided with gcc < 4.8.3 where a static
# assertion fires when you have a non-empty hash functor
check_cxx_source_compiles("
#include <unordered_set>
struct nonempty_hasher : public std::hash<int> { int i = 3; };
int main() {
    std::unordered_set<int, nonempty_hasher> s;
    return 0;
}" META_HAS_NONEMPTY_HASH_SUPPORT)

if (META_HAS_NONEMPTY_HASH_SUPPORT)
  target_compile_definitions(porter2-stemmer PUBLIC
                             -DMETA_HAS_NONEMPTY_HASH_SUPPORT)
endif()

if (HAS_CXX14 OR HAS_CXX1Y)
    target_compile_definitions(porter2-stemmer PUBLIC -DMETA_HAS_CXX14=1)
endif()

if(LIBDL_LIBRARY)
    target_link_libraries(porter2-stemmer PUBLIC ${LIBDL_LIBRARY})
endif()

if(CXXABI_LIBRARY)
    target_link_libraries(porter2-stemmer PUBLIC ${CXXABI_LIBRARY})
endif()

if(LIBCXX_LIBRARY)
    target_include_directories(porter2-stemmer SYSTEM PUBLIC ${LIBCXX_INCLUDE_DIR})
    target_compile_options(porter2-stemmer PUBLIC ${LIBCXX_OPTIONS})
    target_link_libraries(porter2-stemmer PUBLIC -L${LIBCXX_LIB_PATH})
endif()

add_executable(stem main.cpp)
target_link_libraries(stem porter2-stemmer)
