#pragma once
#include <string>
#include "User.h"
#include "Student.h"
class Group :public User
{
public:
	Group(string a, string n, string p, char i,char y);//构造函数
	Group(){}//构造函数
	void MainMenu();//主菜单
	void MultiChoiceMenu(vector<string> Qs, int n);//子菜单
	//学习成绩管理
	void study();
	void newStudy();//录入学习成绩
	void delStudy();//删除学习成绩
	void modifyStudy();//修改学习成绩
	//附加分管理
	void addition();
	void newAddition();//录入附加分
	void modifyAddition();//修改附加分
	void delAddition();//删除附加分
	//综测成绩生成
	void buildMoral();//生成思想品德成绩
	void checkActivity();	//审核课外活动
	bool readyForTotal();//判断是否可以生成综测总分
	void buildActivity();//生成课外活动成绩
	void buildAddition();//生成附加分
	void getGPA();//计算GPA
	void buildStudy();//生成学习成绩
	void buildTotal();//生成综测总分
	//查询
	void search();
	void searchStudy();//查询学习成绩
	void searchGPA();//查询GPA
	void searchActivity();//查询课外活动成绩
	void searchMoral();//查询思想品德成绩
	void searchAddition();//查询附加分
	void searchTotal();//查询综测成绩
	void searchall();//查询所有
	bool exist(string);//判断查找的账号是否存在
};
