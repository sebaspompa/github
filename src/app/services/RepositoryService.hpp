#pragma once
#include <vector>
#include <cstdlib>
#include <functional>
#include <string>
#include <algorithm>
#include <cctype>

// DB
#include <fstream>
#include <filesystem>
#include <string>
#include <stdexcept>
#include <iostream>
#include "src/app/utils.hpp"
#include "src/app/services/Git.hpp"
#include "src/app/services/RecordFile.hpp"
#include "src/app/AppContext.hpp"
#include "src/app/models/Repository.hpp"
#include "src/app/models/Entry.hpp"
#include "src/app/models/Issue.hpp"
#include "src/app/models/PullRequest.hpp"

using std::ifstream;
using std::ofstream;
using std::runtime_error;
using std::getline;
using std::stoi;
using std::function;

using UInt = unsigned int;
namespace fs = std::filesystem;

using std::vector;
using std::string;

struct RepositoryService
{
private:
    const string userId;
    const string repositoryId;
    const fs::path projectRootPath = fs::path(PROJECT_ROOT);
    const fs::path rootPath = projectRootPath / "storage" / "repositories" / userId / repositoryId;

    const fs::path rootPathInfo = projectRootPath / "storage" / "system" / "repositories.txt";
    const fs::path rootPathIssues = projectRootPath / "storage" / "system" / "issues.txt";
    const fs::path rootPathPullRequests = projectRootPath / "storage" / "system" / "pull_requests.txt";
    const char delimiter = '|';

    Git git;

    fs::path buildPath(const vector<string> &directoryNav) const
    {
        fs::path path = rootPath;
        for (const string &directoryName : directoryNav)
        {
            path /= directoryName;
        }

        return path;
    }

    string lastLineOf(const string &filePath) const
    {
        ifstream file(filePath);

        string line;
        string _lastLine;

        while (getline(file, line))
        {
            if (!line.empty())
            {
                _lastLine = line;
            }
        }

        file.close();
        return _lastLine;
    }

