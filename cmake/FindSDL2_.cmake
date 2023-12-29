# - try to find MyPackage library
#
# Example-FindMyPackage-UsingImportedTargets.cmake
#
# This module does the same thing as Example-FindMyPackage.cmake
# except that rather than passing along full path names for libraries,
# it creates imported targets.  The end result is roughly the same to
# the end-user.  Please see that other file for the full documentation
# of the example.
#
# Note that if you're going to be installing target export files, this
# is probably what you should prefer. See cmake mailing list archives.
#
# Start of what would be a minimal module documentation block:
#
# Cache Variables: (not for direct use in CMakeLists.txt)
#  MYPACKAGE_ROOT
#  MYPACKAGE_LIBRARY
#  MYPACKAGE_INCLUDE_DIR
#  MYPACKAGE_a_LIBRARY
#  MYPACKAGE_a_INCLUDE_DIR
#  MYPACKAGE_b_LIBRARY
#  MYPACKAGE_b_INCLUDE_DIR
#  MYPACKAGE_c_LIBRARY
#  MYPACKAGE_c_INCLUDE_DIR
#
# Non-cache variables you might use in your CMakeLists.txt:
#  MYPACKAGE_FOUND
#
#  MYPACKAGE_LIBRARIES
#  MYPACKAGE_INCLUDE_DIRS
#  MYPACKAGE_LINKER_FLAGS
#
#  MYPACKAGE_a_LIBRARIES
#  MYPACKAGE_a_INCLUDE_DIRS
#  MYPACKAGE_a_LINKER_FLAGS
#
#  MYPACKAGE_b_LIBRARIES
#  MYPACKAGE_b_INCLUDE_DIRS
#  MYPACKAGE_b_LINKER_FLAGS
#
#  MYPACKAGE_c_LIBRARIES
#  MYPACKAGE_c_INCLUDE_DIRS
#  MYPACKAGE_c_LINKER_FLAGS
#
# Use this module this way:
#  find_package(MyPackage)
#  include_directories(${MYPACKAGE_INCLUDE_DIRS})
#  add_executable(myapp ${SOURCES})
#  target_link_libraries(myapp ${MYPACKAGE_LIBRARIES})
#  set_property(TARGET myapp PROPERTY LINK_FLAGS ${MYPACKAGE_LINKER_FLAGS})
#
# Requires these CMake modules:
#  FindPackageHandleStandardArgs (CMake standard module)
#
# Original Author:
# 2009-2010 Rylie Pavlik <rylie@ryliepavlik.com>
# https://ryliepavlik.com/
# Iowa State University HCI Graduate Program/VRAC
#
# Copyright 2009-2010, Iowa State University
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# SPDX-License-Identifier: BSL-1.0

set(SDL2_ROOT
	"${SDL2_ROOT}"
	CACHE
	PATH
	"Root directory to look in")

find_library(SDL2_LIBRARY
	NAMES
	    SDL2
	PATHS
	    "${SDL2_ROOT}"
	    PATH_SUFFIXES
	    lib)

find_path(SDL2_INCLUDE_DIR
	NAMES
	    SDL2/SDL.h
	PATHS
	    "${SDL2_ROOT}"
	    PATH_SUFFIXES
	    include)

# Assuming that the components are named libmypackagea, libmypackageb, etc
# foreach(lib a b c)
# 	find_library(MYPACKAGE_${lib}_LIBRARY
# 		NAMES
# 		mypackage${lib}
# 		PATHS
# 		"${MYPACKAGE_ROOT}"
# 		PATH_SUFFIXES
# 		lib)

# 	find_path(MYPACKAGE_${lib}_INCLUDE_DIR
# 		NAMES
# 		mypackage/${lib}/${lib}.h
# 		PATHS
# 		"${MYPACKAGE_ROOT}"
# 		PATH_SUFFIXES
# 		include)

# endforeach()

# see /usr/share/cmake-2.x/Modules/FindBLAS.cmake for the variables this will define
# if(NOT BLAS_FOUND)
# 	find_package(BLAS QUIETLY)
# endif()

