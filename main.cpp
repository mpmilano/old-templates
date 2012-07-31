#include "xlnagla_functional"
#include "assert_once.h"
#include <iostream>
#include <string>

int main(int, char**){

    using namespace std;
	using namespace xlnagla;

	string a = "a";
	string b = "b";
	string c = "c";

	cout << cond(c, false, a, true, b) << endl;
}
