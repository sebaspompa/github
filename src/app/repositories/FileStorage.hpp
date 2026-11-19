#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <functional>
#include <optional>
#include <vector>

// error
#include <stdexcept>

// split
#include <ranges>

using std::function;
using std::getline;
using std::ifstream;
using std::nullopt;
using std::ofstream;
using std::optional;
using std::runtime_error;
using std::string;
using std::vector;

namespace fs = std::filesystem;

struct FileStorage
{
private:
    static vector<string> split(string text, char delimiter)
    {
        vector<string> splitted;

        for (const auto &subcadena : text | std::views::split(delimiter))
        {
            splitted.emplace_back(subcadena.begin(), subcadena.end());
        }

        return splitted;
    }

    static string join(const vector<string> &data, char delimiter)
    {
        string result;

        for (size_t i = 0; i < data.size(); i++)
        {
            if (i > 0)
            {
                result += delimiter;
            }

            result += data[i];
        }

        return result;
    }

public:
    static void create(const string &filePath)
    {
        const fs::path path(filePath);

        fs::create_directories(path.parent_path());

        ofstream file(path);

        if (!file)
        {
            throw runtime_error("No se pudo crear el archivo en la ruta: " + filePath);
        }

        file.close();
    }

    static void forEach(const string &filePath, const size_t fields, char delimiter, function<void(const vector<string> &)> onRowRead)
    {
        ifstream file(filePath);

        if (!file)
        {
            throw runtime_error("Error al abrir el archivo: " + filePath);
        }

        string line;

        while (getline(file, line))
        {
            if (line.empty())
            {
                continue;
            }

            if (line.back() == '\r')
            {
                line.pop_back();
            }

            vector<string> data = split(line, delimiter);

            if (data.size() != fields)
            {
                throw runtime_error("Inconsistencia en la cantidad de propiedades del archivo: " + filePath + ". Cantidad requerida: " + std::to_string(fields) + ". Cantidad encontrada: " + std::to_string(data.size()));
            }

            onRowRead(data);
        }

        file.close();
    }

    // Retorna el primer registro que cumpla con una condición
    static optional<vector<string>> find(const string &filePath, const size_t fields, char delimiter, function<bool(const vector<string> &)> predicate)
    {
        ifstream file(filePath);

        if (!file)
        {
            throw runtime_error("Error al abrir el archivo: " + filePath);
        }

        string line;

        while (getline(file, line))
        {
            if (line.empty())
            {
                continue;
            }

            if (line.back() == '\r')
            {
                line.pop_back();
            }

            vector<string> data = split(line, delimiter);

            if (data.size() != fields)
            {
                throw runtime_error("Inconsistencia en la cantidad de propiedades del archivo: " + filePath + ". Cantidad requerida: " + std::to_string(fields) + ". Cantidad encontrada: " + std::to_string(data.size()));
            }

            if (predicate(data))
            {
                return data;
            }
        }

        file.close();

        return nullopt;
    }

    // Retorna los registros que cumplan con una condición
    static vector<vector<string>> findAll(const string &filePath, const size_t fields, char delimiter, function<bool(const vector<string> &)> predicate)
    {
        ifstream file(filePath);

        if (!file)
        {
            throw runtime_error("Error al abrir el archivo: " + filePath);
        }

        vector<vector<string>> records;

        string line;

        while (getline(file, line))
        {
            if (line.empty())
            {
                continue;
            }

            if (line.back() == '\r')
            {
                line.pop_back();
            }

            vector<string> data = split(line, delimiter);

            if (data.size() != fields)
            {
                throw runtime_error("Inconsistencia en la cantidad de propiedades del archivo: " + filePath + ". Cantidad requerida: " + std::to_string(fields) + ". Cantidad encontrada: " + std::to_string(data.size()));
            }

            if (predicate(data))
            {
                records.push_back(data);
            }
        }

        file.close();

        return records;
    }

