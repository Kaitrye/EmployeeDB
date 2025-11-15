#include "database.hpp"
#include <iostream>
#include <sstream>

Database::Database(const std::string& connStr) : 
    dbConnStr(connStr), 
    conn(pqxx::connection(dbConnStr)){}

void Database::createTable() {
    try {
        pqxx::work W(conn);

        W.exec(
            "CREATE TABLE IF NOT EXISTS employees ("
            "id SERIAL PRIMARY KEY,"
            "full_name TEXT NOT NULL,"
            "birth_date DATE NOT NULL,"
            "gender TEXT NOT NULL);"
        );

        W.commit();
        std::cout << "Table created successfully\n";
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void Database::addEmployee(const Employee& emp) {
    try {
        pqxx::work W(conn);

        W.exec(
            "INSERT INTO employees(full_name, birth_date, gender) VALUES($1, $2, $3)",
            pqxx::params{
                emp.getFullName(),
                emp.getBirthDate(),
                emp.getGender()
            }
        );

        W.commit();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void Database::addEmployeesBatch(const std::vector<Employee>& employees) {
    try {
        pqxx::work W(conn);

        pqxx::stream_to stream = pqxx::stream_to::table(
            W,
            pqxx::table_path{"employees"},
            {"full_name", "birth_date", "gender"}
        );

        for (const auto& emp : employees) {
            stream << std::make_tuple(emp.getFullName(), emp.getBirthDate(), emp.getGender());
        }

        stream.complete();
        W.commit();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

std::vector<Employee> Database::getAllEmployees() const {
    std::vector<Employee> result;

    try {
        pqxx::work W(conn);

        std::string sql =
            "SELECT DISTINCT ON (full_name, birth_date) "
            "full_name, birth_date, gender "
            "FROM employees "
            "ORDER BY full_name, birth_date;";

        pqxx::result R = W.exec(sql);

        for (auto row : R) {
            result.emplace_back(
                row["full_name"].c_str(),
                row["birth_date"].c_str(),
                row["gender"].c_str()
            );
        }

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return result;
}

void Database::getFilteredEmployees() const {
    std::vector<Employee> result;

    try {
        pqxx::work W(conn);

        std::string sql =
            "SELECT full_name, birth_date, gender "
            "FROM employees "
            "WHERE gender = 'Male' AND full_name LIKE 'F%' "
            "ORDER BY full_name, birth_date;";

        auto start = std::chrono::steady_clock::now();
        pqxx::result R = W.exec(sql);
        auto end = std::chrono::steady_clock::now();

        std::cout << "Filtered query execution time: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                  << " ms\n";

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
