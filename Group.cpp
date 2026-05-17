#include "Group.h"
#include "Student.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;
typedef map<string, Student> StudentMap;
Group::Group(string a, string n, string p, char i,char y) :User(a, n, p, i,y)
{
	//导入数据
	importCourseList();
	importMoralList_s();
	importMoralList_t();
	importLoginList();
	importAct_List();
	importAdd_List();
	Student::importTotalList();
	display();//输出界面
	cout << "欢迎，" << getAccount() << ' ' << getName() << "!身份:测评小组成员\n";
	system("pause");
	MainMenu();
}
void Group::MainMenu()
{
	vector<string> questions;
	questions.push_back("学习成绩管理");
	questions.push_back("附加分管理");
	questions.push_back("审核课外活动加分");
	questions.push_back("查询项目");
	questions.push_back("综测成绩生成");
	questions.push_back("修改密码");
	questions.push_back("返回登陆界面");
	questions.push_back("退出系统");
	MultiChoiceMenu(questions, 0);
}
void Group::MultiChoiceMenu(vector<string> Qs, int n)
{
	display();//输出界面
	int num = Qs.size();
	for (int i = 0; i < num; ++i)
	{
		cout << '[' << i + 1 << "] " << Qs[i] << endl;
	}
	cout << "请选择:";
	cin.sync();
	char choice;
	cin >> choice;//输入选项
	//n表示菜单类型 choice表示菜单选项
	switch (n)
	{
	case 0:
		switch (choice)
		{
		case '1': study(); break;
		case '2': addition(); break;
		case '3': checkActivity(); break;
		case '4': search(); break;
		case '5': buildTotal(); break;
		case '6': modifyPassword(); MainMenu(); break;
		case '7': Save(); break;
		case '8': Quit(); break;
		default:cout << "您的输入有误,请重新输入!" << endl;  system("pause"); MainMenu(); break;
		}break;
	case 1:
		switch (choice)
		{
		case '1': newStudy(); break;
		case '2': modifyStudy(); break;
		case '3': delStudy(); break;
		case '4': MainMenu(); break;
		default:cout << "您的输入有误,请重新输入!" << endl;  system("pause"); study(); break;
		}break;
	case 2:
		switch (choice)
		{
		case '1': newAddition(); break;
		case '2': modifyAddition(); break;
		case '3': delAddition(); break;
		case '4': MainMenu(); break;
		default:cout << "您的输入有误,请重新输入!" << endl;  system("pause"); addition(); break;
		}break;
	case 4:
		switch (choice)
		{
		case '1': searchGPA(); break;
		case '2': searchStudy(); break;
		case '3': searchActivity(); break;
		case '4': searchMoral(); break;
		case '5': searchAddition(); break;
		case '6': searchTotal(); break;
		case '7': searchall(); break;
		case '8': MainMenu(); break;
		default:cout << "您的输入有误,请重新输入!" << endl;  system("pause"); search(); break;
		}break;
	}
}
void Group::study()
{
	vector<string> questions;
	questions.push_back("录入学习成绩");
	questions.push_back("修改学习成绩");
	questions.push_back("删除学习成绩");
	questions.push_back("返回");
	(MultiChoiceMenu(questions, 1));
}
void Group::newStudy()
{
	string str, str2;
	float n, m;
	string f;
	display();
	cout << "1:批量录入成绩 2:单个录入成绩 其他:返回" << endl;//输出选项
	cin.sync();
	cout << "请选择:";
	cin >> f;//输入选项
	//判断输入的选项
	if (f == "2")
	{
		display();
		//单个录入成绩
		while (1)
		{
			cout << "录入学生的学号:";
			cin >> str;
			if (exist(str)) //判断输入的学号对应的学生是否存在
				break;
		}
		while (1)
		{
			display();
			cout << "录入<" << searchName(str) << ">的成绩" << endl;//获取学生姓名
			cout << "课程名:";
			cin >> str2;
			judge(n, 1);//输入学分，判断学分是否正确
			judge(m, 2);//输入成绩，判断成绩是否正确
			Course c(str, str2, n, m);//构造Course类对象
			CourseList.insert(pair<string, Course>(str, c));//插入CourseList
			//保存数据
			exportCourseList();
			importCourseList();
			modify();//数据修改，综测成绩需要重新生成
			cout << "录入成功!" << endl;
			system("pause");
			display();
			cout << "1:继续录入 其他:录入结束" << endl;
			cout << "请选择:";
			cin.sync();
			cin >> f;
			//判断是否继续录入
			if (f != "1")
				break;
		}
	}
	if (f == "1")
	{
		display();
		//批量录入成绩
		map<string, User>::iterator iter;
		while (1)
		{
			display();
			cout << "课程名:";
			cin >> str2;
			judge(n, 1);//输入学分，判断学分是否正确
			for (iter = LoginList.begin(); iter != LoginList.end(); iter++)
			{
				if (iter->second.getIdentify() == '0')
				{
					cout << iter->second.getName() << "的成绩:";//获取姓名
					judge(m, 2);//输入成绩，判断学分成绩是否正确
					Course c(iter->first, str2, n, m);//创建临时对象
					CourseList.insert(pair<string, Course>(iter->first, c));//插入CourseList
				}
			}
			//保存数据
			exportCourseList();
			importCourseList();
			modify();//数据修改，综测成绩需要重新生成
			cout << "录入成功!" << endl;
			system("pause");
			display();
			cout << "1:继续批量录入 其他:录入结束" << endl;
			cout << "请选择:";
			cin.sync();
			cin >> f;
			//判断是否继续录入
			if (f != "1")
				break;
		}
	}
	//输入其他选项则返回菜单
	system("pause");
	study();
}
void Group::delStudy()
{
	//判断是否有可以删除的项目
	if (CourseList.size() > 0)
	{
		display();
		multimap<string, Course>::iterator iter;
		string account;
		//打印
		int i = 1,t=0;
		cout <<setw(8)<<"行号"<< setw(10) << "学号" << setw(20) << "课程名" << setw(8) << "学分" << setw(8) << "成绩" <<endl;
		for (iter = CourseList.begin(); iter != CourseList.end(); iter++)
		{
			cout << setw(8) << i << iter->second;
			i++;
			t++;//保存总行数
		}
		while (1)
		{
			line(i, t);//输入需要删除的行数，判断行数是否正确
			if (i == 0)//如果行数为0，则返回
				study();
			iter = CourseList.begin();
			for (i = i - 1; i > 0; i--, iter++);//将iter指向要删除的对象
			CourseList.erase(iter);//删除
			//保存
			exportCourseList();
			importCourseList();
			modify();//数据修改，综测成绩需要重新生成
			cout << "删除成功!" << endl;
			system("pause");
			display();
			//打印
			iter = CourseList.begin();
			i = 1;
			cout << setw(8) << "行号" << setw(10) << "学号" << setw(20) << "课程名" << setw(8) << "学分" << setw(8) << "成绩" << endl;
			for (iter = CourseList.begin(); iter != CourseList.end(); iter++)
			{
				cout << setw(8) << i << iter->second;
				i++;
			}
		}
	}
	else
		cout << "没有可以删除的项目!" << endl;
	system("pause");
	study();
}
void Group::modifyStudy()
{
	//判断是否有可以修改的项目
	if (CourseList.size() > 0)
	{
		display();
		multimap<string, Course>::iterator iter;
		string account, name;
		float credit, grade;
		//打印
		int i = 1,t=0;
		cout << setw(8) << "行号" << setw(10) << "学号" << setw(20) << "课程名" << setw(8) << "学分" << setw(8) << "成绩" << endl;
		for (iter = CourseList.begin(); iter != CourseList.end(); iter++)
		{
			cout << setw(8) << i << iter->second;
			i++;
			t++;//保存行数
		}
		while (1)
		{
			line(i, t);//输入需要修改的行数，并且判断是否正确
			if (i == 0)//如果输入0，则返回
				study();
			iter = CourseList.begin();
			for (i = i - 1; i > 0; i--, iter++);
			while (1)
			{
				cout << "学号:";
				cin >> account;
				if (exist(account))//判断输入的学号对应的学生是否存在
					break;
				else
					cout << "没有学号为" << account << "的学生!" << endl;
			}
			cout << "课程名:";
			cin >> name;
			judge(credit, 1);//输入学分，并且判断是否正确
			judge(grade, 2);//输入成绩，并且判断是否正确
			Course temp(account, name, credit, grade);//创建临时对象
			iter->second = temp;//修改CourseList中对应的对象
			//保存数据
			exportCourseList();
			importCourseList();
			modify();//数据修改，综测成绩需要重新生成
			cout << "修改成功!" << endl;
			system("pause");
			display();
			//打印
			iter = CourseList.begin();
			i = 1;
			cout << setw(8) << "行号" << setw(10) << "学号" << setw(20) << "课程名" << setw(8) << "学分" << setw(8) << "成绩" << endl;
			for (iter = CourseList.begin(); iter != CourseList.end(); iter++)
			{
				cout << setw(8) << i << iter->second;
				i++;
			}
		}
	}
	else
		cout << "没有可以修改的项目!" << endl;
	system("pause");
	study();
}
void Group::addition()
{
	vector<string> questions;
	questions.push_back("录入附加分");
	questions.push_back("修改附加分");
	questions.push_back("删除附加分");
	questions.push_back("返回");
	(MultiChoiceMenu(questions, 2));
}
void Group::newAddition()
{
	display();
	string i;
	string name;
	float grade;
	cout << "1:录入集体项目 2:录入个人项目 其他:返回" << endl;
	cout << "请选择:";
	cin.sync();
	cin >> i;

	//录入集体项目
	if (i == "1")
	{
		display();
		map<string, User>::iterator iter;
		cout << "项目名称:";
		cin >> name;
		judge(grade, 3);//输入附加分，并且判断是否正确
		for (iter = LoginList.begin(); iter != LoginList.end(); iter++)
		{
			if (iter->second.getIdentify() == '0')
				{
					ActAdd temp(iter->second.getAccount(), name, "附加分", grade, "审核通过");//创建临时对象
					Add_List.insert(pair<string, ActAdd>(iter->second.getAccount(), temp));//插入Add_List
				}
		}
		//保存数据
		exportAdd_List();
		importAdd_List();
		modify();//数据修改，综测成绩需要重新生成
		cout << "录入成功!" << endl;
	}
	//录入个人项目
	if (i == "2")
	{
		display();
		string account;
		while (1)
		{
			cout << "学号:";
			cin >> account;
			if (exist(account))//判断学号对应的学生是否存在
				break;
		}

		cout << "项目名称:";
		cin >> name;
		judge(grade, 3);//输入附加分，判断输入的是否正确
		ActAdd temp(account, name, "附加分", grade, "审核通过");//创建临时对象
		Add_List.insert(pair<string, ActAdd>(account, temp));//插入Add_List中
		//保存数据
		exportAdd_List();
		importAdd_List();
		modify();//数据修改，综测成绩需要重新生成
		cout << "录入成功!" << endl;
	}
	system("pause");
	addition();
}
void Group::modifyAddition()
{
	if (Add_List.size() > 0)
	{
		display();
		multimap<string, ActAdd>::iterator iter;
		string account, name;
		int i = 1, t = 0;
		cout << setw(8) << "行数" << setw(10) << "学号" << setw(20) << "活动名" << setw(12) << "类型" << setw(8) << "分数" << setw(14) << "状态" << endl;
		for (iter = Add_List.begin(); iter != Add_List.end(); iter++)
		{
			cout << setw(8) << i << iter->second;
			i++;
			t++;
		}
		while (1)
		{
			line(i, t);
			if (i == 0)
				addition();
			iter = Add_List.begin();
			for (i = i - 1; i > 0; i--, iter++);
			float grade;
			cout << "学号:";
			cin >> account;
			cout << "项目名称:";
			cin >> name;
			judge(grade, 3);
			ActAdd temp(account, name, "附加分", grade, "审核通过");
			iter->second = temp;
			exportAdd_List();
			importAdd_List();
			modify();
			cout << "修改成功!" << endl;
			system("pause");
			display();
			iter = Add_List.begin();
			i = 1;
			cout << setw(8) << "行数" << setw(10) << "学号" << setw(20) << "活动名" << setw(12) << "类型" << setw(8) << "分数" << setw(14) << "状态" << endl;
			for (iter = Add_List.begin(); iter != Add_List.end(); iter++)
			{
				cout << setw(8) << i << iter->second;
				i++;
			}
		}

	}
	else
		cout << "没有可以修改的项目!" << endl;
	system("pause");
	addition();
}
void Group::delAddition()
{
	if (Add_List.size() > 0)
	{
		display();
		multimap<string, ActAdd>::iterator iter;
		string account;
		int i = 1, t = 0;
		cout << setw(8) << "行数" << setw(10) << "学号" << setw(20) << "活动名" << setw(12) << "类型" << setw(8) << "分数" << setw(14) << "状态" << endl;
		for (iter = Add_List.begin(); iter != Add_List.end(); iter++)
		{
			cout << setw(8) << i << iter->second;
			i++;
			t++;
		}
		while (1)
		{
			line(i, t);
			if (i == 0)
				addition();
			display();
			iter = Add_List.begin();
			for (i = i - 1; i > 0; i--, iter++);
			Add_List.erase(iter);
			exportAdd_List();
			importAdd_List();
			modify();
			cout << "删除成功!" << endl;
			system("pause");
			display();
			iter = Add_List.begin();
			i = 1;
			cout << setw(8) << "行数" << setw(10) << "学号" << setw(20) << "活动名" << setw(12) << "类型" << setw(8) << "分数" << setw(14) << "状态" << endl;
			for (iter = Add_List.begin(); iter != Add_List.end(); iter++)
			{
				cout << setw(8) << i << iter->second;
				i++;
			}
		}

	}
	cout << "没有可以删除的项目!" << endl;
	system("pause");
	addition();
}
void Group::buildMoral()
{
	Student::importTotalList();
	typedef multimap<string, Moral_s>::iterator Iter;
	pair<Iter, Iter> iter1;
	map<string, User>::iterator iter2;
	multimap<float, string> temp;
	multimap<float, string>::reverse_iterator iter3;

	float sum = 0.0;
	int n = 0;
	string str;
	for (iter2 = LoginList.begin(); iter2 != LoginList.end(); iter2++)
	{
		if (iter2->second.getYes() == '0'&&iter2->second.getIdentify() != '2')
		{
			n++;
			cout << searchName(iter2->first) << ' ';
		}
	}
	if (n != 0)
	{
		cout << "思想品德未打分!" << endl;
		system("pause");
		MainMenu();
	}
	for (iter2 = LoginList.begin(); iter2 != LoginList.end(); iter2++)
	{
		if (iter2->second.getIdentify() == '0')
		{
			iter1 = MoralList_sget.equal_range(iter2->first);
			while (iter1.first != iter1.second)
			{
				sum += (iter1.first->second.getsum());
				n += 10;
				iter1.first++;
			}
			sum += (MoralList_t[iter2->first].getsum());
			sum /= (n + 3);

			temp.insert(pair<float, string>(sum, iter2->first));
		}

		sum = 0.0;
		n = 0;
	}

	float s = 100;

	for (iter3 = temp.rbegin(); iter3 != temp.rend(); iter3++, s--)
	{

		Student stu = Student::TotalList[iter3->second];
		stu.inmoral(s);
		Student::TotalList[iter3->second] = stu;
	}

	Student::exportTotalList();
	Student::importTotalList();

}
bool Group::readyForTotal()
{
	bool ready = true;
	int studentCount = 0;
	map<string, User>::iterator userIter;
	for (userIter = LoginList.begin(); userIter != LoginList.end(); userIter++)
	{
		if (userIter->second.getIdentify() == '0')
			studentCount++;
	}
	for (userIter = LoginList.begin(); userIter != LoginList.end(); userIter++)
	{
		if (userIter->second.getIdentify() != '2' && userIter->second.getYes() == '0')
		{
			cout << searchName(userIter->first) << "尚未完成思想品德打分" << endl;
			ready = false;
		}
		if (userIter->second.getIdentify() == '0')
		{
			if (Student::TotalList.find(userIter->first) == Student::TotalList.end())
			{
				cout << searchName(userIter->first) << "缺少综测总表记录" << endl;
				ready = false;
			}
			if (CourseList.find(userIter->first) == CourseList.end())
			{
				cout << searchName(userIter->first) << "缺少课程成绩" << endl;
				ready = false;
			}
			if (MoralList_t.find(userIter->first) == MoralList_t.end())
			{
				cout << searchName(userIter->first) << "缺少班主任思想品德评分" << endl;
				ready = false;
			}
			if (MoralList_sget.count(userIter->first) != static_cast<size_t>(studentCount))
			{
				cout << searchName(userIter->first) << "学生互评记录不完整" << endl;
				ready = false;
			}
		}
	}

	multimap<string, ActAdd>::iterator actIter;
	for (actIter = Act_List.begin(); actIter != Act_List.end(); actIter++)
	{
		if (actIter->second.getGet() == "未审核")
		{
			cout << actIter->second.getAccount() << "存在未审核的课外活动" << endl;
			ready = false;
		}
	}

	if (!ready)
	{
		cout << "请先完成以上项目，再生成综测总分!" << endl;
		system("pause");
	}
	return ready;
}
void Group::checkActivity()
{
	multimap<string, ActAdd>::iterator iter;
	ActAdd temp;
	int i = 0, t;
	string ch;
	//判断有多少课外活动项目未审核
	for (iter = Act_List.begin(); iter != Act_List.end(); iter++)
	{
		if (iter->second.getGet() == "未审核")
			i++;
	}
	if (i > 0)//如果有课外活动未审核
	{
		//审核过程
		t = i;
		for (iter = Act_List.begin(); iter != Act_List.end(); iter++)
		{
			display();//打印界面
			cout << "还有<" << t << ">项未审核" << endl;//打印剩余的未审核数量
			if (iter->second.getGet() == "未审核")//找到未审核的项目
			{
				//输出未审核的项目

				cout << setw(10) << "学号" << setw(20) << "活动名" << setw(12) << "类型" << setw(8) << "分数" << setw(14) << "状态" << endl;
				cout << iter->second;
				temp = iter->second;//将容器中的对象赋值给临时对象
				while (1)
				{
					cout << "1:审核通过  2:审核未通过" << endl;
					cout << "请选择:";
					cin.sync();//清空输入缓存区
					cin >> ch;//输入选项
					if (ch == "1")
					{
						temp.modifyGet(1);//修改临时对象为审核通过
						break;
					}
					if (ch == "2")
					{
						temp.modifyGet(2);//修改临时对象为审核未通过
						break;
					}
					cout << "您的输入有误!" << endl;
				}
				iter->second = temp;//修改容器中对应的对象
				cout << "操作成功" << endl;
				system("pause");
				t--;//剩余项目数减1
			}
		}
		//保存
		exportAct_List();
		importAct_List();
	}
	else
	{
		cout << "没有需要审核的项目" << endl;
		system("pause");
	}

	MainMenu();
}
void Group::buildActivity()
{
	Student::importTotalList();
	map<string, User>::iterator iter1;
	typedef multimap<string, ActAdd>::iterator Iter;
	pair<Iter, Iter> iter2;
	multimap<string, ActAdd>::iterator iter3;
	int i = 0;
	for (iter3 = Act_List.begin(); iter3 != Act_List.end(); iter3++)
	{
		if (iter3->second.getGet() == "未审核")
			i++;
	}
	if (i != 0)
	{
		cout << "存在未审核的课外活动，请审核!" << endl;
		system("pause");
		checkActivity();
	}
	float sum = 0.0;
	for (iter1 = LoginList.begin(); iter1 != LoginList.end(); iter1++)
	{
		if (iter1->second.getIdentify() == '0')
		{
			iter2 = Act_List.equal_range(iter1->first);
			while (iter2.first != iter2.second)
			{
				sum += iter2.first->second.getGrade();
				iter2.first++;
			}
			Student::TotalList[iter1->first].inactivity(sum);
			sum = 0.0;
		}
	}
	Student::exportTotalList();
	Student::importTotalList();
}
void Group::buildAddition()
{
	Student::importTotalList();
	map<string, User>::iterator iter1;
	typedef multimap<string, ActAdd>::iterator Iter;
	pair<Iter, Iter> iter2;
	multimap<string, ActAdd>::iterator iter3;
	float sum = 0.0;
	for (iter1 = LoginList.begin(); iter1 != LoginList.end(); iter1++)
	{
		if (iter1->second.getIdentify() == '0')
		{
			iter2 = Add_List.equal_range(iter1->first);
			while (iter2.first != iter2.second)
			{
				sum += iter2.first->second.getGrade();
				iter2.first++;
			}
			if (sum > 5)
				sum = 5;
			Student temp = Student::TotalList[iter1->first];
			temp.inaddition(sum);
			Student::TotalList[iter1->first] = temp;
			sum = 0.0;
		}
	}
	Student::exportTotalList();
	Student::importTotalList();
}
void Group::getGPA()
{
	Student::importTotalList();
	importCourseList();
	map<string, User>::iterator iter;
	typedef multimap<string, Course>::iterator Iter;
	for (iter = LoginList.begin(); iter != LoginList.end(); iter++)
	{
		if (iter->second.getIdentify() == '0')
		{
			pair<Iter, Iter> iter2 = CourseList.equal_range(iter->second.getAccount());
			float gpa = 0.0, gredit = 0.0;
			while (iter2.first != iter2.second)
			{
				float a = iter2.first->second.getGrade();
				float b = iter2.first->second.getCredit();
				if (a <= 100 && a >= 90)
				{
					gredit += b;
					gpa += b*4.0f;
				}
				if (a < 90 && a >= 85)
				{
					gredit += b;
					gpa += b*3.7f;
				}
				if (a < 85 && a >= 82)
				{
					gredit += b;
					gpa += b*3.3f;
				}
				if (a <82 && a >= 78)
				{
					gredit += b;
					gpa += b*3.0f;
				}
				if (a <78 && a >= 75)
				{
					gredit += b;
					gpa += b*2.7f;
				}
				if (a <75 && a >= 72)
				{
					gredit += b;
					gpa += b*2.3f;
				}
				if (a <72 && a >= 68)
				{
					gredit += b;
					gpa += b*2.0f;
				}
				if (a <68 && a >= 64)
				{
					gredit += b;
					gpa += b*1.5f;
				}
				if (a <64 && a >= 60)
				{
					gredit += b;
					gpa += b*1.0f;
				}
				if (a < 60){
					gredit += b;
					gpa += 0.0f;
				}
				iter2.first++;
			}
			if (gredit != 0.0)
			{
				gpa /= gredit;
				gpa*=100;
				gpa = int(gpa);
				gpa /= 100;
				Student::TotalList[iter->second.getAccount()].ingpa(gpa);
			}
			else
				Student::TotalList[iter->second.getAccount()].ingpa(0.0);

		}
		float gpa = 0.0;
		float gredit = 0.0;
	}
	Student::exportTotalList();
	Student::importTotalList();
}
void Group::buildStudy()
{
	Student::importTotalList();
	getGPA();
	map<string, Student>::iterator iter;
	float gpa=0.0;
	for (iter = Student::TotalList.begin(); iter != Student::TotalList.end(); iter++)
	{
		gpa = iter->second.getgpa();
		gpa *= 25;
		gpa *= 100;
		gpa = int(gpa);
		gpa /= 100;
		iter->second.instudy(gpa);
		gpa = 0.0;
	}
	Student::exportTotalList();
	Student::importTotalList();
}
void Group::buildTotal()
{
	if (!readyForTotal())
	{
		MainMenu();
		return;
	}
	buildMoral();
	buildActivity();
	buildAddition();
	buildStudy();
	Student::importTotalList();
	map<string, Student>::iterator iter;
	multimap<float, Student> rank;
	float sum = 0.0;
	for (iter = Student::TotalList.begin(); iter != Student::TotalList.end(); iter++)
	{
		Student temp = iter->second;
		temp.intotal();
		iter->second = temp;
		rank.insert(pair<float, Student>(temp.gettotal(), temp));
	}
	ofstream ofile;
	ofile.open("Total.txt");
	Close(ofile);
	multimap<float, Student>::iterator iter2;
	ofile << rank.size() << endl;
	int i=rank.size();
	for (iter2 = rank.begin(); iter2 != rank.end(); ++iter2,i--)
	{
		iter2->second.inrange(i);
		ofile << iter2->second;
	}
	ofile.close();
	LoginList[Account].modifyYes();
	exportLoginList();
	importLoginList();
	Student::importTotalList();
	cout << "生成成功！"<<endl;
	system("pause");
	MainMenu();
}
void Group::search()
{
	if (LoginList[Account].getYes() == '0')
	{
		cout << "尚未生成综测成绩,无法查询!" << endl;
		system("pause");
		MainMenu();
	}
	Student::importTotalList();
	vector<string> questions;
	questions.push_back("查询GPA");
	questions.push_back("查询学习成绩");
	questions.push_back("查询课外活动成绩");
	questions.push_back("查询思想品德成绩");
	questions.push_back("查询附加分");
	questions.push_back("查询综测成绩");
	questions.push_back("查询所有");
	questions.push_back("返回");
	(MultiChoiceMenu(questions, 4));
}
void Group::searchStudy()
{
	string f;
	display();
	cout << "1:查询全部 2:查询单个学生 其他:返回" << endl;

	cout << "请选择:";
	cin.sync();
	cin >> f;
	display();
	if (f == "1")
	{
		cout << setw(20) << "姓名" << setw(8) << "学习成绩" << endl;
		map<string, Student>::iterator iter;
		for (iter = Student::TotalList.begin(); iter != Student::TotalList.end(); iter++)
		{
			cout << setw(20) << searchName(iter->first) <<setw(8)<< iter->second.getstudy()<<endl;
		}

	}
	if (f == "2")
	{
		string s;
		cout << "请输入查询的学号(返回请输入0):";
		cin >> s;
		if (s == "0")
			search();
		if (exist(s))
		{
			map<string, Student>::iterator totalIter = Student::TotalList.find(s);
			if (totalIter == Student::TotalList.end())
				cout << "Total.txt 缺少学号 " << s << " 的综测记录!" << endl;
			else
				cout << searchName(s) << "的学习成绩为:" << totalIter->second.getstudy() << endl;
		}
	}
	system("pause");
	search();
}
void Group::searchGPA()
{
	string f;
	display();
	cout << "1:查询全部 2:查询单个学生 其他:返回" << endl;
	cin.sync();
	cout << "请选择:";
	cin >> f;
	display();
	if (f == "1")
	{
		cout << setw(20) << "姓名" << setw(7) << "GPA" << endl;
		map<string, Student>::iterator iter;
		for (iter = Student::TotalList.begin(); iter != Student::TotalList.end(); iter++)
		{
			cout << setw(20) << searchName(iter->first) << setw(7) << iter->second.getgpa() << endl;
		}

	}
	if (f == "2")
	{
		string s;
		cout << "请输入查询的学号(返回请输入0):";
		cin >> s;
		if (s == "0")
			search();
		if (exist(s))
		{
			map<string, Student>::iterator totalIter = Student::TotalList.find(s);
			if (totalIter == Student::TotalList.end())
				cout << "Total.txt 缺少学号 " << s << " 的综测记录!" << endl;
			else
				cout << searchName(s) << "的GPA为:" << totalIter->second.getgpa() << endl;
		}
	}
	system("pause");
	search();
}
void Group::searchActivity()
{
	string f;
	display();//打印界面
	cout << "1:查询全部 2:查询单个学生 其他:返回" << endl;
	cin.sync();
	cout << "请选择:";
	cin >> f;
	display();//打印界面
	if (f == "1")//查询全部
	{
		//输出标题
		cout << setw(20) << "姓名" << setw(16) << "课外活动成绩" << endl;
		map<string, Student>::iterator iter;
		//打印所有学生成绩
		for (iter = Student::TotalList.begin(); iter != Student::TotalList.end(); iter++)
		{
			cout << setw(20) << searchName(iter->first) << setw(16) << iter->second.getactivity() << endl;
		}

	}
	if (f == "2")//查询单个学生
	{
		string s;
		cout << "请输入查询的学号(返回请输入0):";
		cin >> s;
		if (s == "0")//如果输入0，返回菜单
			search();
		if (exist(s))//判断输入的学号是否存在，是否是学生
		{
			map<string, Student>::iterator totalIter = Student::TotalList.find(s);
			if (totalIter == Student::TotalList.end())
				cout << "Total.txt 缺少学号 " << s << " 的综测记录!" << endl;
			else
				cout << searchName(s) << "的课外活动成绩为:" << totalIter->second.getactivity() << endl;
		}
	}
	system("pause");
	search();//返回菜单
}
void Group::searchMoral()
{
	string f;
	display();
	cout << "1:查询全部 2:查询单个学生 其他:返回" << endl;
	cin.sync();
	cout << "请选择:";
	cin >> f;
	display();
	if (f == "1")
	{
		cout << setw(20) << "姓名" << setw(16) << "思想品德成绩" << endl;
		map<string, Student>::iterator iter;
		for (iter = Student::TotalList.begin(); iter != Student::TotalList.end(); iter++)
		{
			cout << setw(20) << searchName(iter->first) << setw(16) << iter->second.getmoral() << endl;
		}

	}
	if (f == "2")
	{
		string s;
		cout << "请输入查询的学号(返回请输入0):";
		cin >> s;
		if (s == "0")
			search();
		if (exist(s))
		{
			map<string, Student>::iterator totalIter = Student::TotalList.find(s);
			if (totalIter == Student::TotalList.end())
				cout << "Total.txt 缺少学号 " << s << " 的综测记录!" << endl;
			else
				cout << searchName(s) << "的思想品德成绩为:" << totalIter->second.getmoral() << endl;
		}
	}
	system("pause");
	search();
}
void Group::searchAddition()
{
	string f;
	display();
	cout << "1:查询全部 2:查询单个学生 其他:返回" << endl;
	cin.sync();
	cout << "请选择:";
	cin >> f;
	display();
	if (f == "1")
	{
		cout << setw(20) << "姓名" << setw(10) << "附加分" << endl;
		map<string, Student>::iterator iter;
		for (iter = Student::TotalList.begin(); iter != Student::TotalList.end(); iter++)
		{
			cout << setw(20) << searchName(iter->first) << setw(10) << iter->second.getaddition() << endl;
		}

	}
	if (f == "2")
	{
		string s;
		cout << "请输入查询的学号(返回请输入0):";
		cin >> s;
		if (s == "0")
			search();
		if (exist(s))
		{
			map<string, Student>::iterator totalIter = Student::TotalList.find(s);
			if (totalIter == Student::TotalList.end())
				cout << "Total.txt 缺少学号 " << s << " 的综测记录!" << endl;
			else
				cout << searchName(s) << "的附加分为:" << totalIter->second.getaddition() << endl;
		}
	}
	system("pause");
	search();
}
void Group::searchTotal()
{
	string f;
	display();
	cout << "1:查询全部 2:查询单个学生 其他:返回" << endl;
	cin.sync();
	cout << "请选择:";
	cin >> f;
	display();
	if (f == "1")
	{
		cout << setw(20) << "姓名" << setw(12) << "综测成绩" << endl;
		map<string, Student>::iterator iter;
		for (iter = Student::TotalList.begin(); iter != Student::TotalList.end(); iter++)
		{
			cout << setw(20) << searchName(iter->first) << setw(12) << iter->second.gettotal() << endl;
		}

	}
	if (f == "2")
	{
		string s;
		cout << "请输入查询的学号(返回请输入0):";
		cin >> s;
		if (s == "0")
			search();
		if (exist(s))
		{
			map<string, Student>::iterator totalIter = Student::TotalList.find(s);
			if (totalIter == Student::TotalList.end())
				cout << "Total.txt 缺少学号 " << s << " 的综测记录!" << endl;
			else
				cout << searchName(s) << "的综测成绩为:" << totalIter->second.gettotal() << endl;
		}
	}
	system("pause");
	search();
}
void Group::searchall()
{
	display();
	cout <<setw(10) <<"姓名"<< setw(8) << "学号" << setw(12) << "学习成绩" << setw(7) << "GPA" << setw(14)
		<< "课外活动成绩" << setw(14) << "思想品德成绩" << setw(10) << "附加分" << setw(12)
		<< "综测成绩" << setw(8) << "排名" << endl;
	map<string, Student>::iterator iter;
	for (iter = Student::TotalList.begin(); iter != Student::TotalList.end(); iter++)
	{
		cout <<setw(10)<<searchName(iter->first)<< iter->second;
	}
	system("pause");
	search();
}
bool Group::exist(string s)
{
	//在LoginList用户信息中查找，判断是否为学生
	map<string, User>::iterator iter = LoginList.find(s);
	if (iter == LoginList.end() || iter->second.getIdentify() != '0')//不是学生
	{
		cout << "没有学号为" << s << "的学生!" << endl;
		return false;//返回查找失败
	}
	return true;//返回查找成功
}
