#ifndef _EMPLOYEE_H_
#define _EMPLOYEE_H_

#include <string>
using namespace std;

enum departmentType { hr = 0, it };	// definition of libraryType

class Employee {
private:
	string name; // private data members
	int years;
	departmentType dept;

public:
	Employee(string employeeName, int noOfYears, departmentType deptType); // constructor

	// accessor methods
	string getName();
	int getYears();
	departmentType getDepartmentType();

	virtual void displayEmployee()
	{}

	// Q3a: Declare Friend Function changeNoOfYears()  (1 point)
	// Declare a friend function named changeNoOfYears() which has 2 parameters and no return value.
	// The first parameter is a pointer to Employee class, and the second is an integer which is the new number of years.
	// You need to define this function in hw9.cpp and call this function in case 'c' of executeAction() in hw9.cpp file
	friend void changeNoOfYears(Employee *employee, int years);

	
};

#endif // _EMPLOYEE_H_