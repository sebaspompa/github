#pragma once

#include <string>
#include <optional>
#include <cstdint>

#include "src/app/repositories/FileStorage.hpp"
#include "src/data_structures/Dictionary.hpp"

using std::string;

struct AccountEntity
{
    uint64_t id;
    string username;
    string password;
};

// schema: ID - USERNAME - PASSWORD
struct AccountRepository
{
private:
    static constexpr char DELIMITER = '|';
    static constexpr int FIELD_COUNT = 3;

    static constexpr int ID_INDEX = 0;
    static constexpr int USERNAME_INDEX = 1;
    static constexpr int PASS_INDEX = 2;

    const string filePath;

    // DB principal indexado por id
    Dictionary<uint64_t, AccountEntity> accounts;

    // Mapea: username -> id
    Dictionary<string, uint64_t> usernameToId;

private:
    void load()
    {
        FileStorage::forEach(filePath, FIELD_COUNT, DELIMITER, [&](const auto &row)
        {
            AccountEntity ae;
            ae.id = static_cast<uint64_t>(std::stoull(row[ID_INDEX]));
            ae.username = row[USERNAME_INDEX];
            ae.password = row[PASS_INDEX];

            accounts.add(ae.id, ae);
            usernameToId.add(ae.username, ae.id);
        });
    }

    void persist(const AccountEntity &ae) const
    {
        const string id = std::to_string(ae.id);

        FileStorage::upsert(filePath, FIELD_COUNT, DELIMITER, { id, ae.username, ae.password },
            [&id](const auto &row) -> bool
            {
                return id == row[ID_INDEX];
            }
        );
    }

public:
    AccountRepository(const string &filePath) : filePath(filePath)
    {
        load();
    }

    bool create(const AccountEntity &ae)
    {
        if (accounts.contains(ae.id) || usernameToId.contains(ae.username))
        {
            return false;
        }

        persist(ae);
        accounts.add(ae.id, ae);
        usernameToId.add(ae.username, ae.id);

        return true;
    }

    std::optional<AccountEntity> findById(uint64_t id) const
    {
        return accounts.get(id);
    }

    std::optional<AccountEntity> findByUsername(const string &username) const
    {
        auto accountId = usernameToId.get(username);

        if (!accountId) { return std::nullopt; }

        return accounts.get(*accountId);
    }

    size_t count() const
    {
        return accounts.count();
    }
};
