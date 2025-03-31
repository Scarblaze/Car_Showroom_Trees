#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define ORDER 5
#define MAX_KEYS (ORDER - 1)
#define MIN_KEYS (ceil(ORDER/2)-1)
#define MODELS 20



bool isLeaf(BTreeNode* root)
{
    bool ans= false;
    if(root->children[0]==NULL) ans=true;
    return ans;
}

typedef struct
{
    int VIN;
    char name[50];
    char color[20];
    double price;
    char fuelType[10];
    char carType[10];
    int customer_id;
    int salesperson_id;
} Car;

typedef struct SoldCarTreeNode {
    Car keys[MAX_KEYS];
    struct SoldCarTreeNode* children[ORDER];
    int num_keys;
} SoldCarTreeNode;

SoldCarTreeNode* create_node_sold_car() {
    SoldCarTreeNode* node = (SoldCarTreeNode*)malloc(sizeof(SoldCarTreeNode));
    node->num_keys = 0;
    for (int i = 0; i < ORDER; i++) {
        node->children[i] = NULL;
    }
    return node;
}

void traverse_sold_car(SoldCarTreeNode* node) {
    if (node == NULL) return;
    
    int i;
    for (i = 0; i < node->num_keys; i++) {
        if (node->children[0]) {
            traverse_sold_car(node->children[i]);
        }
        printf("%d ", node->keys[i].VIN);
    }
    
    // Traverse the last child
    if (node->children[0]) {
        traverse_sold_car(node->children[i]);
    }
}

void split_child_sold_car(SoldCarTreeNode* parent, int child_index) {
    SoldCarTreeNode* child = parent->children[child_index];
    SoldCarTreeNode* new_child = create_node_sold_car();
    int mid_index = MAX_KEYS/2;

    // Copy upper half of child to new_child
    new_child->num_keys = MAX_KEYS - mid_index - 1;
    for (int i = 0; i < new_child->num_keys; i++) {
        new_child->keys[i] = child->keys[mid_index + 1 + i];
    }

    // Copy children if not leaf
    if (child->children[0]) {
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

void insert_non_full_sold_car(SoldCarTreeNode* node, Car key) {
    int i = node->num_keys - 1;

    //leaf
    if (node->children[0]==NULL) {
        // Shift keys and insert
        while (i >= 0 && key.VIN < node->keys[i].VIN) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    } else {
        // Find child to insert into
        while (i >= 0 && key.VIN < node->keys[i].VIN) {
            i--;
        }
        i++; //because you have to go to child. i was key index until now

        // Split if child is full
        if (node->children[i]->num_keys == MAX_KEYS) {
            split_child_sold_car(node, i);
            if (key.VIN > node->keys[i].VIN) {
                i++;
            }
        }
        insert_non_full_sold_car(node->children[i], key);
    }
}

void insert_sold_car(SoldCarTreeNode** root, Car key) {
    if (*root == NULL) {
        *root = create_node_sold_car();
        (*root)->keys[0] = key;
        (*root)->num_keys = 1;
        return;
    }

    if ((*root)->num_keys == MAX_KEYS) {
        SoldCarTreeNode* new_root = create_node_sold_car();
        new_root->children[0] = *root;
        split_child_sold_car(new_root, 0);
        *root = new_root;
    }
    insert_non_full_sold_car(*root, key);
}



typedef struct AvailableCarTreeNode {
    Car keys[MAX_KEYS];
    struct AvailableCarTreeNode *children[ORDER];
    int num_keys; 
} AvailableCarTreeNode;


SoldCarTreeNode* create_node_available_car() {
    AvailableCarTreeNode* node = (AvailableCarTreeNode*)malloc(sizeof(AvailableCarTreeNode));
    node->num_keys = 0;
    for (int i = 0; i < ORDER; i++) {
        node->children[i] = NULL;
    }
    return node;
}

void traverse_available_car(AvailableCarTreeNode* node) {
    if (node == NULL) return;
    
    int i;
    for (i = 0; i < node->num_keys; i++) {
        if (node->children[0]) {
            traverse_available_car(node->children[i]);
        }
        printf("%d ", node->keys[i].VIN);
    }
    
    // Traverse the last child
    if (node->children[0]) {
        traverse_available_car(node->children[i]);
    }
}

void split_child_available_car(AvailableCarTreeNode* parent, int child_index) {
    AvailableCarTreeNode* child = parent->children[child_index];
    AvailableCarTreeNode* new_child = create_node_available_car();
    int mid_index = MAX_KEYS/2;

    // Copy upper half of child to new_child
    new_child->num_keys = MAX_KEYS - mid_index - 1;
    for (int i = 0; i < new_child->num_keys; i++) {
        new_child->keys[i] = child->keys[mid_index + 1 + i];
    }

    // Copy children if not leaf
    if (child->children[0]) {
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

void insert_non_full_available_car(AvailableCarTreeNode* node, Car key) {
    int i = node->num_keys - 1;

    //leaf
    if (node->children[0]==NULL) {
        // Shift keys and insert
        while (i >= 0 && key.VIN < node->keys[i].VIN) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    } else {
        // Find child to insert into
        while (i >= 0 && key.VIN < node->keys[i].VIN) {
            i--;
        }
        i++; //because you have to go to child. i was key index until now

        // Split if child is full
        if (node->children[i]->num_keys == MAX_KEYS) {
            split_child_available_car(node, i);
            if (key.VIN > node->keys[i].VIN) {
                i++;
            }
        }
        insert_non_full_available_car(node->children[i], key);
    }
}

void insert_available_car(AvailableCarTreeNode** root, Car key) {
    if (*root == NULL) {
        *root = create_node_available_car();
        (*root)->keys[0] = key;
        (*root)->num_keys = 1;
        return;
    }

    if ((*root)->num_keys == MAX_KEYS) {
        AvailableCarTreeNode* new_root = create_node_available_car();
        new_root->children[0] = *root;
        split_child_available_car(new_root, 0);
        *root = new_root;
    }
    insert_non_full_available_car(*root, key);
}



//refrence 
typedef struct BTreeNode {
    int keys[MAX_KEYS];
    struct BTreeNode* children[ORDER];
    int num_keys;
} BTreeNode;

typedef struct
{
    int customer_id;
    char name[50];
    char mobile[15];
    char address[100];
    int soldCarVIN;
    char registrationNumber[20];
    char payment_type[20];
} Customer;

typedef struct CustomerTreeNode {
    int keys[ORDER - 1];
    Customer customer;
    struct CustomerTreeNode *children_ptr[ORDER];
    int numKeys;
    int leaf;
} CustomerTreeNode;

typedef struct
{
    int id;
    char name[50];
    float salesTarget;
    float salesAchieved;
    float commission;
    SoldCarTreeNode *root;
} SalesPerson;

typedef struct SalesTreeNode {
    int keys[ORDER - 1]; 
    SalesPerson salesPersons;
    struct SalesTreeNode *children[ORDER];
    int numKeys;
    int leaf;
} SalesTreeNode;

typedef struct 
{
    char car_model[20];
    int sold_cars;
    int available_cars;  
} stock_details;

typedef struct showroom
{
    int showroom_id;
    SalesTreeNode* sales_root;
    CustomerTreeNode* customer_root;
    AvailableCarTreeNode* available_car_root;
    SoldCarTreeNode* sold_car_root;
    stock_details stock[MODELS];
};

