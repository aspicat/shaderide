# ShaderIDE - CMakeLists.txt
#
# On Windows, don't forget to tell CMAKE these directories (examples):
# -DCMAKE_PREFIX_PATH="C:/Qt/6.0.0/mingw81_64"
# -DGLM_INCLUDE_DIRS="[...]/glm-0.9.9.5"
# -DGLM_LIBRARY_DIRS="[...]/glm-0.9.9.5/lib"
# -DBOOST_ROOT="[...]/boost_1_74_0"
# -DBOOST_LIBS="[...]/boost_1_74_0/stage/lib"
#
# Also, include the Qt shared libraries (WIN32):
# Qt6Core.dll
# Qt6Gui.dll
# Qt6OpenGL.dll
# Qt6Widgets.dll

cmake_minimum_required(VERSION 3.14)
project(ShaderIDE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

if(WIN32)
    set(CMAKE_CXX_FLAGS "-mwindows")
endif()

# Experimental Features
#add_compile_definitions(SHADERIDE_SPIRV_EXPORT)

# Include Project Headers & Sources (exclude main.cpp)
include_directories("include")
file(GLOB_RECURSE INCLUDE_FILES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")
list(REMOVE_ITEM SOURCE_FILES src/main.cpp)

# Qt6
find_package(Qt6Widgets REQUIRED)
find_package(Qt6OpenGL REQUIRED)
find_package(Qt6OpenGLWidgets REQUIRED)

# GLM
include_directories(${GLM_INCLUDE_DIRS})
link_directories(${GLM_LIBRARY_DIRS})

# Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

IF(WIN32)
    link_directories(${BOOST_LIBS})
ENDIF()

# Link Libraries
IF(WIN32)
    set(LINK_LIBRARIES Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets ${Boost_LIBRARIES}
            "-lopengl32 -lglm_static")
ELSE()
    set(LINK_LIBRARIES Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets ${Boost_LIBRARIES}
            "-lGL")
ENDIF()

IF(WIN32)
    add_executable(${PROJECT_NAME} ${INCLUDE_FILES} ${SOURCE_FILES}
            resources.qrc src/main.cpp assets/app/shaderide.rc)
ELSE()
    add_executable(${PROJECT_NAME} ${INCLUDE_FILES} ${SOURCE_FILES}
            resources.qrc src/main.cpp)
ENDIF()

target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})

# Unit Tests
enable_testing()

set(PROJECT_TEST "ProjectTest")
add_executable(${PROJECT_TEST} ${INCLUDE_FILES} ${SOURCE_FILES} test/ProjectTest.cpp)
target_link_libraries(${PROJECT_TEST} ${LINK_LIBRARIES} "-lboost_unit_test_framework")
add_test(NAME ${PROJECT_TEST} COMMAND ${PROJECT_TEST})
