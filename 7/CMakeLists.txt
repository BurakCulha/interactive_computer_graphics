cmake_minimum_required(VERSION 3.22)
project(tessellationAndDisplacementMapping)


set(project_dir ${CMAKE_SOURCE_DIR})
set(project_source_dir ${project_dir}/src)
set(project_include_dir ${project_dir}/include)


set(project_implementations_dir  
                        ${project_dir}/src/implementations)

set(transformations_dir ${project_dir}/../utils/transformations)
set(transformations_sources ${transformations_dir}/transformations.cpp)

set(stbi_include_dir ${project_dir}/../utils/stbi)

set(imgui_dir ${project_dir}/../utils/imguiLib)
file(GLOB imgui_sources ${imgui_dir}/*.cpp)
# add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW)

set(glad_dir ${project_dir}/../utils/glad)
set(glad_sources ${glad_dir}/glad.c)




set(project_sources 
        ${project_source_dir}/main.cpp
        ${project_implementations_dir}/shaderProgram.cpp
        ${project_implementations_dir}/camera.cpp
        ${project_implementations_dir}/mesh.cpp
        ${project_implementations_dir}/framebuffer.cpp
        ${transformations_sources}
        ${imgui_sources}
        ${glad_sources}
)


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
   
   

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})


find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})


# find_package(GLEW REQUIRED)
# include_directories(${GLEW_INCLUDE_DIRS})

find_package(glm REQUIRED)
# include_directories(${GLM_INCLUDE_DIRS})

find_package(glfw3 3.4 REQUIRED)
# include_directories(${glfw3_INCLUDE_DIRS})

include_directories(${transformations_dir})
include_directories(${project_include_dir})
include_directories(${stbi_include_dir})
include_directories(${imgui_dir})
include_directories(${glad_dir})


add_executable(${PROJECT_NAME} ${project_sources})

target_link_libraries(
                      ${PROJECT_NAME} 
                      ${GLUT_LIBRARIES}
                      ${OPENGL_LIBRARIES} 
                  #     ${GLEW_LIBRARIES} # stopped working suddenly. 
                #       ${GLM_LIBRARIES} # not needed. i did wrong so far.
                      glfw
)

# # message(STATUS "GLFW3_LIBRARIES: ${GLFW3_LIBRARIES}")
# message(STATUS "GLFW3_INCLUDE_DIRS: ${GLFW3_INCLUDE_DIRS}")
# message(STATUS "opengl libraries: ${OPENGL_LIBRARIES}")
# message(STATUS "glut libraries: ${GLUT_LIBRARIES}")
# message(STATUS "GLUT_INCLUDE_DIRS: ${GLUT_INCLUDE_DIRS}")
# # message(STATUS "GLFW_LIBRARIES: ${GLFW_LIBRARIES}")
# # message(STATUS "GLFW kucuk harflerle:  ${glfw}")
# message(STATUS "glm libraries: ${GLM_LIBRARIES}")
# message(STATUS "GLM_INCLUDE_DIRS: ${GLM_INCLUDE_DIRS}")
# message(STATUS "glew libraries: ${GLEW_LIBRARIES}")
# message(STATUS "GLEW_INCLUDE_DIRS: ${GLEW_INCLUDE_DIRS}")
