#include "Teacher.h"
#include "Student.h"
#include <vector>
#include <functional>
#include <algorithm>
#include <string>
#include <iostream>
Teacher::Teacher(string a, string n, string p, char i,char y) :User(a, n, p, i,y)
{
	importMoralList_t();
	importLoginList();
	display();//输出界面
	cout << "欢迎，" << getAccount() << ' ' << getName() << "!身份:班主任\n";
	system("pause");
	MainMenu();
}
void Teacher::MainMenu()
{
	vector<string> questions;
	questions.push_back("思想品德打分");
	questions.push_back("修改思想品德打分");
	questions.push_back("修改密码");
	questions.push_back("返回登陆界面");
	questions.push_back("退出系统");
	MultiChoiceMenu(questions);
}
void Teacher::MultiChoiceMenu(vector<string> Qs)
{
	display();//输出界面
	int num = Qs.size();
	for (int i = 0; i < num; ++i)
	{
		cout << '[' << i + 1 << "] " << Qs[i] << endl;
	}
	cout << "请选择:";
	char choice;
	cin.sync();
	cin >> choice;//输入选项
	switch (choice)
	{
	case '1':gradeMoral(); break; //思想品德打分
	case '2':modifyMoral(); break;//修改思想品德打分
	case '3':modifyPassword(); MainMenu(); break;//修改密码并且返回主菜单
	case '4':Save(); break;//返回登陆界面
	case '5':Quit(); break;//退出
	default:cout << "您的输入有误,请重新输入!" << endl;  system("pause"); MainMenu();
	}
}
void Teacher::gradeMoral()
{
	//判断是否已经打分
	if (Yes == '1')
	{
		//已经打分
		cout << "已打分!" << endl;
		system("pause");
		MainMenu();
	}
	else
	{
		display();
		//没有打分
		map<string, User>::iterator iter; //从LoginList中获取学生姓名
		for (iter = LoginList.begin(); iter != LoginList.end(); iter++)
		{
			if (iter->second.getIdentify() == '0')//判断是否是学生
			{
				float a[3];
				display();
				cout << "请给" << (iter->second).getName() << "打分" << endl;//获取姓名
				Grade(a, 3);//调用思想品德打分函数
				Moral_t temp((iter->second).getAccount(), Account, a);//创建临时对象
				MoralList_t.insert(pair<string, Moral_t>(iter->second.getAccount(), temp));//插入MoralList_t
			}
		}
		cout << "打分完毕!\n";
		Yes = '1';//修改对象状态为已打分
		LoginList[Account].modifyYes();//修改LoginList中对应的对象状态为已打分
		//保存数据
		exportMoralList_t();
		importMoralList_t();
		exportLoginList();
		importLoginList();
	}
	system("pause");
	MainMenu();
}
void Teacher::modifyMoral()
{
	if (Yes == '1')
	{
		map<string, Moral_t>::iterator iter;
		int i = 1, t = 0;
		//打印
		display();
		cout << "行数\t得分人\t打分人" << endl;
		for (iter = MoralList_t.begin(); iter != MoralList_t.end(); iter++)
		{
			cout << '[' << i << "]\t" << iter->second;
			i++;
			t++;//保存行数
		}
		while (1)
		{
			line(i, t);//输入行数，判断行数是否正确
			if (i == 0)//输入的行数为0，返回
				MainMenu();
			display();
			iter = MoralList_t.begin();
			for (i = i - 1; i > 0; i--, iter++);//将iter指向要修改的行，所对应容器中的对象
			float a[3];
			cout << "请给" << searchName(iter->first) << "打分" << endl;//根据账号获取姓名
			Grade(a, 3);//调用思想品德打分函数
			Moral_t temp(iter->first, Account, a);//创建临时对象
			iter->second = temp;//修改容器中的对象
			//保存数据
			exportMoralList_t();
			importMoralList_t();
			modify();//数据修改，综测成绩需要重新生成
			cout << "修改成功!" << endl;
			system("pause");
			display();
			//打印
			i = 1;
			cout << "\t得分人\t打分人" << endl;
			iter = MoralList_t.begin();
			for (iter = MoralList_t.begin(); iter != MoralList_t.end(); iter++)
			{
				cout << '[' << i << "]\t" << iter->second;
				i++;
			}
		}
	}
	else
	{
		cout << "思想品德未打分!" << endl;
		system("pause");
	}
	MainMenu();
}
