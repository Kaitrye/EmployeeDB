#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "employee.hpp"
#include <pqxx/pqxx>
#include <string>
#include <vector>

class Database {
private:
    std::string dbConnStr;
    mutable pqxx::connection conn;

public:
    Database(const std::string& connStr);
    void createTable();
    void addEmployee(const Employee& emp);
    void addEmployeesBatch(const std::vector<Employee>& employees);
    std::vector<Employee> getAllEmployees() const;
    void getFilteredEmployees() const;
};

#endif // DATABASE_HPP