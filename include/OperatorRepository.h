#pragma once

#include "Database.h"
#include <optional>
#include <vector>

class IOperatorRepository {
public:
    virtual ~IOperatorRepository() = default;

    virtual int add(const Operator& item) = 0;
    virtual std::optional<Operator> findById(int id) const = 0;
    virtual std::vector<Operator> findAll() const = 0;
    virtual void update(const Operator& item) = 0;
    virtual void remove(int id) = 0;
};

class OperatorRepository : public IOperatorRepository {
private:
    decltype(createStorage("callcenter.db"))* storage;

public:
    explicit OperatorRepository(decltype(createStorage("callcenter.db"))& db);

    int add(const Operator& item) override;
    std::optional<Operator> findById(int id) const override;
    std::vector<Operator> findAll() const override;
    void update(const Operator& item) override;
    void remove(int id) override;
};