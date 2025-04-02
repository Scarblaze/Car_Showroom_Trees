#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define ORDER 5
#define MAX_KEYS (ORDER - 1)
#define MIN_KEYS (ceil(ORDER / 2) - 1)
#define MODELS 20
#define MAX_MONTHS 12

// Car structure
typedef struct {
    int VIN;
    char name[50];
    char color[20];
    double price;
    char fuelType[10];
    char carType[10]; // Hatchback, Sedan or SUV
    bool isSold;
    int customer_id;
    int salesperson_id;
    char soldDate[11]; // YYYY-MM-DD format
} Car;

typedef struct SoldCarTreeNode {
    Car keys[MAX_KEYS];
    struct SoldCarTreeNode *children[ORDER];
    int num_keys;
} SoldCarTreeNode;

typedef struct AvailableCarTreeNode {
    Car keys[MAX_KEYS];
    struct AvailableCarTreeNode *children[ORDER];
    int num_keys;
} AvailableCarTreeNode;

typedef struct {
    Car car;
    int showroom_id;
} MergedCar;

typedef struct MergedCarTreeNode {
    MergedCar keys[MAX_KEYS];
    struct MergedCarTreeNode *children[ORDER];
    int num_keys;
} MergedCarTreeNode;

// Customer structure with loan details
typedef struct {
    int id;
    char name[50];
    char mobile[15];
    char address[100];
    int soldCarVIN;
    char registrationNumber[20];
    char payment_type[20]; // "Cash" or "Loan"
    float downPayment;
    int loanMonths;
    float interestRate;
    float monthlyEMI;
    float loanAmount;
} Customer;

typedef struct CustomerTreeNode {
    Customer keys[MAX_KEYS];
    struct CustomerTreeNode *children[ORDER];
    int num_keys;
} CustomerTreeNode;

// SalesPerson structure with monthly sales tracking
typedef struct {
    int id;
    char name[50];
    float salesTarget; // 50 lakhs/month
    float salesAchieved; // Current month sales
    float totalSales; // Total sales across all months
    float commission; // 2% of sales achieved
    SoldCarTreeNode* sold_car_root;
    CustomerTreeNode *customer_root;
} SalesPerson;

typedef struct SalesTreeNode {
    SalesPerson keys[MAX_KEYS];
    struct SalesTreeNode *children[ORDER];
    int num_keys;
} SalesTreeNode;

typedef struct {
    char car_model[20];
    int sold_cars;
    int available_cars;
} StockDetails;

typedef struct {
    char model[50];
    int count;
} ModelCount;

// Updated: customer_root is now of type CustomerTreeNode *
typedef struct Showroom {
    int showroom_id;
    SalesTreeNode *sales_root;
    AvailableCarTreeNode *available_car_root;
    SoldCarTreeNode *sold_car_root;
    StockDetails stock[MODELS];
    CustomerTreeNode *customer_root;
} Showroom;

SoldCarTreeNode *create_node_sold_car()
{
    SoldCarTreeNode *node = (SoldCarTreeNode *)malloc(sizeof(SoldCarTreeNode));
    node->num_keys = 0;
    for (int i = 0; i < ORDER; i++)
    {
        node->children[i] = NULL;
    }
    return node;
}

void traverse_sold_car(SoldCarTreeNode *node)
{
    if (node == NULL)
        return;

    int i;
    for (i = 0; i < node->num_keys; i++)
    {
        if (node->children[0])
        {
            traverse_sold_car(node->children[i]);
        }
        printf("%d ", node->keys[i].VIN);
    }

    if (node->children[0])
    {
        traverse_sold_car(node->children[i]);
    }
}

void split_child_sold_car(SoldCarTreeNode *parent, int child_index)
{
    SoldCarTreeNode *child = parent->children[child_index];
    SoldCarTreeNode *new_child = create_node_sold_car();
    int mid_index = MAX_KEYS / 2;

    new_child->num_keys = MAX_KEYS - mid_index - 1;
    for (int i = 0; i < new_child->num_keys; i++)
    {
        new_child->keys[i] = child->keys[mid_index + 1 + i];
    }

    if (child->children[0])
    {
        for (int i = 0; i <= new_child->num_keys; i++)
        {
            new_child->children[i] = child->children[mid_index + 1 + i];
        }
    }

    child->num_keys = mid_index;

    for (int i = parent->num_keys; i > child_index; i--)
    {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }

    parent->children[child_index + 1] = new_child;
    parent->keys[child_index] = child->keys[mid_index];
    parent->num_keys++;
}

