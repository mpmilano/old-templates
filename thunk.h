#pragma once
#include "xlnagla_functional"
#include <future>

namespace xlnagla{

template<typename T, std::launch lnch = std::launch::async | std::launch::deferred>
class thunk{

private:

std::future<T> value;
std::function<T ()> fun;

public:

thunk(std::function<T () > f):fun(f),value(std::async(lnch,f)){}
operator T(){return value.get();}

};


template<class T, std::launch lnch = std::launch::deferred >
	thunk<T, lnch> make_thunk( std::function<T ()> t){
	return std::move(thunk<T, lnch>(t));
}


#define THUNK(Tstar) [&](){				\
        auto f = xlnagla::convert_to_function([&]() { return std::move(Tstar); }); \
		return xlnagla::make_thunk(f);									\
	}()


}
