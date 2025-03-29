#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ORDER 3

typedef struct BTreeNode
{
    int keys[ORDER - 1];                 // Keys
    struct BTreeNode* children[ORDER];   // Child pointers
    int n;                               // Number of keys
    bool leaf;                           // Is leaf
} BNode;

// Function to create a new B-Tree node
BNode* createNode(bool leaf)
{
    BNode* newNode = (BNode*)malloc(sizeof(BNode));
    newNode->n = 0;
    newNode->leaf = leaf;
    for (int i = 0; i < ORDER; i++)
        newNode->children[i] = NULL;
    return newNode;
}

// Function to traverse the B-Tree
void traverse(BNode* root)
{
    if (root == NULL)
        return;

    int i;
    for (i = 0; i < root->n; i++)
    {
        // Traverse left child
        if (!root->leaf)
            traverse(root->children[i]);
        // Print key
        printf("%d ", root->keys[i]);
    }

    // Traverse the last child
    if (!root->leaf)
        traverse(root->children[i]);
}

// Function to search a key in B-Tree
BNode* search(BNode* root, int key)
{
    if (root == NULL)
        return NULL;

    int i = 0;
    while (i < root->n && key > root->keys[i])
        i++;

    if (i < root->n && root->keys[i] == key)
        return root;

    if (root->leaf)
        return NULL;

    return search(root->children[i], key);
}

// Function to split a full child
void splitChild(BNode* parent, int i, BNode* child)
{
    BNode* newChild = createNode(child->leaf);
    newChild->n = (ORDER - 1) / 2;

    // Copy last (ORDER-1)/2 keys of child to newChild
    for (int j = 0; j < (ORDER - 1) / 2; j++)
        newChild->keys[j] = child->keys[j + ((ORDER + 1) / 2)];

    // Copy child pointers if not leaf
    if (!child->leaf)
    {
        for (int j = 0; j < (ORDER + 1) / 2; j++)
            newChild->children[j] = child->children[j + ((ORDER + 1) / 2)];
    }

    child->n = (ORDER - 1) / 2;

    // Shift parent's children
    for (int j = parent->n; j >= i + 1; j--)
        parent->children[j + 1] = parent->children[j];

    parent->children[i + 1] = newChild;

    // Shift parent's keys
    for (int j = parent->n - 1; j >= i; j--)
        parent->keys[j + 1] = parent->keys[j];

    parent->keys[i] = child->keys[(ORDER - 1) / 2];
    parent->n++;
}

// Function to insert in non-full node
void insertNonFull(BNode* root, int key)
{
    int i = root->n - 1;

    if (root->leaf)
    {
        // Shift keys to make space
        while (i >= 0 && key < root->keys[i])
        {
            root->keys[i + 1] = root->keys[i];
            i--;
        }
        root->keys[i + 1] = key;
        root->n++;
    }
    else
    {
        while (i >= 0 && key < root->keys[i])
            i--;

        i++;

        if (root->children[i]->n == ORDER - 1)
        {
            splitChild(root, i, root->children[i]);

            if (key > root->keys[i])
                i++;
        }
        insertNonFull(root->children[i], key);
    }
}

// Function to insert a key
BNode* insert(BNode* root, int key)
{
    if (root == NULL)
    {
        root = createNode(true);
        root->keys[0] = key;
        root->n = 1;
        return root;
    }

    if (root->n == ORDER - 1)
    {
        BNode* newRoot = createNode(false);
        newRoot->children[0] = root;
        splitChild(newRoot, 0, root);

        int i = 0;
        if (key > newRoot->keys[0])
            i++;
        insertNonFull(newRoot->children[i], key);

        return newRoot;
    }
    else
    {
        insertNonFull(root, key);
        return root;
    }
}