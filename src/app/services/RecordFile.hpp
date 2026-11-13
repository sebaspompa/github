#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#include "src/app/utils.hpp"

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::runtime_error;
using std::getline;

namespace fs = std::filesystem;

// Lectura y escritura de archivos de registros separados por '|'
// en storage/system. Maneja \r (Windows) y BOM.
struct RecordFile
{
private:
    // Contador de cambios: las vistas lo usan para saber si deben recargar datos
    static unsigned long& counter()
    {
        static unsigned long value = 0;
        return value;
    }

    static void cleanLine(string& line, const bool first)
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

    static bool endsWithNewline(const fs::path& target)
    {
        ifstream file(target, std::ios::binary | std::ios::ate);

        if (!file || file.tellg() == 0)
        {
            return true;
        }

        file.seekg(-1, std::ios::end);

        char last = '\n';
        file.get(last);

        return last == '\n';
    }

    static string join(const vector<string>& record)
    {
        string line;

        for (size_t i = 0; i < record.size(); i++)
        {
            if (i > 0)
            {
                line += "|";
            }

            line += record[i];
        }

        return line;
    }

public:
    static fs::path filePath(const string& fileName)
    {
        return fs::path(PROJECT_ROOT) / "storage" / "system" / fileName;
    }

    static unsigned long revision()
    {
        return counter();
    }

    // strict = true: una linea con columnas incorrectas lanza error
    // strict = false: esa linea se ignora
    static vector<vector<string>> read(const string& fileName, const size_t fields, const bool strict = true)
    {
        const fs::path target = filePath(fileName);

        ifstream file(target);

        if (!file)
        {
            throw runtime_error("No se pudo abrir: " + target.string());
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

            vector<string> data = split(line, '|');

            if (data.size() != fields)
            {
                if (strict)
                {
                    throw runtime_error("Registro en `" + fileName + "` no cuenta con la cantidad de propiedades necesarias.");
                }

                continue;
            }

            records.push_back(data);
        }

        return records;
    }

    // Igual que read, pero si el archivo no existe devuelve una lista vacia
    static vector<vector<string>> readIfExists(const string& fileName, const size_t fields, const bool strict = true)
    {
        if (!fs::exists(filePath(fileName)))
        {
            return {};
        }

        return read(fileName, fields, strict);
    }

    // Reescribe el archivo completo (via archivo temporal)
    static void write(const string& fileName, const vector<vector<string>>& records)
    {
        const fs::path target = filePath(fileName);

        fs::create_directories(target.parent_path());

        fs::path temp = target;
        temp += ".tmp";

        {
            ofstream out(temp, std::ios::trunc);

            if (!out)
            {
                throw runtime_error("No se pudo escribir en: " + temp.string());
            }

            for (const vector<string>& record : records)
            {
                out << join(record) << "\n";
            }

            if (!out)
            {
                throw runtime_error("Error al escribir en: " + temp.string());
            }
        }

        fs::rename(temp, target);

        counter()++;
    }

    static void append(const string& fileName, const vector<string>& record)
    {
        const fs::path target = filePath(fileName);

        fs::create_directories(target.parent_path());

        const bool needsNewline = fs::exists(target) && !endsWithNewline(target);

        ofstream file(target, std::ios::app);

        if (!file)
        {
            throw runtime_error("No se pudo escribir en: " + target.string());
        }

        if (needsNewline)
        {
            file << "\n";
        }

        file << join(record) << "\n";

        counter()++;
    }
};