cmake_minimum_required(VERSION 3.22)
project(createWindow)


set(project_dir ${CMAKE_SOURCE_DIR})
set(project_source_dir ${project_dir}/src)


set(project_sources 
        ${project_source_dir}/main.cpp
   )
    

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})


find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})




add_executable(${PROJECT_NAME} ${project_sources})

target_link_libraries(${PROJECT_NAME} 
                      ${GLUT_LIBRARIES}
                      ${OPENGL_LIBRARIES} 
                      )
