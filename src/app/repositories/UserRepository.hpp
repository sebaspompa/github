#pragma once

#include <string>
#include <optional>
#include <functional>
#include <cstdint>

#include "src/app/repositories/FileStorage.hpp"
#include "src/data_structures/Dictionary.hpp"

using std::function;
using std::optional;
using std::string;

struct UserEntity
{
    uint64_t id;
    string name;
    string bio;
};

// schema: ID - NAME - BIO
struct UserRepository
{
private:
    // Config
    static constexpr char DELIMITER = '|';
    static constexpr int FIELD_COUNT = 3;

    static constexpr int ID_INDEX = 0;
    static constexpr int NAME_INDEX = 1;
    static constexpr int BIO_INDEX = 2;

    const string filePath;

    // DB principal indexado por id del usuario
    Dictionary<uint64_t, UserEntity> usersById;

private:
    void load()
    {
        FileStorage::forEach(filePath, FIELD_COUNT, DELIMITER, [this](const auto &row)
        {
            UserEntity user;

            user.id = static_cast<uint64_t>(std::stoull(row[ID_INDEX]));
            user.name = row[NAME_INDEX];
            user.bio = row[BIO_INDEX];

            usersById.add(user.id, user);
        });
    }

    void persist(const UserEntity &user) const
    {
        const string id = std::to_string(user.id);

        FileStorage::upsert(filePath, FIELD_COUNT, DELIMITER,
        {
            id,
            user.name,
            user.bio
        },
        [&id](const auto &row) -> bool
        {
            return id == row[ID_INDEX];
        });
    }

public:
    UserRepository(const string &filePath) : filePath(filePath)
    {
        load();
    }

    bool create(const UserEntity &user)
    {
        if (usersById.contains(user.id))
        {
            return false;
        }

        persist(user);
        usersById.add(user.id, user);

        return true;
    }

    bool update(const UserEntity &user)
    {
        if (!usersById.contains(user.id))
        {
            return false;
        }

        persist(user);
        usersById.add(user.id, user);

        return true;
    }

    optional<UserEntity> findById(uint64_t id) const
    {
        return usersById.get(id);
    }

    size_t count() const
    {
        return usersById.count();
    }

    void visitAll(function<void(const UserEntity &)> onEachVisit) const
    {
        for (const auto &entry : usersById)
        {
            onEachVisit(entry.value);
        }
    }
};
