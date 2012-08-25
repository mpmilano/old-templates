#pragma once

#include <tuple>
#include <algorithm>
#include <numeric>
#include <list>
#include <iterator>

#include "memoize.h"

namespace xlnagla{

        template<typename F, typename Ret, typename... Args>
        Ret return_helper(Ret (F::*)(Args...) const);

        template<typename F, typename Ret, typename... Args>
        std::function<Ret (Args...)> function_helper(Ret (F::*)(Args...) const);

        template<typename R, typename... A>
        constexpr int num_args(std::function<R (A...)>){
            return sizeof...(A);
        }
        template<typename R, typename F, typename... A>
        constexpr int num_args(R (F::*)(A...)){
            return sizeof...(A);
        }


        template<int want, typename... T>
        class ith_type{
        private:
            template<int desired, int current, typename A, typename... B>
            struct ith_type_helper{
                typedef std::conditional<desired == current, A, ith_type_helper<desired, current+1, B...> > type;
            };
        public:
            typedef ith_type_helper<want, 0, T...> type;
        };

        template<typename A, typename... B>
        struct args_list{
            typedef A arg;
            static constexpr bool end = sizeof...(B) == 0;
            typedef std::conditional<sizeof...(B) == 0 , void , args_list<B...> > next;
        };



        template<typename F, typename Ret, typename... Args>
        args_list<Args...> args_list_helper(Ret (F::*)(Args...));


        template<typename F, typename Ret, typename... Args>
        args_list<Args...> args_list_helper(Ret (F::*)(Args...) const);

        template<typename F, typename Ret, typename... Args>
        constexpr int args_size(Ret (F::*)(Args...) const){
            return sizeof...(Args);
        }


        template<typename F>
        struct lambda_to_function{
            typedef decltype(return_helper(&F::operator())) return_type;
            typedef decltype(function_helper(&F::operator())) function_type;
            static constexpr int num_args = args_size(&F::operator());
           /* typedef std::conditional< num_args == 0 , void* , decltype(args_list_helper(&F::operator())) > args_list_type;
            static constexpr args_list_type args = (num_args == 0? nullptr : args_list_type());*/

        };

        template<typename F>
        typename lambda_to_function<F>::function_type convert_to_function(F f){
            typename lambda_to_function<F>::function_type ret = f;
            return std::move(ret);
        }




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

    template<typename R, typename A, typename... B>
        inline const std::function< std::function<R (B...)> (A) > curry_helper(std::function<R (A, B...)> f){
        return [f](A a){
            return [a, f](B... b){
                return f(a, b...);
            };
        };
    }
    #define FTYPE xlnagla::lambda_to_function<T>::function_type
    template<typename T>
inline const auto curry(T t) -> decltype(curry_helper(xlnagla::convert_to_function(t))){
        return curry_helper(convert_to_function(t));
    }
#undef FTYPE


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
