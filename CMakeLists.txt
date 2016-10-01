project (liquiddb)

# Custom modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Project header
project_header_default ("POSITION_INDEPENDENT_CODE:YES")

# 3rdparty
find_package (Platform REQUIRED)
include_directories (${PLATFORM_INCLUDE})

find_package (Sqlite REQUIRED)
include_directories (${SQLITE_INCLUDE})
list (APPEND ${PROJECT_NAME}_LIBS ${SQLITE_LIBRARY})

# Sources
add_subdirectory (src)

# Target - liquiddb
add_library(liquiddb STATIC ${${PROJECT_NAME}_SOURCES})
target_link_libraries (liquiddb ${${PROJECT_NAME}_LIBS})

# Documentation
add_documentation ("liquidDB" 0.0.1 "This library is a front-end for SQLite database")

# Install rules
install_header_files (liquiddb "src/ldb_Constraint.h:Constraint"
                               "src/ldb_Database.h:Database"
                               "src/ldb_DataSet.h:DataSet"
                               "src/entities/ldb_Entity.h:Entity"
                               "src/entities/ldb_EntityConstraint.h:EntityConstraint"
                               "src/entities/ldb_EntityTitle.h:EntityTitle"
                               "src/entities/ldb_EntityValue.h:EntityValue"
                               "src/entities/ldb_EntityValueReader.h:EntityValueReader"
                               "src/ldb_Field.h:Field"
                               "src/ldb_Join.h:Join"
                               "src/ldb_Link.h:Link"
                               "src/ldb_Query.h:Query"
                               "src/ldb_Storage.h:Storage"
                               "src/ldb_Table.h:Table"
                               "src/ldb_UndoStack.h:UndoStack"
                               "src/ldb_Value.h:Value")
install_target (liquiddb)
install_cmake_files ("cmake/FindSqlite.cmake"
                     "cmake/FindLiquidDb.cmake")
