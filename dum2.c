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

typedef struct Showroom {
    int showroom_id;
    SalesTreeNode *sales_root;
    AvailableCarTreeNode *available_car_root;
    SoldCarTreeNode *sold_car_root;
    StockDetails stock[MODELS];
    Customer *customer_root;
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

    // Traverse the last child
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

    // Copy upper half of child to new_child
    new_child->num_keys = MAX_KEYS - mid_index - 1;
    for (int i = 0; i < new_child->num_keys; i++)
    {
        new_child->keys[i] = child->keys[mid_index + 1 + i];
    }

    // Copy children if not leaf
    if (child->children[0])
    {
        for (int i = 0; i <= new_child->num_keys; i++)
        {
            new_child->children[i] = child->children[mid_index + 1 + i];
        }
    }

    // Reduce keys in original child
    child->num_keys = mid_index;

    // Make space in parent for new child
    for (int i = parent->num_keys; i > child_index; i--)
    {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }

    // Move middle key to parent and link new child
    parent->children[child_index + 1] = new_child;
    parent->keys[child_index] = child->keys[mid_index];
    parent->num_keys++;
}

void insert_non_full_sold_car(SoldCarTreeNode *node, Car key)
{
    int i = node->num_keys - 1;

    // leaf
    if (node->children[0] == NULL)
    {
        // Shift keys and insert
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
        // Find child to insert into
        while (i >= 0 && key.VIN < node->keys[i].VIN)
        {
            i--;
        }
        i++; // because you have to go to child. i was key index until now

        // Split if child is full
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

SoldCarTreeNode *create_node_available_car()
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

    // Traverse the last child
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

    // Copy upper half of child to new_child
    new_child->num_keys = MAX_KEYS - mid_index - 1;
    for (int i = 0; i < new_child->num_keys; i++)
    {
        new_child->keys[i] = child->keys[mid_index + 1 + i];
    }

    // Copy children if not leaf
    if (child->children[0])
    {
        for (int i = 0; i <= new_child->num_keys; i++)
        {
            new_child->children[i] = child->children[mid_index + 1 + i];
        }
    }

    // Reduce keys in original child
    child->num_keys = mid_index;

    // Make space in parent for new child
    for (int i = parent->num_keys; i > child_index; i--)
    {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }

    // Move middle key to parent and link new child
    parent->children[child_index + 1] = new_child;
    parent->keys[child_index] = child->keys[mid_index];
    parent->num_keys++;
}

void insert_non_full_available_car(AvailableCarTreeNode *node, Car key)
{
    int i = node->num_keys - 1;

    // leaf
    if (node->children[0] == NULL)
    {
        // Shift keys and insert
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
        // Find child to insert into
        while (i >= 0 && key.VIN < node->keys[i].VIN)
        {
            i--;
        }
        i++; // because you have to go to child. i was key index until now

        // Split if child is full
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

    // Traverse the last child
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

    // Copy upper half of child to new_child
    new_child->num_keys = MAX_KEYS - mid_index - 1;
    for (int i = 0; i < new_child->num_keys; i++)
    {
        new_child->keys[i] = child->keys[mid_index + 1 + i];
    }

    // Copy children if not leaf
    if (child->children[0])
    {
        for (int i = 0; i <= new_child->num_keys; i++)
        {
            new_child->children[i] = child->children[mid_index + 1 + i];
        }
    }

    // Reduce keys in original child
    child->num_keys = mid_index;

    // Make space in parent for new child
    for (int i = parent->num_keys; i > child_index; i--)
    {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }

    // Move middle key to parent and link new child
    parent->children[child_index + 1] = new_child;
    parent->keys[child_index] = child->keys[mid_index];
    parent->num_keys++;
}

void insert_non_full_customer(CustomerTreeNode *node, Customer key)
{
    int i = node->num_keys - 1;

    // leaf
    if (node->children[0] == NULL)
    {
        // Shift keys and insert
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
        // Find child to insert into
        while (i >= 0 && key.id < node->keys[i].id)
        {
            i--;
        }
        i++; // because you have to go to child. i was key index until now

        // Split if child is full
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

    // Traverse the last child
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

    // Copy upper half of child to new_child
    new_child->num_keys = MAX_KEYS - mid_index - 1;
    for (int i = 0; i < new_child->num_keys; i++)
    {
        new_child->keys[i] = child->keys[mid_index + 1 + i];
    }

    // Copy children if not leaf
    if (child->children[0])
    {
        for (int i = 0; i <= new_child->num_keys; i++)
        {
            new_child->children[i] = child->children[mid_index + 1 + i];
        }
    }

    // Reduce keys in original child
    child->num_keys = mid_index;

    // Make space in parent for new child
    for (int i = parent->num_keys; i > child_index; i--)
    {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }

    // Move middle key to parent and link new child
    parent->children[child_index + 1] = new_child;
    parent->keys[child_index] = child->keys[mid_index];
    parent->num_keys++;
}

void insert_non_full_sales_person(SalesTreeNode *node, SalesPerson key)
{
    int i = node->num_keys - 1;

    // leaf
    if (node->children[0] == NULL)
    {
        // Shift keys and insert
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
        // Find child to insert into
        while (i >= 0 && key.id < node->keys[i].id)
        {
            i--;
        }
        i++; // because you have to go to child. i was key index until now

        // Split if child is full
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

// Function to save sold cars to file
void save_sold_cars_to_file(SoldCarTreeNode *root, FILE *file) {
    if (root == NULL) return;

    int i;
    for (i = 0; i < root->num_keys; i++) {
        if (root->children[i] != NULL) {
            save_sold_cars_to_file(root->children[i], file);
        }
        fprintf(file, "%d|%s|%s|%.2f|%s|%s|%d|%d|%s\n",
                root->keys[i].VIN,
                root->keys[i].name,
                root->keys[i].color,
                root->keys[i].price,
                root->keys[i].fuelType,
                root->keys[i].carType,
                root->keys[i].customer_id,
                root->keys[i].salesperson_id,
                root->keys[i].soldDate);
    }
    if (root->children[i] != NULL) {
        save_sold_cars_to_file(root->children[i], file);
    }
}

// Function to load sold cars from file
void load_sold_cars_from_file(SoldCarTreeNode **root, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    char line[256];
    // Skip header line
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

// Function to save available cars to file
void save_available_cars_to_file(AvailableCarTreeNode *root, FILE *file) {
    if (root == NULL) return;

    int i;
    for (i = 0; i < root->num_keys; i++) {
        if (root->children[i] != NULL) {
            save_available_cars_to_file(root->children[i], file);
        }
        fprintf(file, "%d|%s|%s|%.2f|%s|%s\n",
                root->keys[i].VIN,
                root->keys[i].name,
                root->keys[i].color,
                root->keys[i].price,
                root->keys[i].fuelType,
                root->keys[i].carType);
    }
    if (root->children[i] != NULL) {
        save_available_cars_to_file(root->children[i], file);
    }
}

// Function to load available cars from file
void load_available_cars_from_file(AvailableCarTreeNode **root, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    char line[256];
    // Skip header line
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

// Function to save sales persons to file
void save_sales_persons_to_file(SalesTreeNode *root, FILE *file) {
    if (root == NULL) return;

    int i;
    for (i = 0; i < root->num_keys; i++) {
        if (root->children[i] != NULL) {
            save_sales_persons_to_file(root->children[i], file);
        }
        fprintf(file, "%d|%s|%.2f|%.2f|%.2f|%.2f\n",
                root->keys[i].id,
                root->keys[i].name,
                root->keys[i].salesTarget,
                root->keys[i].salesAchieved,
                root->keys[i].totalSales,
                root->keys[i].commission);
    }
    if (root->children[i] != NULL) {
        save_sales_persons_to_file(root->children[i], file);
    }
}

// Function to load sales persons from file
void load_sales_persons_from_file(SalesTreeNode **root, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    char line[256];
    // Skip header line
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

// Function to save customers to file
void save_customers_to_file(CustomerTreeNode *root, FILE *file) {
    if (root == NULL) return;

    int i;
    for (i = 0; i < root->num_keys; i++) {
        if (root->children[i] != NULL) {
            save_customers_to_file(root->children[i], file);
        }
        fprintf(file, "%d|%s|%s|%s|%d|%s|%s|%.2f|%d|%.2f|%.2f|%.2f\n",
                root->keys[i].id,
                root->keys[i].name,
                root->keys[i].mobile,
                root->keys[i].address,
                root->keys[i].soldCarVIN,
                root->keys[i].registrationNumber,
                root->keys[i].payment_type,
                root->keys[i].downPayment,
                root->keys[i].loanMonths,
                root->keys[i].interestRate,
                root->keys[i].monthlyEMI,
                root->keys[i].loanAmount);
    }
    if (root->children[i] != NULL) {
        save_customers_to_file(root->children[i], file);
    }
}

// Function to load customers from file
void load_customers_from_file(CustomerTreeNode **root, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    char line[512];
    // Skip header line
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

// Function to save all showroom data to files
void save_showroom_data(Showroom *showroom) {
    FILE *file;
    
    // Save sold cars
    file = fopen("sold_cars.txt", "w");
    if (file == NULL) {
        printf("Error opening sold_cars.txt for writing\n");
        return;
    }
    fprintf(file, "VIN|name|color|price|fuelType|carType|customer_id|salesperson_id|soldDate\n");
    save_sold_cars_to_file(showroom->sold_car_root, file);
    fclose(file);
    
    // Save available cars
    file = fopen("available_cars.txt", "w");
    if (file == NULL) {
        printf("Error opening available_cars.txt for writing\n");
        return;
    }
    fprintf(file, "VIN|name|color|price|fuelType|carType\n");
    save_available_cars_to_file(showroom->available_car_root, file);
    fclose(file);
    
    // Save sales persons
    file = fopen("sales_persons.txt", "w");
    if (file == NULL) {
        printf("Error opening sales_persons.txt for writing\n");
        return;
    }
    fprintf(file, "id|name|salesTarget|salesAchieved|totalSales|commission\n");
    save_sales_persons_to_file(showroom->sales_root, file);
    fclose(file);
    
    // Save customers
    file = fopen("customers.txt", "w");
    if (file == NULL) {
        printf("Error opening customers.txt for writing\n");
        return;
    }
    fprintf(file, "id|name|mobile|address|soldCarVIN|registrationNumber|payment_type|downPayment|loanMonths|interestRate|monthlyEMI|loanAmount\n");
    save_customers_to_file(showroom->customer_root, file);
    fclose(file);
}

// Function to load all showroom data from files
void load_showroom_data(Showroom *showroom) {
    // Initialize all roots to NULL
    showroom->available_car_root = NULL;
    showroom->sold_car_root = NULL;
    showroom->sales_root = NULL;
    showroom->customer_root = NULL;
    
    // Load data from files
    load_available_cars_from_file(&showroom->available_car_root, "available_cars.txt");
    load_sold_cars_from_file(&showroom->sold_car_root, "sold_cars.txt");
    load_sales_persons_from_file(&showroom->sales_root, "sales_persons.txt");
    load_customers_from_file(&showroom->customer_root, "customers.txt");
}