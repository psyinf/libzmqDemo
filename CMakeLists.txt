cmake_minimum_required(VERSION 3.22)
project(app_template)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(ENABLE_TESTING "Enable testing" ON)



include(cmake/PreventInSourceBuilds.cmake)
include(cmake/coverage.cmake)
include(cmake/stacktrace.cmake)
include(cmake/setup_cpm.cmake)
include(cmake/deps_cpm.cmake)


add_subdirectory(libs)
add_subdirectory(apps)

if (ENABLE_TESTING)
    enable_testing()
	add_subdirectory(tests)
endif()

install(DIRECTORY
        data/
    DESTINATION
        data
    COMPONENT
        ${RUNTIME_COMPONENT}
)