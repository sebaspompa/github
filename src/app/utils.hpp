#pragma once
#include <string>
#include <ranges>
#include <vector>
#include <stdexcept>

#include <filesystem>
#include <fstream>

using std::string;
using std::vector;

using std::ifstream;
using std::ofstream;

namespace fs = std::filesystem;

vector<string> split(const string &text, char delimiter)
{
    vector<string> splitted;

    for (const auto &subcadena : text | std::views::split(delimiter))
    {
        splitted.emplace_back(subcadena.begin(), subcadena.end());
    }

    return splitted;
}

string toLower(string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    return value;
}

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
        }
    }
}

namespace DB
{
    // Quita el BOM (primera linea) y los \r / \n al final (archivos de Windows)
    void cleanLine(string &line, const bool first)
    {
        if (first && line.size() >= 3 &&
            (unsigned char)line[0] == 0xEF &&
            (unsigned char)line[1] == 0xBB &&
            (unsigned char)line[2] == 0xBF)
        {
            line.erase(0, 3);
        }

        while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
        {
            line.pop_back();
        }
    }

    string trim(const string &value)
    {
        const size_t start = value.find_first_not_of(" \t");

        if (start == string::npos)
        {
            return "";
        }

        const size_t end = value.find_last_not_of(" \t");

        return value.substr(start, end - start + 1);
    }

    bool endsWithNewline(const fs::path &path)
    {
        ifstream file(path, std::ios::binary | std::ios::ate);

        if (!file || file.tellg() == 0)
        {
            return true;
        }

        file.seekg(-1, std::ios::end);

        char last = '\n';
        file.get(last);

        return last == '\n';
    }

    void appendLine(const string &filePath, const string &line)
    {
        const fs::path path = filePath;

        fs::create_directories(path.parent_path());

        const bool needsNewline = fs::exists(path) && !endsWithNewline(path);

        ofstream file(path, std::ios::app);

        if (!file)
        {
            throw runtime_error("No se pudo escribir en: " + path.string());
        }

        if (needsNewline)
        {
            file << "\n";
        }

        file << line << "\n";
    }

    bool hasForbiddenChars(const string &value)
    {
        return value.find_first_of("|\r\n") != string::npos;
    }

    // Lee un archivo de registros
    vector<vector<string>> read(const string &filePath, const size_t fields, char delimiter, bool restricted = false)
    {
        ifstream file(filePath);

        if (!file)
        {
            throw runtime_error("Error al abrir: " + filePath);
        }

        vector<vector<string>> records;

        string line;
        bool first = true;

        while (getline(file, line))
        {
            cleanLine(line, first);
            first = false;

            if (line.empty())
            {
                continue;
            }

            vector<string> data = split(line, delimiter);

            if (data.size() != fields)
            {
                if (restricted)
                {
                    throw runtime_error("Registro en `" + filePath + "` no cuenta con la cantidad de propiedades necesarias.");
                }
                
                continue;
            }

            records.push_back(data);
        }

        return records;
    }
}