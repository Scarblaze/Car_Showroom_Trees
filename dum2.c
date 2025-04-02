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

// Function to load available cars from file for a specific showroom
void load_available_cars_from_file(AvailableCarTreeNode **root, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    char line[256];
    // Skip header line
    fgets(line, sizeof(line), file);
    
    while (fgets(line, sizeof(line), file) {
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
        
        // Set default values for unsold cars
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
    // Skip header line
    fgets(line, sizeof(line), file);
    
    while (fgets(line, sizeof(line), file) {
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
    // Skip header line
    fgets(line, sizeof(line), file);
    
    while (fgets(line, sizeof(line), file) {
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
        
        // Initialize tree pointers
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

// Function to load all data for a specific showroom
void load_showroom_data(Showroom *showroom, int showroom_num) {
    char filename[50];
    
    // Initialize all roots to NULL
    showroom->available_car_root = NULL;
    showroom->sold_car_root = NULL;
    showroom->sales_root = NULL;
    showroom->customer_root = NULL;
    
    // Load available cars
    snprintf(filename, sizeof(filename), "showroom%d_available_cars.txt", showroom_num);
    load_available_cars_from_file(&showroom->available_car_root, filename);
    
    // Load sold cars
    snprintf(filename, sizeof(filename), "showroom%d_sold_cars.txt", showroom_num);
    load_sold_cars_from_file(&showroom->sold_car_root, filename);
    
    // Load sales persons
    snprintf(filename, sizeof(filename), "showroom%d_salesperson.txt", showroom_num);
    load_sales_persons_from_file(&showroom->sales_root, filename);
    
    // Load customers
    snprintf(filename, sizeof(filename), "showroom%d_customers.txt", showroom_num);
    load_customers_from_file(&showroom->customer_root, filename);
    
    // Initialize stock details (you may want to load this from file too)
    for (int i = 0; i < MODELS; i++) {
        strcpy(showroom->stock[i].car_model, "");
        showroom->stock[i].sold_cars = 0;
        showroom->stock[i].available_cars = 0;
    }
}

// Example usage in main()
int main() {
    Showroom showrooms[3];
    
    // Load data for all three showrooms
    for (int i = 0; i < 3; i++) {
        showrooms[i].showroom_id = i + 1;
        load_showroom_data(&showrooms[i], i + 1);
        
        // Print loaded data
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
    
    return 0;
}