#include "User.h"
#include "Student.h"
#include "Teacher.h"
#include "Group.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <algorithm>
#include <cstdlib>
using namespace std;
ifstream ifile;
ofstream ofile;

map<string, User> User::LoginList;
multimap<string, Course> User::CourseList;
multimap<string, Moral_s> User::MoralList_sget;
multimap<string, Moral_s> User::MoralList_sgive;
map<string, Moral_t> User::MoralList_t;
multimap<string, ActAdd> User::Act_List;
multimap<string, ActAdd> User::Add_List;

namespace
{
	void failInputFile(const char* file)
	{
		cout << "数据文件读取失败: " << file << endl;
		system("pause");
		exit(1);
	}

	void skipUtf8Bom(ifstream& in)
	{
		if (static_cast<unsigned char>(in.peek()) != 0xEF)
			return;
		in.get();
		if (static_cast<unsigned char>(in.peek()) != 0xBB)
			failInputFile("UTF-8 BOM");
		in.get();
		if (static_cast<unsigned char>(in.peek()) != 0xBF)
			failInputFile("UTF-8 BOM");
		in.get();
	}

	int openInputFile(const char* file)
	{
		if (ifile.is_open())
			ifile.close();
		ifile.clear();
		ifile.open(file);
		if (!ifile)
			failInputFile(file);
		skipUtf8Bom(ifile);
		int lines = 0;
		if (!(ifile >> lines) || lines < 0)
			failInputFile(file);
		return lines;
	}
}

