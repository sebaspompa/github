#pragma once
#include <string>

#include "src/app/utils.hpp"
#include "src/app/services/UserService.hpp"
#include "src/app/services/RepositoryService.hpp"

using std::string;

struct CreateRepositoryViewModel
{
private:
    string error;

public:
    const string& getError() const
    {
        return error;
    }

    // Devuelve true si el repositorio se creo
    bool create(const string& rawName, const string& rawDescription, const string& rawLanguage, const bool isPublic)
    {
        error.clear();

        const string name = DB::trim(rawName);
        const string description = DB::trim(rawDescription);
        const string language = DB::trim(rawLanguage);

        if (name.empty() || description.empty() || language.empty())
        {
            error = "Completa todos los campos.";
            return false;
        }

        if (DB::hasForbiddenChars(name) ||
            DB::hasForbiddenChars(description) ||
            DB::hasForbiddenChars(language))
        {
            error = "No se permite el caracter '|' en los datos.";
            return false;
        }

        try
        {
            RepositoryService::create(UserService::currentUserId(), name, description, language, isPublic ? "1" : "0");

            return true;
        }
        catch (const std::exception& e)
        {
            error = e.what();
            return false;
        }
    }
};