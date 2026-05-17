#pragma once
#include "User.h"
#include <iostream>
#include <map>
using namespace std;
class Student:public User
{
protected:
	float study;//学习成绩
	float gpa;//gpa
	float activity;//课外活动成绩
	float moral;//思想品德成绩
	float addition;//附加分
	float total;//综测成绩
	int range;//排名
public:
	Student(string a, string n, string p, char i,char y,float st=0.0,float gp=0.0,float ac=0.0,float mo=0.0,float ad=0.0,float to=0.0,int ra=0);//构造函数
	Student();//构造函数
	static map<string, Student> TotalList;//综测总表 关键字:学号
	static void importTotalList();//导入综测总表
	static void exportTotalList();//导出综测总表
	void MainMenu();//主菜单
	void MultiChoiceMenu(vector<string> Qs, int n);//子菜单
	//思想品德项目
	void Moral();
	void gradeMoral();//思想品德打分
	void modifyMoral();//修改思想品德打分
	//查询项目
	void Search();
	void searchStudy();//查询学习成绩
	void searchGPA();//查询GPA
	void searchActivity();//查询课外活动成绩
	void searchMoral();//查询思想品德成绩
	void searchAddition();//查询附加分
	void searchTotal();//查询综测总分
	void searchrange();//查询排名
	void searchall();//查询所有
	//课外活动项目
	void Activity();
	void newActivity();//录入课外活动
	void delActivity();//删除课外活动
	void modifyActivity();//修改课外活动
	void displayActivity();//打印课外活动

	//获取数据
	float getstudy();//获取学习成绩
	float getgpa();//获取gpa
	float getactivity();//获取课外活动成绩
	float getmoral();//获取思想品德成绩
	float getaddition();//获取附加分
	float gettotal();//获取综测成绩
	int getrange();//获取排名
	//输入数据
	void instudy(float);//输入学习成绩
	void ingpa(float);//输入gpa
	void inactivity(float);//输入课外活动成绩
	void inmoral(float);//输入思想品德成绩
	void inaddition(float);//输入附加分
	void intotal();//输入综测成绩
	void inrange(int);//输入排名
	//重载运算符
	friend istream& operator>>(istream &in, Student &u);
	friend ostream& operator<<(ostream &out, const Student &u);
};
