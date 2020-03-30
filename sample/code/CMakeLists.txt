#######################################################################
macro(CREATE_SHARED NAME SOURCES LIBRARIES)
	set(srcs)
	set(flsrcs)
	foreach(src ${SOURCES})
		if("${src}" MATCHES ".fl$")
         list(APPEND flsrcs ${src})
		else()
			list(APPEND srcs ${src})
		endif("${src}" MATCHES ".fl$")
	endforeach(src)

        add_library(${NAME} SHARED ${srcs})
	include_directories(${PROJ_INCLUDES} ${Boost_INCLUDE_DIR} ${PROJ_OPENCV_INCLUDES})
	target_link_libraries(${NAME} ${LIBRARIES} ${PROJ_LIBRARIES} ${Boost_LIBRARIES} ${OpenCV_LIBRARIES})

	# ------------------------
	# DEFINITIONS
	# ------------------------

	add_definitions(-DUNICODE -D_UNICODE)

    install(TARGETS ${NAME}
      DESTINATION ${PREFIX_DOC}/examples
    )

    IF(MSVC)
	ADD_DEFINITIONS(/arch:SSE2)
	ADD_DEFINITIONS(-D__SSE2__)
    ENDIF(MSVC)

endmacro(CREATE_SHARED NAME SOURCES LIBRARIES)


#######################################################################
macro(CREATE_EXAMPLE NAME SOURCES LIBRARIES)
	set(srcs)
	set(flsrcs)
	foreach(src ${SOURCES})
		if("${src}" MATCHES ".fl$")
         list(APPEND flsrcs ${src})
		else()
			list(APPEND srcs ${src})
		endif("${src}" MATCHES ".fl$")
	endforeach(src)

	add_executable(${NAME} ${srcs})
	include_directories(${PROJ_INCLUDES} ${Boost_INCLUDE_DIR} ${PROJ_OPENCV_INCLUDES})
	target_link_libraries(${NAME} ${LIBRARIES} ${PROJ_LIBRARIES} ${Boost_LIBRARIES} ${OpenCV_LIBRARIES})

	# ------------------------
	# DEFINITIONS
	# ------------------------

	add_definitions(-DUNICODE -D_UNICODE)

    install(TARGETS ${NAME}
      DESTINATION ${PREFIX_DOC}/examples
    )

    IF(MSVC)
	ADD_DEFINITIONS(/arch:SSE2)
	ADD_DEFINITIONS(-D__SSE2__)
    ENDIF(MSVC)

endmacro(CREATE_EXAMPLE NAME SOURCES LIBRARIES)


#######################################################################
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

#######################################################################
CREATE_EXAMPLE(string_common_StringOp "string_common_StringOp.cpp" "")
CREATE_EXAMPLE(string_common_StringTypeConversion "string_common_StringTypeConversion.cpp" "")
CREATE_EXAMPLE(sanity_sanity "sanity_sanity.cpp" "")
CREATE_EXAMPLE(random_StringGenerator "random_StringGenerator.cpp" "")
CREATE_EXAMPLE(enum_enum_helper "enum_enum_helper.cpp" "")
CREATE_EXAMPLE(io_calibrationpointsio "io_calibrationpointsio.cpp" "")
CREATE_EXAMPLE(io_pclviewerionaive "io_pclviewerionaive.cpp" "")
CREATE_EXAMPLE(io_rgbd "io_rgbd.cpp" "")
CREATE_EXAMPLE(shm_common_SharedPCServer "shm_common_SharedPCServer.cpp" "")
CREATE_EXAMPLE(shm_common_SharedPCClient "shm_common_SharedPCClient.cpp" "")

#######################################################################
if (USE_STATIC)
  set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
  foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
  endforeach()
endif (USE_STATIC)