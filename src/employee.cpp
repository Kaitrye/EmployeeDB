#include "employee.hpp"
#include "database.hpp"
#include <ctime>
#include <sstream>

Employee::Employee(
    const std::string& fullName, 
    const std::string& birthDate, 
    const std::string& gender
): 
    fullName(checkFullname(fullName)), 
    birthDate(parseBirthDate(birthDate)), 
    gender(normalizeGender(gender)) {}


std::string Employee::checkFullname(const std::string& name) const {
    std::regex fullNameRegex(R"(^\s*\S+\s+\S+\s+\S+\s*$)");
    std::string trimmedFullName = name;
    trimmedFullName.erase(std::remove(trimmedFullName.begin(), trimmedFullName.end(), '\n'), trimmedFullName.end());
    trimmedFullName.erase(std::remove(trimmedFullName.begin(), trimmedFullName.end(), '\r'), trimmedFullName.end());

    if (!std::regex_match(trimmedFullName, fullNameRegex)) {
        throw std::invalid_argument("Full name must contain exactly three words.");
    }

    return trimmedFullName;
}

std::chrono::year_month_day Employee::parseBirthDate(const std::string& s) const {
    using namespace std::chrono;
    std::regex dateRegex(R"(^\d{4}-\d{2}-\d{2}$)");
    if (!std::regex_match(s, dateRegex)) {
        throw std::invalid_argument("Birth date must be in format YYYY-MM-DD.");
    }

    int y;
    unsigned int m, d;
    std::sscanf(s.c_str(), "%d-%d-%d", &y, &m, &d);

    year_month_day ymd = year{y} / month{m} / day{d};

    if (!ymd.ok()) {
        throw std::invalid_argument("Birth date is not a valid calendar date.");
    }

    return ymd;
}

std::string Employee::normalizeGender(const std::string& g) const {
    std::string lower = g;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "male") return "Male";
    if (lower == "female") return "Female";     
    throw std::invalid_argument("Gender must be 'Male' or 'Female'.");
}

unsigned int Employee::getAge() const { 
    using namespace std::chrono;

    auto today = floor<days>(system_clock::now());
    year_month_day ymd_today = year_month_day{today};

    // Если день рождения еще не наступил в этом году, уменьшаем возраст на 1
    unsigned int age = static_cast<unsigned int>(int(ymd_today.year()) - int(birthDate.year()));
    if (ymd_today.month() < birthDate.month() || 
        (ymd_today.month() == birthDate.month() && ymd_today.day() < birthDate.day())) {
        age--;
    }

    return age;
}

void Employee::saveToDatabase(Database& db) const {
        db.addEmployee(*this);
}