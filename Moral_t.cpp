#include "Moral_t.h"
#include <string>
using namespace std;
Moral_t::Moral_t(string get, string give, float n[3])
{
	Account_get = get;
	Account_give = give;
	for (int i = 0; i < 3; i++)
	{
		ch[i] = n[i];
	}
}
string Moral_t::getAccount_get()
{
	return Account_get;
}
string Moral_t::getAccount_give()
{
	return Account_give;
}
float Moral_t::getsum()
{
	int i;
	float sum = 0.0;
	for (i = 0; i < 3; i++)
	{
		sum += ch[i];
	}
	return sum;
}
istream& operator>>(istream &in, Moral_t &u)
{
	int i;
	in >> u.Account_get >> u.Account_give;
	for (i = 0; i < 3; i++)
	{
		in >> u.ch[i];
	}
	return in;
}
ostream& operator<<(ostream &out, const Moral_t &u)
{
	int i;
	out << u.Account_get << '\t' << u.Account_give;
	for (i = 0; i < 3; i++)
	{
		out << '\t' << u.ch[i];
	}
	out << endl;
	return out;
}
