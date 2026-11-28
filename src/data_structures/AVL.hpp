#pragma once

template <typename T>
class AVLTree
{
private:
    struct Node
    {
        T value;
        Node *left;
        Node *right;
        int height;

        explicit Node(const T &value)
            : value(value), left(nullptr), right(nullptr), height(1)
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

    static Node *insert(Node *node, const T &value)
    {
        if (node == nullptr)
        {
            return new Node(value);
        }

        if (value < node->value)
        {
            node->left = insert(node->left, value);
        }
        else if (node->value < value)
        {
            node->right = insert(node->right, value);
        }
        else
        {
            // No se insertan valores duplicados.
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
    AVLTree() = default;

    ~AVLTree()
    {
        destroy(root);
    }

    AVLTree(const AVLTree &) = delete;
    AVLTree &operator=(const AVLTree &) = delete;

    void insert(const T &value)
    {
        root = insert(root, value);
    }
};
