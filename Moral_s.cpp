#include "Moral_s.h"
Moral_s::Moral_s(string aget, string agive, float a[9])
{
	Account_get = aget;
	Account_give = agive;
	for (int i = 0; i < 9; i++)
	{
		ch[i] = a[i];
	}
}
string Moral_s::getAccount_get()
{
	return Account_get;
}
string Moral_s::getAccount_give()
{
	return Account_give;
}
float Moral_s::getsum()
{
	int i;
	float sum = 0.0;
	for (i = 0; i < 9; i++)
	{
		sum += ch[i];
	}
	return sum;
}
istream& operator>>(istream &in, Moral_s &u)
{
	int i;
	in >> u.Account_get >> u.Account_give;
	for (i = 0; i < 9 ; i++)
	{
		in >> u.ch[i];
	}
	return in;
}
ostream& operator<<(ostream &out, const Moral_s &u)
{
	int i;
	out << u.Account_get << '\t' << u.Account_give;
	for (i = 0; i < 9; i++)
	{
		out << '\t' << u.ch[i];
	}
	out << endl;
	return out;
}