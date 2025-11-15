#include "employee_generator.hpp"
#include <fstream>
#include <stdexcept>
#include <format>

static const std::string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

EmployeeGenerator::EmployeeGenerator()
    : rng(std::random_device{}()) {}

std::string EmployeeGenerator::getRandomFromJson(const std::string& filename, char first_letter) {
    if (!cache.contains(filename)) {
        std::ifstream f(filename);
        if (!f.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        f >> cache[filename];
    }

    const json& j = cache[filename];

    char letter;
    if (first_letter != 0) {
        if (!j.contains(std::string(1, first_letter))) {
            throw std::runtime_error("Letter not found in JSON: " + std::string(1, first_letter));
        }
        letter = first_letter;
    } else {
        // выбираем случайную букву, которая есть в файле.
        do {
            letter = letters[std::uniform_int_distribution<int>(0, letters.size() - 1)(rng)];
        } while (!j.contains(std::string(1, letter)));
    }

    const auto& arr = j.at(std::string(1, letter));
    return arr[std::uniform_int_distribution<size_t>(0, arr.size() - 1)(rng)];
}

std::string EmployeeGenerator::getRandomName(bool isMale, char first_letter) {
    return getRandomFromJson(
        isMale ? "../data/names_male.json" : "../data/names_female.json",
        first_letter
    );
}

std::string EmployeeGenerator::getRandomSurname(bool isMale, char first_letter) {
    return getRandomFromJson(
        isMale ? "../data/surnames_male.json" : "../data/surnames_female.json",
        first_letter
    );
}

std::string EmployeeGenerator::getRandomPatronymic(bool isMale, char first_letter) {
    return getRandomFromJson(
        isMale ? "../data/patronymics_male.json" : "../data/patronymics_female.json",
        first_letter
    );
}

std::string EmployeeGenerator::getRandomBirthDate() {
    std::uniform_int_distribution<int> year(1970, 2003);
    std::uniform_int_distribution<int> month(1, 12);

    int y = year(rng);
    int m = month(rng);

    int max_day =
        (m == 2) ? ((y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) ? 29 : 28)
      : (m == 4 || m == 6 || m == 9 || m == 11 ? 30 : 31);

    std::uniform_int_distribution<int> day(1, max_day);
    int d = day(rng);

    return std::format("{:04}-{:02}-{:02}", y, m, d);
}

Employee EmployeeGenerator::generateRandom(uint8_t gender_mode, char first_letter) {
    bool isMale =
        (gender_mode == 0)
        ? (std::uniform_int_distribution<int>(0, 1)(rng) == 0)
        : (gender_mode == 1);

    std::string name = getRandomName(isMale, first_letter);
    std::string surname = getRandomSurname(isMale);
    std::string patronymic = getRandomPatronymic(isMale);
    std::string birthdate = getRandomBirthDate();

    return {
        surname + " " + name + " " + patronymic,
        birthdate,
        isMale ? "Male" : "Female"
    };
}
