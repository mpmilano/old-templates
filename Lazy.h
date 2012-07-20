#include <functional>

template<typename T>
class Lazy{
private:
	std::function<T ()> f;
	T t;
	bool evaluated;
public:
Lazy(std::function<T ()> t):f(t),evaluated(false){}
	const T& value() const 	{
		//logically const - as far as the semantics are concerned, this is just a wrapper to unique_ptr.
		Lazy<T>* l = const_cast<Lazy<T>*>(this);
		if (!l->evaluated) {
			l->t = std::move(l->f());
		} 
		l->evaluated = true; 
		return t;
	}

	T& value() 	{
		if (!evaluated) 
			t = std::move(f()); 
		evaluated = true; 
		return t;
	}
	
};
