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

struct BranchEntity
{
    uint64_t repositoryId;
    uint64_t id;
    string name;
    string commitId;
};

// schema: REPOSITORY_ID - BRANCH_ID - NAME - COMMIT_ID
struct BranchRepository
{
private:
    // Config
    static constexpr char DELIMITER = '|';
    static constexpr int FIELD_COUNT = 4;

    static constexpr int REPOSITORY_ID_INDEX = 0;
    static constexpr int ID_INDEX = 1;
    static constexpr int NAME_INDEX = 2;
    static constexpr int COMMIT_ID_INDEX = 3;

    const string filePath;

    // DB principal indexado por id de la rama
    Dictionary<uint64_t, BranchEntity> branchesById;

    // Mapea: repositoryId -> [branchId]
    Dictionary<uint64_t, vector<uint64_t>> repoToBranchIds;

private:
    void indexRepositoryBranch(uint64_t repositoryId, uint64_t branchId)
    {
        if (repoToBranchIds.contains(repositoryId))
        {
            repoToBranchIds[repositoryId].push_back(branchId);
        }
        else
        {
            repoToBranchIds.add(repositoryId, {branchId});
        }
    }

    void load()
    {
        FileStorage::forEach(filePath, FIELD_COUNT, DELIMITER, [this](const auto &row)
        {
            BranchEntity branch;

            branch.repositoryId = static_cast<uint64_t>(std::stoull(row[REPOSITORY_ID_INDEX]));
            branch.id = static_cast<uint64_t>(std::stoull(row[ID_INDEX]));
            branch.name = row[NAME_INDEX];
            branch.commitId = row[COMMIT_ID_INDEX];

            branchesById.add(branch.id, branch);

            indexRepositoryBranch(branch.repositoryId, branch.id);
        });
    }

    void persist(const BranchEntity &branch) const
    {
        const string id = std::to_string(branch.id);

        FileStorage::upsert(filePath, FIELD_COUNT, DELIMITER,
        {
            std::to_string(branch.repositoryId),
            id,
            branch.name,
            branch.commitId
        },
        [&id](const auto &row) -> bool
        {
            return id == row[ID_INDEX];
        });
    }

public:
    BranchRepository(const string &filePath) : filePath(filePath)
    {
        load();
    }

    bool create(const BranchEntity &branch)
    {
        if (branchesById.contains(branch.id))
        {
            return false;
        }

        persist(branch);
        branchesById.add(branch.id, branch);
        indexRepositoryBranch(branch.repositoryId, branch.id);

        return true;
    }

    bool update(const BranchEntity &branch)
    {
        if (!branchesById.contains(branch.id))
        {
            return false;
        }

        persist(branch);
        branchesById.add(branch.id, branch);

        return true;
    }

    optional<BranchEntity> findById(uint64_t id) const
    {
        return branchesById.get(id);
    }

    vector<BranchEntity> findByRepositoryId(uint64_t repositoryId) const
    {
        vector<BranchEntity> result;
        auto branchIds = repoToBranchIds.get(repositoryId);

        if (!branchIds)
        {
            return result;
        }

        for (uint64_t branchId : *branchIds)
        {
            auto branch = branchesById.get(branchId);

            if (branch)
            {
                result.push_back(*branch);
            }
        }

        return result;
    }

    size_t count() const
    {
        return branchesById.count();
    }

    void visitAll(function<void(const BranchEntity &)> onEachVisit) const
    {
        for (const auto &entry : branchesById)
        {
            onEachVisit(entry.value);
        }
    }
};
