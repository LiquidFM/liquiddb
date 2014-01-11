project (liquiddb)

# Project header
project_cxx_library_header_default ()

# 3rdparty
find_package (Sqlite REQUIRED)
include_directories (${SQLITE_INCLUDE})
list (APPEND ${PROJECT_NAME}_LIBS ${SQLITE_LIBRARY})

# Sources
add_subdirectory (src)

# Target - liquiddb
add_library(liquiddb STATIC ${${PROJECT_NAME}_SOURCES})
target_link_libraries (liquiddb ${${PROJECT_NAME}_LIBS})

# Version
build_version_file (liquiddb 0.0.1)

# Documentation
add_documentation ("liquidDB" 0.0.1 "This library is a front-end for SQLite database")
