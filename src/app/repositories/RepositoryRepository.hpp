#pragma once

#include <string>
#include <optional>
#include <vector>
#include <functional>
#include <cstdint>

#include "src/app/repositories/FileStorage.hpp"
#include "src/data_structures/Dictionary.hpp"
#include "src/app/repositories/TextTokenizer.hpp"

using std::function;
using std::optional;
using std::string;
using std::vector;

struct RepositoryEntity
{
    uint64_t userId;
    uint64_t id;
    string name;
    string about;
    uint64_t languageId;
    int stars;
    int forks;
    bool isPublic;
};

// schema: USER_ID - ID - NAME - ABOUT - LANGUAGE_ID - STARS - FORKS - IS_PUBLIC
struct RepositoryRepository
{
private:
    // Config
    static constexpr char DELIMITER = '|';
    static constexpr int FIELD_COUNT = 8;

    static constexpr int USER_ID_INDEX = 0;
    static constexpr int ID_INDEX = 1;
    static constexpr int NAME_INDEX = 2;
    static constexpr int ABOUT_INDEX = 3;
    static constexpr int LANGUAGE_ID_INDEX = 4;
    static constexpr int STARS_INDEX = 5;
    static constexpr int FORKS_INDEX = 6;
    static constexpr int IS_PUBLIC_INDEX = 7;

    const string filePath;

    // DB principal indexado por id del repositorio
    Dictionary<uint64_t, RepositoryEntity> repositoriesById;

    // Mapea: userId -> [repositoryId]
    Dictionary<uint64_t, vector<uint64_t>> userToRepoIds;

    // Mapea: palabra clave -> [repositoryId]
    Dictionary<string, vector<uint64_t>> wordToRepoIds;

private:
    void indexUserRepository(uint64_t userId, uint64_t repoId)
    {
        if (userToRepoIds.contains(userId))
        {
            userToRepoIds[userId].push_back(repoId);
        }
        else
        {
            userToRepoIds.add(userId, { repoId });
        }
    }

    void indexRepositoryKeywords(const RepositoryEntity &repository)
    {
        vector<string> keywords = TextTokenizer::extractKeywords(repository.name + " " + repository.about);

        for (const string &word : keywords)
        {
            if (wordToRepoIds.contains(word))
            {
                wordToRepoIds[word].push_back(repository.id);
            }
            else
            {
                wordToRepoIds.add(word, {repository.id});
            }

        }
    }

    void load()
    {
        FileStorage::forEach(filePath, FIELD_COUNT, DELIMITER, [this](const auto &row)
        {
            RepositoryEntity repository;

            repository.userId = static_cast<uint64_t>(std::stoull(row[USER_ID_INDEX]));
            repository.id = static_cast<uint64_t>(std::stoull(row[ID_INDEX]));
            repository.name = row[NAME_INDEX];
            repository.about = row[ABOUT_INDEX];
            repository.languageId = static_cast<uint64_t>(std::stoull(row[LANGUAGE_ID_INDEX]));
            repository.stars = std::stoi(row[STARS_INDEX]);
            repository.forks = std::stoi(row[FORKS_INDEX]);
            repository.isPublic = row[IS_PUBLIC_INDEX] == "1";

            repositoriesById.add(repository.id, repository);

            indexUserRepository(repository.userId, repository.id);

            indexRepositoryKeywords(repository);
        });
    }

    void persist(const RepositoryEntity &repository) const
    {
        const string id = std::to_string(repository.id);

        FileStorage::upsert(filePath, FIELD_COUNT, DELIMITER,
        {
            std::to_string(repository.userId),
            id,
            repository.name,
            repository.about,
            std::to_string(repository.languageId),
            std::to_string(repository.stars),
            std::to_string(repository.forks),
            repository.isPublic ? "1" : "0"
        },
        [&id](const auto &row) -> bool
        {
            return id == row[ID_INDEX];
        });
    }

public:
    RepositoryRepository(const string &filePath) : filePath(filePath)
    {
        load();
    }

    bool create(const RepositoryEntity &repository)
    {
        if (repositoriesById.contains(repository.id))
        {
            return false;
        }

        persist(repository);
        repositoriesById.add(repository.id, repository);
        indexUserRepository(repository.userId, repository.id);
        indexRepositoryKeywords(repository);

        return true;
    }

    optional<RepositoryEntity> findById(uint64_t id) const
    {
        return repositoriesById.get(id);
    }

    vector<RepositoryEntity> findByUserId(uint64_t userId) const
    {
        vector<RepositoryEntity> result;
        auto repoIds = userToRepoIds.get(userId);

        if (!repoIds)
        {
            return result;
        }

        for (uint64_t repoId : *repoIds)
        {
            auto repo = repositoriesById.get(repoId);

            if (repo)
            {
                result.push_back(*repo);
            }
        }

        return result;
    }

    size_t count() const
    {
        return repositoriesById.count();
    }

    vector<RepositoryEntity> search(const string &query) const
    {
        vector<RepositoryEntity> result;
        unordered_set<uint64_t> matchedIds;

        vector<string> keywords = TextTokenizer::extractKeywords(query);

        for (const string &word : keywords)
        {
            auto repoIds = wordToRepoIds.get(word);

            if (!repoIds)
            {
                continue;
            }

            for (uint64_t repoId : *repoIds)
            {
                if (matchedIds.insert(repoId).second)
                {
                    auto repo = repositoriesById.get(repoId);

                    if (repo)
                    {
                        result.push_back(*repo);
                    }
                }
            }
        }

        return result;
    }

    void visitAll(function<void(const RepositoryEntity &)> onEachVisit) const
    {
        for (const auto &entry : repositoriesById)
        {
            onEachVisit(entry.value);
        }
    }
};
