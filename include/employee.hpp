#ifndef EMPLOYEE_HPP
#define EMPLOYEE_HPP

#include <string>
#include <regex>
#include <chrono>
#include <format>

class Database;

class Employee {
public:
    Employee() = default;
    Employee(const std::string& fullName, const std::string& birthDate, const std::string& gender);

    const std::string& getFullName() const { return fullName; }
    std::string getBirthDate() const {
        using namespace std::chrono;
        return std::format("{:04}-{:02}-{:02}", int(birthDate.year()), unsigned(birthDate.month()), unsigned(birthDate.day()));
    }
    const std::string& getGender() const { return gender; }
    unsigned int getAge() const;

    void saveToDatabase(Database& db) const;

private:
    std::string fullName{};
    std::chrono::year_month_day birthDate{};
    std::string gender{};

    std::string checkFullname(const std::string& name) const;
    std::chrono::year_month_day parseBirthDate(const std::string& s) const;
    std::string normalizeGender(const std::string& g) const;
};

#endif // EMPLOYEE_HPP