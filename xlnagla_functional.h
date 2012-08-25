#pragma once

#include <tuple>
#include <algorithm>
#include <numeric>
#include <list>
#include <iterator>

#include "memoize.h"

namespace xlnagla{



    template<typename F, typename Ret, typename A, typename... Rest>
        A helper(Ret (F::*)(A, Rest...));

    template<typename F, typename Ret, typename A, typename... Rest>
        A helper(Ret (F::*)(A, Rest...) const);

    template<typename F, typename Ret, typename A, typename B, typename... Rest>
        B second_argument_helper(Ret (F::*)(A, B, Rest...));

    template<typename F, typename Ret, typename A, typename B, typename... Rest>
        B second_argument_helper(Ret (F::*)(A, B, Rest...) const);


    template<typename F>
        struct first_argument {
            typedef decltype( helper(&F::operator()) ) type;
        };

    template<typename F>
        struct second_argument {
            typedef decltype( second_argument_helper(&F::operator()) ) type;
        };


        template<typename R, typename... A>
        constexpr int num_args(std::function<R (A...)>){
            return sizeof...(A);
        }
        template<typename R, typename F, typename... A>
        constexpr int num_args(R (F::*)(A...)){
            return sizeof...(A);
        }

        template<typename T>
        constexpr bool has_n_arguments(T f, int i){
                return num_args(f) == i;
            }

        template<typename A, typename... B>
        struct args_list{
            typedef A arg;
            static constexpr bool end = sizeof...(B) == 0;
            typedef std::conditional<sizeof...(B) == 0 , void , args_list<B...> > next;
        };

        template<typename R, typename F, typename... A>
        struct labmda_to_function{
            typedef std::function< R (A...)> function_type;
            typedef R return_type;
            static constexpr args_list<A...> args = args_list<A...>();
            static constexpr int num_args = sizeof...(A);
        };




//this is better as a macro
//the base case is the last argument.
    template<typename T>
        inline const T& cond(bool acond, const T& res, const T& basecase){
        return acond ? res : basecase;
    }

    template<typename T, typename... L>
        inline const T& cond(bool acond, const T& res, const L&... l){
        return acond ? res : cond(l...);
    }

    template<typename T, typename B>
        inline const T&	lazy_cond(const B &acond, const T &res, const T& basecase){
        return *acond ? res : basecase;
    }

    template<typename T, typename B, typename... L>
        inline const T& lazy_cond(const B& acond, const T& res, const L&... l){
        return *acond ? res : lazy_cond(l...);
    }


//because irritatingly enough trying to pass a lambda to something that expects a 
//function of the same type will not work.  grr.  This only works on functions 
//of no arguments.
template<typename T>
    struct statement_helper{
        typedef T f_type;
        typedef typename std::result_of<f_type()>::type rettype;
        typedef typename std::function<rettype () >  ftype;
    };
template<typename T>
    constexpr typename statement_helper<T>::ftype convert_to_statement(T f){
    typename statement_helper<T>::ftype retf = std::move(f);
    return std::move(retf);

}


template<typename T>
    struct convert_helper{
        typedef T f_type;
        typedef typename xlnagla::first_argument<T>::type argtype;
        typedef typename std::result_of<f_type(argtype)>::type rettype;
        typedef typename std::function<rettype (argtype) >  ftype;
    };
template<typename T>
    constexpr typename convert_helper<T>::ftype convert_to_function(T f){
    typename convert_helper<T>::ftype retf = std::move(f);
    return std::move(retf);

}

template<typename T>
    struct convert_2_helper{
        typedef T f_type;
        typedef typename xlnagla::first_argument<T>::type argtype;
        typedef typename xlnagla::second_argument<T>::type arg2type;
        typedef typename std::result_of<f_type(argtype, arg2type)>::type rettype;
        typedef typename std::function<rettype (argtype, arg2type) >  ftype;
    };
template<typename T>
    constexpr typename convert_2_helper<T>::ftype convert_to_2arg_function(T f){
    typename convert_2_helper<T>::ftype retf = std::move(f);
    return std::move(retf);

}

    template<typename R, typename A, typename... B>
        inline const std::function< std::function<R (B...)> (A) > curry(std::function<R (A, B...)> f){
        return [f](A a){
            return [a, f](B... b){
                return f(a, b...);
            };
        };
    }
    template<typename T>
        inline const std::function< std::function<typename convert_2_helper<T>::rettype (typename convert_2_helper<T>::arg2type)> (typename convert_2_helper<T>::argtype) > curry2(T t){
        return curry(convert_to_2arg_function(t));
    }


    template<typename T, typename A, typename L>
        std::list<T> map(std::function<T (A)> f, const L& list){
        std::list<T> ret;
        std::transform(std::begin(list), std::end(list),std::back_inserter(ret), f);
        return std::move(ret);
    }

    template<typename T, typename A>
        std::list<T> map(std::function<T (A)> f, const std::initializer_list<A>& list){
        std::list<T> ret;
        std::transform(std::begin(list), std::end(list),std::back_inserter(ret), f);
        return std::move(ret);
    }

    template<typename T, typename L>
        T foldl(std::function<T (T, T) > f, const L& list, T t = T()){
        return std::accumulate(std::begin(list), std::end(list), t, f);
    }
    template<typename T>
        T foldl(std::function<T (T, T) > f, const std::initializer_list<T>& list, T t = T()){
        return std::accumulate(std::begin(list), std::end(list), t, f);
    }


    /*
currying: T f(a,b) -> T f(a)(b)

that means it's implemented something like this:

std::function< std::function<T (b) > (a) >curry( function<T (a,b)> f){

//close over f, takes an a
return [f](a){
  return [a,f](b){
    return f(a,b);
}
}

}
     */

}