    // Reescribe el archivo aplicando una transformación a cada fila
    static void rewrite(const string &filePath, const size_t fields, char delimiter, function<optional<vector<string>>(const vector<string> &)> transform)
    {
        fs::path originalPath(filePath);
        fs::path tempPath = originalPath;

        tempPath += ".tmp";

        ifstream input(originalPath);

        if (!input)
        {
            throw runtime_error("Error al abrir el archivo: " + filePath);
        }

        ofstream output(tempPath, std::ios::trunc);

        if (!output)
        {
            throw runtime_error("Error al crear el archivo temporal: " + tempPath.string());
        }

        string line;

        while (getline(input, line))
        {
            if (line.empty())
            {
                continue;
            }

            if (line.back() == '\r')
            {
                line.pop_back();
            }

            vector<string> data = split(line, delimiter);

            if (data.size() != fields)
            {
                throw runtime_error("Inconsistencia en la cantidad de propiedades del archivo: " + filePath + ". Cantidad requerida: " + std::to_string(fields) + ". Cantidad encontrada: " + std::to_string(data.size()));
            }

            optional<vector<string>> transformed = transform(data);

            if (!transformed)
            {
                continue;
            }

            if (transformed->size() != fields)
            {
                throw runtime_error("La transformacion genero una cantidad incorrecta de propiedades.");
            }

            for (size_t i = 0; i < transformed->size(); ++i)
            {
                if (i > 0)
                {
                    output << delimiter;
                }

                output << (*transformed)[i];
            }

            output << '\n';
        }

        input.close();
        output.close();

        if (!output)
        {
            fs::remove(tempPath);
            throw runtime_error("Error al escribir el archivo temporal: " + tempPath.string());
        }

        std::error_code error;
        fs::rename(tempPath, originalPath, error);

        if (error)
        {
            fs::remove(tempPath);
            throw runtime_error("Error al reemplazar el archivo original: " + error.message());
        }
    }

    static void update(const string &filePath, const size_t fields, char delimiter, const vector<string> &newRecord, function<bool(const vector<string> &)> predicate)
    {
        // Verificación redundante y temprana sobre la cantidad de propiedades adecuada de la nueva informacion del registro
        if (newRecord.size() != fields)
        {
            throw runtime_error("Cantidad incorrecta de propiedades de la nuevo informacion del registro a actualizar.");
        }

        rewrite(filePath, fields, delimiter, [&newRecord, &predicate](const auto &row) -> optional<vector<string>>
        {
            if (predicate(row))
            {
                return newRecord;
            }
            
            return row;
        });
    }

    // Agrega un nuevo registro al archivo
    static void write(const string &filePath, const string &record)
    {
        ofstream file(filePath, std::ios::app);

        if (!file.is_open())
        {
            throw runtime_error("Error al abrir el archivo para agregar informacion: " + filePath);
        }

        file << record << '\n';

        if (!file)
        {
            throw runtime_error("Error al escribir en el archivo: " + filePath);
        }

        file.close();
    }

    // Agrega un nuevo registro al archivo
    static void write(const string &filePath, const vector<string> &record, char delimiter)
    {
        ofstream file(filePath, std::ios::app);

        if (!file.is_open())
        {
            throw runtime_error("Error al abrir el archivo para agregar informacion: " + filePath);
        }

        file << join(record, delimiter) << '\n';

        if (!file)
        {
            throw runtime_error("Error al escribir en el archivo: " + filePath);
        }

        file.close();
    }

    // Retorna `true` si al menos una fila del archivo cumple con el predicado dado, `false` caso contrario
    static bool contain(const string &filePath, const size_t fields, char delimiter, function<bool(const vector<string> &)> predicate)
    {
        ifstream file(filePath);

        if (!file)
        {
            throw runtime_error("Error al abrir el archivo: " + filePath);
        }

        string line;

        while (getline(file, line))
        {
            if (line.empty())
            {
                continue;
            }

            if (line.back() == '\r')
            {
                line.pop_back();
            }

            vector<string> data = split(line, delimiter);

            if (data.size() != fields)
            {
                throw runtime_error("Inconsistencia en la cantidad de propiedades del archivo: " + filePath + ". Cantidad requerida: " + std::to_string(fields) + ". Cantidad encontrada: " + std::to_string(data.size()));
            }

            if (predicate(data))
            {
                return true;
            }
        }

        file.close();

        return false;
    }

    // Actualiza un registro, en caso exista, o lo inserta en caso contrario.
    static void upsert(const string &filePath, const size_t fields, char delimiter, const vector<string> &record, function<bool(const vector<string> &)> predicate)
    {
        // const bool exists = contain(filePath, fields, delimiter, true, [&record](const auto &row) { return record == row; });
        const bool exists = contain(filePath, fields, delimiter, predicate);

        if (!exists)
        {
            write(filePath, record, delimiter);
            return;
        }

        update(filePath, fields, delimiter, record, predicate);
    }

    static void remove(const string &filePath, const size_t fields, char delimiter, function<bool(const vector<string> &)> predicate)
    {
        const bool exists = contain(filePath, fields, delimiter, predicate);

        if (!exists) { throw runtime_error("Registro a eliminar no encontrado."); }

        rewrite(filePath, fields, delimiter, [&predicate](const auto &row) -> optional<vector<string>>
        {
            if (predicate(row))
            {
                return nullopt;
            }
            
            return row;
        });
    }
};