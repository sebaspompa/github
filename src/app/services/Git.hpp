#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <filesystem>

#include "src/app/models/Commit.hpp"
#include "src/app/models/Branch.hpp"
#include "src/app/utils.hpp"

// Data Structures
#include "src/data_structures/Stack.hpp"
#include "src/data_structures/LinkedList.hpp"
#include "src/data_structures/DoublyLinkedList.hpp"

// *** SCHEMAS DB ***
// branches.txt
// REPOSITORY_ID - BRANCH_ID - NAME - COMMIT_ID
// commits.txt
// REPOSITORY_ID - COMMIT_ID - TITLE - DESCRIPTION - COMMIT_PARENT_ID
// pull_requests.txt
// REPOSITORY_ID - PR_ID - TITLE - DESCRIPTION - TARGET_BRANCH - SOURCE_BRANCH - PRIORITY - STATE
// issues.txt
// REPOSITORY_ID - ISSUE_ID - TITLE - DESCRIPTION - PRIORITY - STATE

using std::string;
using std::vector;

using std::runtime_error;
using std::ifstream;
using std::ofstream;
using std::getline;
using Log = vector<Commit>;

namespace fs = std::filesystem;

struct Git
{
private:
    string repositoryId;

    fs::path commitPath = fs::path(PROJECT_ROOT) / "storage" / "system" / "commits.txt";
    fs::path branchPath = fs::path(PROJECT_ROOT) / "storage" / "system" / "branches.txt";
    fs::path prPath = fs::path(PROJECT_ROOT) / "storage" / "system" / "pull_requests.txt";

    LinkedList<Branch> branches;
    string currentBranchId;

    DoublyLinkedList<Commit> commits;

    Branch currentBranch() const
    {
        Branch b;
        bool f = false;
        branches.forEach(
            [this, &b, &f](Branch branch)
            {
                if (branch.id == currentBranchId)
                {
                    b = branch;
                    f = true;
                }
            });

        if (f)
        {
            return b;
        }

        throw runtime_error("No se encontró la rama actual.");
    }

    Branch branchOf(const string &branchName) const
    {
        Branch b;
        bool f = false;
        branches.forEach(
            [&b, &f, &branchName](Branch branch)
            {
                if (branch.name == branchName)
                {
                    b = branch;
                    f = true;
                }
            });

        if (f)
        {
            return b;
        }

        throw runtime_error("La rama no existe.");
    }

    void loadCommits()
    {
        Branch _currentBranch;

        try
        {
            _currentBranch = currentBranch();
        }
        catch(const std::exception& e)
        {
            return;
        }

        ifstream file(commitPath);

        if (!file)
        {
            return;
        }

        string line;

        while (std::getline(file, line))
        {
            if (line.empty())
            {
                continue;
            }

            vector<string> data = split(line, '|');

            // Valida que el commit pertenezca al repositorio y que pertenezca a la rama actual.
            if (repositoryId != data[0])
            {
                continue;
            }

            Commit commit;

            commit.id = data[1];
            commit.title = data[2];
            commit.description = data[3];
            commit.parentId = data[4];

            commits.append(commit);
        }
    }

    void loadBranches()
    {
        ifstream file(branchPath);

        if (!file)
        {
            return;
        }

        string line;

        while (getline(file, line))
        {
            if (line.empty())
            {
                continue;
            }

            vector<string> data = split(line, '|');

            if (data.size() < 4)
            {
                continue;
            }

            if (data[0] != repositoryId)
            {
                continue;
            }

            Branch branch{
                data[1],
                data[2],
                data[3]};

            branches.append(branch);

            if (branch.name == "main")
            {
                currentBranchId = branch.id;
            }
        }

        file.close();
    }

    bool branchExists(const string &name) const
    {
        bool f = false;
        branches.forEach(
            [&name, &f](Branch branch)
            {
                if (branch.name == name)
                {
                    f = true;
                }
            }
        );

        return f;
    }

    void updateCurrentBranch(const string &commitId)
    {
        branches.map(
            [this, &commitId](Branch &branch)
            {
                if (branch.id == currentBranchId)
                {
                    branch.commitId = commitId;
                }
            });
    }

    void saveBranches()
    {
        ofstream file(branchPath);

        if (!file)
        {
            return;
        }

        branches.forEach(
            [this, &file](const Branch &branch)
            {
                file << repositoryId << '|'
                     << branch.id << '|'
                     << branch.name << '|'
                     << branch.commitId << '\n';
            }
        );

        file.close();
    }

    void createDefaultBranch()
    {
        if (!branches.isEmpty())
        {
            return;
        }

        ofstream file(branchPath, std::ios::app);

        if (!file)
        {
            return;
        }

        Branch defaultBranch =
        {
            "0",
            "main",
            ""
        };

        file << repositoryId << '|'
             << defaultBranch.id << '|'
             << defaultBranch.name << '|'
             << defaultBranch.commitId << '\n';

        file.close();

        currentBranchId = defaultBranch.id;

        branches.append(defaultBranch);
    }

public:
    Git(const string &repositoryId) : repositoryId(repositoryId)
    {
        loadBranches();
        loadCommits();
        createDefaultBranch();
    }

    bool commit(const string &title, const string &description)
    {
        UInt commitId = 0;
        string parentId = "";

        if (!commits.isEmpty())
        {
            Commit lastCommit = commits.lastValue().value();
            commitId = std::stoul(lastCommit.id) + 1;

            Branch currentBranch = this->currentBranch();

            parentId = currentBranch.commitId;
        }

        Commit newCommit{
            std::to_string(commitId),
            title,
            description,
            parentId};

        commits.append(newCommit);

        updateCurrentBranch(newCommit.id);
        saveBranches();

        ofstream file(commitPath, std::ios::app);

        if (!file)
        {
            return false;
        }

        file << repositoryId << '|'
             << newCommit.id << '|'
             << newCommit.title << '|'
             << newCommit.description << '|'
             << newCommit.parentId << '\n';

        file.close();

        return true;
    }

    // Crea una rama a partir de la rama actual.
    bool branch(const string &name)
    {   
        if (name.empty() || branches.isEmpty() || branchExists(name))
        {
            return false;
        }

        Branch current = currentBranch();

        UInt branchId = 0;

        if (!branches.isEmpty())
        {
            Branch last = branches.lastValue().value();
            branchId = std::stoul(last.id) + 1;
        }

        Branch newBranch{
            std::to_string(branchId),
            name,
            current.commitId};

        branches.append(newBranch);

        ofstream file(branchPath, std::ios::app);

        if (!file)
        {
            return false;
        }

        file << repositoryId << '|'
             << newBranch.id << '|'
             << newBranch.name << '|'
             << newBranch.commitId << '\n';

        file.close();

        return true;
    }

    // Cambia la rama actual a la rama especificada.
    bool switchBranch(const string &name)
    {
        Branch branch;

        try
        {
            branch = branchOf(name);
        }
        catch(const std::exception& e)
        {
            return false;
        }
        
        currentBranchId = branch.id;

        return true;
    }

    vector<Branch> getBranches()
    {
        vector<Branch> _branches;

        branches.forEach(
            [&_branches](Branch branch)
            {
                _branches.push_back(branch);
            }
        );

        return _branches;
    }

    Log log()
    {
        Log log;
        Branch currentBranch = this->currentBranch();
        string currentId = currentBranch.commitId;

        commits.forEachFromLast(
            [&log, &currentId](Commit &commit)
            {
                if (currentId == commit.id)
                {
                    log.push_back(commit);
                    currentId = commit.parentId;
                }
            });

        return log;
    }
};