# 🚗 Car Showroom Management System (Using B-Trees)

This is a C-based simulation of a Car Showroom Management System implemented entirely using **B-Trees** for efficient record management and search operations.

---

## 📌 Problem Overview

The system maintains and manages the following entities using **B-Trees**:

1. **Car Stock (Available & Sold Cars)** — Indexed by `VIN`
2. **Salespersons** — Indexed by `Salesperson ID`
3. **Customers** — Indexed by `Customer ID`
4. **Sold Car Records** — Indexed by `VIN`

Each showroom, salesperson, customer, and vehicle is uniquely identified. The system supports all major operations like buying/selling cars, updating stock, predicting showroom sales, and searching across B-Trees.

---

## 🛠 Features

### 🚘 Cars
- Each car has:
  - `VIN` (Primary Key)
  - `Name`, `Colour`, `Price`, `Fuel Type`, `Car Type (Hatchback/Sedan/SUV)`
- Maintains:
  - **Available Cars Tree**
  - **Sold Cars Tree**
  - Increment/decrement stock on sale

### 🧑‍💼 Salespersons
- Each has:
  - `Salesperson ID` (Primary Key)
  - `Name`
  - Monthly sales target (₹50 Lakhs)
  - Achieved sales (in ₹ Lakhs)
  - Commission (2% of achieved sales)
- **Salesperson Tree** maintained per showroom

### 👥 Customers
- Each customer record includes:
  - `Name`, `Mobile`, `Address`
  - `Sold Car VIN`, `Car Registration Number`
  - `Payment Type` (Cash/Loan)
  - EMI info (if applicable)
- Maintains a separate **Customer Tree**

---

## 💸 Loan/EMI Criteria

- **Down payment** must be > 20% of car price
- **Interest Rates**:
  - 9.00% for 84 months
  - 8.75% for 60 months
  - 8.50% for 36 months

---

## 🧮 Core Functionalities

### A. `mergeShowrooms()`  
Merge all three showroom B-Trees and sort by `VIN`.

### B. `addSalesperson(details)`  
Add a new salesperson with given info.

### C. `mostPopularCar()`  
Identify and return the most sold car across all showrooms.

### D. `topSalesperson()`  
Find the most successful salesperson based on sales achieved. Award extra 1% incentive and display total.

### E. `sellCar(salesperson_id, customer_details)`  
Complete a car sale transaction and update all related B-Trees:
- Decrease available cars
- Add to sold cars
- Update salesperson's sales
- Add customer record
- Create loan record if needed

### F. `predictNextMonthSales()`  
Estimate next month’s sales using last month’s data (simple trend projection).

### G. `getCarByVIN(VIN)`  
Display complete details of a car (whether sold or available).

### H. `searchSalespersonBySales(min_sales, max_sales)`  
Find salespersons whose sales lie in the given range.

### I. `listCustomersByEMI()`  
List customers with EMI plans > 36 months and < 48 months.

---

## 🧱 Data Structures

- All major entities are managed via **B-Trees** for fast insertion and deletion.
- Custom implementation of B-Trees for:
  - Cars (by `VIN`)
  - Customers (by `Customer ID`)
  - Salespersons (by `Salesperson ID`)

---

## 🗃 File Handling

- Initial dataset populated from files:
  - `showroom1_available_cars.txt`, `showroom1_sold_cars.txt`, `showroom1_customers.txt`, `showroom1_salesperson.txt`
  - There are 3 such showrooms
- Supports persistent read/write to simulate real-time data

---

