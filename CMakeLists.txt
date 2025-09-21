cmake_minimum_required(VERSION 3.28)
project(Dyno LANGUAGES CXX)


# Find Julia bindir and datarootdir to construct path to julia-config.jl
execute_process(
        COMMAND julia -e "print(joinpath(Sys.BINDIR, Base.DATAROOTDIR, \"julia\"))"
        OUTPUT_VARIABLE JL_SHARE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Adds CxxWrap to Pkg if not already present and gets the prefix path
execute_process(
        COMMAND julia -e "using Pkg;Pkg.add(\"CxxWrap\");using CxxWrap;println(CxxWrap.prefix_path())"
        OUTPUT_VARIABLE CXXWRAP_PREFIX_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Necessary for CxxWrap to compile
list(APPEND CMAKE_PREFIX_PATH "${CXXWRAP_PREFIX_PATH}")

# Get and split flags
macro(get_julia_flag FLAG_VAR FLAG_CMD)
    execute_process(
            COMMAND ${JL_SHARE}/julia-config.jl ${FLAG_CMD}
            OUTPUT_VARIABLE ${FLAG_VAR}_STR
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    separate_arguments(${FLAG_VAR}_STR NATIVE_COMMAND ${FLAG_VAR})
endmacro()

# Get Julia install base dir (like /opt/homebrew/Cellar/julia/1.11.6)
execute_process(
        COMMAND julia -e "print(Sys.BINDIR)"
        OUTPUT_VARIABLE JULIA_BINDIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Derive root install dir (usually 1 level up from bin)
get_filename_component(JULIA_DIR ${JULIA_BINDIR} DIRECTORY)

# Set lib and include paths
set(JULIA_LIB_DIR "${JULIA_DIR}/lib")
set(JULIA_INCLUDE_DIR "${JULIA_DIR}/include/julia")

get_julia_flag(JULIA_CFLAGS --cflags)
get_julia_flag(JULIA_LDFLAGS --ldflags)
get_julia_flag(JULIA_LDLIBS --ldlibs)

# Find JlCxx (CxxWrap C++ component)
find_package(JlCxx REQUIRED)

# Define imported Julia library
add_library(Julia::julia SHARED IMPORTED)
if(APPLE)
    set_target_properties(
            Julia::julia
            PROPERTIES
            IMPORTED_LOCATION "${JULIA_LIB_DIR}/libjulia.dylib"
            INTERFACE_INCLUDE_DIRECTORIES "${JULIA_INCLUDE_DIR}"
            INTERFACE_LINK_DIRECTORIES "${JULIA_LIB_DIR}"
            INSTALL_RPATH "${JULIA_LIB_DIR}"
            BUILD_RPATH "${JULIA_LIB_DIR}"
    )
elseif (UNIX)
    set_target_properties(
            Julia::julia
            PROPERTIES
            IMPORTED_LOCATION "${JULIA_LIB_DIR}/libjulia.so"
            INTERFACE_INCLUDE_DIRECTORIES "${JULIA_INCLUDE_DIR}"
            INTERFACE_LINK_DIRECTORIES "${JULIA_LIB_DIR}"
            INSTALL_RPATH "${JULIA_LIB_DIR}"
            BUILD_RPATH "${JULIA_LIB_DIR}"
    )
endif()

#####################
# Compiler settings #
#####################

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#################################
# Set constants for the project #
#################################

file(GLOB_RECURSE SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE MODULES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cppm
)

######################
# Target: executable #
######################

add_executable(${PROJECT_NAME}
	${SOURCES}
)

if (MODULES)
    target_sources(${PROJECT_NAME}
            PUBLIC
            FILE_SET allModules
            TYPE CXX_MODULES
            FILES ${MODULES}
    )
endif()

target_link_libraries(${PROJECT_NAME}
        PUBLIC
            JlCxx::cxxwrap_julia
            JlCxx::cxxwrap_julia_stl
            Julia::julia
)

# Since <ranges> isn't fully supported on Apple, we disable it
if (APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE JLCXX_FORCE_RANGES_OFF=1)
endif()

# Use Julia's flags
target_compile_options(${PROJECT_NAME} PRIVATE ${JULIA_CFLAGS})
target_link_options(${PROJECT_NAME} PRIVATE ${JULIA_LDFLAGS} ${JULIA_LDLIBS})

# Enable C++23
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

##################
# Copy resources #
##################

if (1)
foreach(dir scripts)
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/${dir}
            COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_CURRENT_SOURCE_DIR}/${dir}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/${dir}
            COMMENT "Symlinking ./${dir} -> build/Debug/${dir}"
    )
endforeach()
else()
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/scripts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
