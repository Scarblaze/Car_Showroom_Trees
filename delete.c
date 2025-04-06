#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 4  
#define MIN 2  

// Structure for Student
typedef struct Student {
    char name[50];
    int roll_no;
} Student;

// B-Tree Node Structure
typedef struct BTreeNode {
    Student keys[MAX + 1];
    struct BTreeNode *children[MAX + 2];
    struct BTreeNode *parent;
    int count;
    int isLeaf;
} BTreeNode;

BTreeNode *root = NULL;


void rebalanceNode(BTreeNode *node);

// Create a new B-Tree node
BTreeNode* createNode(int isLeaf) {
    BTreeNode *newNode = (BTreeNode *)malloc(sizeof(BTreeNode));
    if (!newNode) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    newNode->count = 0;
    newNode->isLeaf = isLeaf;
    newNode->parent = NULL;
    for (int i = 0; i <= MAX + 1; i++) {
        newNode->children[i] = NULL;
    }
    return newNode;
}

// Find position to insert based on roll_no
int findPosition(int roll_no, BTreeNode *node) {
    int pos = 0;
    while (pos < node->count && roll_no > node->keys[pos].roll_no) {
        pos++;
    }
    return pos;
}

// Find a student by roll number, returns both node and position
void findStudent(int roll_no, BTreeNode **node, int *pos) {
    BTreeNode *current = root;
    *node = NULL;
    *pos = -1;
    
    while (current) {
        int i = 0;
        while (i < current->count && roll_no > current->keys[i].roll_no) {
            i++;
        }
        
        if (i < current->count && roll_no == current->keys[i].roll_no) {
            *node = current;
            *pos = i;
            return;
        }
        
        if (current->isLeaf) {
            return;
        }
        
        current = current->children[i];
    }
}

// Split a node (non-recursive, uses parent pointers)
void splitNode(BTreeNode *node) {
    // Middle index
    int mid = node->count / 2;
    
    // Create a new node for the right half
    BTreeNode *rightNode = createNode(node->isLeaf);
    
    // Copy the right half keys to the new node
    for (int i = mid + 1, j = 0; i < node->count; i++, j++) {
        rightNode->keys[j] = node->keys[i];
        if (!node->isLeaf) {
            rightNode->children[j] = node->children[i];
            if (rightNode->children[j]) {
                rightNode->children[j]->parent = rightNode;
            }
        }
    }
    
    // Copy the last child if not a leaf
    if (!node->isLeaf) {
        rightNode->children[node->count - mid - 1] = node->children[node->count];
        if (rightNode->children[node->count - mid - 1]) {
            rightNode->children[node->count - mid - 1]->parent = rightNode;
        }
    }
    
    // Update the count for the right node
    rightNode->count = node->count - mid - 1;
    
    // Update the count for the original node
    node->count = mid;
    
    // The middle key that needs to move up
    Student midKey = node->keys[mid];
    
    // If this is the root, create a new root
    if (!node->parent) {
        // Create a new root
        BTreeNode *newRoot = createNode(0);
        newRoot->keys[0] = midKey;
        newRoot->children[0] = node;
        newRoot->children[1] = rightNode;
        newRoot->count = 1;
        
        // Update parent pointers
        node->parent = newRoot;
        rightNode->parent = newRoot;
        
        // Update the global root
        root = newRoot;
    } else {
        // Insert the middle key into the parent
        BTreeNode *parent = node->parent;
        
        // Find the position to insert in the parent
        int pos = findPosition(midKey.roll_no, parent);
        
        // Shift keys and children to make room
        for (int i = parent->count; i > pos; i--) {
            parent->keys[i] = parent->keys[i - 1];
            parent->children[i + 1] = parent->children[i];
        }
        
        // Insert the middle key and right child
        parent->keys[pos] = midKey;
        parent->children[pos + 1] = rightNode;
        parent->count++;
        
        // Update parent pointer for the right node
        rightNode->parent = parent;
        
        // If the parent is now overfull, split it
        if (parent->count > MAX) {
            splitNode(parent);
        }
    }
}