    static string toLower(string value)
    {
        std::transform(value.begin(), value.end(), value.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        return value;
    }

    static size_t toCount(const string &value)
    {
        try
        {
            const unsigned long count = std::stoul(value);

            return count > 1000000UL ? 1000000UL : static_cast<size_t>(count);
        }
        catch (const std::exception &)
        {
            return 0;
        }
    }

    // userId|repositoryId|titulo|descripcion|lenguaje|estrellas|forks|visibilidad
    static Repository repositoryFrom(const vector<string> &data)
    {
        Repository repository;

        repository.id = data[1];
        repository.name = data[2];
        repository.description = data[3];
        repository.language = Language{data[4]};
        repository.stars = vector<Star>(toCount(data[5]));
        repository.forks = vector<Fork>(toCount(data[6]));
        repository.visibility = data[7];

        return repository;
    }

public:
    RepositoryService(const string &userId, const string &repositoryId) : userId(userId), repositoryId(repositoryId), git(repositoryId) {}

    // Convierte "id;userId;fecha,id;userId;fecha" en un vector de Star o Fork
    template <typename T>
    static vector<T> parseItems(const string &field)
    {
        vector<T> items;

        if (field.empty())
        {
            return items;
        }

        for (const string &entry : split(field, ','))
        {
            vector<string> parts = split(entry, ';');

            if (parts.size() < 3)
            {
                continue;
            }

            items.push_back({parts[0], parts[1], parts[2]});
        }

        return items;
    }

    // CAMBIADO: ahora lee el formato
    // userId|repositoryId|titulo|descripcion|lenguaje|estrellas|forks|visibilidad
    static vector<Repository> repositoriesOf(const string &userId)
    {
        ifstream file(fs::path(PROJECT_ROOT) / "storage" / "system" / "repositories.txt");

        if (!file)
        {
            throw runtime_error("No se pudo abrir el archivo de los repositorios.");
        }

        vector<Repository> repositories;

        string line;
        while (getline(file, line))
        {
            while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
            {
                line.pop_back();
            }

            if (line.empty())
            {
                continue;
            }

            vector<string> data = split(line, '|');

            // Evita acceder fuera de rango si la linea esta incompleta
            if (data.size() < 8)
            {
                continue;
            }

            if (data[0] == userId)
            {
                repositories.push_back(repositoryFrom(data));
            }
        }

        return repositories;
    }

    static Repository create(const string &userId, const string &name, const string &description, const string &language, const string &visibility)
    {
        const vector<vector<string>> records = RecordFile::readIfExists("repositories.txt", 8, false);

        long nextId = 0;

        for (const vector<string> &record : records)
        {
            if (record[0] == userId && toLower(record[2]) == toLower(name))
            {
                throw runtime_error("Ya tienes un repositorio con ese nombre.");
            }

            try
            {
                const long id = std::stol(record[1]);

                if (id >= nextId)
                {
                    nextId = id + 1;
                }
            }
            catch (const std::exception &)
            {
            }
        }

        const string newId = std::to_string(nextId);

        // Carpeta fisica del repositorio: storage/repositories/<userId>/<repositoryId>
        fs::create_directories(fs::path(PROJECT_ROOT) / "storage" / "repositories" / userId / newId);

        const vector<string> record = {userId, newId, name, description, language, "0", "0", visibility};

        RecordFile::append("repositories.txt", record);

        return repositoryFrom(record);
    }

    vector<Entry> entries(const vector<string> &entryNames) const
    {
        const fs::path path = buildPath(entryNames);
        vector<Entry> entries;

        if (!fs::is_directory(path))
        {
            return entries;
        }

        // Solo observa directorios y archivos.
        for (const auto& _entry : fs::directory_iterator(path))
        {
            Entry::Type type;

            if (_entry.is_directory())
            {
                type = Entry::Type::Directory;
            }
            else if (_entry.is_regular_file())
            {
                type = Entry::Type::File;
            }
            else
            {
                continue;
            }
            
            entries.push_back(
                {
                    _entry.path().filename().string(),
                    type
                }
            );
        }

        return entries;
    }

    Entry::Type typeOf(const vector<string> &entryNames)
    {
        const fs::path path = buildPath(entryNames);

        if (fs::is_directory(path))
        {
            return Entry::Type::Directory;
        }
        else if (fs::is_regular_file(path))
        {
            return Entry::Type::File;
        }

        throw runtime_error("Tipo de entrada no identificado.");
    }

    void openFile(const vector<string> &entries)
    {
        const fs::path filePath = buildPath(entries);

        // Verifica que la entrada sea un archivo.
        if (!fs::is_regular_file(filePath))
        {
            throw runtime_error("La entrada no es un archivo.");
        }

        AppContext::instance().terminal().disableMouse();

#ifdef _WIN32
        const string command = "notepad " + filePath.string();
#else
        const string command = "nano " + filePath.string();
#endif

        std::system(command.c_str());
        AppContext::instance().terminal().enableMouse();
    }

    // CODE

    // VCS
    vector<Branch> branches()
    {
        return git.getBranches();
    }

    void changeBranchTo(const string &branchName)
    {
        git.switchBranch(branchName);
    }

    // Entries
    void createDirectory(const string &name, const vector<string> &directoryNav) const
    {
        fs::create_directory(buildPath(directoryNav) / name);
    }

    void createFile(const string &name, const vector<string> &directoryNav) const
    {
        const fs::path path = buildPath(directoryNav) / name;

        ofstream file(path.string(), std::ios::app);

        if (!file)
        {
            throw runtime_error("No se pudo crear el archivo: " + path.string());
        }
    }

    void createBranch(const string &name)
    {
        git.branch(name);
    }

    // REPOSITORY_ID - ISSUE_ID - TITLE - DESCRIPTION - PRIORITY - STATE
    void loadIssues(function<void(Issue)> closure)
    {
        ifstream file(rootPathIssues.string());

        if (!file)
        {
            throw runtime_error("No se pudo abrir el archivo de los issues.");
        }

        string line;
        while (getline(file, line))
        {
            vector<string> data = split(line, delimiter);

            if (data[0] == repositoryId)
            {
                Issue issue;
                issue.id = data[1];
                issue.title = data[2];
                issue.description = data[3];
                issue.priority = static_cast<Issue::Priority>(stoi(data[4]));
                issue.state = static_cast<Issue::State>(stoi(data[5]));

                closure(issue);
            }
        }
    }

    void createIssue(const string &title, const string &description, Issue::Priority priority)
    {
        const string lastLine = lastLineOf(rootPathIssues.string());
        const UInt issueId = stoul(split(lastLine, delimiter)[1]) + 1;

        // Issue issue;
        // issue.id = issueId;
        // issue.title = title;
        // issue.description = description;
        // issue.priority = priority;
        // issue.state = Issue::State::Open;

        ofstream file(rootPathIssues, std::ios::app);

        file << repositoryId << delimiter
        << issueId << delimiter
        << title << delimiter
        << description << delimiter
        << static_cast<int>(priority) << delimiter
        << static_cast<int>(Issue::State::Open) << '\n';

        file.close();

        // return issue;
    }

    // REPOSITORY_ID - PR_ID - TITLE - DESCRIPTION - TARGET_BRANCH - SOURCE_BRANCH - PRIORITY - STATE
    void loadPullRequests(function<void(PullRequest)> closure)
    {
        ifstream file(rootPathPullRequests);

        if (!file)
        {
            throw runtime_error("No se pudo abrir el archivo de los pull requests.");
        }

        string line;
        while (getline(file, line))
        {
            vector<string> data = split(line, delimiter);

            if (data[0] == repositoryId)
            {
                PullRequest pr;
                pr.id = data[1];
                pr.title = data[2];
                pr.description = data[3];
                pr.targetBranch = { "", data[4], ""};
                pr.sourceBranch = { "", data[5], ""};
                pr.priority = static_cast<PullRequest::Priority>(stoi(data[6]));
                pr.state = static_cast<PullRequest::State>(stoi(data[7]));

                closure(pr);
            }
        }
    }

    void createPullRequest(const string &title, const string &description, const string &sourceBranchName, const string &targetBranchName, PullRequest::Priority priority, PullRequest::State state)
    {
        const string lastLine = lastLineOf(rootPathPullRequests.string());
        const UInt prId = stoul(split(lastLine, delimiter)[1]) + 1;

        ofstream file(rootPathPullRequests, std::ios::app);

        file << repositoryId << delimiter
             << prId << delimiter
             << title << delimiter
             << description << delimiter
             << sourceBranchName << delimiter
             << targetBranchName << delimiter
             << static_cast<int>(priority) << delimiter
             << static_cast<int>(state) << '\n';

        file.close();
    }

    Log getLog()
    {
        return git.log();
    }

    void createCommit(const string &title, const string &description)
    {
        git.commit(title, description);
    }
};