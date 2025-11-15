cmake_minimum_required(VERSION 3.16)
project(employee_app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PostgreSQL REQUIRED)

add_subdirectory(libpqxx)

include_directories(
    include
    ${PostgreSQL_INCLUDE_DIRS}
    libpqxx/include
)

add_executable(myApp
    src/main.cpp
    src/employee.cpp
    src/database.cpp
    src/employee_generator.cpp
)

target_link_libraries(myApp PRIVATE pqxx ${PostgreSQL_LIBRARIES})
