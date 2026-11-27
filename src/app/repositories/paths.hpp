#pragma once

#include <filesystem>

namespace fs = std::filesystem;

namespace Paths
{
    // `inline` permite definir la misma variable en múltiples unidades de traducción `.cpp`, manteniendo una única entidad en el programa

    inline const fs::path root = PROJECT_ROOT;
    
    namespace Storage
    {
        inline const fs::path path = Paths::root / "storage";
        inline const fs::path repositories = path / "repositories";
        
        namespace System
        {
            inline const fs::path path = Storage::path / "system";

            inline const fs::path accounts = path / "accounts.txt";
            inline const fs::path branches = path / "branches.txt";
            inline const fs::path commits = path / "commits.txt";
            inline const fs::path issues = path / "issues.txt";
            inline const fs::path pullRequests = path / "pull_requests.txt";
            inline const fs::path repositories = path / "repositories.txt";
            inline const fs::path users = path / "users.txt";
            inline const fs::path discussions = path / "discussions.txt";
            inline const fs::path notifications = path / "notifications.txt";
        }
    }
}