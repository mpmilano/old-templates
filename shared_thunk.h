#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <thread>
#include <future>

namespace xlnagla{

template<class T, std::launch lnch = std::launch::any, class Del = std::default_delete<T> >
class shared_thunk{
private:

std::future<T*> f;
//std::function<T* ()> f;
std::shared_ptr<T> t;

bool evaluated() const{
	return t.get() != nullptr;
}


public:

shared_thunk(std::function<T* ()> f):f(std::async(lnch,f)),t(nullptr)
{
	assert(!evaluated() ? t.get() == nullptr : true);
}
	const T& value() const 	{
		//logically const - as far as the semantics are concerned, this is just a wrapper to shared_ptr.
		shared_thunk<T, lnch, Del>* l = const_cast<shared_thunk<T, lnch, Del>*>(this);
		assert(!evaluated() ? t.get() == nullptr : true);
		if (!l->evaluated()) {
			l->t = std::shared_ptr<T>(std::move(l->f.get()));
		} 
		return *t;
	}

	T& value() 	{
		assert(!evaluated() ? t.get() == nullptr : true);

		if (!evaluated()) 
			t = std::shared_ptr<T>(std::move(f.get())); 
		return *t;
	}
shared_thunk& operator= (shared_thunk&& _Right){
	if (_Right.evaluated()){
		t = std::move(_Right.t);
	}
	else{
		f = std::move(_Right.f);
		t.reset();
	}
	return (*this);
}
template<class Type2, std::launch lnch2, class Del2>
shared_thunk& operator= (shared_thunk<Type2, lnch2, Del2>&& _Right){
	if (_Right.evaluated())
		t = std::move(_Right.t);
	else{
		f = std::move(_Right.f);
		t.reset();
	}
	return (*this);
}
void swap(shared_thunk& _Right){
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

shared_thunk(const shared_thunk& thunk)
:f(thunk.f),t(thunk.t)
{}
shared_thunk& operator=(const shared_thunk& thunk)
{
	f = thunk.f;
	t = thunk.t;
	}



};

}
