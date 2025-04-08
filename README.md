Implementation of Car Management System, entirely in B-Trees.

Problem Statement:

Car Showroom Management System Using Trees 
Stock details in individual showroom: 
Number of car models with details like “Name of car, Colour of car, Price of car, Fuel type of 
car and Type of car” (Hatchback, Sedan or SUV). Maintain the VIN (Vehicle Identification 
Number) as a primary key for each car.  
1. Number of sold cars. 
2. Number of available cars. 
Note: Count of cars should increment and decrement in the tree of sold cars and available cars 
respectively as per the car sale. Maintain separate tree for car showroom. Root of tree should 
be unique id. 
Sales person in individual showroom: 
Sales person can login and search the details of the stock and customer. Sales person will 
maintain the record of sold car and its respective customer. Details of sales person is “Id and 
Name”. 
1. Sales person sales target. (50 lakhs rupees/Month) 
2. Sales person sales achieved. (Rupees in lakhs) 
3. Sales person commission is 2% of sales achieved. 
Note: Sales person will access data from stock details and customer details tree. Maintain 
separate tree for sales persons. Root of tree should be showroom id. 
Customer details in individual showroom: 
The sales person will keep record of the customer details like Name of customer, Mobile No. 
of customer, Address of customer, Sold car (VIN) and Car registration number in a tree. 
The sales person will also keep the record of sold cars like Car (VIN) and Payment type 
(Cash/Loan) in another tree. If car is purchased on loan, then following is the criteria. 
1. Down payment should be greater than 20% of car price. 
2. 9.00% rate of interest for 84 months EMI. 
3. 8.75% rate of interest for 60 months EMI. 
4. 8.50% rate of interest for 36 months EMI. 
Note: Maintain separate tree for customers. Root of tree should be sales person id. 
A. Assume there are three showrooms of different car manufacturer. Maintain the database 
for all and write a function with input name of to merge them together and sort by VIN. 
B. Write a function with input details of sales person to add new sales person. 
C. Find the most popular car among all three showrooms. 
D. Find the most successful sales person according to sales. Award him/her with 1% extra 
incentives as per sales achieved and print the same. 
E. Write a function with input details of sales person and customer to sale the car to 
customer. 
F. Based on previous month’s sales figures, predict the next month sales. 
G. Write a function with one argument (VIN) which will display all information of car 
even if it is in stock or sold. 
H. Search the range of sales person who have achieved sales target within given range of 
values (min_sales, max_sales). 
I. Print list of customers having EMI plan for less than 48 months but greater than 36 
months. 
Used B tree to maintain all database. 
Populate sufficient initial dataset with file handling to test all test cases.
