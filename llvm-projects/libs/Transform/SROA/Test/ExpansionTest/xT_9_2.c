//-sdasgup3 cannot do promotion of arrays
#include <stdio.h>
#include <string.h>
 
struct student_college_detail
{
    int college_id;
    char college_name[50];
};
 
struct student_detail
{
    int id;
    char name[20];
    float percentage;
    // structure within structure
    struct student_college_detail clg_data;
}stu_data;
 
int main()
{
  struct student_detail stu_data;
 
    printf(" Id is: %d \n", stu_data.id);
    printf(" Percentage is: %f \n\n", stu_data.percentage);
    printf(" College Id is: %d \n", stu_data.clg_data.college_id);
 
    return 0;
}
