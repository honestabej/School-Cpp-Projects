#ifndef _IT_H_
#define _IT_H_
#include "employee.h"

// Q1b: Create Noble class (5 points)
// Part 1: Create a child class of the Employee class named 'IT'
class IT : public Employee {

    // Part2: Declare constructor which accepts the same 3 parameters as the parent class Employee's constructor.
    // Pass the 3 parameters to the super constructor of the Employee class.
    public: IT(string employeeName, int noOfYears, departmentType deptType):Employee(employeeName,noOfYears,deptType){}
    
    // Part 3: Re-declare the method displayEmployee (virtual method found inside of parent class Employee)
    virtual void displayEmployee();
};
#endif // _IT_H_