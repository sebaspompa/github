#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <filesystem>

#include "src/app/utils.hpp"

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::runtime_error;
using std::getline;

namespace fs = std::filesystem;

struct AuthService
{
private:
    static bool isValidEmail(const string& email)
    {
        const size_t at = email.find('@');

        if (at == string::npos || at == 0)
        {
            return false;
        }

        const size_t dot = email.find('.', at);

        return dot != string::npos && dot > at + 1 && dot + 1 < email.size() &&
            email.find(' ') == string::npos;
    }

public:
    // accounts.txt: id|username|password
    static string login(const string& username, const string& password)
    {
        for (const vector<string>& account : DB::read(Paths::Storage::System::accounts.string(), 3, '|'))
        {
            if (username == account[1] && password == account[2])
            {
                return account[0];
            }
        }

        throw runtime_error("Usuario o contrasena incorrectos.");
    }

    // Guarda las credenciales en accounts.txt (id|username|password)
    // y el perfil en users.txt (id|name|email). Devuelve el `id`.
    static string registerAccount(const string& rawName, const string& rawUsername, const string& rawEmail, const string& password)
    {
        const string name = DB::trim(rawName);
        const string username = DB::trim(rawUsername);
        const string email = DB::trim(rawEmail);

        if (name.empty() || username.empty() || email.empty() || password.empty())
        {
            throw runtime_error("Completa todos los campos.");
        }

        if (DB::hasForbiddenChars(name) || DB::hasForbiddenChars(username) ||
            DB::hasForbiddenChars(email) || DB::hasForbiddenChars(password))
        {
            throw runtime_error("No se permite el caracter '|' en los datos.");
        }

        if (username.find(' ') != string::npos)
        {
            throw runtime_error("El usuario no puede tener espacios.");
        }

        if (!isValidEmail(email))
        {
            throw runtime_error("El email no es valido.");
        }

        const vector<vector<string>> accounts = DB::read(Paths::Storage::System::accounts.string(), 3, '|');
        const vector<vector<string>> users = DB::read(Paths::Storage::System::users.string(), 3, '|');

        long nextId = 0;

        for (const vector<string>& account : accounts)
        {
            if (account[1] == username)
            {
                throw runtime_error("El nombre de usuario ya existe.");
            }

            try
            {
                const long id = std::stol(account[0]);

                if (id >= nextId)
                {
                    nextId = id + 1;
                }
            }
            catch (const std::exception&)
            {
                // id no numerico: se ignora para el calculo
            }
        }

        for (const vector<string>& user : users)
        {
            if (toLower(user[2]) == toLower(email))
            {
                throw runtime_error("El email ya esta registrado.");
            }
        }

        const string userId = std::to_string(nextId);

        DB::appendLine("accounts.txt", userId + "|" + username + "|" + password);
        DB::appendLine("users.txt", userId + "|" + name + "|" + email);

        return userId;
    }
};