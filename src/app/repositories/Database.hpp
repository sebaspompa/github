#pragma once

#include "src/app/repositories/UserRepository.hpp"
#include "src/app/repositories/AccountRepository.hpp"
#include "src/app/repositories/RepositoryRepository.hpp"
#include "src/app/repositories/BranchRepository.hpp"
#include "src/app/repositories/CommitRepository.hpp"
#include "src/app/repositories/PullRequestRepository.hpp"
#include "src/app/repositories/IssueRepository.hpp"
#include "src/app/repositories/DiscussionRepository.hpp"
#include "src/app/repositories/NotificationRepository.hpp"

#include "src/app/repositories/paths.hpp"

struct Database
{
public:
    UserRepository userRepository { Paths::Storage::System::users };
    AccountRepository accountRepository { Paths::Storage::System::accounts };
    RepositoryRepository repositoryRepository { Paths::Storage::System::repositories };

    BranchRepository branchRepository { Paths::Storage::System::branches };
    CommitRepository commitRepository { Paths::Storage::System::commits };
    PullRequestRepository pullRequestRepository { Paths::Storage::System::pullRequests };
    IssueRepository issueRepository { Paths::Storage::System::issues };
    DiscussionRepository discussionRepository { Paths::Storage::System::discussions };
    NotificationRepository notificationRepository { Paths::Storage::System::notifications };

private:
    Database() = default;

    ~Database() = default;

public:
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

    static Database &instance()
    {
        static Database instance;
        return instance;
    }
};