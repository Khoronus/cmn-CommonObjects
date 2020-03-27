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