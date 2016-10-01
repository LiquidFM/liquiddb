# - Find liquiddb
# Find the liquiddb library
#
#  This module defines the following variables:
#     LIQUIDDB_FOUND   - True if LIQUIDDB_INCLUDE is found
#     LIQUIDDB_INCLUDE - where to find header files
#     LIQUIDDB_LIB     - the library files


if (NOT LIQUIDDB_FOUND)
    find_path (LIQUIDDB_INCLUDE
               NAMES "liquiddb/Table"
               PATH_SUFFIXES include
               DOC "LiquidDb include directory")

    find_library (LIQUIDDB_LIB
                  NAMES liquiddb
                  DOC "LiquidDb library file")

    # handle the QUIETLY and REQUIRED arguments and set LIQUIDDB_FOUND to TRUE if all listed variables are TRUE
    include (FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS (LiquidDb DEFAULT_MSG LIQUIDDB_INCLUDE LIQUIDDB_LIB)

    find_package (Sqlite REQUIRED)
    list (APPEND LIQUIDDB_INCLUDE ${SQLITE_INCLUDE})
    list (APPEND LIQUIDDB_LIB ${SQLITE_LIB})

    list (REMOVE_DUPLICATES LIQUIDDB_INCLUDE)

    mark_as_advanced (LIQUIDDB_INCLUDE LIQUIDDB_LIB)
endif ()
