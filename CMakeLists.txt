cmake_minimum_required(VERSION 3.16)
project(employee_app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# PostgreSQL
find_package(PostgreSQL REQUIRED)

# libpqxx
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

include_directories(
    include
    ${PostgreSQL_INCLUDE_DIRS}
    ${PQXX_INCLUDE_DIRS}
)

add_executable(myApp
    src/main.cpp
    src/employee.cpp
    src/database.cpp
    src/employee_generator.cpp
)

# Линкуем напрямую с pqxx и pq
target_link_libraries(myApp PRIVATE pqxx pq)
