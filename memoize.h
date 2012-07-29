#ifndef MEMOIZE_H
#define MEMOIZE_H
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>

namespace xlnagla{

    typedef std::string hash_key;

    hash_key default_map_key(){
        return "";
    }

    template<typename T, typename... Args>
    hash_key default_map_key(const T &t, const Args... args){
        std::stringstream ss;
        ss << t;
        ss  << default_map_key(args...);
        return ss.str();
    }

    int default_map_key_int(int i){
        return i;
    }

template<typename Return, typename... Args>
std::function<Return (Args...)> memoize(std::function<Return (Args...)>  f/*function to memoize*/,
                                        std::function<hash_key (Args...)> get_key = default_map_key<Args...>){
    std::shared_ptr<std::map<hash_key, Return> > map(new std::map<hash_key, Return>);
    std::function<Return (Args...)> old_func = f;
    return [map, old_func, &get_key](Args... args){
        auto key = get_key(args...);
        if (map->find(key) == map->end()){
            auto put_in_map = old_func(args...);
            (*map)[key] = put_in_map;
        }
        return map->at(key);
    };
}

template<typename Return>
std::function<Return (int)> memoize(std::function<Return (int)>  f/*function to memoize*/,
                                        std::function<int (int)> get_key = default_map_key_int){
    std::shared_ptr<std::map<int, Return> > map(new std::map<int, Return>);
    std::function<Return (int)> old_func = f;
    return [map, old_func, &get_key](int arg){
        auto key = get_key(arg);
        if (map->find(key) == map->end()){
            auto put_in_map = old_func(arg);
            (*map)[key] = put_in_map;
        }
        return map->at(key);
    };

}

template<typename Return, typename... Args>
std::function<Return (Args...)> memoize(std::shared_ptr<std::function<Return (Args...)> > f_p/*function to memoize*/,
                                        std::function<hash_key (Args...)> get_key = default_map_key<Args...>){
    std::shared_ptr<std::map<hash_key, Return> > map(new std::map<hash_key, Return>);
    return [map, f_p, &get_key](Args... args){
        auto &f = f_p;
        auto key = get_key(args...);
        if (map->find(key) == map->end()){
            auto put_in_map = f(args...);
            (*map)[key] = put_in_map;
        }

        return map->at(key);
    };
}

}


#endif // MEMOIZE_H
