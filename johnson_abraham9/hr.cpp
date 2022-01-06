#include "hr.h"
#include <iostream>

// Q2a: Define displayEmployee() for HR class (5 points)
// Define the function displayEmployee() that you declared within the HR class in the header file
// See expected output in question file.
void HR::displayEmployee(){
    cout << "Employee Name: " << getName() << endl;
    cout << "Years: " << getYears() << endl;
    if (getDepartmentType() == 0){
        cout << "Depertment: HR \n" << endl;
    } else {
        cout << "Department: IT \n" << endl;
    }
}
// (displayList() function in hw9.cpp should call this function.)
// Include necessary header files

