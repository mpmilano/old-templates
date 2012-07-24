#include "Lazy.h"
#include <iostream>


template<typename T>
class LazyList
{

public:

	Lazy<T> elem;
	Lazy<LazyList<T> > next;

	LazyList(std::function<T* () > elem, std::function<LazyList<T>* () > next):elem(elem),next(next){}
	static LazyList* make_list(std::function<T* () > elem, std::function<LazyList<T>* () > next){
 		return new LazyList(elem, next);
	}

};


	int main(){
		std::function<int* ()  > foo1 = [](){int* a = new int(12); return a;};
		std::function<LazyList<int>* () > foo2 = [foo1, &foo2](){
			
			return LazyList<int>::make_list([&](){return new int(*(foo1()) + 1);}, foo2);
		};
		LazyList<int> test(foo1, foo2);
		while(true){
			std::cout << *test.elem << std::endl;
			test = std::move(*test.next);
		}
	}
