#pragma once
#include "User.h"
class Teacher :public User
{
public:
	Teacher(string a, string n, string p, char i,char y);//构造函数
	Teacher(){}//构造函数
	void MainMenu();//主菜单
	void MultiChoiceMenu(vector<string> Qs);//子菜单
	void gradeMoral();//思想品德打分
	void modifyMoral();//修改思想品德打分
};
