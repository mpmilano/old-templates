#include "xlnagla_functional"
#include "thunk"
#include "assert_once.h"
#include <iostream>
#include <string>
#include "memoize.h"



int main(int argc, char**){

    using namespace std;
	using namespace xlnagla;

	string a = "a";
	string b = "b";
	string c = "c";
	bool abool = false;
	bool bbool = true;

	
	 
	std::function<std::string* ()> test = [](){return new string("a");};

	auto macrod = UNIQUE_THUNK(new string("a"));


	cout << *macrod << endl;

	std::function<int (int)> fibs = [&](int a){
		if (a > 1)
			return fibs(a - 1 ) + fibs (a - 2);
		else if (a == 1)
			return 1;
		else if (a == 0)
			return 0;
		else
			throw "AAAAAA";
		return -1;
	};

	fibs = memoize(fibs);

	std::function<int ()> three = [](){return 3;};
	auto thunktest = make_thunk(three);

	auto thunkmacrotest = THUNK("foo");

	cout << "thunk test " << thunktest << endl;
	cout << thunkmacrotest << endl;

	cout << fibs(30) << endl;

	int anint = int();
	
	cout << "foo foo " << anint << endl;

	std::function<int (int, int) > fadd = [](int a, int b){return a+b;};

	cout << xlnagla::foldl(fadd, {1,2,3,4, 5}) << endl;

	auto unique1 = unique_ptr<string>(new string(a));
	auto unique2 = std::move(unique1);

	auto lazy1 = make_unique_thunk(test);
	auto lazy2 = std::move(lazy1);

	cout << lazy_cond(
		&abool, a, 
		&bbool, b, 
		c) << endl;

	cout << cond(abool, a, bbool, b, c ) << endl;

	std::function<int (int)> times2 = [](int i){return i*2;};

	for (auto foo : xlnagla::map(times2,{1,2,3,4}) )
		cout << foo << endl;

	auto curried = curry2([](int a, int b){ return a + b;});
	std::cout << curried(1)(2) << std::endl;

	std::function<int (int, int, int) > othercurry = [](int a, int b, int c){return a + b + c;};
	std::cout << curry(curry(othercurry)(1))(2)(3) << endl;


	auto convert_to_function_non_macro_test = convert_to_function([&](int i){return i;});

	auto convert_to_2arg_function_test = convert_to_2arg_function([&](int i, double j){return std::string("foo");});

}
