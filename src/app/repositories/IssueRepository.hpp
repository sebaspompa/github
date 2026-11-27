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

struct IssueEntity
{
    uint64_t repositoryId;
    uint64_t id;
    string title;
    string description;
    int priority;
};

// schema: REPOSITORY_ID - ISSUE_ID - TITLE - DESCRIPTION - PRIORITY
struct IssueRepository
{
private:
    // Config
    static constexpr char DELIMITER = '|';
    static constexpr int FIELD_COUNT = 5;

    static constexpr int REPOSITORY_ID_INDEX = 0;
    static constexpr int ID_INDEX = 1;
    static constexpr int TITLE_INDEX = 2;
    static constexpr int DESCRIPTION_INDEX = 3;
    static constexpr int PRIORITY_INDEX = 4;

    const string filePath;

    // DB principal indexado por id del issue
    Dictionary<uint64_t, IssueEntity> issuesById;

    // Mapea: repositoryId -> [issueId]
    Dictionary<uint64_t, vector<uint64_t>> repoToIssueIds;

private:
    void indexRepositoryIssue(uint64_t repositoryId, uint64_t issueId)
    {
        if (repoToIssueIds.contains(repositoryId))
        {
            repoToIssueIds[repositoryId].push_back(issueId);
        }
        else
        {
            repoToIssueIds.add(repositoryId, { issueId });
        }
    }

    void load()
    {
        FileStorage::forEach(filePath, FIELD_COUNT, DELIMITER, [this](const auto &row)
        {
            IssueEntity issue;

            issue.repositoryId = static_cast<uint64_t>(std::stoull(row[REPOSITORY_ID_INDEX]));
            issue.id = static_cast<uint64_t>(std::stoull(row[ID_INDEX]));
            issue.title = row[TITLE_INDEX];
            issue.description = row[DESCRIPTION_INDEX];
            issue.priority = std::stoi(row[PRIORITY_INDEX]);

            issuesById.add(issue.id, issue);

            indexRepositoryIssue(issue.repositoryId, issue.id);
        });
    }

    void persist(const IssueEntity &issue) const
    {
        const string id = std::to_string(issue.id);

        FileStorage::upsert(filePath, FIELD_COUNT, DELIMITER,
        {
            std::to_string(issue.repositoryId),
            id,
            issue.title,
            issue.description,
            std::to_string(issue.priority)
        },
        [&id](const auto &row) -> bool
        {
            return id == row[ID_INDEX];
        });
    }

public:
    IssueRepository(const string &filePath) : filePath(filePath)
    {
        load();
    }

    bool create(const IssueEntity &issue)
    {
        if (issuesById.contains(issue.id))
        {
            return false;
        }

        persist(issue);
        issuesById.add(issue.id, issue);
        indexRepositoryIssue(issue.repositoryId, issue.id);

        return true;
    }

    bool update(const IssueEntity &issue)
    {
        if (!issuesById.contains(issue.id))
        {
            return false;
        }

        persist(issue);
        issuesById.add(issue.id, issue);

        return true;
    }

    optional<IssueEntity> findById(uint64_t id) const
    {
        return issuesById.get(id);
    }

    vector<IssueEntity> findByRepositoryId(uint64_t repositoryId) const
    {
        vector<IssueEntity> result;
        auto issueIds = repoToIssueIds.get(repositoryId);

        if (!issueIds)
        {
            return result;
        }

        for (uint64_t issueId : *issueIds)
        {
            auto issue = issuesById.get(issueId);

            if (issue)
            {
                result.push_back(*issue);
            }
        }

        return result;
    }

    size_t count() const
    {
        return issuesById.count();
    }

    void visitAll(function<void(const IssueEntity &)> onEachVisit) const
    {
        for (const auto &entry : issuesById)
        {
            onEachVisit(entry.value);
        }
    }
};
