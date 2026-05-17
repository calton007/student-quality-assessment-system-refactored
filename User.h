#pragma once
#include <string>
#include <functional>
#include <algorithm>
#include <vector>
#include <map>
#include "Course.h"
#include "ActAdd.h"
#include "Moral_s.h"
#include "Moral_t.h"
#include <fstream>
#include <iostream>


using namespace std;
class User
{
protected:
	string Account;//账号
	string Name;//姓名
	string Password;//密码
	char Identify;//用户类型（0:学生 1:班主任 2:测评小组）
	char Yes;//学生、班主任：思想品德是否已打分（0:未打分 1:已打分） 测评小组：综测成绩是否生成
	static map<string, User> LoginList; //储存用户信息 关键字:学号
	static multimap<string, Course> CourseList; //储存课程信息 关键字：学号
	static multimap<string, Moral_s> MoralList_sget; //储存学生打分信息 关键字：得分人学号
	static multimap<string, Moral_s> MoralList_sgive; //储存学生打分信息 关键字：打分人学号
	static map<string, Moral_t>  MoralList_t;  //储存班主任打分信息 关键字:得分人学号
	static multimap<string, ActAdd > Act_List; //储存课外活动信息 关键字：学号
	static multimap<string, ActAdd> Add_List;//储存附加分信息 关键字：学号

public:
	User(string,string,string,char,char);//构造函数
	User();//构造函数
	void display();//打印界面
	User Login();//登陆
	void Menu(User);//根据登陆用户类型创建对应对象
	void modifyPassword();//修改密码
	void Save();//返回登陆界面
	void Quit();//退出
	//判断
	void line(int&, const int&);//判断输入的行数是否正确
	void judge(float&,int);//判断分数是否正确
	void Grade(float*,int);//思想品德打分（同时判断分数是否正确）
	//获取
	string getAccount() ;//获取账号
	void modifyAccount(string);//修改账号
	string getPassword() ;//获取密码
	string getName() ;//获取姓名
	char getIdentify() ;//获取用户类型
	char getYes() ;//获取是否打分
	void modifyYes();//修改打分状态（未打分->已打分）
	void modify();
	void modifyNo();//修改综测生成状态（生成->未生成）
	string searchName(string);//根据学号返回姓名
	//数据存取
	void importLoginList();//导入用户信息
	void importCourseList();//导入课程信息
	void importAct_List();//导入课外活动信息
	void importAdd_List();//导入附加分信息
	void importMoralList_s();//导入学生打分信息
	void importMoralList_t();//导入班主任打分信息
	void exportLoginList();//导出用户信息
	void exportCourseList();//导出课程信息
	void exportAct_List();//导出课外活动信息
	void exportAdd_List();//导出附加分信息
	void exportMoralList_s();//导出学生打分信息
	void exportMoralList_s2();//导出学生打分信息
	void exportMoralList_t();//导出班主任打分信息
	//运算符重载
	friend istream& operator>>(istream &in, User &u);
	friend ostream& operator<<(ostream &out, const User &u);
	static void Close(ofstream&);//判断是否正常打开文件
};