User::User(string a,string n,string p,char i,char y)
{
	Account = a;
	Name = n;
	Password = p;
	Identify = i;
	Yes = y;
}
User::User()
{
	Account = "";
	Name = "";
	Password = "";
	Identify = '0';
	Yes = '0';
}
void User::display()
{
	system("cls");
	cout << "*****************************************************" << endl
		<< "**                                                 **" << endl
		<< "**         欢迎进入班级综合素质测评管理系统        **" << endl
		<< "**                                                 **" << endl
		<< "*****************************************************" << endl;
}
User User::Login()
{
	importLoginList();
	display();
	string password, account;
	while (1)
	{
		cout << "账号:";
		cin >> account;
		cout << "密码:";
		cin >> password;
		map<string, User>::iterator iter = LoginList.find(account);
		if (iter == LoginList.end())
		{
			cout << "用户名或密码错误！" << endl;
			continue;
		}
		User u = iter->second;
		if (password == u.getPassword())
		{
			cout << "登陆成功！" << endl;
			system("pause");
			return u;
		}
		else
			cout << "用户名或密码错误！" << endl;
	}
}
void User::Menu(User u)
{
	switch (u.getIdentify())
	{
	case '0':
	{
		Student s(u.getAccount(), u.getName(), u.getPassword(), u.getIdentify(), u.getYes());
		break;
	}
	case '1':
	{
		Teacher t(u.getAccount(), u.getName(), u.getPassword(), u.getIdentify(), u.getYes());
		break;
	}
	case '2':
	{
		Group g(u.getAccount(), u.getName(), u.getPassword(), u.getIdentify(), u.getYes());
		break;
	}
	}
}
void User::modifyPassword()
{
	display();
	string s1, s2;
	while (1)
	{
		cout << "请输入原密码:";
		cin >> s1;
		if (s1 == Password)
			break;
		else
			cout << "原密码错误！请重新输入！\n";
	}
	while (1)
	{
		while (1)
		{
			cout << "请输入新密码:";
			cin >> s1;
			if (s1 != Password)
				break;
			else
				cout << "与原密码相同，请重新输入！" << endl;
		}

		cout << "请再输入一次:";
		cin >> s2;
		if (s1 == s2)
		{

			Password = s1;
			LoginList[this->getAccount()] = *this;
			exportLoginList();
			cout << "修改成功！\n";
			system("pause");
			break;
		}
		else
			cout << "两次输入的密码不相同，请重新输入！\n";
	}
}
void User::Save()
{
	Menu(Login());
}
void User::Quit()
{
	exit(0);
}
void User::line(int &a, const int &b)
{
	a = 0;
	//进入输入循环
	while (1)
	{
		cout << "请输入行数(返回请输入0):";
		cin.sync();//清空输入缓存区
		cin >> a;
		if (cin.rdstate() == ios::goodbit)//如果输入流正常
		{
			if (a >= 0 && a <= b)//判断行数是否符合要求（参数b为最大的行数）
				break;
		}
		cin.sync();//清空输入缓存区
		cin.clear();//使输入流恢复正常
		cout << "请输入正确的行数!" << endl;
	}
}
void User::judge(float &f, int n)//1：学分 2:学习成绩 3:附加分 4:课外活动成绩
{
	bool flag = false;//标志输入是否正确
	while (1)
	{
		switch (n)//判断输入的类型
		{
		case 1:cout << "学分(0.5-10):";  break;
		case 2:cout << "\n成绩(0-100):"; break;
		case 3:cout << "分数(0.5-5):"; break;
		case 4:cout << "分数(0.5-20):"; break;
		}
		cin.sync();//清空输入缓存区
		cin >> f;//输入分数
		if (cin.rdstate() == ios::goodbit)//如果输入流正常
		{
			switch (n)//判断输入的类型
			{
			case 1:
			{
				if (f>=0.5&&f<=10)//判断学分是否为0.5-10之间
					flag = true;
			}break;
			case 2:
			{
				if (f >= 0 && f <= 100)//判断成绩是否为0-100之间
					flag = true;
			}break;
			case 3:
			{
				if (f >= 0.5 && f <= 5)//判断附加分是否为0.5-5之间
					flag = true;
			}break;
			case 4:
			{
				if (f >= 0.5 && f <= 20)//判断课外活动成绩是否为0.5-20之间
					flag = true;
			}break;

			}
			if (flag == true)//如果输入正确，跳出循环
				break;
		}
		else
		{
			//输入流异常
			cin.sync();//清空输入缓存区
			cin.clear();//使输入流恢复正常
		}
		switch (n)//根据输入类型，输出错误信息
		{
		case 1:cout << "请输入0.5-10之间的分数!" << endl; break;
		case 2:cout << "请输入0-100之间的分数!" << endl; break;
		case 3:cout << "请输入0.5-5之间的分数!" << endl; break;
		case 4:cout << "请输入0.5-20之间的分数!" << endl; break;
		}
		cout << "请重新输入:" << endl;

	}
}
void User::Grade(float*a, int n)
{
	//a为储存打分的数组
	//n代表数组的大小，同时3表示班主任打分，9表示学生打分
	float t;
	for (int i = 0; i < n; i++)
	{
		while (1)
		{
			cout << "第" << i + 1 << "项(0-10)" << endl;//输出当前是第几项
			switch (n)
			{
			case 3://班主任打分
				switch (i + 1)
				{
				case 1:cout << "学生日常学习情况，上课出勤率，学习认真程度:"; break;
				case 2:cout << "学生平时生活情况，自律、诚信情况:"; break;
				case 3:cout << "学生参与班会及班级集体活动情况:"; break;
				}break;
			case 9://学生打分
				switch (i + 1)
				{
				case 1:cout << "严格遵守校规校纪及学院各项规章制度:"; break;
				case 2:cout << "深入科研实践、勤奋学习:"; break;
				case 3:cout << "合理安排课余时间，按时自习，不沉迷游戏，不过滥用电脑:"; break;
				case 4:cout << "生活简朴节约，为人诚实守信:"; break;
				case 5:cout << "注意保持个人卫生，爱护环境，维护公物:"; break;
				case 6:cout << "积极配合并参加班级与学院组织的各项活动，有责任感:"; break;
				case 7:cout << "积极参加体育锻炼:"; break;
				case 8:cout << "培养自身素质，积极参加各项社会实践活动和志愿服务活动:"; break;
				case 9:cout << "尊敬师长，关心集体，文明礼貌，团结同学，乐于助人:"; break;
				}break;
			}
			cin.sync();//清空输入缓存区
			cin.clear();//使输入流正常
			cin >> t;//输入分数
			if (cin.rdstate() == ios::goodbit)//如果输入流正常
			{
				if (t >= 0 && t <= 10)//判断输入的分数是否为0-10之间
				{
					a[i] = t;//插入数组
					break;
				}
			}
			else
			{
				cin.sync();//清空输入缓存区
				cin.clear();//使输入流正常
			}
			cout << "请输入0-10之间的分数!" << endl;
		}
	}
}
string User::getAccount()
{
	return Account;
}
void User::modifyAccount(string str)
{
	Account = str;
}
string User::getPassword()
{
	return Password;
}
string User::getName()
{
	return Name;
}
char User::getIdentify()
{
	return Identify;
}
char User::getYes()
{
	return Yes;
}
void User::modifyYes()
{
	Yes = '1';
}
void User::modify()
{
	importLoginList();
	map<string, User>::iterator iter;
	for (iter = LoginList.begin(); iter != LoginList.end(); iter++)
	{
		if (iter->second.getIdentify() == '2')
		{
			iter->second.modifyNo();
			exportLoginList();
			importLoginList();
			break;
		}
	}
}
void User::modifyNo()
{
	Yes = '0';
}
string User::searchName(string s)
{
	map<string, User>::iterator iter = LoginList.find(s);
	if (iter == LoginList.end())
		return "";
	return iter->second.getName();
}
void User::importLoginList()
{
	LoginList.clear();
	int lines = openInputFile("User.txt");
	int i;
	for (i = 0; i < lines; i++)
	{
		User u;
		if (!(ifile >> u))
			failInputFile("User.txt");
		if (u.getIdentify() != '0' && u.getIdentify() != '1' && u.getIdentify() != '2')
			failInputFile("User.txt");
		if (u.getYes() != '0' && u.getYes() != '1')
			failInputFile("User.txt");
		if (!LoginList.insert(pair<string, User>(u.getAccount(), u)).second)
			failInputFile("User.txt");
	}
	ifile.close();
}
void User::importCourseList()
{
	CourseList.clear();
	int lines = openInputFile("Course.txt");
	int i;
	for (i = 0; i < lines; i++)
	{
		Course c;
		if (!(ifile >> c))
			failInputFile("Course.txt");
		CourseList.insert(pair<string, Course>(c.getAccount(), c));
	}
	ifile.close();

}
void User::importAct_List()
{
	Act_List.clear();//清空容器
	int lines = openInputFile("Act.txt");
	//下面会读取行数，如果文件不存在打开失败，下面的for循环不会执行
	//容器为空，所以不需要判断文件是否存在
	int i;
	for (i = 0; i < lines; i++)
	{
		ActAdd c;
		if (!(ifile >> c))
			failInputFile("Act.txt");
		if (c.getGet() != "未审核" && c.getGet() != "审核通过" && c.getGet() != "审核未通过")
			failInputFile("Act.txt");
		Act_List.insert(pair<string, ActAdd>(c.getAccount(), c));//插入容器
	}
	ifile.close();//关闭文件
}
void User::importAdd_List()
{
	Add_List.clear();
	int lines = openInputFile("Add.txt");
	int i;
	for (i = 0; i < lines; i++)
	{
		ActAdd c;
		if (!(ifile >> c))
			failInputFile("Add.txt");
		if (c.getGet() != "审核通过")
			failInputFile("Add.txt");
		Add_List.insert(pair<string, ActAdd>(c.getAccount(), c));
	}
	ifile.close();
}
void User::importMoralList_s()
{
	MoralList_sget.clear();
	MoralList_sgive.clear();
	int lines = openInputFile("Moral_s.txt");
	int i;
	for (i = 0; i < lines; i++)
	{
		Moral_s s;
		if (!(ifile >> s))
			failInputFile("Moral_s.txt");
		MoralList_sget.insert(pair<string,Moral_s>(s.getAccount_get(), s));
		MoralList_sgive.insert(pair<string, Moral_s>(s.getAccount_give(), s));
	}
	ifile.close();

}
void User::importMoralList_t()
{
	MoralList_t.clear();
	int lines = openInputFile("Moral_t.txt");
	int i;
	for (i = 0; i < lines; i++)
	{
		Moral_t s;
		if (!(ifile >> s))
			failInputFile("Moral_t.txt");
		MoralList_t.insert(pair<string,Moral_t>(s.getAccount_get(), s));
	}
	ifile.close();
}
void User::exportLoginList()
{
	ofile.open("User.txt");
	Close(ofile);
	map<string, User>::iterator iter;
	ofile << LoginList.size() << endl;
	for (iter = LoginList.begin(); iter != LoginList.end(); ++iter)
	{
		ofile << iter->second;
	}
	ofile.close();
}
void User::exportCourseList()
{
	ofile.open("Course.txt");
	Close(ofile);
	multimap<string, Course>::iterator iter;
	ofile << CourseList.size() << endl;
	for (iter = CourseList.begin(); iter != CourseList.end(); ++iter)
	{
		ofile << iter->second;
	}
	ofile.close();
}
void User::exportAct_List()
{
	ofile.open("Act.txt");//打开文件
	Close(ofile);//判断文件是否打开成功
	multimap<string, ActAdd>::iterator iter;
	ofile << Act_List.size() << endl;//输出记录数
	//输出容器中的纪录
	for (iter = Act_List.begin(); iter != Act_List.end(); ++iter)
	{
		ofile << iter->second; //<<运算符已重载
	}
	ofile.close();//关闭文件
}
void User::exportAdd_List()
{
	ofile.open("Add.txt");
	Close(ofile);
	multimap<string, ActAdd>::iterator iter;
	ofile << Add_List.size() << endl;
	for (iter = Add_List.begin(); iter != Add_List.end(); ++iter)
	{
		ofile << iter->second;
	}
	ofile.close();
}
void User::exportMoralList_s()
{
	ofile.open("Moral_s.txt");
	Close(ofile);
	multimap<string, Moral_s>::iterator iter;
	ofile << MoralList_sget.size() << endl;
	for (iter = MoralList_sget.begin(); iter != MoralList_sget.end(); ++iter)
	{
		ofile << iter->second;
	}
	ofile.close();
}
void User::exportMoralList_s2()
{
	ofile.open("Moral_s.txt");
	Close(ofile);
	multimap<string, Moral_s>::iterator iter;
	ofile << MoralList_sgive.size() << endl;
	for (iter = MoralList_sgive.begin(); iter != MoralList_sgive.end(); ++iter)
	{
		ofile << iter->second;
	}
	ofile.close();
}
void User::exportMoralList_t()
{
	ofile.open("Moral_t.txt");
	Close(ofile);
	map<string, Moral_t>::iterator iter;
	ofile << MoralList_t.size() << endl;
	for (iter = MoralList_t.begin(); iter != MoralList_t.end(); ++iter)
	{
		ofile << iter->second;
	}
	ofile.close();
}
istream& operator>>(istream &in, User &u)
{
	in >> u.Account >> u.Name >> u.Password >> u.Identify >> u.Yes;
	return in;
}
ostream& operator<<(ostream &out, const User &u)
{
	out << u.Account << '\t' << u.Name << '\t' << u.Password << '\t' << u.Identify << '\t' << u.Yes << endl;
	return out;
}

void User::Close(ofstream &o)
{
	try
	{
		if (!o)//判断文件是否打开成功
			throw 1;//打开不成功，抛出异常
	}
	catch (int)//捕获异常
	{
		cout << "文件打开失败!" << endl;
		system("pause");
		exit(1);//退出程序
	}
}