void insert_non_full_sold_car(SoldCarTreeNode *node, Car key)
{
    int i = node->num_keys - 1;

    if (node->children[0] == NULL)
    {
        while (i >= 0 && key.VIN < node->keys[i].VIN)
        {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    }
    else
    {
        while (i >= 0 && key.VIN < node->keys[i].VIN)
        {
            i--;
        }
        i++;

        if (node->children[i]->num_keys == MAX_KEYS)
        {
            split_child_sold_car(node, i);
            if (key.VIN > node->keys[i].VIN)
            {
                i++;
            }
        }
        insert_non_full_sold_car(node->children[i], key);
    }
}

void insert_sold_car(SoldCarTreeNode **root, Car key)
{
    if (*root == NULL)
    {
        *root = create_node_sold_car();
        (*root)->keys[0] = key;
        (*root)->num_keys = 1;
        return;
    }

    if ((*root)->num_keys == MAX_KEYS)
    {
        SoldCarTreeNode *new_root = create_node_sold_car();
        new_root->children[0] = *root;
        split_child_sold_car(new_root, 0);
        *root = new_root;
    }
    insert_non_full_sold_car(*root, key);
}

AvailableCarTreeNode *create_node_available_car()
{
    AvailableCarTreeNode *node = (AvailableCarTreeNode *)malloc(sizeof(AvailableCarTreeNode));
    node->num_keys = 0;
    for (int i = 0; i < ORDER; i++)
    {
        node->children[i] = NULL;
    }
    return node;
}

void traverse_available_car(AvailableCarTreeNode *node)
{
    if (node == NULL)
        return;

    int i;
    for (i = 0; i < node->num_keys; i++)
    {
        if (node->children[0])
        {
            traverse_available_car(node->children[i]);
        }
        printf("%d ", node->keys[i].VIN);
    }

    if (node->children[0])
    {
        traverse_available_car(node->children[i]);
    }
}

void split_child_available_car(AvailableCarTreeNode *parent, int child_index)
{
    AvailableCarTreeNode *child = parent->children[child_index];
    AvailableCarTreeNode *new_child = create_node_available_car();
    int mid_index = MAX_KEYS / 2;

    new_child->num_keys = MAX_KEYS - mid_index - 1;
    for (int i = 0; i < new_child->num_keys; i++)
    {
        new_child->keys[i] = child->keys[mid_index + 1 + i];
    }

    if (child->children[0])
    {
        for (int i = 0; i <= new_child->num_keys; i++)
        {
            new_child->children[i] = child->children[mid_index + 1 + i];
        }
    }

    child->num_keys = mid_index;

    for (int i = parent->num_keys; i > child_index; i--)
    {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }

    parent->children[child_index + 1] = new_child;
    parent->keys[child_index] = child->keys[mid_index];
    parent->num_keys++;
}

void insert_non_full_available_car(AvailableCarTreeNode *node, Car key)
{
    int i = node->num_keys - 1;

    if (node->children[0] == NULL)
    {
        while (i >= 0 && key.VIN < node->keys[i].VIN)
        {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    }
    else
    {
        while (i >= 0 && key.VIN < node->keys[i].VIN)
        {
            i--;
        }
        i++;

        if (node->children[i]->num_keys == MAX_KEYS)
        {
            split_child_available_car(node, i);
            if (key.VIN > node->keys[i].VIN)
            {
                i++;
            }
        }
        insert_non_full_available_car(node->children[i], key);
    }
}

void insert_available_car(AvailableCarTreeNode **root, Car key)
{
    if (*root == NULL)
    {
        *root = create_node_available_car();
        (*root)->keys[0] = key;
        (*root)->num_keys = 1;
        return;
    }

    if ((*root)->num_keys == MAX_KEYS)
    {
        AvailableCarTreeNode *new_root = create_node_available_car();
        new_root->children[0] = *root;
        split_child_available_car(new_root, 0);
        *root = new_root;
    }
    insert_non_full_available_car(*root, key);
}

CustomerTreeNode *create_node_customer()
{
    CustomerTreeNode *node = (CustomerTreeNode *)malloc(sizeof(CustomerTreeNode));
    node->num_keys = 0;
    for (int i = 0; i < ORDER; i++)
    {
        node->children[i] = NULL;
    }
    return node;
}

void traverse_customer(CustomerTreeNode *node)
{
    if (node == NULL)
        return;

    int i;
    for (i = 0; i < node->num_keys; i++)
    {
        if (node->children[0])
        {
            traverse_customer(node->children[i]);
        }
        printf("%d ", node->keys[i].id);
    }

    if (node->children[0])
    {
        traverse_customer(node->children[i]);
    }
}

void split_child_customer(CustomerTreeNode *parent, int child_index)
{
    CustomerTreeNode *child = parent->children[child_index];
    CustomerTreeNode *new_child = create_node_customer();
    int mid_index = MAX_KEYS / 2;

    new_child->num_keys = MAX_KEYS - mid_index - 1;
    for (int i = 0; i < new_child->num_keys; i++)
    {
        new_child->keys[i] = child->keys[mid_index + 1 + i];
    }

    if (child->children[0])
    {
        for (int i = 0; i <= new_child->num_keys; i++)
        {
            new_child->children[i] = child->children[mid_index + 1 + i];
        }
    }

    child->num_keys = mid_index;

    for (int i = parent->num_keys; i > child_index; i--)
    {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }

    parent->children[child_index + 1] = new_child;
    parent->keys[child_index] = child->keys[mid_index];
    parent->num_keys++;
}

void insert_non_full_customer(CustomerTreeNode *node, Customer key)
{
    int i = node->num_keys - 1;

    if (node->children[0] == NULL)
    {
        while (i >= 0 && key.id < node->keys[i].id)
        {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    }
    else
    {
        while (i >= 0 && key.id < node->keys[i].id)
        {
            i--;
        }
        i++;

        if (node->children[i]->num_keys == MAX_KEYS)
        {
            split_child_customer(node, i);
            if (key.id > node->keys[i].id)
            {
                i++;
            }
        }
        insert_non_full_customer(node->children[i], key);
    }
}

void insert_customer(CustomerTreeNode **root, Customer key)
{
    if (*root == NULL)
    {
        *root = create_node_customer();
        (*root)->keys[0] = key;
        (*root)->num_keys = 1;
        return;
    }

    if ((*root)->num_keys == MAX_KEYS)
    {
        CustomerTreeNode *new_root = create_node_customer();
        new_root->children[0] = *root;
        split_child_customer(new_root, 0);
        *root = new_root;
    }
    insert_non_full_customer(*root, key);
}

SalesTreeNode *create_node_sales_person()
{
    SalesTreeNode *node = (SalesTreeNode *)malloc(sizeof(SalesTreeNode));
    node->num_keys = 0;
    for (int i = 0; i < ORDER; i++)
    {
        node->children[i] = NULL;
    }
    return node;
}

void traverse_sales_person(SalesTreeNode *node)
{
    if (node == NULL)
        return;

    int i;
    for (i = 0; i < node->num_keys; i++)
    {
        if (node->children[0])
        {
            traverse_sales_person(node->children[i]);
        }
        printf("%d ", node->keys[i].id);
    }

    if (node->children[0])
    {
        traverse_sales_person(node->children[i]);
    }
}

void split_child_sales_person(SalesTreeNode *parent, int child_index)
{
    SalesTreeNode *child = parent->children[child_index];
    SalesTreeNode *new_child = create_node_sales_person();
    int mid_index = MAX_KEYS / 2;

    new_child->num_keys = MAX_KEYS - mid_index - 1;
    for (int i = 0; i < new_child->num_keys; i++)
    {
        new_child->keys[i] = child->keys[mid_index + 1 + i];
    }

    if (child->children[0])
    {
        for (int i = 0; i <= new_child->num_keys; i++)
        {
            new_child->children[i] = child->children[mid_index + 1 + i];
        }
    }

    child->num_keys = mid_index;

    for (int i = parent->num_keys; i > child_index; i--)
    {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }

    parent->children[child_index + 1] = new_child;
    parent->keys[child_index] = child->keys[mid_index];
    parent->num_keys++;
}

void insert_non_full_sales_person(SalesTreeNode *node, SalesPerson key)
{
    int i = node->num_keys - 1;

    if (node->children[0] == NULL)
    {
        while (i >= 0 && key.id < node->keys[i].id)
        {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    }
    else
    {
        while (i >= 0 && key.id < node->keys[i].id)
        {
            i--;
        }
        i++;

        if (node->children[i]->num_keys == MAX_KEYS)
        {
            split_child_sales_person(node, i);
            if (key.id > node->keys[i].id)
            {
                i++;
            }
        }
        insert_non_full_sales_person(node->children[i], key);
    }
}

void insert_sales_person(SalesTreeNode **root, SalesPerson key)
{
    if (*root == NULL)
    {
        *root = create_node_sales_person();
        (*root)->keys[0] = key;
        (*root)->num_keys = 1;
        return;
    }

    if ((*root)->num_keys == MAX_KEYS)
    {
        SalesTreeNode *new_root = create_node_sales_person();
        new_root->children[0] = *root;
        split_child_sales_person(new_root, 0);
        *root = new_root;
    }
    insert_non_full_sales_person(*root, key);
}

// Function to load available cars from file for a specific showroom
void load_available_cars_from_file(AvailableCarTreeNode **root, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    char line[256];
    fgets(line, sizeof(line), file);
    
    while (fgets(line, sizeof(line), file)) {
        Car car;
        char *token = strtok(line, "|");
        car.VIN = atoi(token);
        
        token = strtok(NULL, "|");
        strcpy(car.name, token);
        
        token = strtok(NULL, "|");
        strcpy(car.color, token);
        
        token = strtok(NULL, "|");
        car.price = atof(token);
        
        token = strtok(NULL, "|");
        strcpy(car.fuelType, token);
        
        token = strtok(NULL, "\n");
        strcpy(car.carType, token);
        
        car.isSold = false;
        car.customer_id = -1;
        car.salesperson_id = -1;
        strcpy(car.soldDate, "");
        
        insert_available_car(root, car);
    }
    fclose(file);
}

// Function to load sold cars from file for a specific showroom
void load_sold_cars_from_file(SoldCarTreeNode **root, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    char line[256];
    fgets(line, sizeof(line), file);
    
    while (fgets(line, sizeof(line), file)) {
        Car car;
        char *token = strtok(line, "|");
        car.VIN = atoi(token);
        
        token = strtok(NULL, "|");
        strcpy(car.name, token);
        
        token = strtok(NULL, "|");
        strcpy(car.color, token);
        
        token = strtok(NULL, "|");
        car.price = atof(token);
        
        token = strtok(NULL, "|");
        strcpy(car.fuelType, token);
        
        token = strtok(NULL, "|");
        strcpy(car.carType, token);
        
        token = strtok(NULL, "|");
        car.customer_id = atoi(token);
        
        token = strtok(NULL, "|");
        car.salesperson_id = atoi(token);
        
        token = strtok(NULL, "\n");
        strcpy(car.soldDate, token);
        
        car.isSold = true;
        
        insert_sold_car(root, car);
    }
    fclose(file);
}

// Function to load sales persons from file for a specific showroom
void load_sales_persons_from_file(SalesTreeNode **root, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    char line[256];
    fgets(line, sizeof(line), file);
    
    while (fgets(line, sizeof(line), file)) {
        SalesPerson sp;
        char *token = strtok(line, "|");
        sp.id = atoi(token);
        
        token = strtok(NULL, "|");
        strcpy(sp.name, token);
        
        token = strtok(NULL, "|");
        sp.salesTarget = atof(token);
        
        token = strtok(NULL, "|");
        sp.salesAchieved = atof(token);
        
        token = strtok(NULL, "|");
        sp.totalSales = atof(token);
        
        token = strtok(NULL, "\n");
        sp.commission = atof(token);
        
        sp.sold_car_root = NULL;
        sp.customer_root = NULL;
        
        insert_sales_person(root, sp);
    }
    fclose(file);
}

// Function to load customers from file for a specific showroom
void load_customers_from_file(CustomerTreeNode **root, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    char line[512];
    fgets(line, sizeof(line), file);
    
    while (fgets(line, sizeof(line), file)) {
        Customer customer;
        char *token = strtok(line, "|");
        customer.id = atoi(token);
        
        token = strtok(NULL, "|");
        strcpy(customer.name, token);
        
        token = strtok(NULL, "|");
        strcpy(customer.mobile, token);
        
        token = strtok(NULL, "|");
        strcpy(customer.address, token);
        
        token = strtok(NULL, "|");
        customer.soldCarVIN = atoi(token);
        
        token = strtok(NULL, "|");
        strcpy(customer.registrationNumber, token);
        
        token = strtok(NULL, "|");
        strcpy(customer.payment_type, token);
        
        token = strtok(NULL, "|");
        customer.downPayment = atof(token);
        
        token = strtok(NULL, "|");
        customer.loanMonths = atoi(token);
        
        token = strtok(NULL, "|");
        customer.interestRate = atof(token);
        
        token = strtok(NULL, "|");
        customer.monthlyEMI = atof(token);
        
        token = strtok(NULL, "\n");
        customer.loanAmount = atof(token);
        
        insert_customer(root, customer);
    }
    fclose(file);
}

// Function to load all data for a specific showroom
void load_showroom_data(Showroom *showroom, int showroom_num) {
    char filename[50];
    
    showroom->available_car_root = NULL;
    showroom->sold_car_root = NULL;
    showroom->sales_root = NULL;
    showroom->customer_root = NULL;
    
    snprintf(filename, sizeof(filename), "showroom%d_available_cars.txt", showroom_num);
    load_available_cars_from_file(&showroom->available_car_root, filename);
    
    snprintf(filename, sizeof(filename), "showroom%d_sold_cars.txt", showroom_num);
    load_sold_cars_from_file(&showroom->sold_car_root, filename);
    
    snprintf(filename, sizeof(filename), "showroom%d_salesperson.txt", showroom_num);
    load_sales_persons_from_file(&showroom->sales_root, filename);
    
    snprintf(filename, sizeof(filename), "showroom%d_customers.txt", showroom_num);
    load_customers_from_file(&showroom->customer_root, filename);
    
    for (int i = 0; i < MODELS; i++) {
        strcpy(showroom->stock[i].car_model, "");
        showroom->stock[i].sold_cars = 0;
        showroom->stock[i].available_cars = 0;
    }
}

MergedCarTreeNode *create_node_merged_car() {
    MergedCarTreeNode *node = (MergedCarTreeNode *)malloc(sizeof(MergedCarTreeNode));
    node->num_keys = 0;
    for (int i = 0; i < ORDER; i++) {
        node->children[i] = NULL;
    }
    return node;
}

void split_child_merged_car(MergedCarTreeNode *parent, int child_index) {
    MergedCarTreeNode *child = parent->children[child_index];
    MergedCarTreeNode *new_child = create_node_merged_car();
    int mid_index = MAX_KEYS / 2;

    new_child->num_keys = MAX_KEYS - mid_index - 1;
    for (int i = 0; i < new_child->num_keys; i++) {
        new_child->keys[i] = child->keys[mid_index + 1 + i];
    }
    if (child->children[0] != NULL) {
        for (int i = 0; i <= new_child->num_keys; i++) {
            new_child->children[i] = child->children[mid_index + 1 + i];
        }
    }
    child->num_keys = mid_index;

    for (int i = parent->num_keys; i > child_index; i--) {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }
    parent->children[child_index + 1] = new_child;
    parent->keys[child_index] = child->keys[mid_index];
    parent->num_keys++;
}

void insert_non_full_merged_car(MergedCarTreeNode *node, MergedCar key) {
    int i = node->num_keys - 1;
    // Since we sort by VIN, compare key.car.VIN.
    if (node->children[0] == NULL) {
        while (i >= 0 && key.car.VIN < node->keys[i].car.VIN) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    } else {
        while (i >= 0 && key.car.VIN < node->keys[i].car.VIN) {
            i--;
        }
        i++;
        if (node->children[i]->num_keys == MAX_KEYS) {
            split_child_merged_car(node, i);
            if (key.car.VIN > node->keys[i].car.VIN) {
                i++;
            }
        }
        insert_non_full_merged_car(node->children[i], key);
    }
}

void insert_merged_car(MergedCarTreeNode **root, MergedCar key) {
    if (*root == NULL) {
        *root = create_node_merged_car();
        (*root)->keys[0] = key;
        (*root)->num_keys = 1;
        return;
    }
    if ((*root)->num_keys == MAX_KEYS) {
        MergedCarTreeNode *new_root = create_node_merged_car();
        new_root->children[0] = *root;
        split_child_merged_car(new_root, 0);
        *root = new_root;
    }
    insert_non_full_merged_car(*root, key);
}

void traverse_merged_car(MergedCarTreeNode *node) {
    if (node == NULL)
        return;
    int i;
    for (i = 0; i < node->num_keys; i++) {
        if (node->children[0] != NULL)
            traverse_merged_car(node->children[i]);
        // Print VIN, model (name), showroom number, and isSold flag.
        printf("VIN: %d, Name: %s, Showroom: %d, isSold: %s\n",
               node->keys[i].car.VIN,
               node->keys[i].car.name,
               node->keys[i].showroom_id,
               node->keys[i].car.isSold ? "true" : "false");
    }
    if (node->children[0] != NULL)
        traverse_merged_car(node->children[i]);
}

// --- Helper callback functions to merge cars ---
// These functions traverse an existing B-Tree (available or sold) and insert each car into the merged tree.
// They take the showroom_id as an extra parameter.

void traverse_available_car_for_merge(AvailableCarTreeNode *node, int showroom_id, MergedCarTreeNode **mergedRoot) {
    if (node == NULL)
        return;
    int i;
    for (i = 0; i < node->num_keys; i++) {
        if (node->children[0] != NULL)
            traverse_available_car_for_merge(node->children[i], showroom_id, mergedRoot);
        MergedCar mcar;
        mcar.car = node->keys[i];
        mcar.showroom_id = showroom_id;
        insert_merged_car(mergedRoot, mcar);
    }
    if (node->children[0] != NULL)
        traverse_available_car_for_merge(node->children[i], showroom_id, mergedRoot);
}

void traverse_sold_car_for_merge(SoldCarTreeNode *node, int showroom_id, MergedCarTreeNode **mergedRoot) {
    if (node == NULL)
        return;
    int i;
    for (i = 0; i < node->num_keys; i++) {
        if (node->children[0] != NULL)
            traverse_sold_car_for_merge(node->children[i], showroom_id, mergedRoot);
        MergedCar mcar;
        mcar.car = node->keys[i];
        mcar.showroom_id = showroom_id;
        insert_merged_car(mergedRoot, mcar);
    }
    if (node->children[0] != NULL)
        traverse_sold_car_for_merge(node->children[i], showroom_id, mergedRoot);
}

// --- Merge function ---
// This function goes through each showroom's available and sold trees
// and inserts every car into a new merged B-Tree.
MergedCarTreeNode *merge_showrooms(Showroom showrooms[], int numShowrooms) {
    MergedCarTreeNode *mergedRoot = NULL;
    for (int i = 0; i < numShowrooms; i++) {
        // Merge available cars from showroom i
        traverse_available_car_for_merge(showrooms[i].available_car_root, showrooms[i].showroom_id, &mergedRoot);
        // Merge sold cars from showroom i
        traverse_sold_car_for_merge(showrooms[i].sold_car_root, showrooms[i].showroom_id, &mergedRoot);
    }
    return mergedRoot;
}



// Function to update the count for a given model name.
void update_model_count(ModelCount models[], int *numModels, const char *modelName) {
    for (int i = 0; i < *numModels; i++) {
        if (strcmp(models[i].model, modelName) == 0) {
            models[i].count++;
            return;
        }
    }
    // Not found: add a new entry
    strcpy(models[*numModels].model, modelName);
    models[*numModels].count = 1;
    (*numModels)++;
}

// Recursively traverse the merged B-Tree and update counts for sold cars.
void traverse_and_count_merged(MergedCarTreeNode *node, ModelCount models[], int *numModels) {
    if (node == NULL)
        return;
    
    int i;
    for (i = 0; i < node->num_keys; i++) {
        if (node->children[0] != NULL)
            traverse_and_count_merged(node->children[i], models, numModels);
        
        // Only count sold cars.
        if (node->keys[i].car.isSold) {
            update_model_count(models, numModels, node->keys[i].car.name);
        }
    }
    if (node->children[0] != NULL)
        traverse_and_count_merged(node->children[i], models, numModels);
}

void find_most_popular_car_model(MergedCarTreeNode *mergedRoot) {
    ModelCount models[MODELS];
    int numModels = 0;
    traverse_and_count_merged(mergedRoot, models, &numModels);
    
    if (numModels == 0) {
        printf("No sold car records found.\n");
        return;
    }
    
    // Find the model with the maximum count.
    int maxIndex = 0;
    for (int i = 1; i < numModels; i++) {
        if (models[i].count > models[maxIndex].count)
            maxIndex = i;
    }
    
    printf("\nMost Popular Car Model: %s (Sold %d times)\n", models[maxIndex].model, models[maxIndex].count);
}

// Function to find the best salesperson in a SalesTreeNode (B-Tree)
// It recursively traverses the tree and returns a SalesPerson with the highest salesAchieved.
SalesPerson find_top_in_sales_tree(SalesTreeNode *node) {
    SalesPerson bestCandidate;
    // Initialize with a very low salesAchieved so any real value wins.
    bestCandidate.salesAchieved = -1.0f;
    
    if (node == NULL)
        return bestCandidate;
    
    int i;
    // Traverse all keys and children in the node
    for (i = 0; i < node->num_keys; i++) {
        // If not a leaf, check the child subtree
        if (node->children[0] != NULL) {
            SalesPerson childCandidate = find_top_in_sales_tree(node->children[i]);
            if (childCandidate.salesAchieved > bestCandidate.salesAchieved)
                bestCandidate = childCandidate;
        }
        // Check the current key
        if (node->keys[i].salesAchieved > bestCandidate.salesAchieved)
            bestCandidate = node->keys[i];
    }
    // Check the last child (if any)
    if (node->children[0] != NULL) {
        SalesPerson childCandidate = find_top_in_sales_tree(node->children[i]);
        if (childCandidate.salesAchieved > bestCandidate.salesAchieved)
            bestCandidate = childCandidate;
    }
    return bestCandidate;
}

// Function to iterate over all showrooms and find the overall best salesperson.
SalesPerson find_top_sales_person(Showroom showrooms[], int numShowrooms) {
    SalesPerson overallBest;
    overallBest.salesAchieved = -1.0f;  // initialize with very low value
    
    for (int i = 0; i < numShowrooms; i++) {
        SalesPerson candidate = find_top_in_sales_tree(showrooms[i].sales_root);
        if (candidate.salesAchieved > overallBest.salesAchieved)
            overallBest = candidate;
    }
    return overallBest;
}

// Helper: Remove a car by VIN from the available-car tree (simplified for leaf nodes).
bool remove_car_from_available(AvailableCarTreeNode **root, int carVIN, Car *removedCar) {
    if (*root == NULL)
        return false;
    
    AvailableCarTreeNode *node = *root;
    // If leaf node:
    if (node->children[0] == NULL) {
        for (int i = 0; i < node->num_keys; i++) {
            if (node->keys[i].VIN == carVIN) {
                *removedCar = node->keys[i];
                // Shift remaining keys left.
                for (int j = i; j < node->num_keys - 1; j++) {
                    node->keys[j] = node->keys[j + 1];
                }
                node->num_keys--;
                return true;
            }
        }
        return false;
    }
    // If not a leaf, search in children.
    for (int i = 0; i <= node->num_keys; i++) {
        if (remove_car_from_available(&node->children[i], carVIN, removedCar))
            return true;
    }
    return false;
}

// Helper: Search for a salesperson in the sales B-Tree.
SalesPerson* find_sales_person(SalesTreeNode *node, int salespersonID) {
    if (node == NULL)
        return NULL;
    for (int i = 0; i < node->num_keys; i++) {
        if (node->keys[i].id == salespersonID)
            return &node->keys[i];
    }
    for (int i = 0; i <= node->num_keys; i++) {
        SalesPerson *result = find_sales_person(node->children[i], salespersonID);
        if (result != NULL)
            return result;
    }
    return NULL;
}

// Main function to process a car sale.
void sell_car(Showroom *showroom, int carVIN, int salespersonID, Customer customer) {
    Car carToSell;
    
    // Remove the car from the available inventory.
    bool found = remove_car_from_available(&showroom->available_car_root, carVIN, &carToSell);
    if (!found) {
        printf("Car with VIN %d not found in available inventory.\n", carVIN);
        return;
    }
    
    // Update the car's details to reflect the sale.
    carToSell.isSold = true;
    carToSell.customer_id = customer.id;
    carToSell.salesperson_id = salespersonID;
    // Set the sold date (here, using a fixed date; replace with dynamic date if needed)
    strcpy(carToSell.soldDate, "2025-04-02");
    
    // Insert the car into the sold-car tree.
    insert_sold_car(&showroom->sold_car_root, carToSell);
    
    // Find the salesperson and update their sales metrics.
    SalesPerson *sp = find_sales_person(showroom->sales_root, salespersonID);
    if (sp == NULL) {
        printf("Salesperson with ID %d not found.\n", salespersonID);
        return;
    }
    sp->salesAchieved += carToSell.price;
    sp->totalSales += carToSell.price;
    sp->commission = 0.02f * sp->salesAchieved;
    
    // Update the customer record with the sold car's VIN and insert into the customer tree.
    customer.soldCarVIN = carVIN;
    insert_customer(&showroom->customer_root, customer);
    
    // Update stock details: decrement available count and increment sold count for the car's model.
    for (int i = 0; i < MODELS; i++) {
        if (strcmp(showroom->stock[i].car_model, carToSell.name) == 0) {
            showroom->stock[i].available_cars--;
            showroom->stock[i].sold_cars++;
            break;
        }
    }
    
    printf("Car with VIN %d sold to Customer ID %d by Salesperson ID %d.\n",
           carVIN, customer.id, salespersonID);
}


// Example usage in main()
int main() {
    Showroom showrooms[3];
    
    for (int i = 0; i < 3; i++) {
        showrooms[i].showroom_id = i + 1;
        load_showroom_data(&showrooms[i], i + 1);
        
        printf("\n=== Showroom %d ===\n", i + 1);
        printf("Available Cars (VINs): ");
        traverse_available_car(showrooms[i].available_car_root);
        printf("\nSold Cars (VINs): ");
        traverse_sold_car(showrooms[i].sold_car_root);
        printf("\nSales Persons (IDs): ");
        traverse_sales_person(showrooms[i].sales_root);
        printf("\nCustomers (IDs): ");
        traverse_customer(showrooms[i].customer_root);
        printf("\n");
    }
    MergedCarTreeNode *mergedRoot = merge_showrooms(showrooms, 3);

    // Traverse the merged B-Tree and display VIN, model name, showroom number, and isSold flag.
    printf("\n--- Merged Car Data ---\n");
    traverse_merged_car(mergedRoot);

    find_most_popular_car_model(mergedRoot);

    SalesPerson topSalesPerson = find_top_sales_person(showrooms, 3);
    
    // Calculate extra incentive (1% of sales achieved)
    float extraIncentive = 0.01f * topSalesPerson.salesAchieved;
    
    // Print the result.
    printf("\nMost Successful Sales Person:\n");
    printf("ID: %d, Name: %s\n", topSalesPerson.id, topSalesPerson.name);
    printf("Sales Achieved: %.2f\n", topSalesPerson.salesAchieved);
    printf("Extra Incentive (1%% bonus): %.2f\n", extraIncentive);

    return 0;
}
