#pragma once

#include <functional>
#include <memory>
#include <thread>
#include <future>

namespace xlnagla{

template<class T, class Del = std::default_delete<T> >
class unique_threaded_thunk{
private:
std::future<T*> f;
//std::function<T* ()> f;
std::unique_ptr<T, Del> t;
bool evaluated;
public:
unique_threaded_thunk(std::function<T* ()> t):f(std::async(t)),evaluated(false){}

const T& value() const 	{
	//logically const - as far as the semantics are concerned, this is just a wrapper to unique_ptr.
	unique_threaded_thunk<T>* l = const_cast<unique_threaded_thunk<T>*>(this);
	if (!l->evaluated) {
		l->t = std::unique_ptr<T>(std::move(l->f.get()));
	} 
	l->evaluated = true; 
	return *t;
}

T& value() 	{
	if (!evaluated) 
		t = std::unique_ptr<T>(std::move(f.get())); 
	evaluated = true; 
	return *t;
}

unique_threaded_thunk& operator= (unique_threaded_thunk&& _Right){
	if (_Right.evaluated)
		t = std::move(_Right.t);
	else{
		f = std::move(_Right.f);
		t.reset();
	}
	evaluated = _Right.evaluated;
	return (*this);
}
template<class Type2, class Del2>
unique_threaded_thunk& operator= (unique_threaded_thunk<Type2, Del2>&& _Right){
	if (_Right.evaluated)
		t = std::move(_Right.t);
	else{
		f = std::move(_Right.f);
		t.reset();
	}
	evaluated = _Right.evaluated;
	return (*this);
}
void swap(unique_threaded_thunk& _Right){
	auto&& tmpf = f;
	f = _Right.f;
	_Right.f = tmpf;
	auto tmpbool = evaluated;
	evaluated = _Right.evaluated;
	_Right.evaluated = tmpbool;
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

unique_threaded_thunk(const unique_threaded_thunk& ) = delete;
unique_threaded_thunk& operator=(const unique_threaded_thunk& ) = delete;

};

}
