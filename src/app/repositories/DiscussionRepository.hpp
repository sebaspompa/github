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

struct Date
{
    int day;
    int month;
    int year;
};

struct DiscussionEntity
{
    uint64_t repositoryId;
    uint64_t id;
    string content;
    uint64_t authorId;
    Date publishDate;
};

// schema: REPOSITORY_ID - ID - CONTENT - USER_ID - DD - MM - YYYY
struct DiscussionRepository
{
private:
    // Config
    static constexpr char DELIMITER = '|';
    static constexpr int FIELD_COUNT = 7;

    static constexpr int REPOSITORY_ID_INDEX = 0;
    static constexpr int ID_INDEX = 1;
    static constexpr int CONTENT_INDEX = 2;
    static constexpr int AUTHOR_ID_INDEX = 3;
    static constexpr int DAY_INDEX = 4;
    static constexpr int MONTH_INDEX = 5;
    static constexpr int YEAR_INDEX = 6;

    const string filePath;

    // DB principal indexado por id de la discusion
    Dictionary<uint64_t, DiscussionEntity> discussionsById;

    // Mapea: repositoryId -> [discussionId]
    Dictionary<uint64_t, vector<uint64_t>> repoToDiscussionIds;

private:
    void indexRepositoryDiscussion(uint64_t repositoryId, uint64_t discussionId)
    {
        if (repoToDiscussionIds.contains(repositoryId))
        {
            repoToDiscussionIds[repositoryId].push_back(discussionId);
        }
        else
        {
            repoToDiscussionIds.add(repositoryId, { discussionId });
        }
    }

    void load()
    {
        FileStorage::forEach(filePath, FIELD_COUNT, DELIMITER, [this](const auto &row)
                             {
            DiscussionEntity discussion;

            discussion.repositoryId = static_cast<uint64_t>(std::stoull(row[REPOSITORY_ID_INDEX]));
            discussion.id = static_cast<uint64_t>(std::stoull(row[ID_INDEX]));
            discussion.content = row[CONTENT_INDEX];
            discussion.authorId = static_cast<uint64_t>(std::stoull(row[AUTHOR_ID_INDEX]));
            discussion.publishDate.day = std::stoi(row[DAY_INDEX]);
            discussion.publishDate.month = std::stoi(row[MONTH_INDEX]);
            discussion.publishDate.year = std::stoi(row[YEAR_INDEX]);

            discussionsById.add(discussion.id, discussion);

            indexRepositoryDiscussion(discussion.repositoryId, discussion.id); });
    }

    void persist(const DiscussionEntity &discussion) const
    {
        const string id = std::to_string(discussion.id);

        FileStorage::upsert(filePath, FIELD_COUNT, DELIMITER,
                            {std::to_string(discussion.repositoryId),
                             id,
                             discussion.content,
                             std::to_string(discussion.authorId),
                             std::to_string(discussion.publishDate.day),
                             std::to_string(discussion.publishDate.month),
                             std::to_string(discussion.publishDate.year)},
                            [&id](const auto &row) -> bool
                            {
                                return id == row[ID_INDEX];
                            });
    }

public:
    DiscussionRepository(const string &filePath) : filePath(filePath)
    {
        load();
    }

    bool create(const DiscussionEntity &discussion)
    {
        if (discussionsById.contains(discussion.id))
        {
            return false;
        }

        persist(discussion);
        discussionsById.add(discussion.id, discussion);
        indexRepositoryDiscussion(discussion.repositoryId, discussion.id);

        return true;
    }

    bool update(const DiscussionEntity &discussion)
    {
        if (!discussionsById.contains(discussion.id))
        {
            return false;
        }

        persist(discussion);
        discussionsById.add(discussion.id, discussion);

        return true;
    }

    optional<DiscussionEntity> findById(uint64_t id) const
    {
        return discussionsById.get(id);
    }

    vector<DiscussionEntity> findByRepositoryId(uint64_t repositoryId) const
    {
        vector<DiscussionEntity> result;
        auto discussionIds = repoToDiscussionIds.get(repositoryId);

        if (!discussionIds)
        {
            return result;
        }

        for (uint64_t discussionId : *discussionIds)
        {
            auto discussion = discussionsById.get(discussionId);

            if (discussion)
            {
                result.push_back(*discussion);
            }
        }

        return result;
    }

    size_t count() const
    {
        return discussionsById.count();
    }

    void visitAll(function<void(const DiscussionEntity &)> onEachVisit) const
    {
        for (const auto &entry : discussionsById)
        {
            onEachVisit(entry.value);
        }
    }
};
