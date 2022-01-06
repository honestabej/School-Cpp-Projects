
#ifndef _HR_H
#define _HR_H
#include "employee.h"

// Q1a: Create HR class (5 points)
// Part 1: Create a child class of the Employee class named 'HR'
class HR : public Employee {
    
    // Part2: Declare constructor which accepts the same 3 parameters as the parent class Employee's constructor.
    // Pass the 3 parameters to the super constructor of the Employee class.
    public: HR(string employeeName, int noOfYears, departmentType deptType):Employee(employeeName, noOfYears, deptType){}

    // Part 3: Re-declare the method displayEmployee (virtual method found inside of parent class Employee)
    virtual void displayEmployee();
};
#endif // _HR_H