
cmake_minimum_required(VERSION 3.24)
project(exec)

# Our Project
set(SOURCE_FILES
  ./src/main.cpp
  ./src/clamp.cpp
  ./src/Clock.cpp
  ./src/RandomNumberGenerator.cpp
  ./src/NeuralNetwork.cpp
  ./src/ExtenderRecursiveLeastSquareTrainer.cpp
  ./src/ExtenderRecursiveLeastSquareMatrix.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-O3")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-O3")
# set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-g3")
# set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-g3")

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