// Insert a student record into the B-tree
void insert(Student student) {
    // If tree is empty, create the root
    if (!root) {
        root = createNode(1);
        root->keys[0] = student;
        root->count = 1;
        return;
    }
    
    // Find the leaf node where the key should be inserted
    BTreeNode *current = root;
    while (!current->isLeaf) {
        int pos = findPosition(student.roll_no, current);
        current = current->children[pos];
    }
    
    // Find the position to insert in the leaf
    int pos = findPosition(student.roll_no, current);
    
    // Check if the key already exists
    if (pos < current->count && current->keys[pos].roll_no == student.roll_no) {
        printf("Student with roll number %d already exists!\n", student.roll_no);
        return;
    }
    
    // Shift keys to make room
    for (int i = current->count; i > pos; i--) {
        current->keys[i] = current->keys[i - 1];
    }
    
    // Insert the key
    current->keys[pos] = student;
    current->count++;
    
    // If the node is now overfull, split it
    if (current->count > MAX) {
        splitNode(current);
    }
}

// Borrow a key from the right sibling
void borrowFromRight(BTreeNode *node, int index) {
    BTreeNode *parent = node->parent;
    BTreeNode *rightSibling = parent->children[index + 1];
    
    // Make space for the new key in the node
    if (!node->isLeaf) {
        node->children[node->count + 1] = node->children[node->count];
    }
    
    // Move the parent key to the node
    node->keys[node->count] = parent->keys[index];
    node->count++;
    
    // Update the parent key with the first key from the right sibling
    parent->keys[index] = rightSibling->keys[0];
    
    // If not leaf nodes, update children
    if (!node->isLeaf) {
        node->children[node->count] = rightSibling->children[0];
        if (node->children[node->count]) {
            node->children[node->count]->parent = node;
        }
        
        // Shift children in the right sibling
        for (int i = 0; i < rightSibling->count; i++) {
            rightSibling->children[i] = rightSibling->children[i + 1];
        }
    }
    
    // Shift keys in the right sibling
    for (int i = 0; i < rightSibling->count - 1; i++) {
        rightSibling->keys[i] = rightSibling->keys[i + 1];
    }
    rightSibling->count--;
}

// Borrow a key from the left sibling
void borrowFromLeft(BTreeNode *node, int index) {
    BTreeNode *parent = node->parent;
    BTreeNode *leftSibling = parent->children[index - 1];
    
    // Make space for the new key
    for (int i = node->count; i > 0; i--) {
        node->keys[i] = node->keys[i - 1];
    }
    
    // If not leaf nodes, shift children
    if (!node->isLeaf) {
        for (int i = node->count + 1; i > 0; i--) {
            node->children[i] = node->children[i - 1];
        }
        
        // Move the last child from left sibling
        node->children[0] = leftSibling->children[leftSibling->count];
        if (node->children[0]) {
            node->children[0]->parent = node;
        }
    }
    
    // Move the parent key to the node
    node->keys[0] = parent->keys[index - 1];
    node->count++;
    
    // Update the parent key with the last key from the left sibling
    parent->keys[index - 1] = leftSibling->keys[leftSibling->count - 1];
    leftSibling->count--;
}

