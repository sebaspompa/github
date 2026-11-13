#pragma once
#include <string>
#include <vector>

#include "src/app/services/SessionService.hpp"
#include "src/app/services/RepositoryService.hpp"
#include "src/data_structures/LinkedList.hpp"
#include "src/data_structures/Queue.hpp"

using std::vector;
using std::string;


struct IssueQuery
{
    bool filterPriority = false;
    Issue::Priority priority = Issue::Priority::High;

    bool filterState = false;
    Issue::State state = Issue::State::Open;

    bool sortByPriority = false;
    bool ascending = false;
};

struct PullRequestQuery
{
    bool filterPriority = false;
    PullRequest::Priority priority = PullRequest::Priority::High;

    bool filterState = false;
    PullRequest::State state = PullRequest::State::Open;

    bool sortByPriority = false;
    bool ascending = false;
};

struct RepositoryViewModel
{
private:
    string userId;
    string repositoryId;
    RepositoryService repositoryService;
    vector<string> pathNavigation;

    LinkedList<Issue> issues;
    Queue<PullRequest> pullRequests;

public:
    RepositoryViewModel(const string &repositoryId) : userId( SessionService::instance().session() ), repositoryId(repositoryId), repositoryService(userId, repositoryId)
    {
        loadIssues();
    }

    vector<Entry> getEntries() const
    {
        vector<Entry> entries = repositoryService.entries(pathNavigation);

        if (pathNavigation.size() > 0)
        {
            entries.push_back(
                {
                    "..",
                    Entry::Type::Directory
                }
            );
        }
        
        return entries;
    }

    void open(const string &entryName)
    {
        if (entryName == "..")
        {
            pathNavigation.pop_back();
            return;
        }

        vector<string> fullPath = pathNavigation;
        fullPath.push_back(entryName);

        Entry::Type type = repositoryService.typeOf(fullPath);

        switch (type)
        {
        case Entry::Type::Directory:

            pathNavigation.push_back(entryName);
            break;

        case Entry::Type::File:
            repositoryService.openFile(fullPath);
            break;
        }
    }

    vector<Branch> getBranches()
    {
        return repositoryService.branches();
    }

    void switchBranch(const string &branchName)
    {
        repositoryService.changeBranchTo(branchName);
    }

    void createDirectory(const string &name) const
    {
        repositoryService.createDirectory(name, pathNavigation);
    }

    void createFile(const string &name) const
    {
        repositoryService.createFile(name, pathNavigation);
    }

    void createBranch(const string &name)
    {
        repositoryService.createBranch(name);
    }

    void loadIssues()
    {
        repositoryService.loadIssues(
            [this](const Issue &issue)
            {
                issues.append(issue);
            }
        );
    }

    void loadPRs()
    {
        repositoryService.loadPullRequests(
            [this](const PullRequest &pr)
            {
                pullRequests.enqueue(pr);
            }
        );
    }

    vector<Issue> getIssues(const IssueQuery &query)
    {
        vector<Issue> result;
        LinkedList<Issue> _result;

        repositoryService.loadIssues(
            [&_result](const Issue &issue)
            {
                _result.append(issue);
            }
        );

        // Filtro
        if (query.filterPriority)
        {
            _result = _result.filter(
                [&query](const Issue &issue)
                {
                    if (issue.priority == query.priority)
                    {
                        return true;
                    }
                    return false;
                }
            );
        }
        
        if (query.filterState)
        {
            _result = _result.filter(
                [&query](const Issue &issue)
                {
                    if (issue.state == query.state)
                    {
                        return true;
                    }
                    return false;
                }
            );
        }
        
        // Ordenamiento
        if (query.sortByPriority)
        {
            if (query.ascending)
            {
                _result.sort(
                    [](Issue a, Issue b)
                    {
                        return static_cast<int>(a.priority) > static_cast<int>(b.priority);
                    }
                );
            }
            else
            {
                _result.sort(
                    [](Issue a, Issue b)
                    {
                        return static_cast<int>(a.priority) < static_cast<int>(b.priority);
                    }
                );
            }
        }

        _result.forEach(
            [&result](Issue issue)
            {
                result.push_back(issue);
            }
        );

        return result;
    }

    void createIssue(const string &title, const string &description, Issue::Priority priority)
    {
        repositoryService.createIssue(title, description, priority);
    }

    vector<PullRequest> getPullRequests(const PullRequestQuery &query)
    {
        vector<PullRequest> result;
        LinkedList<PullRequest> _result;

        repositoryService.loadPullRequests(
            [&_result](const PullRequest &issue)
            {
                _result.append(issue);
            });

        // Filtro
        if (query.filterPriority)
        {
            _result = _result.filter(
                [&query](const PullRequest &issue)
                {
                    if (issue.priority == query.priority)
                    {
                        return true;
                    }
                    return false;
                });
        }

        if (query.filterState)
        {
            _result = _result.filter(
                [&query](const PullRequest &issue)
                {
                    if (issue.state == query.state)
                    {
                        return true;
                    }
                    return false;
                });
        }

        // Ordenamiento
        if (query.sortByPriority)
        {
            if (query.ascending)
            {
                _result.sort(
                    [](PullRequest a, PullRequest b)
                    {
                        return static_cast<int>(a.priority) > static_cast<int>(b.priority);
                    });
            }
            else
            {
                _result.sort(
                    [](PullRequest a, PullRequest b)
                    {
                        return static_cast<int>(a.priority) < static_cast<int>(b.priority);
                    });
            }
        }

        _result.forEach(
            [&result](PullRequest issue)
            {
                result.push_back(issue);
            });

        return result;
    }

    Log getLog()
    {
        return repositoryService.getLog();
    }
    
    void createCommit(const string &title, const string &description)
    {
        repositoryService.createCommit(title, description);
    }

    void createPullRequest(const string &title, const string &description, const string &sourceBranchName, const string &targetBranchName, PullRequest::Priority priority)
    {
        repositoryService.createPullRequest(title, description, sourceBranchName, targetBranchName, priority, PullRequest::State::Open);
    }
};