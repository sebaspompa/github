#pragma once

#include <string>
#include <sstream>
#include <optional>
#include <vector>
#include <functional>
#include <cstdint>

#include "src/app/repositories/FileStorage.hpp"
#include "src/data_structures/Dictionary.hpp"

using std::function;
using std::optional;
using std::string;
using std::stringstream;
using std::vector;

struct CommitEntity
{
    uint64_t repositoryId;
    string id;
    string title;
    string parentCommitId;
};

// schema: REPOSITORY_ID - COMMIT_ID - TITLE - PARENT_IDS
struct CommitRepository
{
private:
    // Config
    static constexpr char DELIMITER = '|';
    static constexpr char PARENT_DELIMITER = ',';
    static constexpr int FIELD_COUNT = 4;

    static constexpr int REPOSITORY_ID_INDEX = 0;
    static constexpr int ID_INDEX = 1;
    static constexpr int TITLE_INDEX = 2;
    static constexpr int PARENT_IDS_INDEX = 3;

    const string filePath;

    // DB principal indexado por id del commit
    Dictionary<string, CommitEntity> commitsById;

    // Mapea: repositoryId -> [commitId]
    Dictionary<uint64_t, vector<string>> repoToCommitIds;

private:
    void indexRepositoryCommit(uint64_t repositoryId, const string &commitId)
    {
        if (repoToCommitIds.contains(repositoryId))
        {
            repoToCommitIds[repositoryId].push_back(commitId);
        }
        else
        {
            repoToCommitIds.add(repositoryId, { commitId });
        }
    }

    static string serializeParentIds(const vector<string> &parentIds)
    {
        string result;

        for (size_t i = 0; i < parentIds.size(); ++i)
        {
            if (i > 0)
            {
                result += PARENT_DELIMITER;
            }

            result += parentIds[i];
        }

        return result;
    }

    static vector<string> parseParentIds(const string &raw)
    {
        vector<string> result;

        if (raw.empty())
        {
            return result;
        }

        stringstream ss(raw);
        string item;

        while (std::getline(ss, item, PARENT_DELIMITER))
        {
            if (!item.empty())
            {
                result.push_back(item);
            }
        }

        return result;
    }

    void load()
    {
        FileStorage::forEach(filePath, FIELD_COUNT, DELIMITER, [this](const auto &row)
        {
            CommitEntity commit;

            commit.repositoryId = static_cast<uint64_t>(std::stoull(row[REPOSITORY_ID_INDEX]));
            commit.id = row[ID_INDEX];
            commit.title = row[TITLE_INDEX];
            commit.parentCommitId = row[PARENT_IDS_INDEX];

            commitsById.add(commit.id, commit);

            indexRepositoryCommit(commit.repositoryId, commit.id);
        });
    }

    void persist(const CommitEntity &commit) const
    {
        FileStorage::upsert(filePath, FIELD_COUNT, DELIMITER,
        {
            std::to_string(commit.repositoryId),
            commit.id,
            commit.title,
            commit.parentCommitId
        },
        [&commit](const auto &row) -> bool
        {
            return commit.id == row[ID_INDEX];
        });
    }

public:
    CommitRepository(const string &filePath) : filePath(filePath)
    {
        load();
    }

    bool create(const CommitEntity &commit)
    {
        if (commitsById.contains(commit.id))
        {
            return false;
        }

        persist(commit);
        commitsById.add(commit.id, commit);
        indexRepositoryCommit(commit.repositoryId, commit.id);

        return true;
    }

    bool update(const CommitEntity &commit)
    {
        if (!commitsById.contains(commit.id))
        {
            return false;
        }

        persist(commit);
        commitsById.add(commit.id, commit);

        return true;
    }

    optional<CommitEntity> findById(const string &id) const
    {
        return commitsById.get(id);
    }

    vector<CommitEntity> findByRepositoryId(uint64_t repositoryId) const
    {
        vector<CommitEntity> result;
        auto commitIds = repoToCommitIds.get(repositoryId);

        if (!commitIds)
        {
            return result;
        }

        for (const string &commitId : *commitIds)
        {
            auto commit = commitsById.get(commitId);

            if (commit)
            {
                result.push_back(*commit);
            }
        }

        return result;
    }

    size_t count() const
    {
        return commitsById.count();
    }

    void visitAll(function<void(const CommitEntity &)> onEachVisit) const
    {
        for (const auto &entry : commitsById)
        {
            onEachVisit(entry.value);
        }
    }
};
