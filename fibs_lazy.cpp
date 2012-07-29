#include "thunk"
#include <iostream>

using namespace xlnagla;

template<typename T>
class unique_thunkList
{

public:

	unique_thunk<T> elem;
	unique_thunk<unique_thunkList<T> > next;

	unique_thunkList(std::function<T* () > elem, std::function<unique_thunkList<T>* () > next):elem(elem),next(next){}
	static unique_thunkList* make_list(std::function<T* () > elem, std::function<unique_thunkList<T>* () > next){
 		return new unique_thunkList(elem, next);
	}

	inline friend std::ostream& operator<<(std::ostream& out, const unique_thunkList& t){
		out << *t.elem;
		return out;
	}


};

class destroyInt{


public:
	destroyInt(int i):value(i){}
	const int value;
	
	inline friend std::ostream& operator<<(std::ostream& out, const destroyInt& i){
		out << i.value;
		return out;
	}
	
	int operator+(int i ){return i + value;}

	~destroyInt(){
		std::cout << "destroy " << value << std::endl;
	}
};

std::function<unique_thunkList<destroyInt>* () > thunk_helper(std::function<destroyInt* () > f){


	std::function<destroyInt* ()> f2 = [f](){
		return new destroyInt(*(f())+ 2);
	};


	std::function<unique_thunkList<destroyInt>* ()> arg2 = [f2 ]()
		{
			return thunk_helper(f2)();
		};
	std::function<destroyInt* () > arg1 = [f](){return new destroyInt(*(f()) + 1);};

	std::function<unique_thunkList<destroyInt>* ()> ret = [arg1, arg2](){
		return unique_thunkList<destroyInt>::make_list(arg1 ,arg2 );
	};

	return ret;
}


int main(){
	
	std::cout << destroyInt(12) << std::endl;

	std::function<destroyInt* ()  > foo1 = [](){destroyInt* a = new destroyInt(12); return a;};

	
	std::function<unique_thunkList<destroyInt>* () > next = [foo1](){return thunk_helper(foo1)();};
	unique_thunkList<destroyInt> test(foo1, next);
	while(true){
		std::cout << test.elem->value << std::endl;
		test = std::move(*test.next);
	}
}

