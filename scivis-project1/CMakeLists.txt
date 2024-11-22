cmake_minimum_required(VERSION 3.5)

project(SciVis)

set(SRC learnply)
set(SOURCES 
    ${SRC}/learnply.cpp
    ${SRC}/ply.cpp
    ${SRC}/polyhedron.cpp
    ${SRC}/ibfv.cpp
    ${SRC}/tmatrix.cpp
    ${SRC}/trackball.cpp
    ${SRC}/project1.cpp
    ${SRC}/project2.cpp
)

add_executable(learnply ${SOURCES})

if(WIN32)
	set(FREEGLUT ${CMAKE_CURRENT_SOURCE_DIR}/libraries/freeglut-3.0.0)
	set(GLEW ${CMAKE_CURRENT_SOURCE_DIR}/libraries/glew-2.1.0)

	target_include_directories(learnply PRIVATE
		${FREEGLUT}/include
		${GLEW}/include
	)

	target_link_directories(learnply PRIVATE
		${FREEGLUT}/lib
		${GLEW}/lib
	)

	target_link_libraries(learnply PRIVATE freeglut glew32)

    # Copy DLLs to build directory
    set(TO_COPY
        ${FREEGLUT}/bin/freeglut.dll
        ${GLEW}/bin/glew32.dll
    )

    add_custom_command(
        TARGET learnply POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${TO_COPY}
        ${CMAKE_CURRENT_BINARY_DIR}
    )
else()
	find_package(OpenGL REQUIRED)
	find_package(GLUT REQUIRED)
	find_package(GLEW REQUIRED)

	target_include_directories(learnply PRIVATE
		${OPENGL_INCLUDE_DIRS}
        ${GLUT_INCLUDE_DIRS}
		${GLEW_INCLUDE_DIRS}
	)
    
    target_link_libraries(learnply PRIVATE
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES}
        GLEW::GLEW
    )

	# Silence OpenGL deprecation warnings on Mac
    if(APPLE)
        add_definitions(-DGL_SILENCE_DEPRECATION)
    endif()
endif()

