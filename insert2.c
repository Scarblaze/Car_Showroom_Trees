#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ORDER 5
#define MAX_KEYS (ORDER - 1)
#define MIN_KEYS (ORDER/2)

typedef struct BTreeNode {
    int keys[MAX_KEYS];
    struct BTreeNode* children[ORDER];
    int num_keys;
} BTreeNode;

bool isLeaf(BTreeNode* root)
{
    bool ans= false;
    if(root->children[0]==NULL) ans=true;
    return ans;
}

BTreeNode* create_node() {
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));
    node->num_keys = 0;
    for (int i = 0; i < ORDER; i++) {
        node->children[i] = NULL;
    }
    return node;
}

void traverse(BTreeNode* node) {
    if (node == NULL) return;
    
    int i;
    for (i = 0; i < node->num_keys; i++) {
        if (!isLeaf(node)) {
            traverse(node->children[i]);
        }
        printf("%d ", node->keys[i]);
    }
    
    // Traverse the last child
    if (!isLeaf(node)) {
        traverse(node->children[i]);
    }
}

BTreeNode* search(BTreeNode* node, int key) {
    if (node == NULL) return NULL;
    
    int i = 0;
    while (i < node->num_keys && key > node->keys[i]) {
        i++;
    }
    
    if (i < node->num_keys && key == node->keys[i]) {
        return node;
    }
    
    if (isLeaf(node)) {
        return NULL;
    }
    
    return search(node->children[i], key);
}

void split_child(BTreeNode* parent, int child_index) {
    BTreeNode* child = parent->children[child_index];
    BTreeNode* new_child = create_node(isLeaf(child));
    int mid_index = MAX_KEYS/2;

    // Copy upper half of child to new_child
    new_child->num_keys = MAX_KEYS - mid_index - 1;
    for (int i = 0; i < new_child->num_keys; i++) {
        new_child->keys[i] = child->keys[mid_index + 1 + i];
    }

    // Copy children if not leaf
    if (!isLeaf(child)) {
        for (int i = 0; i <= new_child->num_keys; i++) {
            new_child->children[i] = child->children[mid_index + 1 + i];
        }
    }

    // Reduce keys in original child
    child->num_keys = mid_index;

    // Make space in parent for new child
    for (int i = parent->num_keys; i > child_index; i--) {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }

    // Move middle key to parent and link new child
    parent->children[child_index + 1] = new_child;
    parent->keys[child_index] = child->keys[mid_index];
    parent->num_keys++;
}

void insert_non_full(BTreeNode* node, int key) {
    int i = node->num_keys - 1;

    if (isLeaf(node)) {
        // Shift keys and insert
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    } else {
        // Find child to insert into
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;

        // Split if child is full
        if (node->children[i]->num_keys == MAX_KEYS) {
            split_child(node, i);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insert_non_full(node->children[i], key);
    }
}

void insert(BTreeNode** root, int key) {
    if (*root == NULL) {
        *root = create_node(true);
        (*root)->keys[0] = key;
        (*root)->num_keys = 1;
        return;
    }

    if ((*root)->num_keys == MAX_KEYS) {
        BTreeNode* new_root = create_node(false);
        new_root->children[0] = *root;
        split_child(new_root, 0);
        *root = new_root;
    }
    insert_non_full(*root, key);
}

void print_tree(BTreeNode* node, int level) {
    if (node == NULL) return;
    
    printf("Level %d: ", level);
    for (int i = 0; i < node->num_keys; i++) {
        printf("%d ", node->keys[i]);
    }
    printf("\n");

    if (!isLeaf(node)) {
        for (int i = 0; i <= node->num_keys; i++) {
            print_tree(node->children[i], level + 1);
        }
    }
}

int main() {
    BTreeNode* root = NULL;
    int keys[] = {10, 20, 5, 6, 12, 30, 7, 17, 100, 4000, 1};
    int num_keys = sizeof(keys)/sizeof(keys[0]);

    // Insert keys
    for (int i = 0; i < num_keys; i++) {
        insert(&root, keys[i]);
    }

    // Print the tree structure
    printf("B-Tree structure:\n");
    print_tree(root, 0);

    // Traverse and print all keys in order
    printf("\nIn-order traversal: ");
    traverse(root);
    printf("\n");

    // Search for keys
    int search_keys[] = {1, 20, 4000};
    for (int i = 0; i < 3; i++) {
        BTreeNode* result = search(root, search_keys[i]);
        printf("Search %d: %s\n", search_keys[i], result ? "Found" : "Not found");
    }

    return 0;
}