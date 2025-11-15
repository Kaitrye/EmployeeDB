#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include "employee.hpp"
#include "employee_generator.hpp"
#include "database.hpp"

std::string getConnectionString() {
    const char* user = std::getenv("POSTGRES_USER");
    const char* password = std::getenv("POSTGRES_PASSWORD");
    const char* db = std::getenv("POSTGRES_DB");
    const char* host = std::getenv("POSTGRES_HOST");
    const char* port = std::getenv("POSTGRES_PORT");

    return "host=" + std::string(host ? host : "localhost") +
           " port=" + std::string(port ? port : "5432") +
           " dbname=" + std::string(db ? db : "employees") +
           " user=" + std::string(user ? user : "postgres") +
           " password=" + std::string(password ? password : "123");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: myApp <mode> [parameters]\n";
        return 1;
    }

    int mode = std::stoi(argv[1]);
    Database db(getConnectionString());

    switch (mode) {
    case 1: {
        db.createTable();
        break;
    }
    case 2: {
        if (argc < 5) {
            std::cout << "Usage: myApp 2 \"Full Name\" YYYY-MM-DD [Male/Female]\n";
            return 1;
        }

        Employee emp(argv[2], argv[3], argv[4]);
        emp.saveToDatabase(db);
        std::cout << "Employee added.\n";
        break;
    }
    case 3: {
        auto employees = db.getAllEmployees();
        for (const auto& emp : employees) {
            std::cout << emp.getFullName() << " " << emp.getBirthDate() << " " << emp.getGender()
                      << " Age: " << emp.getAge() << "\n";
        }
        break;
    }
    case 4: {
        const int THREADS = 8;                       // потоки генерации
        const int TOTAL_RECORDS = 10'000'000;         // всего сотрудников
        const int BATCH_SIZE = 100'000;               // размер батча
        const int PER_THREAD = TOTAL_RECORDS / THREADS;

        std::queue<std::vector<Employee>> queue;
        std::mutex queue_mtx;
        std::condition_variable queue_cv;
        bool done = false;

        // --- поток вставки ---
        std::thread writer([&]() {
            std::vector<Employee> batch;
            while (true) {
                {
                    std::unique_lock<std::mutex> lock(queue_mtx);
                    queue_cv.wait(lock, [&] { return !queue.empty() || done; });
                    if (queue.empty() && done) break;
                    batch = std::move(queue.front());
                    queue.pop();
                }
                db.addEmployeesBatch(batch);
            }
            std::cout << "Writer finished inserting all batches.\n";
        });

        auto tstart = std::chrono::steady_clock::now();

        // --- потоки генерации ---
        std::vector<std::thread> workers;
        for (int t = 0; t < THREADS; ++t) {
            workers.emplace_back([&, t]() {
                EmployeeGenerator gen;
                std::vector<Employee> batch;
                batch.reserve(BATCH_SIZE);

                for (int i = 0; i < PER_THREAD; ++i) {
                    batch.push_back(gen.generateRandom());
                    std::cout << "Thread " << t + 1 << " generated " << (i + 1) << " / " << PER_THREAD << "\r";

                    if (batch.size() == BATCH_SIZE) {
                        {
                            std::unique_lock<std::mutex> lock(queue_mtx);
                            queue.push(std::move(batch));
                        }
                        queue_cv.notify_one();
                        batch.clear();
                    }
                }

                if (!batch.empty()) {
                    {
                        std::unique_lock<std::mutex> lock(queue_mtx);
                        queue.push(std::move(batch));
                    }
                    queue_cv.notify_one();
                }

                std::cout << "Thread " << t << " finished generating\n";
            });
        }

        for (auto &th : workers)
            th.join();

        {
            std::unique_lock<std::mutex> lock(queue_mtx);
            done = true;
        }
        queue_cv.notify_all();

        writer.join();

        // --- добавляем 100 сотрудников мужского пола с фамилией на "F" ---
        {
            EmployeeGenerator gen;
            std::vector<Employee> special_batch;
            special_batch.reserve(100);

            for (int i = 0; i < 100; ++i) {
                special_batch.push_back(gen.generateRandom(1, 'F')); // 1 = male, 'F' = фамилия на F
            }
            db.addEmployeesBatch(special_batch);
            std::cout << "Added 100 special employees (Male + F*)\n";
        }

        auto tend = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count();

        std::cout << "All threads finished. Total time: " << ms << " ms\n";
        break;
    }
    case 5: {
        db.getFilteredEmployees();
        break;
    }
    default:
        std::cout << "Unknown mode\n";
    }

    return 0;
}
