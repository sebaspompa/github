#pragma once

#include <string>
#include <optional>
#include <cstdint>
#include <vector>

#include "src/app/repositories/FileStorage.hpp"
#include "src/data_structures/Dictionary.hpp"

using std::string;

struct AccountEntity
{
    uint64_t id;
    string username;
    string password;
};

// schema: ID - USERNAME - PASSWORD
struct AccountRepository
{
private:
    static constexpr char DELIMITER = '|';
    static constexpr int FIELD_COUNT = 3;

    static constexpr int ID_INDEX = 0;
    static constexpr int USERNAME_INDEX = 1;
    static constexpr int PASS_INDEX = 2;

    const string filePath;

    // DB principal indexado por id
    Dictionary<uint64_t, AccountEntity> accounts;

    // Mapea: username -> id
    Dictionary<string, uint64_t> usernameToId;

    // Arbol AVL: mantiene las cuentas ordenadas y balanceadas por username.
    // Se usa como indice para las busquedas por prefijo (autocompletado).
    class UsernameAVLTree
    {
    private:
        struct Node
        {
            AccountEntity account;
            Node *left;
            Node *right;
            int height;

            explicit Node(const AccountEntity &value)
                : account(value), left(nullptr), right(nullptr), height(1)
            {
            }
        };

        Node *root = nullptr;

        static int heightOf(const Node *node)
        {
            return node == nullptr ? 0 : node->height;
        }

        static int balanceFactor(const Node *node)
        {
            return node == nullptr ? 0 : heightOf(node->left) - heightOf(node->right);
        }

        static void updateHeight(Node *node)
        {
            const int leftHeight = heightOf(node->left);
            const int rightHeight = heightOf(node->right);
            node->height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
        }

        static Node *rotateRight(Node *node)
        {
            Node *newRoot = node->left;
            Node *transferredSubtree = newRoot->right;

            newRoot->right = node;
            node->left = transferredSubtree;

            updateHeight(node);
            updateHeight(newRoot);

            return newRoot;
        }

        static Node *rotateLeft(Node *node)
        {
            Node *newRoot = node->right;
            Node *transferredSubtree = newRoot->left;

            newRoot->left = node;
            node->right = transferredSubtree;

            updateHeight(node);
            updateHeight(newRoot);

            return newRoot;
        }

        static Node *insert(Node *node, const AccountEntity &account)
        {
            if (node == nullptr)
            {
                return new Node(account);
            }

            if (account.username < node->account.username)
            {
                node->left = insert(node->left, account);
            }
            else if (account.username > node->account.username)
            {
                node->right = insert(node->right, account);
            }
            else
            {
                // Los usernames son unicos: si ya existe, se actualiza la cuenta.
                node->account = account;
                return node;
            }

            updateHeight(node);
            const int balance = balanceFactor(node);

            // Caso izquierda-izquierda.
            if (balance > 1 && balanceFactor(node->left) >= 0)
            {
                return rotateRight(node);
            }

            // Caso izquierda-derecha.
            if (balance > 1 && balanceFactor(node->left) < 0)
            {
                node->left = rotateLeft(node->left);
                return rotateRight(node);
            }

            // Caso derecha-derecha.
            if (balance < -1 && balanceFactor(node->right) <= 0)
            {
                return rotateLeft(node);
            }

            // Caso derecha-izquierda.
            if (balance < -1 && balanceFactor(node->right) > 0)
            {
                node->right = rotateRight(node->right);
                return rotateLeft(node);
            }

            return node;
        }

        static void collectByPrefix(
            const Node *node,
            const string &prefix,
            std::vector<AccountEntity> &result)
        {
            if (node == nullptr)
            {
                return;
            }

            // Compara username con el prefijo sin crear subcadenas temporales.
            const int comparison = node->account.username.compare(
                0, prefix.size(), prefix);

            if (comparison < 0)
            {
                // Este nodo y su subarbol izquierdo quedan antes del rango buscado.
                collectByPrefix(node->right, prefix, result);
            }
            else if (comparison > 0)
            {
                // Este nodo y su subarbol derecho quedan despues del rango buscado.
                collectByPrefix(node->left, prefix, result);
            }
            else
            {
                // Recorrido en orden: devuelve los resultados ordenados por username.
                collectByPrefix(node->left, prefix, result);
                result.push_back(node->account);
                collectByPrefix(node->right, prefix, result);
            }
        }

        static void destroy(Node *node)
        {
            if (node == nullptr)
            {
                return;
            }

            destroy(node->left);
            destroy(node->right);
            delete node;
        }

    public:
        UsernameAVLTree() = default;

        ~UsernameAVLTree()
        {
            destroy(root);
        }

        UsernameAVLTree(const UsernameAVLTree &) = delete;
        UsernameAVLTree &operator=(const UsernameAVLTree &) = delete;

        void insert(const AccountEntity &account)
        {
            root = insert(root, account);
        }

        std::vector<AccountEntity> searchByPrefix(const string &prefix) const
        {
            std::vector<AccountEntity> result;
            collectByPrefix(root, prefix, result);
            return result;
        }
    };

    UsernameAVLTree accountsByUsername;

private:
    void load()
    {
        FileStorage::forEach(filePath, FIELD_COUNT, DELIMITER, [&](const auto &row)
        {
            AccountEntity ae;
            ae.id = static_cast<uint64_t>(std::stoull(row[ID_INDEX]));
            ae.username = row[USERNAME_INDEX];
            ae.password = row[PASS_INDEX];

            accounts.add(ae.id, ae);
            usernameToId.add(ae.username, ae.id);
            accountsByUsername.insert(ae);
        });
    }

    void persist(const AccountEntity &ae) const
    {
        const string id = std::to_string(ae.id);

        FileStorage::upsert(filePath, FIELD_COUNT, DELIMITER, { id, ae.username, ae.password },
            [&id](const auto &row) -> bool
            {
                return id == row[ID_INDEX];
            }
        );
    }

public:
    AccountRepository(const string &filePath) : filePath(filePath)
    {
        load();
    }

    bool create(const AccountEntity &ae)
    {
        if (accounts.contains(ae.id) || usernameToId.contains(ae.username))
        {
            return false;
        }

        persist(ae);
        accounts.add(ae.id, ae);
        usernameToId.add(ae.username, ae.id);
        accountsByUsername.insert(ae);

        return true;
    }

    std::optional<AccountEntity> findById(uint64_t id) const
    {
        return accounts.get(id);
    }

    std::optional<AccountEntity> findByUsername(const string &username) const
    {
        auto accountId = usernameToId.get(username);

        if (!accountId) { return std::nullopt; }

        return accounts.get(*accountId);
    }

    // Devuelve todas las cuentas cuyo username comienza con prefix.
    // Un prefijo vacio devuelve todas las cuentas, ordenadas alfabeticamente.
    std::vector<AccountEntity> findByUsernamePrefix(const string &prefix) const
    {
        return accountsByUsername.searchByPrefix(prefix);
    }

    size_t count() const
    {
        return accounts.count();
    }
};