// Merge two nodes during deletion
void mergeNodes(BTreeNode *leftNode, BTreeNode *rightNode, int parentKeyIndex) {
    BTreeNode *parent = leftNode->parent;
    
    // Add the parent key to the left node
    leftNode->keys[leftNode->count] = parent->keys[parentKeyIndex];
    leftNode->count++;
    
    // Copy all keys from right node to left node
    for (int i = 0, j = leftNode->count; i < rightNode->count; i++, j++) {
        leftNode->keys[j] = rightNode->keys[i];
        leftNode->count++;
    }
    
    // Copy children if not leaf nodes
    if (!leftNode->isLeaf) {
        for (int i = 0, j = leftNode->count - rightNode->count; i <= rightNode->count; i++, j++) {
            leftNode->children[j] = rightNode->children[i];
            if (leftNode->children[j]) {
                leftNode->children[j]->parent = leftNode;
            }
        }
    }
    
    // Remove the parent key and shift keys and children
    for (int i = parentKeyIndex; i < parent->count - 1; i++) {
        parent->keys[i] = parent->keys[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }
    parent->count--;
    
    // Free the right node
    free(rightNode);
    
    // If the parent is the root and it's empty, update the root
    if (parent == root && parent->count == 0) {
        free(parent);
        root = leftNode;
        leftNode->parent = NULL;
        return;
    }
    
    // If the parent count falls below minimum, rebalance
    if (parent->count < MIN && parent != root) {
        rebalanceNode(parent);
    }
}

// Rebalance a node that has fewer than MIN keys
void rebalanceNode(BTreeNode *node) {
    BTreeNode *parent = node->parent;
    
    // Find the index of node in parent's children
    int nodeIndex = 0;
    while (nodeIndex <= parent->count && parent->children[nodeIndex] != node) {
        nodeIndex++;
    }
    
    // Try to borrow from right sibling
    if (nodeIndex < parent->count) {
        BTreeNode *rightSibling = parent->children[nodeIndex + 1];
        if (rightSibling->count > MIN) {
            borrowFromRight(node, nodeIndex);
            return;
        }
    }
    
    // Try to borrow from left sibling
    if (nodeIndex > 0) {
        BTreeNode *leftSibling = parent->children[nodeIndex - 1];
        if (leftSibling->count > MIN) {
            borrowFromLeft(node, nodeIndex);
            return;
        }
    }
    
    // If borrowing is not possible, merge with a sibling
    if (nodeIndex < parent->count) {
        // Merge with right sibling
        mergeNodes(node, parent->children[nodeIndex + 1], nodeIndex);
    } else {
        // Merge with left sibling
        mergeNodes(parent->children[nodeIndex - 1], node, nodeIndex - 1);
    }
}

// Delete a key from a leaf node
void deleteFromLeaf(BTreeNode *node, int pos) {
    // Shift keys to remove the target
    for (int i = pos; i < node->count - 1; i++) {
        node->keys[i] = node->keys[i + 1];
    }
    node->count--;
    
    // If node is the root and empty, just return
    if (node == root && node->count == 0) {
        free(root);
        root = NULL;
        return;
    }
    
    // If node is not the root and has fewer than MIN keys, rebalance
    if (node->count < MIN && node != root) {
        rebalanceNode(node);
    }
}


void findInOrderSuccessor(BTreeNode *node, int pos, BTreeNode **successorNode, int *successorPos) {
    BTreeNode *current = node->children[pos + 1];
    
    
    while (!current->isLeaf) {
        current = current->children[0];
    }
    
    *successorNode = current;
    *successorPos = 0;
}


void deleteStudent(int roll_no) {
   
    if (!root) {
        printf("Tree is empty!\n");
        return;
    }
    
    // Find the node containing the key
    BTreeNode *node;
    int pos;
    findStudent(roll_no, &node, &pos);
    
    // If key not found
    if (!node) {
        printf("Student with roll number %d not found!\n", roll_no);
        return;
    }
    
    // If node is a leaf node
    if (node->isLeaf) {
        deleteFromLeaf(node, pos);
        return;
    }
    
    // If node is an internal node
    BTreeNode *successorNode;
    int successorPos;
    
    // Find the in-order successor
    findInOrderSuccessor(node, pos, &successorNode, &successorPos);
    
    // Replace the key with its successor
    node->keys[pos] = successorNode->keys[successorPos];
    
    // Delete the successor from the leaf
    deleteFromLeaf(successorNode, successorPos);
}

// Inorder Traversal
void traverse(BTreeNode *node, int level) {
    if (!node) return;
    
    printf("Level %d: ", level);
    for (int i = 0; i < node->count; i++) {
        printf("%d (%s) ", node->keys[i].roll_no, node->keys[i].name);
    }
    printf("\n");
    
    if (!node->isLeaf) {
        for (int i = 0; i <= node->count; i++) {
            traverse(node->children[i], level + 1);
        }
    }
}

// Print the B-tree structure
void printTree() {
    if (!root) {
        printf("Tree is empty!\n");
        return;
    }
    
    printf("B-Tree Structure:\n");
    traverse(root, 0);
}

// Free the memory used by the B-tree
void freeTree(BTreeNode *node) {
    if (!node) return;
    
    if (!node->isLeaf) {
        for (int i = 0; i <= node->count; i++) {
            freeTree(node->children[i]);
        }
    }
    
    free(node);
}

// Main function
int main() {
    int choice;
    Student student;
    int roll_no;
    
    while (1) {
        printf("\nB-Tree Operations:\n");
        printf("1. Insert Student\n");
        printf("2. Delete Student\n");
        printf("3. Print B-Tree\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                printf("Enter student roll number: ");
                scanf("%d", &student.roll_no);
                printf("Enter student name: ");
                scanf(" %[^\n]s", student.name);
                insert(student);
                break;
            
            case 2:
                printf("Enter roll number to delete: ");
                scanf("%d", &roll_no);
                deleteStudent(roll_no);
                break;
            
            case 3:
                printTree();
                break;
            
            case 4:
                freeTree(root);
                printf("Exiting...\n");
                return 0;
            
            default:
                printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}