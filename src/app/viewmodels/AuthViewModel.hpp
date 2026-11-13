#pragma once
#include <string>

#include "src/app/services/AuthService.hpp"
#include "src/app/services/SessionService.hpp"

using std::string;

struct AuthViewModel
{
private:
    string error;

public:
    const string& getError() const
    {
        return error;
    }

    void clearError()
    {
        error.clear();
    }

    // Devuelve true si el login fue exitoso
    bool login(const string& username, const string& password)
    {
        try
        {
            // Se obtiene el `id` del usuario
            const string userId = AuthService::login(username, password);

            error.clear();

            // Se crea una sesion para el usuario.
            // Puede reemplazar la vista actual; no invocar miembros despues.
            SessionService::instance().create(userId);

            return true;
        }
        catch (const std::exception& e)
        {
            error = e.what();
            return false;
        }
    }

    // Devuelve true si el registro fue exitoso (e inicia sesion automaticamente)
    bool registerAccount(const string& name, const string& username, const string& email, const string& password)
    {
        try
        {
            const string userId = AuthService::registerAccount(name, username, email, password);

            error.clear();

            // Se crea una sesion para el usuario.
            // Puede reemplazar la vista actual; no invocar miembros despues.
            SessionService::instance().create(userId);

            return true;
        }
        catch (const std::exception& e)
        {
            error = e.what();
            return false;
        }
    }
};