#include "xlnagla_functional"
#include "assert_once.h"
#include <iostream>
#include <string>




int main(int argc, char**){

    using namespace std;
	using namespace xlnagla;

	string a = "a";
	string b = "b";
	string c = "c";
	bool abool = false;
	bool bbool = true;

	
	 
	std::function<std::string* ()> test = [](){return new string("a");};



	{
	}

	auto macrod = UNIQUE_THUNK(new string("a"));

	constexpr auto constest = 3;

	auto unique1 = unique_ptr<string>(new string(a));
	auto unique2 = std::move(unique1);

	auto lazy1 = make_unique_thunk(test);
	auto lazy2 = std::move(lazy1);

	cout << lazy_cond(
		&abool, a, 
		&bbool, b, 
		c) << endl;

	cout << cond(abool, a, bbool, b, c ) << endl;
	
}
