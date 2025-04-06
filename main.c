#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include<time.h>

#define ORDER 5
#define MAX_KEYS (ORDER - 1)
#define MIN_KEYS (ceil(ORDER / 2) - 1)
#define MODELS 20
#define MAX_MONTHS 12

// Car structure
typedef struct
{
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

typedef struct SoldCarTreeNode
{
    Car keys[MAX_KEYS];
    struct SoldCarTreeNode *children[ORDER];
    int num_keys;
} SoldCarTreeNode;

typedef struct AvailableCarTreeNode
{
    Car keys[MAX_KEYS];
    struct AvailableCarTreeNode *children[ORDER];
    int num_keys;
} AvailableCarTreeNode;

typedef struct
{
    Car car;
    int showroom_id;
} MergedCar;

typedef struct MergedCarTreeNode
{
    MergedCar keys[MAX_KEYS];
    struct MergedCarTreeNode *children[ORDER];
    int num_keys;
} MergedCarTreeNode;

// Customer structure with loan details
typedef struct
{
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

typedef struct CustomerTreeNode
{
    Customer keys[MAX_KEYS];
    struct CustomerTreeNode *children[ORDER];
    int num_keys;
} CustomerTreeNode;

// SalesPerson structure with monthly sales tracking
typedef struct
{
    int id;
    char name[50];
    float salesTarget;   // 50 lakhs/month
    float salesAchieved; // Current month sales
    float totalSales;    // Total sales across all months
    float commission;    // 2% of sales achieved
    SoldCarTreeNode *sold_car_root;
    CustomerTreeNode *customer_root;
} SalesPerson;

typedef struct SalesTreeNode
{
    SalesPerson keys[MAX_KEYS];
    struct SalesTreeNode *children[ORDER];
    int num_keys;
} SalesTreeNode;

typedef struct
{
    char car_model[20];
    int sold_cars;
    int available_cars;
} StockDetails;

typedef struct
{
    char model[50];
    int count;
} ModelCount;

// Updated: customer_root is now of type CustomerTreeNode *
typedef struct Showroom
{
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
void load_available_cars_from_file(AvailableCarTreeNode **root, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file %s - creating empty inventory\n", filename);
        *root = NULL;
        return;
    }

    char line[256];
    if (fgets(line, sizeof(line), file) == NULL)
    { // Skip header
        fclose(file);
        *root = NULL;
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        // Trim trailing newline (works for both \n and \r\n)
        line[strcspn(line, "\r\n")] = 0;

        if (strlen(line) == 0)
            continue; // Skip empty lines

        Car car = {0}; // Initialize all fields to zero/null
        char *token = strtok(line, "|");
        if (!token)
            continue;
        car.VIN = atoi(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(car.name, token, sizeof(car.name) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(car.color, token, sizeof(car.color) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        car.price = atof(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(car.fuelType, token, sizeof(car.fuelType) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(car.carType, token, sizeof(car.carType) - 1);

        // Ensure null-termination
        car.name[sizeof(car.name) - 1] = '\0';
        car.color[sizeof(car.color) - 1] = '\0';
        car.fuelType[sizeof(car.fuelType) - 1] = '\0';
        car.carType[sizeof(car.carType) - 1] = '\0';

        car.isSold = false;
        car.customer_id = -1;
        car.salesperson_id = -1;
        car.soldDate[0] = '\0';

        insert_available_car(root, car);
    }
    printf("Available cars loaded\n");
    fclose(file);
}

// Function to load sold cars from file for a specific showroom
void load_sold_cars_from_file(SoldCarTreeNode **root, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file %s\n", filename);
        *root = NULL; // Initialize to empty tree
        return;
    }

    char line[256];
    if (fgets(line, sizeof(line), file) == NULL)
    { // Skip header
        fclose(file);
        *root = NULL;
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        // Trim newline characters
        line[strcspn(line, "\r\n")] = 0;

        if (strlen(line) == 0)
            continue; // Skip empty lines

        Car car = {0}; // Initialize all fields
        char *token = strtok(line, "|");
        if (!token)
            continue;
        car.VIN = atoi(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(car.name, token, sizeof(car.name) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(car.color, token, sizeof(car.color) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        car.price = atof(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(car.fuelType, token, sizeof(car.fuelType) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(car.carType, token, sizeof(car.carType) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        car.customer_id = atoi(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        car.salesperson_id = atoi(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(car.soldDate, token, sizeof(car.soldDate) - 1);

        // Ensure null-termination
        car.name[sizeof(car.name) - 1] = '\0';
        car.color[sizeof(car.color) - 1] = '\0';
        car.fuelType[sizeof(car.fuelType) - 1] = '\0';
        car.carType[sizeof(car.carType) - 1] = '\0';
        car.soldDate[sizeof(car.soldDate) - 1] = '\0';

        car.isSold = true;

        insert_sold_car(root, car);
    }
    fclose(file);
}
// Function to load sales persons from file for a specific showroom
void load_sales_persons_from_file(SalesTreeNode **root, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file %s\n", filename);
        *root = NULL;
        return;
    }

    char line[256];
    if (fgets(line, sizeof(line), file) == NULL)
    { // Skip header
        fclose(file);
        *root = NULL;
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0)
            continue;

        SalesPerson sp = {0};
        char *token = strtok(line, "|");
        if (!token)
            continue;
        sp.id = atoi(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(sp.name, token, sizeof(sp.name) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        sp.salesTarget = atof(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        sp.salesAchieved = atof(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        sp.totalSales = atof(token);

        token = strtok(NULL, "\n");
        if (token)
        { // Commission might be last field
            sp.commission = atof(token);
        }

        sp.name[sizeof(sp.name) - 1] = '\0';
        sp.sold_car_root = NULL;
        sp.customer_root = NULL;

        insert_sales_person(root, sp);
    }
    fclose(file);
}
// Function to load customers from file for a specific showroom
void load_customers_from_file(CustomerTreeNode **root, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file %s\n", filename);
        *root = NULL;
        return;
    }

    char line[512];
    if (fgets(line, sizeof(line), file) == NULL)
    { // Skip header
        fclose(file);
        *root = NULL;
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0)
            continue;

        Customer customer = {0};
        char *token = strtok(line, "|");
        if (!token)
            continue;
        customer.id = atoi(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(customer.name, token, sizeof(customer.name) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(customer.mobile, token, sizeof(customer.mobile) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(customer.address, token, sizeof(customer.address) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        customer.soldCarVIN = atoi(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(customer.registrationNumber, token, sizeof(customer.registrationNumber) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(customer.payment_type, token, sizeof(customer.payment_type) - 1);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        customer.downPayment = atof(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        customer.loanMonths = atoi(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        customer.interestRate = atof(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        customer.monthlyEMI = atof(token);

        token = strtok(NULL, "\n");
        if (token)
        {
            customer.loanAmount = atof(token);
        }

        // Ensure null-termination
        customer.name[sizeof(customer.name) - 1] = '\0';
        customer.mobile[sizeof(customer.mobile) - 1] = '\0';
        customer.address[sizeof(customer.address) - 1] = '\0';
        customer.registrationNumber[sizeof(customer.registrationNumber) - 1] = '\0';
        customer.payment_type[sizeof(customer.payment_type) - 1] = '\0';

        insert_customer(root, customer);
    }
    fclose(file);
}
// Function to load all data for a specific showroom
void load_showroom_data(Showroom *showroom, int showroom_num)
{
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

    for (int i = 0; i < MODELS; i++)
    {
        strcpy(showroom->stock[i].car_model, "");
        showroom->stock[i].sold_cars = 0;
        showroom->stock[i].available_cars = 0;
    }
}

MergedCarTreeNode *create_node_merged_car()
{
    MergedCarTreeNode *node = (MergedCarTreeNode *)malloc(sizeof(MergedCarTreeNode));
    node->num_keys = 0;
    for (int i = 0; i < ORDER; i++)
    {
        node->children[i] = NULL;
    }
    return node;
}

void split_child_merged_car(MergedCarTreeNode *parent, int child_index)
{
    MergedCarTreeNode *child = parent->children[child_index];
    MergedCarTreeNode *new_child = create_node_merged_car();
    int mid_index = MAX_KEYS / 2;

    new_child->num_keys = MAX_KEYS - mid_index - 1;
    for (int i = 0; i < new_child->num_keys; i++)
    {
        new_child->keys[i] = child->keys[mid_index + 1 + i];
    }
    if (child->children[0] != NULL)
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

void insert_non_full_merged_car(MergedCarTreeNode *node, MergedCar key)
{
    int i = node->num_keys - 1;
    // Since we sort by VIN, compare key.car.VIN.
    if (node->children[0] == NULL)
    {
        while (i >= 0 && key.car.VIN < node->keys[i].car.VIN)
        {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    }
    else
    {
        while (i >= 0 && key.car.VIN < node->keys[i].car.VIN)
        {
            i--;
        }
        i++;
        if (node->children[i]->num_keys == MAX_KEYS)
        {
            split_child_merged_car(node, i);
            if (key.car.VIN > node->keys[i].car.VIN)
            {
                i++;
            }
        }
        insert_non_full_merged_car(node->children[i], key);
    }
}

void insert_merged_car(MergedCarTreeNode **root, MergedCar key)
{
    if (*root == NULL)
    {
        *root = create_node_merged_car();
        (*root)->keys[0] = key;
        (*root)->num_keys = 1;
        return;
    }
    if ((*root)->num_keys == MAX_KEYS)
    {
        MergedCarTreeNode *new_root = create_node_merged_car();
        new_root->children[0] = *root;
        split_child_merged_car(new_root, 0);
        *root = new_root;
    }
    insert_non_full_merged_car(*root, key);
}

void traverse_merged_car(MergedCarTreeNode *node)
{
    if (node == NULL)
        return;
    int i;
    for (i = 0; i < node->num_keys; i++)
    {
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

void traverse_available_car_for_merge(AvailableCarTreeNode *node, int showroom_id, MergedCarTreeNode **mergedRoot)
{
    if (node == NULL)
        return;
    int i;
    for (i = 0; i < node->num_keys; i++)
    {
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

void traverse_sold_car_for_merge(SoldCarTreeNode *node, int showroom_id, MergedCarTreeNode **mergedRoot)
{
    if (node == NULL)
        return;
    int i;
    for (i = 0; i < node->num_keys; i++)
    {
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
MergedCarTreeNode *merge_showrooms(Showroom showrooms[], int numShowrooms)
{
    MergedCarTreeNode *mergedRoot = NULL;
    for (int i = 0; i < numShowrooms; i++)
    {
        // Merge available cars from showroom i
        traverse_available_car_for_merge(showrooms[i].available_car_root, showrooms[i].showroom_id, &mergedRoot);
        // Merge sold cars from showroom i
        traverse_sold_car_for_merge(showrooms[i].sold_car_root, showrooms[i].showroom_id, &mergedRoot);
    }
    return mergedRoot;
}

// Function to update the count for a given model name.
void update_model_count(ModelCount models[], int *numModels, const char *modelName)
{
    for (int i = 0; i < *numModels; i++)
    {
        if (strcmp(models[i].model, modelName) == 0)
        {
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
void traverse_and_count_merged(MergedCarTreeNode *node, ModelCount models[], int *numModels)
{
    if (node == NULL)
        return;

    int i;
    for (i = 0; i < node->num_keys; i++)
    {
        if (node->children[0] != NULL)
            traverse_and_count_merged(node->children[i], models, numModels);

        // Only count sold cars.
        if (node->keys[i].car.isSold)
        {
            update_model_count(models, numModels, node->keys[i].car.name);
        }
    }
    if (node->children[0] != NULL)
        traverse_and_count_merged(node->children[i], models, numModels);
}

void find_most_popular_car_model(MergedCarTreeNode *mergedRoot)
{
    ModelCount models[MODELS];
    int numModels = 0;
    traverse_and_count_merged(mergedRoot, models, &numModels);

    if (numModels == 0)
    {
        printf("No sold car records found.\n");
        return;
    }

    // Find the model with the maximum count.
    int maxIndex = 0;
    for (int i = 1; i < numModels; i++)
    {
        if (models[i].count > models[maxIndex].count)
            maxIndex = i;
    }

    printf("\nMost Popular Car Model: %s (Sold %d times)\n", models[maxIndex].model, models[maxIndex].count);
}

// Helper function to initialize a default SalesPerson
SalesPerson default_sales_person()
{
    SalesPerson sp = {0};
    sp.salesAchieved = -1.0f; // Impossible value for real data
    return sp;
}

// Revised recursive search function
SalesPerson find_top_in_sales_tree(SalesTreeNode *node)
{
    SalesPerson best = default_sales_person();

    if (!node)
        return best;

    // Check all children and keys
    for (int i = 0; i <= node->num_keys; i++)
    {
        // Process child first (if exists)
        if (i < node->num_keys + 1 && node->children[i])
        {
            SalesPerson child_best = find_top_in_sales_tree(node->children[i]);
            if (child_best.salesAchieved > best.salesAchieved)
            {
                best = child_best;
            }
        }

        // Process key (if exists)
        if (i < node->num_keys)
        {
            if (node->keys[i].salesAchieved > best.salesAchieved)
            {
                best = node->keys[i];
            }
        }
    }

    return best;
}

// Revised top salesperson finder
SalesPerson find_top_sales_person(Showroom *showrooms, int count)
{
    SalesPerson overall_best = default_sales_person();

    for (int i = 0; i < count; i++)
    {
        if (!showrooms[i].sales_root)
            continue;

        SalesPerson current = find_top_in_sales_tree(showrooms[i].sales_root);
        if (current.salesAchieved > overall_best.salesAchieved)
        {
            overall_best = current;
        }
    }

    return overall_best;
}

// Helper: Search for a salesperson in the sales B-Tree.
SalesPerson *find_sales_person(SalesTreeNode *node, int salespersonID)
{
    if (node == NULL)
        return NULL;
    for (int i = 0; i < node->num_keys; i++)
    {
        if (node->keys[i].id == salespersonID)
            return &node->keys[i];
    }
    for (int i = 0; i <= node->num_keys; i++)
    {
        SalesPerson *result = find_sales_person(node->children[i], salespersonID);
        if (result != NULL)
            return result;
    }
    return NULL;
}

// Borrow a key from the right sibling
// Helper to find the predecessor (rightmost key in left subtree)
Car get_predecessor(AvailableCarTreeNode *node, int pos)
{
    AvailableCarTreeNode *curr = node->children[pos];
    while (curr->children[0])
        curr = curr->children[curr->num_keys];
    return curr->keys[curr->num_keys - 1];
}

// Helper to find the successor (leftmost key in right subtree)
Car get_successor(AvailableCarTreeNode *node, int pos)
{
    AvailableCarTreeNode *curr = node->children[pos + 1];
    while (curr->children[0])
        curr = curr->children[0];
    return curr->keys[0];
}

// Borrow a key from the right sibling
void borrow_from_right(AvailableCarTreeNode *node, int idx)
{
    AvailableCarTreeNode *child = node->children[idx];
    AvailableCarTreeNode *sibling = node->children[idx + 1];

    // Move parent key to child
    child->keys[child->num_keys] = node->keys[idx];
    child->num_keys++;

    // Move sibling's first child to child
    if (child->children[0])
    {
        child->children[child->num_keys] = sibling->children[0];
    }

    // Update parent key
    node->keys[idx] = sibling->keys[0];

    // Shift sibling's keys left
    for (int i = 0; i < sibling->num_keys - 1; i++)
        sibling->keys[i] = sibling->keys[i + 1];

    // Shift sibling's children left
    if (sibling->children[0])
    {
        for (int i = 0; i < sibling->num_keys; i++)
            sibling->children[i] = sibling->children[i + 1];
    }

    sibling->num_keys--;
}

// Borrow a key from the left sibling
void borrow_from_left(AvailableCarTreeNode *node, int idx)
{
    AvailableCarTreeNode *child = node->children[idx];
    AvailableCarTreeNode *sibling = node->children[idx - 1];

    // Shift child's keys right
    for (int i = child->num_keys - 1; i >= 0; i--)
        child->keys[i + 1] = child->keys[i];

    // Move parent key to child
    child->keys[0] = node->keys[idx - 1];
    child->num_keys++;

    // Move sibling's last child to child
    if (child->children[0])
    {
        for (int i = child->num_keys; i > 0; i--)
            child->children[i] = child->children[i - 1];
        child->children[0] = sibling->children[sibling->num_keys];
    }

    // Update parent key
    node->keys[idx - 1] = sibling->keys[sibling->num_keys - 1];

    sibling->num_keys--;
}

// Merge child[idx] with child[idx+1]
void merge_nodes(AvailableCarTreeNode *node, int idx)
{
    AvailableCarTreeNode *child = node->children[idx];
    AvailableCarTreeNode *sibling = node->children[idx + 1];

    // Move parent key to child
    child->keys[child->num_keys] = node->keys[idx];
    child->num_keys++;

    // Copy keys from sibling
    for (int i = 0; i < sibling->num_keys; i++)
        child->keys[child->num_keys + i] = sibling->keys[i];

    // Copy children from sibling
    if (child->children[0])
    {
        for (int i = 0; i <= sibling->num_keys; i++)
            child->children[child->num_keys + i] = sibling->children[i];
    }

    // Shift parent keys left
    for (int i = idx; i < node->num_keys - 1; i++)
        node->keys[i] = node->keys[i + 1];

    // Shift parent children left
    for (int i = idx + 1; i < node->num_keys; i++)
        node->children[i] = node->children[i + 1];

    node->num_keys--;
    free(sibling);
}

// Fill underflowed child (borrow or merge)
void fill(AvailableCarTreeNode *node, int idx)
{
    if (idx != 0 && node->children[idx - 1]->num_keys >= MIN_KEYS)
        borrow_from_left(node, idx);
    else if (idx != node->num_keys && node->children[idx + 1]->num_keys >= MIN_KEYS)
        borrow_from_right(node, idx);
    else
    {
        if (idx != node->num_keys)
            merge_nodes(node, idx);
        else
            merge_nodes(node, idx - 1);
    }
}

bool remove_car_from_available(AvailableCarTreeNode **root, int carVIN, Car *removedCar);
void remove_from_non_leaf(AvailableCarTreeNode *node, int idx)
{
    Car k = node->keys[idx];

    if (node->children[idx]->num_keys >= MIN_KEYS)
    {
        Car pred = get_predecessor(node, idx);
        node->keys[idx] = pred;
        remove_car_from_available(&node->children[idx], pred.VIN, NULL);
    }
    else if (node->children[idx + 1]->num_keys >= MIN_KEYS)
    {
        Car succ = get_successor(node, idx);
        node->keys[idx] = succ;
        remove_car_from_available(&node->children[idx + 1], succ.VIN, NULL);
    }
    else
    {
        merge_nodes(node, idx);
        remove_car_from_available(&node->children[idx], k.VIN, NULL);
    }
}

// Main deletion function (fixed type consistency)
bool remove_car_from_available(AvailableCarTreeNode **root, int carVIN, Car *removedCar)
{
    if (!*root)
        return false;

    AvailableCarTreeNode *node = *root;
    int idx = 0;

    // Find key position
    while (idx < node->num_keys && carVIN > node->keys[idx].VIN)
        idx++;

    // Key found in this node
    if (idx < node->num_keys && node->keys[idx].VIN == carVIN)
    {
        if (removedCar)
            *removedCar = node->keys[idx];

        if (!node->children[0])
        { // Leaf node
            // Simple deletion from leaf
            for (int i = idx; i < node->num_keys - 1; i++)
                node->keys[i] = node->keys[i + 1];
            node->num_keys--;

            // Handle root underflow
            if (node == *root && node->num_keys == 0)
            {
                free(*root);
                *root = NULL;
            }
        }
        else
        { // Non-leaf node
            remove_from_non_leaf(node, idx);
        }
        return true;
    }

    // Key not found in this node
    if (!node->children[0])
        return false;

    // Recursively delete from child
    bool flag = (idx == node->num_keys);
    if (node->children[idx]->num_keys < MIN_KEYS)
        fill(node, idx);

    if (flag && idx > node->num_keys)
        return remove_car_from_available(&node->children[idx - 1], carVIN, removedCar);
    else
        return remove_car_from_available(&node->children[idx], carVIN, removedCar);
}

void traverse_and_write_available_cars(AvailableCarTreeNode *node, FILE *file)
{
    if (node == NULL)
        return;

    int i;
    for (i = 0; i < node->num_keys; i++)
    {
        // Traverse left child first (for in-order traversal)
        if (node->children[0])
        {
            traverse_and_write_available_cars(node->children[i], file);
        }
        // Write the current car to file
        fprintf(file, "%d|%s|%s|%.2f|%s|%s\n",
                node->keys[i].VIN,
                node->keys[i].name,
                node->keys[i].color,
                node->keys[i].price,
                node->keys[i].fuelType,
                node->keys[i].carType);
    }
    // Traverse the last child (rightmost)
    if (node->children[0])
    {
        traverse_and_write_available_cars(node->children[i], file);
    }
}

void update_available_cars_file(Showroom *showroom)
{
    char filename[50];
    sprintf(filename, "showroom%d_available_cars.txt", showroom->showroom_id);
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Failed to update available cars file");
        return;
    }

    // Write header
    fprintf(file, "VIN|name|color|price|fuelType|carType\n");

    // Write all cars using in-order traversal
    traverse_and_write_available_cars(showroom->available_car_root, file);

    fclose(file);
}

void update_sold_car_file(Showroom *showroom, Car soldCar)
{
    char filename[50];
    sprintf(filename, "showroom%d_sold_cars.txt", showroom->showroom_id);
    FILE *file = fopen(filename, "a");
    if (!file)
    {
        file = fopen(filename, "w");
        fprintf(file, "VIN|name|color|price|fuelType|carType|customer_id|salesperson_id|soldDate\n");
    }
    else
    {
        fseek(file, 0, SEEK_END);
        if (ftell(file) == 0)
            fprintf(file, "VIN|name|color|price|fuelType|carType|customer_id|salesperson_id|soldDate\n");
    }

    fprintf(file, "%d|%s|%s|%.2f|%s|%s|%d|%d|%s\n",
            soldCar.VIN, soldCar.name, soldCar.color, soldCar.price,
            soldCar.fuelType, soldCar.carType, soldCar.customer_id,
            soldCar.salesperson_id, soldCar.soldDate);
    fclose(file);
}

void update_customer_file(Showroom *showroom, Customer customer)
{
    char filename[50];
    sprintf(filename, "showroom%d_customers.txt", showroom->showroom_id);
    FILE *file = fopen(filename, "a");
    if (!file)
    {
        file = fopen(filename, "w");
        fprintf(file, "id|name|mobile|address|soldCarVIN|registrationNumber|payment_type|downPayment|loanMonths|interestRate|monthlyEMI|loanAmount\n");
    }
    else
    {
        fseek(file, 0, SEEK_END);
        if (ftell(file) == 0)
            fprintf(file, "id|name|mobile|address|soldCarVIN|registrationNumber|payment_type|downPayment|loanMonths|interestRate|monthlyEMI|loanAmount\n");
    }

    fprintf(file, "%d|%s|%s|%s|%d|%s|%s|%.2f|%d|%.1f|%.2f|%.2f\n",
            customer.id, customer.name, customer.mobile, customer.address,
            customer.soldCarVIN, customer.registrationNumber, customer.payment_type,
            customer.downPayment, customer.loanMonths, customer.interestRate,
            customer.monthlyEMI, customer.loanAmount);
    fclose(file);
}

void update_salesperson_file(Showroom *showroom, SalesPerson *updatedSp)
{
    char filename[50];
    sprintf(filename, "showroom%d_salesperson.txt", showroom->showroom_id);

    // Create a temporary file for writing
    char tempFilename[60];
    sprintf(tempFilename, "showroom%d_salesperson_temp.txt", showroom->showroom_id);
    FILE *tempFile = fopen(tempFilename, "w");
    if (!tempFile)
    {
        perror("Failed to create temporary salesperson file");
        return;
    }

    // Write header
    fprintf(tempFile, "id|name|salesTarget|salesAchieved|totalSales|commission\n");

    // Traverse the sales tree and write all records
    SalesTreeNode *node = showroom->sales_root;
    if (node)
    {
        // Stack for iterative traversal
        SalesTreeNode *stack[100];
        int top = -1;
        stack[++top] = node;

        while (top >= 0)
        {
            node = stack[top--];

            // Process current node's keys
            for (int i = 0; i < node->num_keys; i++)
            {
                // Write left child first if exists
                if (node->children[i])
                {
                    stack[++top] = node->children[i];
                }

                // Write the salesperson record
                SalesPerson *sp = &node->keys[i];
                if (sp->id == updatedSp->id)
                {
                    // Use the updated values for the matching salesperson
                    fprintf(tempFile, "%d|%s|%.2f|%.2f|%.2f|%.2f\n",
                            updatedSp->id,
                            updatedSp->name,
                            updatedSp->salesTarget,
                            updatedSp->salesAchieved,
                            updatedSp->totalSales,
                            updatedSp->commission);
                }
                else
                {
                    // Write original values for other salespersons
                    fprintf(tempFile, "%d|%s|%.2f|%.2f|%.2f|%.2f\n",
                            sp->id,
                            sp->name,
                            sp->salesTarget,
                            sp->salesAchieved,
                            sp->totalSales,
                            sp->commission);
                }
            }

            // Push rightmost child if exists
            if (node->children[node->num_keys])
            {
                stack[++top] = node->children[node->num_keys];
            }
        }
    }

    fclose(tempFile);

    // Replace the original file with the temporary file
    remove(filename);
    rename(tempFilename, filename);
}

void update_sales_person_in_tree(SalesTreeNode **root, SalesPerson updatedSp)
{
    if (*root == NULL)
    {
        return;
    }

    SalesTreeNode *current = *root;
    while (current != NULL)
    {
        int i;
        for (i = 0; i < current->num_keys; i++)
        {
            // If we found the salesperson to update
            if (current->keys[i].id == updatedSp.id)
            {
                // Update only the sales metrics (not the ID or name)
                current->keys[i].salesTarget = updatedSp.salesTarget;
                current->keys[i].salesAchieved = updatedSp.salesAchieved;
                current->keys[i].totalSales = updatedSp.totalSales;
                current->keys[i].commission = updatedSp.commission;
                return;
            }

            // If the ID we're looking for is smaller, go to left child
            if (updatedSp.id < current->keys[i].id)
            {
                break;
            }
        }

        // Move to appropriate child node
        current = current->children[i];
    }

    // If we get here, the salesperson wasn't found
    printf("Warning: Salesperson ID %d not found in tree for update\n", updatedSp.id);
}

// Finds a car in the available tree by VIN.
Car *find_in_available(AvailableCarTreeNode *root, int carVIN)
{
    // For simplicity, we do an in-order search.
    if (root == NULL)
        return NULL;
    for (int i = 0; i < root->num_keys; i++)
    {
        if (root->keys[i].VIN == carVIN)
        {
            return &root->keys[i];
        }
    }
    // In a non-leaf node, search children.
    for (int i = 0; i <= root->num_keys; i++)
    {
        Car *res = find_in_available(root->children[i], carVIN);
        if (res != NULL)
            return res;
    }
    return NULL;
}

// Main function to process a car sale.
void sell_car(Showroom *showroom, int carVIN, int salespersonID, Customer customer)
{
    Car carToSell;

    // Remove the car from the available inventory.
    bool found = remove_car_from_available(&showroom->available_car_root, carVIN, &carToSell);
    if (!found)
    {
        printf("Car with VIN %d not found in available inventory.\n", carVIN);
        return;
    }

    // Update the car's details to reflect the sale.
    carToSell.isSold = true;
    carToSell.customer_id = customer.id;
    carToSell.salesperson_id = salespersonID;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char date_str[11]; // YYYY-MM-DD + null terminator

    // Format the date as string
    strftime(date_str, sizeof(date_str), "%Y-%m-%d", t);

    // Use it in your car sale record
    strcpy(carToSell.soldDate, date_str);
    // strcpy(carToSell.soldDate, "2025-04-02");

    // Insert the car into the sold-car tree.
    insert_sold_car(&showroom->sold_car_root, carToSell);
    update_sold_car_file(showroom, carToSell);

    // Find the salesperson and update their sales metrics.
    SalesPerson *sp = find_sales_person(showroom->sales_root, salespersonID);
    if (sp == NULL)
    {
        printf("Salesperson with ID %d not found.\n", salespersonID);
        return;
    }

    // Create a copy with updated sales metrics
    SalesPerson updatedSp = *sp;
    updatedSp.salesAchieved += carToSell.price;
    updatedSp.totalSales += carToSell.price;
    updatedSp.commission = 0.02f * updatedSp.salesAchieved;

    // Update the tree (you'll need an update function in your B-tree implementation)
    update_sales_person_in_tree(&showroom->sales_root, updatedSp);

    // Update the file
    update_salesperson_file(showroom, &updatedSp);

    // Insert the customer record into the customer tree.
    insert_customer(&showroom->customer_root, customer);

    // Update stock details: decrement available count and increment sold count.
    for (int i = 0; i < MODELS; i++)
    {
        if (strcmp(showroom->stock[i].car_model, carToSell.name) == 0)
        {
            showroom->stock[i].available_cars--;
            showroom->stock[i].sold_cars++;
            break;
        }
    }

    // After removal, update available cars file.
    update_available_cars_file(showroom);

    printf("Car with VIN %d sold to Customer ID %d by Salesperson ID %d.\n",
           carVIN, customer.id, salespersonID);
}

// Function to search for a car in the sold cars B-Tree
Car *find_in_sold(SoldCarTreeNode *root, int vin)
{
    if (!root)
        return NULL;

    int i = 0;
    while (i < root->num_keys && vin > root->keys[i].VIN)
        i++;

    if (i < root->num_keys && vin == root->keys[i].VIN)
        return &root->keys[i];

    return find_in_sold(root->children[i], vin);
}

Customer *find_customer(CustomerTreeNode *root, int customer_id)
{
    if (!root)
        return NULL;

    int i = 0;
    while (i < root->num_keys && customer_id > root->keys[i].id)
        i++;

    if (i < root->num_keys && customer_id == root->keys[i].id)
        return &root->keys[i];

    return find_customer(root->children[i], customer_id);
}

// Function to display all car details (available or sold)
void display_car_info(Showroom *showrooms, int vin)
{
    printf("\n=== Car Information for VIN %d ===\n", vin);

    Car *car = NULL;
    bool is_sold = false;
    int showroom_number = -1;

    // Search in all showrooms for available cars
    for (int i = 0; i < 3; i++)
    {
        car = find_in_available(showrooms[i].available_car_root, vin);
        if (car)
        {
            showroom_number = showrooms[i].showroom_id;
            break;
        }
    }

    // If not found in available cars, search in sold cars
    if (!car)
    {
        for (int i = 0; i < 3; i++)
        {
            car = find_in_sold(showrooms[i].sold_car_root, vin);
            if (car)
            {
                is_sold = true;
                showroom_number = showrooms[i].showroom_id;
                break;
            }
        }
    }

    if (!car)
    {
        printf("Car not found in any showroom!\n");
        return;
    }

    // Display car details
    printf("Showroom:     %d\n", showroom_number);
    printf("Model:        %s\n", car->name);
    printf("Color:        %s\n", car->color);
    printf("Price:        Rs.%.2f\n", car->price);
    printf("Fuel Type:    %s\n", car->fuelType);
    printf("Category:     %s\n", car->carType);
    printf("Status:       %s\n", is_sold ? "Sold" : "Available");

    // If sold, display sale details
    if (is_sold)
    {
        printf("\n--- Sale Details ---\n");
        printf("Sold Date:    %s\n", car->soldDate);
        printf("Customer ID:  %d\n", car->customer_id);
        printf("Salesperson:  %d\n", car->salesperson_id);
    }
}

int print_emi_customers(CustomerTreeNode *root)
{
    int count = 0;
    if (root == NULL)
        return count;

    // Check current node's customers
    for (int i = 0; i < root->num_keys; i++)
    {
        Customer *c = &root->keys[i];
        if (strcmp(c->payment_type, "Loan") == 0 &&
            c->loanMonths >= 36 &&
            c->loanMonths <= 48)
        {

            printf("\nCustomer ID: %d", c->id);
            printf("\nName: %s", c->name);
            printf("\nMobile: %s", c->mobile);
            printf("\nVIN: %d", c->soldCarVIN);
            printf("\nEMI Duration: %d months", c->loanMonths);
            printf("\nMonthly EMI: Rs.%.2f", c->monthlyEMI);
            printf("\nLoan Amount: Rs.%.2f", c->loanAmount);
            printf("\n-----------------------");
            count++;
        }
    }

    // Recursively check child nodes
    for (int i = 0; i <= root->num_keys; i++)
    {
        count += print_emi_customers(root->children[i]);
    }

    return count;
}

// Search function using existing traversal logic
SalesPerson *search_sales_person(SalesTreeNode *node, int id)
{
    if (node == NULL)
        return NULL;

    int i = 0;
    while (i < node->num_keys && id > node->keys[i].id)
        i++;

    if (i < node->num_keys && id == node->keys[i].id)
        return &node->keys[i];

    if (node->children[0] == NULL)
        return NULL;

    return search_sales_person(node->children[i], id);
}

// Function to append new sales person to file
void append_sales_person(int showroom_id, SalesPerson *sp)
{
    char filename[50];
    sprintf(filename, "showroom%d_salesperson.txt", showroom_id);

    FILE *fp = fopen(filename, "a");
    if (fp == NULL)
    {
        printf("Error opening file for appending!\n");
        return;
    }

    // Write all fields in the correct order matching your file format
    fprintf(fp, "%d|%s|%.2f|%.2f|%.2f|%.2f\n",
            sp->id,
            sp->name,
            sp->salesTarget,
            sp->salesAchieved,
            sp->totalSales,
            sp->commission);

    fclose(fp);
}

// Complete add_sales_person function
void add_sales_person(Showroom showrooms[3])
{
    int showroom_num;
    printf("\nSelect Showroom (1-3): ");

    // Input validation for showroom number
    if (scanf("%d", &showroom_num) != 1 || showroom_num < 1 || showroom_num > 3)
    {
        printf("Invalid showroom selection!\n");
        while (getchar() != '\n')
            ; // Clear input buffer
        return;
    }
    getchar(); // Consume newline

    Showroom *sr = &showrooms[showroom_num - 1];
    SalesPerson sp;

    // ID Input and validation loop
    int valid_id = 0;
    while (!valid_id)
    {
        printf("Enter Sales Person ID: ");
        if (scanf("%d", &sp.id) != 1)
        {
            printf("Invalid ID format! Please enter a number.\n");
            while (getchar() != '\n')
                ;
            continue;
        }
        getchar(); // Consume newline

        // Check for existing ID
        SalesPerson *existing = search_sales_person(sr->sales_root, sp.id);
        if (existing != NULL)
        {
            printf("ID %d already exists! Try another ID.\n", sp.id);
        }
        else
        {
            valid_id = 1;
        }
    }

    // Name Input
    printf("Enter Name: ");
    fgets(sp.name, 50, stdin);
    sp.name[strcspn(sp.name, "\n")] = '\0'; // Remove newline

    // Initialize sales data
    sp.salesTarget = 5000000.0f; // 50 lakhs default
    sp.salesAchieved = 0.0f;
    sp.totalSales = 0.0f;
    sp.commission = 0.0f;
    sp.sold_car_root = NULL;
    sp.customer_root = NULL;

    // Insert into B-tree
    insert_sales_person(&sr->sales_root, sp);

    // Append to text file
    append_sales_person(showroom_num, &sp);

    // Success message
    printf("\nSales person added successfully!\n");
    printf("Showroom: %d\nID: %d\nName: %s\n",
           showroom_num, sp.id, sp.name);
    printf("Data saved to showroom%d_salesperson.txt\n", showroom_num);
}

// Available Cars B-Tree cleanup
void free_available_cars(AvailableCarTreeNode *root)
{
    if (root == NULL)
        return;

    // Recursively free all children
    for (int i = 0; i <= root->num_keys; i++)
    {
        free_available_cars(root->children[i]);
    }

    // Free current node
    free(root);
}

// Sold Cars B-Tree cleanup
void free_sold_cars(SoldCarTreeNode *root)
{
    if (root == NULL)
        return;

    for (int i = 0; i <= root->num_keys; i++)
    {
        free_sold_cars(root->children[i]);
    }

    free(root);
}

// Sales Persons B-Tree cleanup
void free_sales_persons(SalesTreeNode *root)
{
    if (root == NULL)
        return;

    for (int i = 0; i <= root->num_keys; i++)
    {
        free_sales_persons(root->children[i]);
    }

    free(root);
}

// Customers B-Tree cleanup
void free_customers(CustomerTreeNode *root)
{
    if (root == NULL)
        return;

    for (int i = 0; i <= root->num_keys; i++)
    {
        free_customers(root->children[i]);
    }

    free(root);
}

// Merged Cars B-Tree cleanup
void free_merged_cars(MergedCarTreeNode *root)
{
    if (root == NULL)
        return;

    for (int i = 0; i <= root->num_keys; i++)
    {
        free_merged_cars(root->children[i]);
    }

    free(root);
}

void search_salesperson_by_sales_range(SalesTreeNode *node, float min_sales, float max_sales)
{
    if (node == NULL)
        return;

    int i;
    for (i = 0; i < node->num_keys; i++)
    {
        if (node->children[0] != NULL)
        {
            search_salesperson_by_sales_range(node->children[i], min_sales, max_sales);
        }

        float sales = node->keys[i].salesAchieved;
        if (sales >= min_sales && sales <= max_sales)
        {
            printf("Salesperson ID: %d | Name: %s | Sales Achieved: %.2f\n",
                   node->keys[i].id,
                   node->keys[i].name,
                   node->keys[i].salesAchieved);
        }
    }

    if (node->children[0] != NULL)
    {
        search_salesperson_by_sales_range(node->children[i], min_sales, max_sales);
    }
}

void search_all_showrooms_by_sales_range(Showroom showrooms[], int count, float min_sales, float max_sales)
{
    for (int i = 0; i < count; i++)
    {
        printf("\n--- Searching Salespersons in Showroom %d ---\n", showrooms[i].showroom_id);
        search_salesperson_by_sales_range(showrooms[i].sales_root, min_sales, max_sales);
    }
}

int find_max_customer_id(CustomerTreeNode *root)
{
    if (root == NULL)
        return -1;
    int max_id = -1;
    for (int i = 0; i < root->num_keys; i++)
    {
        if (root->keys[i].id > max_id)
        {
            max_id = root->keys[i].id;
        }
    }
    if (root->children[0] != NULL)
    {
        int child_max = find_max_customer_id(root->children[root->num_keys]);
        if (child_max > max_id)
        {
            max_id = child_max;
        }
    }
    return max_id;
}

void process_car_sale(Showroom *showroom, int carVIN, int salespersonID,
                      const char *customerName, const char *mobile, const char *address,
                      const char *paymentType, float downPayment, int loanMonths, float interestRate)
{
    // Check if car is available
    Car *car = find_in_available(showroom->available_car_root, carVIN);
    if (car == NULL)
    {
        printf("Error: Car with VIN %d is not available for sale.\n", carVIN);
        return;
    }

    // Check if salesperson exists
    SalesPerson *sp = find_sales_person(showroom->sales_root, salespersonID);
    if (sp == NULL)
    {
        printf("Error: Salesperson with ID %d not found.\n", salespersonID);
        return;
    }

    // Validate payment type
    if (strcmp(paymentType, "Cash") != 0 && strcmp(paymentType, "Loan") != 0)
    {
        printf("Error: Invalid payment type. Must be 'Cash' or 'Loan'.\n");
        return;
    }

    // Validate down payment and loan details
    if (strcmp(paymentType, "Cash") == 0)
    {
        if (fabs(downPayment - car->price) > 0.01)
        {
            printf("Error: For Cash payment, down payment must equal car price (%.2f).\n", car->price);
            return;
        }
        loanMonths = 0;
        interestRate = 0.0f;
    }
    else
    {
        if (downPayment >= car->price)
        {
            printf("Error: Down payment must be less than car price for a loan.\n");
            return;
        }
        if (loanMonths <= 0 || loanMonths > MAX_MONTHS)
        {
            printf("Error: Loan months must be between 1 and %d.\n", MAX_MONTHS);
            return;
        }
        if (interestRate <= 0.0f)
        {
            printf("Error: Interest rate must be positive.\n");
            return;
        }
    }

    // Generate new customer ID
    int newCustomerID = find_max_customer_id(showroom->customer_root) + 1;

    // Populate customer details
    Customer newCustomer;
    newCustomer.id = newCustomerID;
    strncpy(newCustomer.name, customerName, sizeof(newCustomer.name) - 1);
    newCustomer.name[sizeof(newCustomer.name) - 1] = '\0';
    strncpy(newCustomer.mobile, mobile, sizeof(newCustomer.mobile) - 1);
    newCustomer.mobile[sizeof(newCustomer.mobile) - 1] = '\0';
    strncpy(newCustomer.address, address, sizeof(newCustomer.address) - 1);
    newCustomer.address[sizeof(newCustomer.address) - 1] = '\0';
    newCustomer.soldCarVIN = carVIN;
    strcpy(newCustomer.registrationNumber, "PENDING"); // Placeholder for registration
    strncpy(newCustomer.payment_type, paymentType, sizeof(newCustomer.payment_type) - 1);
    newCustomer.payment_type[sizeof(newCustomer.payment_type) - 1] = '\0';
    newCustomer.downPayment = downPayment;
    newCustomer.loanMonths = loanMonths;
    newCustomer.interestRate = interestRate;

    // Calculate EMI for loan
    if (strcmp(paymentType, "Loan") == 0)
    {
        float principal = car->price - downPayment;
        float monthlyInterest = interestRate / 12.0f / 100.0f;
        float emi = (principal * monthlyInterest * pow(1 + monthlyInterest, loanMonths)) /
                    (pow(1 + monthlyInterest, loanMonths) - 1);
        newCustomer.monthlyEMI = emi;
        newCustomer.loanAmount = principal;
    }
    else
    {
        newCustomer.monthlyEMI = 0.0f;
        newCustomer.loanAmount = 0.0f;
    }

    // Process the sale
    sell_car(showroom, carVIN, salespersonID, newCustomer);

    update_customer_file(showroom, newCustomer);

    printf("Sale processed successfully. Customer ID: %d\n", newCustomerID);
}

void countAndFreq(SoldCarTreeNode *node, int *count, int freq[])
{
    if (!node)
        return;

    int i;
    for (i = 0; i < node->num_keys; i++)
    {
        // Count the car
        (*count)++;

        // Get the month from soldDate[5] and soldDate[6]
        char month_str[3];
        month_str[0] = node->keys[i].soldDate[5];
        month_str[1] = node->keys[i].soldDate[6];
        month_str[2] = '\0';
        int month = atoi(month_str);

        if (month >= 1 && month <= 12)
            freq[month - 1] = 1;

        // Traverse left child if exists
        if (node->children[0])
            countAndFreq(node->children[i], count, freq);
    }

    // Traverse last child
    if (node->children[0])
        countAndFreq(node->children[i], count, freq);
}

void Prediction(SoldCarTreeNode *ptr1, SoldCarTreeNode *ptr2, SoldCarTreeNode *ptr3)
{
    SoldCarTreeNode *roots[3] = {ptr1, ptr2, ptr3};

    for (int i = 0; i < 3; i++)
    {
        int count = 0;
        int freq[12] = {0};

        countAndFreq(roots[i], &count, freq);

        int uniqueMonths = 0;
        for (int j = 0; j < 12; j++)
        {
            if (freq[j] == 1)
                uniqueMonths++;
        }

        double salesPerMonth = (uniqueMonths == 0) ? 0 : (double)count / uniqueMonths;

        salesPerMonth = ceil(salesPerMonth);

        printf("Showroom %d: Sales Prediction = %.2f\n",
               i + 1, salesPerMonth);
    }
}

int main()
{
    MergedCarTreeNode *mergedRoot = NULL;

    // Load showroom data
    printf("Initializing showrooms...\n");
    Showroom showrooms[3];

    // Initialize all showrooms
    for (int i = 0; i < 3; i++)
    {
        printf("Loading showroom %d...\n", i + 1);
        showrooms[i].showroom_id = i + 1;
        load_showroom_data(&showrooms[i], i + 1);
    }

    printf("Showroom data loaded successfully.\n");

    int choice;
    do
    {
        printf("\n=== Car Showroom Management System ===\n");
        printf("1. Display Individual Showroom Details\n");
        printf("2. Merge Showrooms and Display Car Data\n");
        printf("3. Find Most Popular Car Model\n");
        printf("4. Find Top Salesperson\n");
        printf("5. Search Car by VIN\n");
        printf("6. List Customers with EMI 36-48 Months\n");
        printf("7. Add New Sales Person\n");
        printf("8. Search Salesperson in Range\n");
        printf("9. Process Car Sale\n"); // New option
        printf("10. Sales Prediction\n");
        printf("11. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            for (int i = 0; i < 3; i++)
            {
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
            break;

        case 2:
            mergedRoot = merge_showrooms(showrooms, 3);
            printf("\n--- Merged Car Data ---\n");
            traverse_merged_car(mergedRoot);
            break;

        case 3:
            if (mergedRoot == NULL)
            {
                printf("Please merge showrooms first (Option 2)!\n");
                break;
            }
            find_most_popular_car_model(mergedRoot);
            break;

        case 4:
        {
            SalesPerson top = find_top_sales_person(showrooms, 3);
            if (top.salesAchieved >= 0)
            {
                printf("\nTop Salesperson \n");
                printf("ID: %d\nName: %s\nSales: Rs.%.2f\nCommission: Rs.%.2f\n",
                       top.id, top.name, top.salesAchieved, top.commission);
                float extraIncentive = 0.01f * top.salesAchieved;
                printf("Extra Incentive (1%% bonus): %.2f\n", extraIncentive);
            }
            else
            {
                printf("\nNo sales records found!\n");
            }
            break;
        }

        case 5:
        {
            int vin;
            printf("Enter VIN to search: ");
            scanf("%d", &vin);
            display_car_info(showrooms, vin);
            break;
        }

        case 6:
        {
            printf("\n--- Customers with EMI 36-48 Months ---\n");
            int found = 0;
            for (int i = 0; i < 3; i++)
            {
                printf("\nShowroom %d:\n", i + 1);
                found += print_emi_customers(showrooms[i].customer_root);
            }
            if (!found)
            {
                printf("\nNo customers found with EMI plans between 36-48 months.\n");
            }
            break;
        }

        case 7:
            add_sales_person(showrooms);
            break;

        case 8:
        {
            float min_sales, max_sales;
            printf("Enter minimum sales amount: ");
            if (scanf("%f", &min_sales) != 1)
            {
                printf("Invalid input for min_sales.\n");
                return 1;
            }

            printf("Enter maximum sales amount: ");
            if (scanf("%f", &max_sales) != 1)
            {
                printf("Invalid input for max_sales.\n");
                return 1;
            }

            if (min_sales > max_sales)
            {
                printf("Error: Minimum sales cannot be greater than maximum sales.\n");
                return 1;
            }
            search_all_showrooms_by_sales_range(showrooms, 3, min_sales, max_sales);
            break;
        }
        case 9:
        {
            int showroom_num, carVIN, salespersonID;
            char customerName[50], mobile[15], address[100], paymentType[20];
            float downPayment = 0.0f, interestRate = 0.0f;
            int loanMonths = 0;

            printf("Enter Showroom Number (1-3): ");
            scanf("%d", &showroom_num);
            if (showroom_num < 1 || showroom_num > 3)
            {
                printf("Invalid showroom number.\n");
                break;
            }
            Showroom *selected_showroom = &showrooms[showroom_num - 1];

            printf("Enter Car VIN: ");
            scanf("%d", &carVIN);

            printf("Enter Salesperson ID: ");
            scanf("%d", &salespersonID);

            printf("Enter Customer Name: ");
            scanf(" %49[^\n]", customerName); // Read up to 49 characters to avoid overflow

            printf("Enter Mobile Number: ");
            scanf(" %14s", mobile);

            printf("Enter Address: ");
            scanf(" %99[^\n]", address);

            printf("Enter Payment Type (Cash/Loan): ");
            scanf(" %19s", paymentType);

            if (strcmp(paymentType, "Loan") == 0)
            {
                printf("Enter Down Payment: ");
                scanf("%f", &downPayment);
                printf("Enter Loan Months (1-%d): ", MAX_MONTHS);
                scanf("%d", &loanMonths);
                printf("Enter Interest Rate (%%): ");
                scanf("%f", &interestRate);
            }

            // Process the sale
            process_car_sale(
                selected_showroom, carVIN, salespersonID,
                customerName, mobile, address,
                paymentType, downPayment, loanMonths, interestRate);
            break;
        }
        case 10:
        {
            SoldCarTreeNode *ptr1 = showrooms[0].sold_car_root;
            SoldCarTreeNode *ptr2 = showrooms[1].sold_car_root;
            SoldCarTreeNode *ptr3 = showrooms[2].sold_car_root;
            Prediction(ptr1, ptr2, ptr3);
            break;
        }

        case 11:
            printf("Exiting program...\n");
            break;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 11);

    for (int i = 0; i < 3; i++)
    {
        free_available_cars(showrooms[i].available_car_root);
        free_sold_cars(showrooms[i].sold_car_root);
        free_sales_persons(showrooms[i].sales_root);
        free_customers(showrooms[i].customer_root);
    }

    if (mergedRoot != NULL)
    {
        free_merged_cars(mergedRoot);
    }
    return 0;
}

// int main()
// {
//     Showroom showrooms[3];

//     for (int i = 0; i < 3; i++)
//     {
//         showrooms[i].showroom_id = i + 1;
//         load_showroom_data(&showrooms[i], i + 1);

//         printf("\n=== Showroom %d ===\n", i + 1);
//         printf("Available Cars (VINs): ");
//         traverse_available_car(showrooms[i].available_car_root);
//         printf("\nSold Cars (VINs): ");
//         traverse_sold_car(showrooms[i].sold_car_root);
//         printf("\nSales Persons (IDs): ");
//         traverse_sales_person(showrooms[i].sales_root);
//         printf("\nCustomers (IDs): ");
//         traverse_customer(showrooms[i].customer_root);
//         printf("\n");
//     }

//     MergedCarTreeNode *mergedRoot = merge_showrooms(showrooms, 3);

//     // Traverse the merged B-Tree and display VIN, model name, showroom number, and isSold flag.
//     printf("\n--- Merged Car Data ---\n");
//     traverse_merged_car(mergedRoot);

//     find_most_popular_car_model(mergedRoot);

//     SalesPerson top = find_top_sales_person(showrooms, 3);

//     if (top.salesAchieved >= 0)
//     {
//         printf("\nTop Salesperson \n");
//         printf("ID: %d\nName: %s\nSales: Rs.%.2f\nCommission: Rs.%.2f\n",
//                top.id, top.name, top.salesAchieved, top.commission);
//     }
//     else
//     {
//         printf("\n No sales records found!\n");
//     }

//     // // Calculate extra incentive (1% of sales achieved)
//     float extraIncentive = 0.01f * top.salesAchieved;
//     printf("Extra Incentive (1%% bonus): %.2f\n", extraIncentive);

//     int vin;
//     printf("Enter VIN to search: ");
//     scanf("%d", &vin);

//     display_car_info(showrooms, vin);
//     return 0;
// }
