#pragma once
#include <string>
#include <random>
#include <unordered_map>
#include "employee.hpp"
#include "json.hpp"

using json = nlohmann::json;

class EmployeeGenerator {
public:
    EmployeeGenerator();

    // gender_mode: 0 - random, 1 - male, 2 - female
    // first_letter: 0 - random, 'A'-'Z'
    Employee generateRandom(uint8_t gender_mode = 0, char first_letter = 0);

private:
    std::mt19937 rng;                           // свой RNG на поток
    std::unordered_map<std::string, json> cache; // свой кеш на поток

    std::string getRandomFromJson(const std::string& filename, char first_letter = 0);
    std::string getRandomName(bool isMale = 0, char first_letter = 0);
    std::string getRandomSurname(bool isMale = 0, char first_letter = 0);
    std::string getRandomPatronymic(bool isMale = 0, char first_letter = 0);
    std::string getRandomBirthDate();
};
