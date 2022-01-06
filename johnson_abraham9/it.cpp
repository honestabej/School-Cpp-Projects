#include "it.h"
#include <iostream>

// Q2b: Define displayEmployee() for IT class (5 points)
// Define the function displayEmployee() that you declared within the IT class in the header file
// See expected output in question file.
void IT::displayEmployee(){
    cout<<"Employee Name: "<<getName() <<endl;
    cout<<"Years: "<<getYears() <<endl;
    if (getDepartmentType() == 0){
        cout << "Depertment: HR \n" << endl;
    } else {
        cout << "Department: IT \n" << endl;
    }
}
// (displayList() function in hw9.cpp should call this function.)
// Include necessary header files


