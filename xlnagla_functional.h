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

        template<typename F, typename Ret, typename... Args>
        Ret return_helper(Ret (F::*)(Args...) const);

        template<typename F, typename Ret, typename... Args>
        std::function<Ret (Args...)> function_helper(Ret (F::*)(Args...) const);

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
        template<typename T, int i>
        constexpr bool has_n_arguments_lambda(){
            return num_args(&T::operator()) == i;
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
        args_list<Args...> args_list_helper(Ret (F::*)(Args...) const);

        template<typename F, typename Ret, typename... Args>
        constexpr int args_size(Ret (F::*)(Args...) const){
            return sizeof...(Args);
        }


        template<typename F>
        struct lambda_to_function{
            typedef decltype(return_helper(&F::operator())) return_type;
            typedef decltype(function_helper(&F::operator())) function_type;
            typedef decltype(args_list_helper(&F::operator())) args_list_type;
            static constexpr args_list_type args = args_list_type();
            static constexpr int num_args = args_size(&F::operator());
        };

        template<typename F>
        typename lambda_to_function<F>::function_type proper_convert_to_function(F f){
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
        inline const std::function< std::function<R (B...)> (A) > curry_helper(std::function<R (A, B...)> f){
        return [f](A a){
            return [a, f](B... b){
                return f(a, b...);
            };
        };
    }
    #define FTYPE xlnagla::lambda_to_function<T>::function_type
    template<typename T>
inline const auto curry(T t) -> decltype(curry_helper(xlnagla::proper_convert_to_function(t))){
        return curry_helper(proper_convert_to_function(t));
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
