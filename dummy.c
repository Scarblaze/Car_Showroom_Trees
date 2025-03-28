#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdbool.h>

#define ORDER 4

typedef struct
{
    int VIN;
    char name[50];
    char color[20];
    double price;
    char fuelType[10];
    char carType[10];
    bool isSold;
} Car;

typedef struct CarTreeNode {
    int keys[ORDER-1];
    Car car; 
    struct BTreeNode *children_ptr[ORDER];
    int numKeys; 
    int leaf;    
} CarTreeNode;

typedef struct SoldCarTreeNode {
    int keys[ORDER-1];
    Car car; 
    int customer_id;
    int salesperson_id;
    struct BTreeNode *children_ptr[ORDER];
    int numKeys; 
    int leaf;    
} SoldCarTreeNode;

typedef struct AvailableCarTreeNode {
    int keys[ORDER-1];
    Car car; 
    struct BTreeNode *children_ptr[ORDER];
    int numKeys; 
    int leaf;    
} AvailbaleCarTreeNode;

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
};

