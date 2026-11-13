#pragma once
#include <string>

#include "src/app/services/UserService.hpp"
#include "src/app/utils.hpp"

using std::string;

struct ProfileViewModel
{
private:
    User profile;
    string error;
    string notice;

    void load()
    {
        try
        {
            profile = UserService::getProfile(UserService::currentUserId());
        }
        catch (const std::exception& e)
        {
            error = e.what();
        }
    }

public:
    ProfileViewModel()
    {
        load();
    }

    const User& getProfile() const
    {
        return profile;
    }

    const string& getError() const
    {
        return error;
    }

    const string& getNotice() const
    {
        return notice;
    }

    // Los campos vacios conservan el valor actual.
    // Devuelve true si se guardo (o no habia cambios).
    bool save(const string& rawName, const string& rawUsername, const string& rawEmail, const string& rawPassword)
    {
        if (profile.id.empty())
        {
            error.clear();
            load();

            if (profile.id.empty())
            {
                return false;
            }
        }

        error.clear();
        notice.clear();

        User updated = profile;

        const string name = DB::trim(rawName);
        const string username = DB::trim(rawUsername);
        const string email = DB::trim(rawEmail);

        if (!name.empty())
        {
            updated.name = name;
        }

        if (!username.empty())
        {
            updated.username = username;
        }

        if (!email.empty())
        {
            updated.email = email;
        }

        if (DB::hasForbiddenChars(updated.name) ||
            DB::hasForbiddenChars(updated.username) ||
            DB::hasForbiddenChars(updated.email) ||
            DB::hasForbiddenChars(rawPassword))
        {
            error = "No se permite el caracter '|' en los datos.";
            return false;
        }

        if (updated.name.empty())
        {
            error = "El nombre no puede estar vacio.";
            return false;
        }

        if (updated.username.find(' ') != string::npos)
        {
            error = "El usuario no puede tener espacios.";
            return false;
        }

        // if (!Validators::isValidEmail(updated.email))
        // {
        //     error = "El email no es valido.";
        //     return false;
        // }

        const bool changed = updated.name != profile.name ||
            updated.username != profile.username ||
            updated.email != profile.email ||
            !rawPassword.empty();

        if (!changed)
        {
            notice = "No hay cambios que guardar.";
            return true;
        }

        try
        {
            UserService::updateProfile(updated, rawPassword);

            profile = updated;
            notice = "Perfil actualizado correctamente.";

            return true;
        }
        catch (const std::exception& e)
        {
            error = e.what();
            return false;
        }
    }
};