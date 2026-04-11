cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 20)
project(real-car-adas-monitor VERSION 0.1.0 LANGUAGES C CXX)

# 1. Библиотеки
# OpenCV
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    message(STATUS "OpenCV found: ${OpenCV_VERSION}")
    message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "OpenCV not found!")
endif()

# ONNX Runtime
set(ONNXRUNTIME_ROOT "/opt/onnxruntime" CACHE PATH "Path to ONNX Runtime")

#  Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0  # Стабильная версия
    GIT_SHALLOW    TRUE     # Ускорение клонирования
)
FetchContent_MakeAvailable(googletest)

# 2. Пути к заголовкам и библиотекам
set(ONNXRUNTIME_INCLUDE_DIRS "${ONNXRUNTIME_ROOT}/include")
set(ONNXRUNTIME_LIBRARIES "${ONNXRUNTIME_ROOT}/lib/libonnxruntime.so")

# 3. Проверка существования файлов
if(NOT EXISTS "${ONNXRUNTIME_INCLUDE_DIRS}")
    message(FATAL_ERROR "ONNX Runtime include directory not found: ${ONNXRUNTIME_INCLUDE_DIRS}")
endif()
if(NOT EXISTS "${ONNXRUNTIME_LIBRARIES}")
    message(FATAL_ERROR "ONNX Runtime library not found: ${ONNXRUNTIME_LIBRARIES}")
endif()

message(STATUS "ONNX Runtime include: ${ONNXRUNTIME_INCLUDE_DIRS}")
message(STATUS "ONNX Runtime library: ${ONNXRUNTIME_LIBRARIES}")

# 4. Библиотека с общим исходным кодом
file(GLOB SOURCES "src/*.cpp")
add_library(lib STATIC ${SOURCES})
target_include_directories(lib PUBLIC 
    ${CMAKE_SOURCE_DIR}/src
    ${OpenCV_INCLUDE_DIRS}
    ${ONNXRUNTIME_INCLUDE_DIRS}
)

target_link_libraries(lib PRIVATE
    ${OpenCV_LIBS}
    ${ONNXRUNTIME_LIBRARIES}
)

# 5. Сборка исполняемого файла
set (EXECUTABLE_NAME RealCarMonitor)
add_executable(${EXECUTABLE_NAME} src/main.cpp)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE lib)

# Поиск .so файлы при запуске
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    INSTALL_RPATH "${ONNXRUNTIME_ROOT}/lib"
    BUILD_WITH_INSTALL_RPATH FALSE
)

# 6. Сборка тестов
set (RUN_TESTS_NAME RunTests)
file(GLOB TEST_SOURCES "tests/*.cpp")
add_executable(${RUN_TESTS_NAME} ${TEST_SOURCES})

target_link_libraries(${RUN_TESTS_NAME} PRIVATE
    GTest::gtest_main
    lib
)