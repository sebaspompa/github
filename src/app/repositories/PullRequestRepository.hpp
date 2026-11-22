#pragma once

#include <string>
#include <optional>
#include <vector>
#include <functional>
#include <cstdint>

#include "src/app/repositories/FileStorage.hpp"
#include "src/data_structures/Dictionary.hpp"

using std::function;
using std::optional;
using std::string;
using std::vector;

struct PullRequestEntity
{
    uint64_t repositoryId;
    uint64_t id;
    string title;
    string description;
    uint64_t targetBranchId;
    uint64_t sourceBranchId;
    string mergedCommitId;
    int priority;
};

// schema: REPOSITORY_ID - PR_ID - TITLE - DESCRIPTION - TARGET_BRANCH_ID - SOURCE_BRANCH_ID - MERGED_COMMIT_ID - PRIORITY
struct PullRequestRepository
{
private:
    // Config
    static constexpr char DELIMITER = '|';
    static constexpr int FIELD_COUNT = 8;

    static constexpr int REPOSITORY_ID_INDEX = 0;
    static constexpr int ID_INDEX = 1;
    static constexpr int TITLE_INDEX = 2;
    static constexpr int DESCRIPTION_INDEX = 3;
    static constexpr int TARGET_BRANCH_ID_INDEX = 4;
    static constexpr int SOURCE_BRANCH_ID_INDEX = 5;
    static constexpr int MERGED_COMMIT_ID_INDEX = 6;
    static constexpr int PRIORITY_INDEX = 7;

    const string filePath;

    // DB principal indexado por id del pull request
    Dictionary<uint64_t, PullRequestEntity> pullRequestsById;

    // Mapea: repositoryId -> [pullRequestId]
    Dictionary<uint64_t, vector<uint64_t>> repoToPullRequestIds;

private:
    void indexRepositoryPullRequest(uint64_t repositoryId, uint64_t pullRequestId)
    {
        if (repoToPullRequestIds.contains(repositoryId))
        {
            repoToPullRequestIds[repositoryId].push_back(pullRequestId);
        }
        else
        {
            repoToPullRequestIds.add(repositoryId, { pullRequestId });
        }
    }

    void load()
    {
        FileStorage::forEach(filePath, FIELD_COUNT, DELIMITER, [this](const auto &row)
        {
            PullRequestEntity pr;

            pr.repositoryId = static_cast<uint64_t>(std::stoull(row[REPOSITORY_ID_INDEX]));
            pr.id = static_cast<uint64_t>(std::stoull(row[ID_INDEX]));
            pr.title = row[TITLE_INDEX];
            pr.description = row[DESCRIPTION_INDEX];
            pr.targetBranchId = static_cast<uint64_t>(std::stoull(row[TARGET_BRANCH_ID_INDEX]));
            pr.sourceBranchId = static_cast<uint64_t>(std::stoull(row[SOURCE_BRANCH_ID_INDEX]));
            pr.mergedCommitId = row[MERGED_COMMIT_ID_INDEX];
            pr.priority = std::stoi(row[PRIORITY_INDEX]);

            pullRequestsById.add(pr.id, pr);

            indexRepositoryPullRequest(pr.repositoryId, pr.id);
        });
    }

    void persist(const PullRequestEntity &pr) const
    {
        const string id = std::to_string(pr.id);

        FileStorage::upsert(filePath, FIELD_COUNT, DELIMITER,
        {
            std::to_string(pr.repositoryId),
            id,
            pr.title,
            pr.description,
            std::to_string(pr.targetBranchId),
            std::to_string(pr.sourceBranchId),
            pr.mergedCommitId,
            std::to_string(pr.priority)
        },
        [&id](const auto &row) -> bool
        {
            return id == row[ID_INDEX];
        });
    }

public:
    PullRequestRepository(const string &filePath) : filePath(filePath)
    {
        load();
    }

    bool create(const PullRequestEntity &pr)
    {
        if (pullRequestsById.contains(pr.id))
        {
            return false;
        }

        persist(pr);
        pullRequestsById.add(pr.id, pr);
        indexRepositoryPullRequest(pr.repositoryId, pr.id);

        return true;
    }

    bool update(const PullRequestEntity &pr)
    {
        if (!pullRequestsById.contains(pr.id))
        {
            return false;
        }

        persist(pr);
        pullRequestsById.add(pr.id, pr);

        return true;
    }

    optional<PullRequestEntity> findById(uint64_t id) const
    {
        return pullRequestsById.get(id);
    }

    vector<PullRequestEntity> findByRepositoryId(uint64_t repositoryId) const
    {
        vector<PullRequestEntity> result;
        auto pullRequestIds = repoToPullRequestIds.get(repositoryId);

        if (!pullRequestIds)
        {
            return result;
        }

        for (uint64_t pullRequestId : *pullRequestIds)
        {
            auto pr = pullRequestsById.get(pullRequestId);

            if (pr)
            {
                result.push_back(*pr);
            }
        }

        return result;
    }

    size_t count() const
    {
        return pullRequestsById.count();
    }

    void visitAll(function<void(const PullRequestEntity &)> onEachVisit) const
    {
        for (const auto &entry : pullRequestsById)
        {
            onEachVisit(entry.value);
        }
    }
};
