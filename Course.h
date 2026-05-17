#pragma once
#include <string>
#include <iostream>
using namespace std;
class Course
{
private:
	string Account;//学号
	string Name;//课程名
	float Credit;//学分
	float Grade;//成绩
public:
	Course(string, string, float, float);//构造函数
	Course(){}//构造函数
	string getAccount();//返回学号
	float getCredit();//返回学分
	float getGrade();//返回成绩
	//重载运算符
	friend istream& operator>>(istream &in, Course &u);
	friend ostream& operator<<(ostream &out, const Course &u);
};