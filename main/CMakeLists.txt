#
# Use the Global Recursive action to gather up all the source files under a variable called SOURCES
#
# NOTE: Anytime you add a new .cpp files, the entire project must be cleaned and rebuilt or the new files
# will not be included in the compilation process.  This is the one drawback of GLOB_RECURSE implicit inclusion.
#
FILE(GLOB_RECURSE SOURCES ${PROJECT_DIR}/main/*.cpp)
#
# Included components which are exposed in public header files.
set(REQUIRES
    esp_event
    esp_timer
    driver
    indication_ws2812_5.2
)
#
# Anything that must be included, but may remain hidden from the public header files.
# Limiting component exposure can reduce Undefined Reference linking problems in larger applications.
set(PRIV_REQUIRES
    nvs 
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