# handle the QUIETLY and REQUIRED arguments and set xxx_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2
	DEFAULT_MSG
	SDL2_LIBRARY
	SDL2_INCLUDE_DIR
	# MYPACKAGE_a_LIBRARY
	# MYPACKAGE_a_INCLUDE_DIR
	# MYPACKAGE_b_LIBRARY
	# MYPACKAGE_b_INCLUDE_DIR
	# MYPACKAGE_c_LIBRARY
	# MYPACKAGE_c_INCLUDE_DIR
	# BLAS_FOUND
)

if(SDL2_FOUND)
	# Set variables containing libraries and their dependencies
	# Always use the plural form for the variables defined by other find modules:
	# they might have dependencies too!

	add_library(SDL2 IMPORTED)
	set_target_properties(SDL2
		PROPERTIES
		IMPORTED_LOCATION
		${SDL2_LIBRARY}
		IMPORTED_LINK_INTERFACE_LIBRARIES
		# ${BLAS_LIBRARIES}
    )

    set(SDL2_INCLUDE_DIRS
		${SDL2_INCLUDE_DIR}
		${SDL2_INCLUDE_DIRS}
    )

	# set(MYPACKAGE_c_LIBRARIES mypackage_c)
	# set(MYPACKAGE_c_INCLUDE_DIRS ${MYPACKAGE_c_INCLUDE_DIR})	# No include dir for BLAS?
	# set(MYPACKAGE_c_LINKER_FLAGS ${BLAS_LINKER_FLAGS})

	# add_library(mypackage_b IMPORTED)
	# set_target_properties(mypackage_b
	# 	PROPERTIES
	# 	IMPORTED_LOCATION
	# 	${MYPACKAGE_b_LIBRARY}
	# 	IMPORTED_LINK_INTERFACE_LIBRARIES
	# 	${MYPACKAGE_c_LIBRARIES})
	# set(MYPACKAGE_b_LIBRARIES mypackage_b)
	# set(MYPACKAGE_b_INCLUDE_DIRS
	# 	${MYPACKAGE_b_INCLUDE_DIR}
	# 	${MYPACKAGE_c_INCLUDE_DIRS})
	# set(MYPACKAGE_b_LINKER_FLAGS ${MYPACKAGE_c_LINKER_FLAGS})

	# add_library(mypackage_a IMPORTED)
	# set_target_properties(mypackage_a
	# 	PROPERTIES
	# 	IMPORTED_LOCATION
	# 	${MYPACKAGE_a_LIBRARY}
	# 	IMPORTED_LINK_INTERFACE_LIBRARIES
	# 	${MYPACKAGE_b_LIBRARIES})
	# set(MYPACKAGE_a_LIBRARIES mypackage_a)
	# set(MYPACKAGE_a_INCLUDE_DIRS
	# 	${MYPACKAGE_a_INCLUDE_DIR}
	# 	${MYPACKAGE_b_INCLUDE_DIRS})
	# set(MYPACKAGE_a_LINKER_FLAGS ${MYPACKAGE_b_LINKER_FLAGS})

	# add_library(mypackage_core IMPORTED)
	# set_target_properties(mypackage_core
	# 	PROPERTIES
	# 	IMPORTED_LOCATION
	# 	${MYPACKAGE_LIBRARY}
	# 	IMPORTED_LINK_INTERFACE_LIBRARIES
	# 	${MYPACKAGE_a_LIBRARIES})
	# set(MYPACKAGE_LIBRARIES mypackage_core)
	# set(MYPACKAGE_INCLUDE_DIRS
	# 	${MYPACKAGE_INCLUDE_DIR}
	# 	${MYPACKAGE_a_INCLUDE_DIRS})
	# set(MYPACKAGE_LINKER_FLAGS ${MYPACKAGE_a_LINKER_FLAGS}
    # )

endif()

mark_as_advanced(SDL2_LIBRARY
	SDL2_INCLUDE_DIR
	# MYPACKAGE_a_LIBRARY
	# MYPACKAGE_a_INCLUDE_DIR
	# MYPACKAGE_b_LIBRARY
	# MYPACKAGE_b_INCLUDE_DIR
	# MYPACKAGE_c_LIBRARY
	# MYPACKAGE_c_INCLUDE_DIR
)

if(SDL2_FOUND)
	mark_as_advanced(SDL2_ROOT)
endif()

# End of Example-FindMyPackage-UsingImportedTargets.cmake