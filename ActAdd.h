#pragma once
#include <string>
using namespace std;
class ActAdd
{
private:
	string Account;//学号
	string Name;//活动名
	string Type;//类型（课外活动或附加分）
	float Grade;//分数
	string Get;//状态（"未审核""审核通过""审核未通过"）
public:
	ActAdd(string, string, string, float, string);//构造函数
	ActAdd(){};//构造函数
	string getAccount();//返回学号
	string getType();//返回类型
	float getGrade();//返回分数
	string getGet();//返回状态
	void modifyGet(int);//修改状态（审核的时候用）
	//重载运算符
	friend istream& operator>>(istream &in, ActAdd &u);
	friend ostream& operator<<(ostream &out, const ActAdd &u);
};