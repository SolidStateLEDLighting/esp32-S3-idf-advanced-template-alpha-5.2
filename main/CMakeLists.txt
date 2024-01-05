#
# Use the Global Recursive action to gather up all the source files under a variable called APP_SOURCES
#
# NOTE: Anytime you add a new .cpp files, the entire project must be cleaned and rebuilt or the new files
# will not be included in the compilation process.  This is the one drawback of GLOB_RECURSE implicit inclusion.
#
FILE(GLOB_RECURSE SOURCES ${PROJECT_DIR}/main/*.cpp)
#
# Exposes components to both source and header files.
set(REQUIRES
)
#
# Anything that must be exposed to the sources files, but may remain hidden from the header files.
set(PRIV_REQUIRES
)
#
#
set(COMPONENT_ADD_INCLUDEDIRS
    ${CMAKE_CURRENT_LIST_DIR}/include
)
#
#
idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS ${COMPONENT_ADD_INCLUDEDIRS}
                       REQUIRES ${REQUIRES}
                       PRIV_REQUIRES ${PRIV_REQUIRES}
)