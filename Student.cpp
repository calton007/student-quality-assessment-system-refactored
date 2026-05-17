#include "Student.h"
#include <functional>
#include <algorithm>
#include <vector>
#include <iostream>
#include <map>
#include <iomanip>
#include <cstdlib>
using namespace std;
extern ifstream ifile;
extern ofstream ofile;

map<string, Student> Student::TotalList;
namespace
{
	void failTotalFile()
	{
		cout << "数据文件读取失败: Total.txt" << endl;
		system("pause");
		exit(1);
	}

	void skipUtf8Bom(ifstream& in)
	{
		if (static_cast<unsigned char>(in.peek()) != 0xEF)
			return;
		in.get();
		if (static_cast<unsigned char>(in.peek()) != 0xBB)
			failTotalFile();
		in.get();
		if (static_cast<unsigned char>(in.peek()) != 0xBF)
			failTotalFile();
		in.get();
	}

	Student& requireTotalRecord(const string& account)
	{
		map<string, Student>::iterator iter = Student::TotalList.find(account);
		if (iter == Student::TotalList.end())
		{
			cout << "Total.txt 缺少学号 " << account << " 的综测记录!" << endl;
			system("pause");
			exit(1);
		}
		return iter->second;
	}
}
Student::Student(string a, string n, string p, char i, char y, float st, float gp, float ac, float mo, float ad, float to, int ra) :User(a, n, p, i, y)
{
	//导入数据
	importMoralList_s();
	importLoginList();
	importAct_List();
	study = st;
	gpa = gp;
	activity = ac;
	moral = mo;
	addition = ad;
	total = to;
	range = ra;
	display();//输出界面
	cout << "欢迎，" << getAccount() << ' ' << getName() << "!身份:学生\n";
	system("pause");
	MainMenu();

}
Student::Student()
{
	study = 0.0;
	gpa = 0.0;
	activity = 0.0;
	moral = 0.0;
	addition = 0.0;
	total = 0.0;
	range = 0;
}
void Student::importTotalList()
{
	TotalList.clear();
	if (ifile.is_open())
		ifile.close();
	ifile.clear();
	ifile.open("Total.txt");
	if (!ifile)
		failTotalFile();
	skipUtf8Bom(ifile);
	int lines=0;
	if (!ifile || !(ifile >> lines) || lines < 0)
		failTotalFile();
	if (lines == 0)
	{
		map<string, User>::iterator iter;
		for (iter = LoginList.begin(); iter != LoginList.end(); iter++)
		{
			if (iter->second.getIdentify() == '0')
			{
				Student s;
				s.modifyAccount(iter->first);
				TotalList.insert(pair<string, Student>(s.getAccount(), s));
			}

		}
	}
	else
	{
		int i;
		for (i = 0; i < lines; i++)
		{
			Student s;
			if (!(ifile >> s))
				failTotalFile();
			if (!TotalList.insert(pair<string, Student>(s.getAccount(), s)).second)
				failTotalFile();
		}
	}
	ifile.close();
}
void Student::exportTotalList()
{
	ofile.open("Total.txt");
	Close(ofile);
	map<string, Student>::iterator iter;
	ofile << TotalList.size() << endl;
	for (iter = TotalList.begin(); iter != TotalList.end(); ++iter)
	{
		ofile << iter->second;
	}
	ofile.close();
}
void Student::MainMenu()
{
	vector<string> questions;
	questions.push_back("思想品德项目");
	questions.push_back("课外活动项目");
	questions.push_back("查询");
	questions.push_back("修改密码");
	questions.push_back("返回登陆界面");
	questions.push_back("退出系统");
	(MultiChoiceMenu(questions, 0));
}
void Student::MultiChoiceMenu(vector<string> Qs, int n)
{
	display();
	int num = Qs.size();
	for (int i = 0; i < num; ++i)
	{
		cout << '[' << i + 1 << "] " << Qs[i] << endl;
	}
	cout << "请选择:";
	char choice;
	cin.sync();
	cin >> choice;

	switch (n)
	{
	case 0:
		switch (choice)
		{
		case '1':Moral(); break;
		case '2':Activity(); break;
		case '3':Search(); break;
		case '4':modifyPassword(); MainMenu(); break;
		case '5':Save(); break;
		case '6':Quit(); break;
		default:cout << "您的输入有误,请重新输入!" << endl;  system("pause"); MainMenu(); break;
		}break;
	case 1:
		switch (choice)
		{
		case '1':gradeMoral(); break;
		case '2':modifyMoral(); break;
		case '3':MainMenu(); break;
		default:cout << "您的输入有误,请重新输入!" << endl;  system("pause"); Moral(); break;
		}break;
	case 2:
		switch (choice)
		{
		case '1':newActivity(); break;
		case '2':modifyActivity(); break;
		case '3':delActivity(); break;
		case '4':displayActivity(); break;
		case '5':MainMenu(); break;
		default:cout << "您的输入有误,请重新输入!" << endl;  system("pause"); Activity(); break;
		}break;
	case 3:
		switch (choice)
		{
		case '1':searchStudy(); break;
		case '2':searchActivity(); break;
		case '3':searchMoral(); break;
		case '4':searchAddition(); break;
		case '5':searchTotal(); break;
		case '6':searchrange(); break;
		case '7':searchall(); break;
		case '8':MainMenu(); break;
		default:cout << "您的输入有误,请重新输入!" << endl;  system("pause"); Search(); break;
		}break;
	}
}
void Student::Moral()
{
	vector<string> questions;
	questions.push_back("思想品德打分");
	questions.push_back("修改思想品德打分");
	questions.push_back("返回");
	(MultiChoiceMenu(questions, 1));
}
void Student::gradeMoral()
{
	if (Yes == '1')
	{
		cout << "已打分!" << endl;
		system("pause");
		Moral();
	}
	else
	{
		display();
		map<string, User>::iterator iter;
		for (iter = LoginList.begin(); iter != LoginList.end(); iter++)
		{
			if ((iter->second).getIdentify() == '0')
			{
				float a[9];
				display();
				cout << "请给" << (iter->second).getName() << "打分" << endl;
				Grade(a, 9);
				Moral_s temp((iter->second).getAccount(), Account, a);
				MoralList_sget.insert(pair<string, Moral_s>(iter->second.getAccount(), temp));
			}
		}
		Yes = '1';
		LoginList[Account].modifyYes();
		exportMoralList_s();
		importMoralList_s();
		exportLoginList();
		importLoginList();
		cout << "打分完毕!" << endl;

	}
	system("pause");
	Moral();
}
void Student::modifyMoral()
{
	if (Yes == '1')
	{
		typedef multimap<string, Moral_s>::iterator Iter;
		pair<Iter, Iter> iter = MoralList_sgive.equal_range(Account);
		int i = 1, t = 0;
		display();
		cout << "行数\t得分人\t打分人" << endl;
		while (iter.first != iter.second)
		{
			cout << '[' << i << "]\t" << iter.first->second;
			i++;
			t++;
			iter.first++;
		}
		if (t == 0)
		{
			cout << "没有可修改的思想品德评分记录!" << endl;
			system("pause");
			Moral();
			return;
		}
		while (1)
		{
			line(i, t);
			if (i == 0)
				Moral();
			display();
			iter = MoralList_sgive.equal_range(Account);
			for (i = i - 1; i > 0; i--, iter.first++);
			float a[9];
			cout << "请给" << searchName((iter.first->second).getAccount_get()) << "打分" << endl;
			Grade(a, 9);
			Moral_s temp((iter.first->second).getAccount_get(), Account, a);
			iter.first->second = temp;
			exportMoralList_s2();
			importMoralList_s();
			modify();
			cout << "修改成功!" << endl;
			system("pause");
			display();
			i = 1;
			cout << "\t得分人\t打分人" << endl;
			iter = MoralList_sgive.equal_range(Account);
			while (iter.first != iter.second)
			{
				cout << '[' << i << "]\t" << iter.first->second;
				i++;
				iter.first++;
			}
		}
	}
	else
	{
		cout << "思想品德未打分！" << endl;
		system("pause");
		Moral();
	}

}
void Student::Search()
{
	map<string, User>::iterator iter;
	for (iter = LoginList.begin(); iter != LoginList.end(); iter++)
	{
		if (iter->second.getIdentify() == '2')
			break;
	}
	if (iter->second.getYes() == '0')
	{
		cout << "尚未生成综测成绩,无法查询!" << endl;
		system("pause");
		MainMenu();
	}
	importTotalList();
	vector<string> questions;
	questions.push_back("查询学习成绩");
	questions.push_back("查询课外活动成绩");
	questions.push_back("查询思想品德成绩");
	questions.push_back("查询附加分");
	questions.push_back("查询综测总分");
	questions.push_back("查询排名");
	questions.push_back("查询所有");
	questions.push_back("返回");
	(MultiChoiceMenu(questions, 3));
}
void Student::searchStudy()
{
	cout << "您的学习成绩为:" << requireTotalRecord(Account).getstudy() << endl;
	system("pause");
	Search();
}
void Student::searchGPA()
{
	cout << "您的GPA为:" << requireTotalRecord(Account).getgpa() << endl;
	system("pause");
	Search();
}
void Student::searchActivity()
{
	cout << "您的课外活动成绩为:" << requireTotalRecord(Account).getactivity() << endl;
	system("pause");
	Search();
}
void Student::searchMoral()
{
	cout << "您的思想品德成绩为:" << requireTotalRecord(Account).getmoral() << endl;
	system("pause");
	Search();
}
void Student::searchAddition()
{
	cout << "您的附加分为:" << requireTotalRecord(Account).getaddition() << endl;
	system("pause");
	Search();
}
void Student::searchTotal()
{
	cout << "您的综测成绩为:" << requireTotalRecord(Account).gettotal() << endl;
	system("pause");
	Search();
}
void Student::searchrange()
{
	cout << "您的排名为:" << requireTotalRecord(Account).getrange() << endl;
	system("pause");
	Search();
}
void Student::searchall()
{
	cout.flags(ios::left);
	cout<<setw(8) << "学号" << setw(12) << "学习成绩" << setw(7) << "gpa" << setw(14)
		<< "课外活动成绩" << setw(14) << "思想品德成绩" << setw(10) << "附加分" << setw(12)
		<< "综测成绩" << setw(8) << "排名" << endl;
	cout << requireTotalRecord(Account) << endl;
	system("pause");
	Search();
}
void Student::Activity()
{
	vector<string> questions;
	questions.push_back("录入课外活动");
	questions.push_back("修改课外活动");
	questions.push_back("删除课外活动");
	questions.push_back("查询课外活动");
	questions.push_back("返回");
	(MultiChoiceMenu(questions, 2));
}
void Student::newActivity()
{
	display();//打印界面
	string name;
	float grade;
	cout << "活动名称:";
	cin >> name;
	judge(grade, 4);//输入分数，并判断分数是否正确
	ActAdd temp(getAccount(), name, "课外活动", grade, "未审核");//创建临时对象
	Act_List.insert(pair<string, ActAdd>(getAccount(), temp));//插入容器Act_List
	modify();//数据修改，综测成绩需要重修生成
	//保存数据
	exportAct_List();
	importAct_List();
	cout << "录入成功!" << endl;
	system("pause");
	Activity();//返回菜单
}
void Student::delActivity()
{
	typedef multimap<string, ActAdd>::iterator Iter;
	pair<Iter, Iter> iter = Act_List.equal_range(Account);//找到学号为Account对应的课外活动项目信息
	if (iter.first != iter.second)//判断是否有对应的信息
	{
		int i = 1, t = 0;
		display();//打印界面
		//打印课外活动项目信息
		cout << setw(8) << "行数" << setw(10) << "学号" << setw(20) << "活动名" << setw(12) << "类型" << setw(8) << "分数" << setw(14) << "状态" << endl;
		while (iter.first != iter.second)
		{
			cout << setw(8) << i << iter.first->second;
			i++;
			t++;//保存行数
			iter.first++;
		}
		//进入删除的循环
		while (1)
		{
			line(i, t);//输入行数，并判断输入的行数是否正确
			if (i == 0)//输入输入的行数为0，返回菜单
				Activity();
			iter = Act_List.equal_range(Account);//找到学号为Account对应的课外活动项目信息
			for (i = i - 1; i > 0; i--, iter.first++);//将iter.first指向需要删除的项目
			//判断删除的项目是否已经通过审核，已经通过审核的项目不能删除
			if (iter.first->second.getGet() == "未审核" || iter.first->second.getGet() == "审核未通过")
			{
				Act_List.erase(iter.first);//删除
				//保存数据
				exportAct_List();
				importAct_List();
				modify();//数据修改，综测成绩需要重新生成
				cout << "删除成功!" << endl;
				system("pause");
				display();//打印界面
				//打印删除后的结果
				iter = Act_List.equal_range(Account);//找到学号为Account对应的课外活动项目信息
				i = 1;
				cout << setw(8) << "行数" << setw(10) << "学号" << setw(20) << "活动名" << setw(12) << "类型" << setw(8) << "分数" << setw(14) << "状态" << endl;
				while (iter.first != iter.second)
				{
					cout << setw(8) << i << iter.first->second;
					i++;
					iter.first++;
				}
			}
			else
			{
				cout << "审核已通过项目不能删除!" << endl;
				system("pause");
			}
		}
	}
	else
	{
		cout << "没有可以删除的项目!" << endl;
		system("pause");
		Activity();
	}
}
void Student::modifyActivity()
{
	typedef multimap<string, ActAdd>::iterator Iter;
	pair<Iter, Iter> iter = Act_List.equal_range(Account);//找到学号为Account对应的课外活动项目信息
	if (iter.first != iter.second)
	{
		int i = 1, t = 0;
		display();//打印界面
		//打印课外活动项目信息
		cout << setw(8) << "行数" << setw(10) << "学号" << setw(20) << "活动名" << setw(12) << "类型" << setw(8) << "分数" << setw(14) << "状态" << endl;
		while (iter.first != iter.second)
		{
			cout << setw(8) << i << iter.first->second;
			i++;
			t++;
			iter.first++;
		}
		//进入修改的循环
		while (1)
		{
			line(i, t);//输入行数，同时判断行数是否正确
			if (i == 0)//输入行数为0，返回菜单
				Activity();
			iter = Act_List.equal_range(Account);//找到学号为Account对应的课外活动项目信息
			for (i = i - 1; i > 0; i--, iter.first++);//将iter.first指向需要删除的项目
			//判断课外活动是否已经审核通过，审核通过的项目不能修改
			if (iter.first->second.getGet() == "未审核" || iter.first->second.getGet() == "审核未通过")
			{
				string name;
				float grade;
				cout << "活动名称:";
				cin >> name;
				judge(grade, 4);//输入分数，同时判断分数是否正确
				ActAdd temp(getAccount(), name, "课外活动", grade, "未审核");//创建临时对象
				iter.first->second = temp;//修改容器Act_List中对应的项目
				//保存数据
				exportAct_List();
				importAct_List();
				modify();//数据修改，综测成绩需要重新生成
				cout << "修改成功!" << endl;
				system("pause");
				display();//打印界面
				iter = Act_List.equal_range(Account);//找到学号为Account对应的课外活动项目信息
				i = 1;
				//打印课外活动项目信息
				cout << setw(8) << "行数" << setw(10) << "学号" << setw(20) << "活动名" << setw(12) << "类型" << setw(8) << "分数" << setw(14) << "状态" << endl;
				while (iter.first != iter.second)
				{
					cout << setw(8) << i << iter.first->second;
					i++;
					iter.first++;
				}

			}
			else
			{
				cout << "审核已通过项目不能修改!" << endl;
				system("pause");
			}
		}
	}
	else
	{
		cout << "没有可以修改的项目!" << endl;
		system("pause");
		Activity();
	}
}
void Student::displayActivity()
{
	typedef multimap<string, ActAdd>::iterator Iter;
	pair<Iter, Iter> iter = Act_List.equal_range(Account);//找到学号为Account对应的课外活动项目信息
	if (iter.first != iter.second)//判断是否有对应的信息
	{
		int i = 1;
		display();//打印界面
		//打印课外活动信息
		cout << setw(8) << "行数" << setw(10) << "学号" << setw(20) << "活动名" << setw(12) << "类型" << setw(8) << "分数" << setw(14) << "状态" << endl;
		while (iter.first != iter.second)
		{
			cout << setw(8) << i << iter.first->second;
			i++;
			iter.first++;
		}
	}
	else
	{
		cout << "没有对应的项目!" << endl;
	}
	system("pause");
	Activity();//返回菜单
}
float Student::getstudy()
{
	return study;
}
float Student::getgpa()
{
	return gpa;
}
float Student::getactivity()
{
	return activity;
}
float Student::getmoral()
{
	return moral;
}
float Student::getaddition()
{
	return addition;
}
float Student::gettotal()
{
	return total;
}
int Student::getrange()
{
	return range;
}
void Student::instudy(float t)
{
	study = t;
}
void Student::ingpa(float t)
{
	gpa = t;
}
void Student::inactivity(float t)
{
	activity = t;
}
void Student::inmoral(float t)
{
	moral = t;
}
void Student::inaddition(float t)
{
	addition = t;
}
void Student::intotal()
{
	total = study*0.7f+activity*0.15f+moral*0.15f+addition;
	if (total > 100)
		total = 100;
	else
	{
		total *= 100;
		total = int(total);
		total /= 100;
	}
}
void Student::inrange(int t)
{
	range = t;
}
istream& operator>>(istream &in, Student &u)
{
	in >> u.Account >> u.study >> u.gpa >> u.activity >> u.moral >> u.addition >> u.total >> u.range;
	return in;
}
ostream& operator<<(ostream &out, const Student &u)
{
	out.flags(ios::left);
	out << u.Account << '\t' << u.study << '\t' << u.gpa << '\t' << u.activity << '\t'
		<< u.moral << '\t' << u.addition << '\t' << u.total << '\t' << u.range << endl;
	return out;
}
