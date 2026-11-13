#pragma once
#include <string>
#include <vector>

#include "src/app/models/Repository.hpp"
#include "src/app/services/RecordFile.hpp"
#include "src/app/services/UserService.hpp"
#include "src/app/services/RepositoryService.hpp"

using std::string;
using std::vector;

struct HomeViewModel
{
private:
    string username;
    vector<Repository> repositories;

    bool loaded = false;
    unsigned long loadedRevision = 0;

    void load()
    {
        username.clear();
        repositories.clear();

        try
        {
            const string userId = UserService::currentUserId();

            try
            {
                username = UserService::getProfile(userId).username;
            }
            catch (const std::exception&)
            {
            }

            try
            {
                repositories = RepositoryService::repositoriesOf(userId);
            }
            catch (const std::exception&)
            {
            }
        }
        catch (const std::exception&)
        {
        }

        loaded = true;
        loadedRevision = RecordFile::revision();
    }

    // Recarga solo si algun archivo cambio desde la ultima lectura
    // (por ejemplo, tras crear un repositorio o editar el perfil)
    void refreshIfNeeded()
    {
        if (!loaded || loadedRevision != RecordFile::revision())
        {
            load();
        }
    }

public:
    const string& getUsername()
    {
        refreshIfNeeded();
        return username;
    }

    const vector<Repository>& getRepositories()
    {
        refreshIfNeeded();
        return repositories;
    }

    // Cambia cada vez que los datos se recargan
    unsigned long getVersion()
    {
        refreshIfNeeded();
        return loadedRevision;
    }
};