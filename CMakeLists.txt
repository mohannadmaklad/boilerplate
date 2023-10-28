cmake_minimum_required(VERSION 3.16.3)

project(boilerplate 
    LANGUAGES CXX
    VERSION 1.0.0)

include(GNUInstallDirs)

configure_file(${CMAKE_SOURCE_DIR}/info.h.in ${CMAKE_SOURCE_DIR}/info.h)
add_executable(boilerplate main.cpp)
target_compile_features(boilerplate PUBLIC cxx_std_17)
target_compile_options(boilerplate PUBLIC "-Wall")

#add FunctionalPlus
include(FetchContent)
# declare dependencies
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/deps)
FetchContent_Declare(FunctionalPlus
GIT_REPOSITORY https://github.com/Dobiasd/FunctionalPlus.git
GIT_TAG v0.2.20-p0
)
FetchContent_MakeAvailable(FunctionalPlus)
target_link_libraries(boilerplate FunctionalPlus::fplus)


#add installation rules
install(TARGETS boilerplate
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/
    DESTINATION ${CMAKE_INSTALL_LOCALSTATEDIR})