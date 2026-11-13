#pragma once
#include <string>
#include <vector>
#include <algorithm>

#include "src/app/services/RecordFile.hpp"
#include "src/app/services/SessionService.hpp"
#include "src/app/models/User.hpp"

using std::string;
using std::vector;
using std::runtime_error;

// accounts.txt: id|username|password
// users.txt:    id|name|email
struct UserService
{
private:
    static string toLower(string value)
    {
        std::transform(value.begin(), value.end(), value.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        return value;
    }

public:
    static string currentUserId()
    {
        return SessionService::instance().session();
    }

    static User getProfile(const string& userId)
    {
        User profile;
        profile.id = userId;

        bool found = false;

        for (const vector<string>& account : RecordFile::read("accounts.txt", 3))
        {
            if (account[0] == userId)
            {
                profile.username = account[1];
                found = true;
                break;
            }
        }

        if (!found)
        {
            throw runtime_error("Usuario no encontrado.");
        }

        // Las cuentas antiguas pueden no tener fila en users.txt
        for (const vector<string>& user : RecordFile::readIfExists("users.txt", 3))
        {
            if (user[0] == userId)
            {
                profile.name = user[1];
                profile.email = user[2];
                break;
            }
        }

        return profile;
    }

    static void updateProfile(const User& profile, const string& newPassword)
    {
        vector<vector<string>> accounts = RecordFile::read("accounts.txt", 3);
        vector<vector<string>> users = RecordFile::readIfExists("users.txt", 3);

        long accountIndex = -1;

        for (size_t i = 0; i < accounts.size(); i++)
        {
            if (accounts[i][0] == profile.id)
            {
                accountIndex = static_cast<long>(i);
            }
            else if (accounts[i][1] == profile.username)
            {
                throw runtime_error("El nombre de usuario ya existe.");
            }
        }

        if (accountIndex < 0)
        {
            throw runtime_error("Usuario no encontrado.");
        }

        long userIndex = -1;

        for (size_t i = 0; i < users.size(); i++)
        {
            if (users[i][0] == profile.id)
            {
                userIndex = static_cast<long>(i);
            }
            else if (toLower(users[i][2]) == toLower(profile.email))
            {
                throw runtime_error("El email ya esta registrado.");
            }
        }

        accounts[accountIndex][1] = profile.username;

        if (!newPassword.empty())
        {
            accounts[accountIndex][2] = newPassword;
        }

        const vector<string> userRecord = { profile.id, profile.name, profile.email };

        if (userIndex >= 0)
        {
            users[userIndex] = userRecord;
        }
        else
        {
            users.push_back(userRecord);
        }

        RecordFile::write("accounts.txt", accounts);
        RecordFile::write("users.txt", users);
    }
};