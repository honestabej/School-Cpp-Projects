#include "employee.h"

// definitions of Employee member functions
Employee::Employee(string employeeName, int noOfYears, departmentType deptType)
{
	name = employeeName;
	years = noOfYears;
	dept = deptType;
}

string Employee::getName()
{
	return name;
}

int Employee::getYears()
{
	return years;
}

departmentType Employee::getDepartmentType()
{
	return dept;
}