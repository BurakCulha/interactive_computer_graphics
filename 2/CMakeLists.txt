cmake_minimum_required(VERSION 3.22)
project(createWindow)


set(project_dir ${CMAKE_SOURCE_DIR})
set(project_source_dir ${project_dir}/src)
set(project_include_dir ${project_dir}/include)


set(project_implementations_dir  
                ${project_dir}/src/implementations)

set(transformations_dir ${project_dir}/../utils/transformations)
set(transformations_sources ${transformations_dir}/transformations.cpp)

set(stbi_include_dir ${project_dir}/../utils/stbi)


set(project_sources 
        ${project_source_dir}/main.cpp
        ${project_implementations_dir}/shaderProgram.cpp
        ${project_implementations_dir}/camera.cpp
        ${project_implementations_dir}/mesh.cpp
        ${transformations_sources}
   )


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
   
   

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})


find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})


find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})



include_directories(${transformations_dir})
include_directories(${project_include_dir})
include_directories(${stbi_include_dir})


add_executable(${PROJECT_NAME} ${project_sources})

target_link_libraries(${PROJECT_NAME} 
                      ${GLUT_LIBRARIES}
                      ${OPENGL_LIBRARIES} 
                      ${GLEW_LIBRARIES} 
                      ${GLM_LIBRARIES}
                      )
