#include "OperatorRepository.h"

// Конструктор: отримує посилання на сховище бази даних
OperatorRepository::OperatorRepository(
    decltype(createStorage("callcenter.db"))& db)
    : storage(&db)
{
}

// Додавання нового оператора
int OperatorRepository::add(const Operator& item)
{
    return static_cast<int>(storage->insert(item));
}

// Пошук оператора за його ID
std::optional<Operator> OperatorRepository::findById(int id) const
{
    return storage->get_optional<Operator>(id);
}

// Отримання списку всіх операторів
std::vector<Operator> OperatorRepository::findAll() const
{
    return storage->get_all<Operator>();
}

// Оновлення інформації про оператора
void OperatorRepository::update(const Operator& item)
{
    storage->update(item);
}

// Видалення оператора за ID
void OperatorRepository::remove(int id)
{
    storage->remove<Operator>(id);
}