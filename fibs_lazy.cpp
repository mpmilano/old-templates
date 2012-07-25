#include "Thunk.h"
#include <iostream>


template<typename T>
class ThunkList
{

public:

	Thunk<T> elem;
	Thunk<ThunkList<T> > next;

	ThunkList(std::function<T* () > elem, std::function<ThunkList<T>* () > next):elem(elem),next(next){}
	static ThunkList* make_list(std::function<T* () > elem, std::function<ThunkList<T>* () > next){
 		return new ThunkList(elem, next);
	}

};

class destroyInt{


public:
	destroyInt(int i):value(i){}
	const int value;

	int operator+(int i ){return i + value;}

	~destroyInt(){
		std::cout << "destroy " << value << std::endl;
	}
};

std::function<ThunkList<destroyInt>* () > thunk_helper(std::function<destroyInt* () > f){


	std::function<destroyInt* ()> f2 = [f](){
		return new destroyInt(*(f())+ 2);
	};


	std::function<ThunkList<destroyInt>* ()> arg2 = [f2 ]()
		{
			return thunk_helper(f2)();
		};
	std::function<destroyInt* () > arg1 = [f](){return new destroyInt(*(f()) + 1);};

	std::function<ThunkList<destroyInt>* ()> ret = [arg1, arg2](){
		return ThunkList<destroyInt>::make_list(arg1 ,arg2 );
	};

	return ret;
}


int main(){
	std::function<destroyInt* ()  > foo1 = [](){destroyInt* a = new destroyInt(12); return a;};

	
	std::function<ThunkList<destroyInt>* () > next = [foo1](){return thunk_helper(foo1)();};
	ThunkList<destroyInt> test(foo1, next);
	while(true){
		std::cout << test.elem->value << std::endl;
		test = std::move(*test.next);
	}
}

