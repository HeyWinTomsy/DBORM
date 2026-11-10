#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <sqlite3.h>

#include "Database.h"
#include "OperatorRepository.h"

using namespace std;
using namespace sqlite_orm;

// Підготовка тестових даних
void prepareData(auto& storage)
{
    if (!storage.template get_all<Operator>().empty())
        return;

    vector<int> ids;

    vector<Operator> operators = {
        {0, "Ivan", "Petrenko", "Sales", 3},
        {0, "Olena", "Kovalenko", "Support", 5},
        {0, "Andriy", "Shevchenko", "Sales", 2},
        {0, "Maria", "Bondarenko", "Technical", 4},
        {0, "Dmytro", "Melnyk", "Support", 6}
    };

    for (const auto& op : operators)
        ids.push_back(static_cast<int>(storage.insert(op)));

    vector<string> phones = {
        "+380501111111",
        "+380502222222",
        "+380503333333"
    };

    vector<int> durations = {120, 240, 180};

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Call call{
                0,
                ids[i],
                phones[j],
                "2026-11-10",
                durations[j] + i * 10,
                j == 0 ? "Resolved" :
                j == 1 ? "Transferred" : "Unresolved"
            };

            storage.insert(call);
        }
    }

    cout << "Test data created successfully!\n";
}

// Демонстрація Repository
void repositoryDemo(OperatorRepository& repo)
{
    cout << "\n=== REPOSITORY ===\n";

    Operator op{
        0, "Test", "Operator", "Sales", 1
    };

    int id = repo.add(op);

    cout << "Added operator ID: " << id << '\n';

    auto found = repo.findById(id);

    if (found)
        cout << "Found: " << found->name
             << " " << found->surname << '\n';

    found->experience = 2;
    repo.update(*found);

    cout << "Operator updated.\n";

    repo.remove(id);

    cout << "Operator removed.\n";
}

// Демонстрація запитів
void queryDemo(auto& storage)
{
    cout << "\n=== QUERY DEMONSTRATION ===\n";

    // 1. Фільтрація
    cout << "\n1. Calls longer than 180 seconds:\n";

    auto filtered = storage.template get_all<Call>(
        where(c(&Call::duration) > 180)
    );

    for (const auto& call : filtered)
    {
        cout << call.id << " | "
             << call.clientPhone << " | "
             << call.duration << " sec\n";
    }

    // 2. Сортування та обмеження
    cout << "\n2. Top 5 longest calls:\n";

    auto sorted = storage.template get_all<Call>(
        order_by(&Call::duration).desc(),
        limit(5)
    );

    for (const auto& call : sorted)
    {
        cout << call.clientPhone << " | "
             << call.duration << " sec\n";
    }

    // 3. Агрегація
    cout << "\n3. Average call duration:\n";

    auto average = storage.avg(&Call::duration);

    if (average)
        cout << average << " seconds\n";

    cout << "Total calls: "
         << storage.template count<Call>() << '\n';

    // 4. JOIN
    cout << "\n4. Operators and their calls:\n";

    auto joined = storage.select(
        columns(
            &Operator::name,
            &Call::clientPhone,
            &Call::duration
        ),
        inner_join<Call>(
            on(c(&Operator::id) == &Call::operatorId)
        )
    );

    for (const auto& [name, phone, duration] : joined)
    {
        cout << name << " | "
             << phone << " | "
             << duration << " sec\n";
    }
}

// Демонстрація транзакцій
void transactionDemo(auto& storage)
{
    cout << "\n=== TRANSACTIONS ===\n";

    // Успішна транзакція
    bool committed = storage.transaction([&] {

        Operator op{
            0, "Transaction", "Success", "Sales", 1
        };

        int operatorId = static_cast<int>(
            storage.insert(op)
        );

        Call call{
            0,
            operatorId,
            "+380509999999",
            "2026-11-10",
            200,
            "Resolved"
        };

        storage.insert(call);

        return true;
    });

    cout << "Successful transaction: "
         << (committed ? "COMMIT" : "ROLLBACK") << '\n';

    // Транзакція з помилкою
    auto existingCalls = storage.template get_all<Call>();

    int existingId = existingCalls.front().id;

    int before = storage.template count<Operator>();

    try
    {
        storage.transaction([&] {

            Operator op{
                0, "Transaction", "Failure", "Support", 1
            };

            int operatorId = static_cast<int>(
                storage.insert(op)
            );

            // Повторний первинний ключ викликає помилку
            Call invalidCall{
                existingId,
                operatorId,
                "+380508888888",
                "2026-11-10",
                100,
                "Failed"
            };

            storage.insert(invalidCall);

            return true;
        });
    }
    catch (const std::exception& ex)
    {
        cout << "Transaction error: "
             << ex.what() << '\n';
    }

    int after = storage.template count<Operator>();

    cout << "Operators before: " << before << '\n';
    cout << "Operators after: " << after << '\n';

    cout << "Rollback verified: "
         << (before == after ? "YES" : "NO") << '\n';
}

// Демонстрація SQLite C API
void rawSqlDemo()
{
    cout << "\n=== SQLITE C API ===\n";

    sqlite3* db = nullptr;

    if (sqlite3_open("callcenter.db", &db) != SQLITE_OK)
    {
        cerr << "Cannot open database\n";
        if (db) sqlite3_close(db);
        return;
    }

    const char* createSql =
        "CREATE TABLE IF NOT EXISTS RawCallDemo ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "phone TEXT NOT NULL);";

    if (sqlite3_exec(db, createSql, nullptr, nullptr, nullptr)
        != SQLITE_OK)
    {
        cerr << sqlite3_errmsg(db) << '\n';
        sqlite3_close(db);
        return;
    }

    const char* insertSql =
        "INSERT INTO RawCallDemo(phone) VALUES(?);";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(
        db, insertSql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(
            stmt, 1, "+380507777777", -1, SQLITE_TRANSIENT
        );

        if (sqlite3_step(stmt) == SQLITE_DONE)
            cout << "Raw SQL: call inserted successfully\n";
        else
            cerr << sqlite3_errmsg(db) << '\n';
    }
    else
    {
        cerr << sqlite3_errmsg(db) << '\n';
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

int main()
{
    try
    {
        auto storage = createStorage("callcenter.db");

        storage.sync_schema();

        cout << "Call Center Database\n";
        cout << "Database initialized successfully!\n";

        prepareData(storage);

        OperatorRepository repo(storage);

        repositoryDemo(repo);

        queryDemo(storage);

        transactionDemo(storage);

        rawSqlDemo();

        cout << "\nProgram finished successfully!\n";
    }
    catch (const std::exception& ex)
    {
        cerr << "Application error: "
             << ex.what() << '\n';

        return 1;
    }

    return 0;
}