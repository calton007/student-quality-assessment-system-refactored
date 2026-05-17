#pragma once
#include <iostream>
#include <string>
using namespace std;
class Moral_t
{
private:
	string Account_get;//得分人学号
	string Account_give;//打分人学号
	float ch[3];//各项得分
public:
	Moral_t(string, string, float n[3]);//构造函数
	Moral_t(){}//构造函数
	string getAccount_get();//返回得分人学号
	string getAccount_give();//返回打分人学号
	float getsum();//返回总分
	//重载运算符
	friend istream& operator>>(istream &in, Moral_t &u);
	friend ostream& operator<<(ostream &out, const Moral_t &u);
};