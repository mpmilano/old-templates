#pragma once
#include "xlnagla_functional"

namespace xlnagla{

template<typename T, std::launch lnch = std::launch::any>
class thunk{

private:

std::future<T> value;
std::function<T ()> fun;

public:

thunk(std::function<T () > f):fun(f),value(std::async(lnch,f)){}
operator T(){return value.get();}

};


template<class T, std::launch lnch = std::launch::sync >
	thunk<T, lnch> make_thunk( std::function<T ()> t){
	return std::move(thunk<T, lnch>(t));
}


#define THUNK(Tstar) [&](){				\
		auto f = convert_to_statement([&]() { return std::move(Tstar); }); \
		return make_thunk(f);						\
	}()


}
