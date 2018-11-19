cmake_minimum_required(VERSION 3.2)

set(PRO_NAME bulk)

if($ENV{TRAVIS_BUILD_NUMBER})
    project(${PRO_NAME} VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else()
    project(${PRO_NAME} VERSION 0.0.1001)
endif()

if(${CMAKE_MINOR_VERSION} GREATER 10)
        cmake_policy(SET CMP0074 NEW)
endif()


add_executable(${PRO_NAME}       main.cpp)

set_target_properties(${PRO_NAME} PROPERTIES
			CXX_STANDARD 14
			CXX_STANDARD_REQUIRED YES
                        COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra;-std=c++14"
                     )


install(TARGETS ${PRO_NAME} RUNTIME DESTINATION bin)
set(CPACK_GENERATOR "DEB")

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT hard.slot@gmail.com)
include (CPack)
