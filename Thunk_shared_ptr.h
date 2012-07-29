#include <functional>
#include <memory>
#include <cassert>

template<class T, class Del = std::default_delete<T> >
class Thunk{
private:
std::function<T* ()> f;
std::shared_ptr<T> t;

bool evaluated() const{
	return t.get() != nullptr;
}


public:

Thunk(std::function<T* ()> f):f(f),t(nullptr)
{
	assert(!evaluated() ? t.get() == nullptr : true);
}
	const T& value() const 	{
		//logically const - as far as the semantics are concerned, this is just a wrapper to shared_ptr.
		Thunk<T>* l = const_cast<Thunk<T>*>(this);
		assert(!evaluated() ? t.get() == nullptr : true);
		if (!l->evaluated()) {
			l->t = std::shared_ptr<T>(std::move(l->f()));
		} 
		return *t;
	}

	T& value() 	{
		assert(!evaluated() ? t.get() == nullptr : true);

		if (!evaluated()) 
			t = std::shared_ptr<T>(std::move(f())); 
		return *t;
	}
Thunk& operator= (Thunk&& _Right){
	if (_Right.evaluated()){
		t = std::move(_Right.t);
	}
	else{
		f = std::move(_Right.f);
		t.reset();
	}
	return (*this);
}
template<class Type2, class Del2>
Thunk& operator= (Thunk<Type2, Del2>&& _Right){
	if (_Right.evaluated())
		t = std::move(_Right.t);
	else{
		f = std::move(_Right.f);
		t.reset();
	}
	return (*this);
}
void swap(Thunk& _Right){
	auto&& tmpf = f;
	f = _Right.f;
	_Right.f = tmpf;
	t.swap(_Right.t);
}
T* release(){
	value();
	return t.release();
}
typedef T* pointer;
void reset(T* ptr = pointer()){
	t.reset(ptr);
}
T* get() const{
	value();
	return t.get();
}
T& operator* () const{
	value();
	return t.operator*();
}
T* operator-> () const{
	value();
	return t.operator->();
}
Del& get_deleter (){
	return t.get_deleter();
}
const Del& get_deleter () const{
	return t.get_deleter();
}
explicit operator bool () const{
	value();
	return [](bool b){return b;}(t);
}

Thunk(const Thunk& thunk)
:f(thunk.f),t(thunk.t)
{}
Thunk& operator=(const Thunk& thunk)
{
	f = thunk.f;
	t = thunk.t;
	}



};
