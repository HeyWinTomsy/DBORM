cmake_minimum_required(VERSION 3.21)

project(DBORM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SqliteOrm CONFIG REQUIRED)

add_executable(DBORM
        src/main.cpp
        src/OperatorRepository.cpp
)

target_include_directories(DBORM PRIVATE include)

target_link_libraries(DBORM PRIVATE
        sqlite_orm::sqlite_orm
)