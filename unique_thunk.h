#pragma once

#include <functional>
#include <memory>
#include <thread>
#include <future>

namespace xlnagla{


	template<class T, std::launch lnch = std::launch::sync, class Del = std::default_delete<T> >
		class unique_thunk{
	private:
	std::future<T*> f;
//std::function<T* ()> f;
	std::unique_ptr<T, Del> t;
	bool evaluated;
	public:
	unique_thunk(std::function<T* ()> t):f(std::async(lnch, t)),evaluated(false){}
	unique_thunk( unique_thunk&& u){
		if (u.evaluated)
			t = std::move(u.t);
		else{
			f = std::move(u.f);
			t.reset();
		}
		evaluated = u.evaluated;
	}
	
	const T& value() const 	{
		//logically const - as far as the semantics are concerned, this is just a wrapper to unique_ptr.
	unique_thunk<T, lnch, Del>* l = const_cast<unique_thunk<T, lnch, Del>*>(this);
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

unique_thunk& operator= (unique_thunk&& _Right){
	if (_Right.evaluated)
		t = std::move(_Right.t);
	else{
		f = std::move(_Right.f);
		t.reset();
	}
	evaluated = _Right.evaluated;
	return (*this);
}
template<class Type2, std::launch lnch2, class Del2>
unique_thunk& operator= (unique_thunk<Type2, lnch2, Del2>&& _Right){
	if (_Right.evaluated)
		t = std::move(_Right.t);
	else{
		f = std::move(_Right.f);
		t.reset();
	}
	evaluated = _Right.evaluated;
	return (*this);
}
void swap(unique_thunk& _Right){
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

unique_thunk(const unique_thunk& ) = delete;
unique_thunk& operator=(const unique_thunk& ) = delete;

};

/*	template<class T, std::launch lnch = std::launch::sync, class Del = std::default_delete<T> >
		unique_thunk<T, lnch, Del> make_unique_thunk(const std::function<T* ()> &t){
		return std::move(unique_thunk<T, lnch, Del>(t));
		}*/
	template<class T, std::launch lnch = std::launch::sync, class Del = std::default_delete<T> >
		unique_thunk<T, lnch, Del> make_unique_thunk( std::function<T* ()> t){
		return std::move(unique_thunk<T, lnch, Del>(t));
	}

	#define UNIQUE_THUNK(Tstar) [&](){\
			auto f = [&]() { return Tstar; };\
			typedef decltype(f) f_type;\
			std::result_of<f_type()>::type v;\
			typedef std::result_of<f_type()>::type rettype;\
			std::function<rettype () > retf = f;\
			return make_unique_thunk(retf);\
		}();



}
