#include "thunk"
#include <iostream>

using namespace xlnagla;

template<typename T>
class shared_thunkList
{

public:

	shared_thunk<T, std::launch::deferred> elem;
	shared_thunk<shared_thunkList<T>, std::launch::deferred > next;

	shared_thunkList(std::function<T* () > elem, std::function<shared_thunkList<T>* () > next):elem(elem),next(next){}
	static shared_thunkList* make_list(std::function<T* () > elem, std::function<shared_thunkList<T>* () > next){
 		return new shared_thunkList(elem, next);
	}

	inline friend std::ostream& operator<<(std::ostream& out, const shared_thunkList& t){
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

std::function<shared_thunkList<destroyInt>* () > thunk_helper(std::function<destroyInt* () > f){

	std::function<destroyInt* ()> f2 = [f](){
		return new destroyInt(*(f())+ 2);
	};


	std::function<shared_thunkList<destroyInt>* ()> arg2 = [f2 ]()
		{
			return thunk_helper(f2)();
		};
	std::function<destroyInt* () > arg1 = [f](){return new destroyInt(*(f()) + 1);};

	std::function<shared_thunkList<destroyInt>* ()> ret = [arg1, arg2](){
		return shared_thunkList<destroyInt>::make_list(arg1 ,arg2 );
	};

	return ret;
}


int main(){
	
	std::cout << destroyInt(12) << std::endl;

	std::function<destroyInt* ()  > foo1 = [](){destroyInt* a = new destroyInt(12); return a;};

	std::cout << std::async(foo1).get() << std::endl;

	
	std::function<shared_thunkList<destroyInt>* () > next = [foo1](){return thunk_helper(foo1)();};
	shared_thunkList<destroyInt> test(foo1, next);
	while(true){
		std::cout << test.elem->value << std::endl;
		test = std::move(*test.next);
	}
}

