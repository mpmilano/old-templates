#include <cassert>
#include <functional>
#include <memory>
#include <map>
#include <string>
#include <iostream>

#define SEED 21
namespace xlnagla{


class assert_once_c{
public:
    static std::map<std::string, std::map<int, bool> >& seen_map();

};
}
#define assert_once(Y) assert_once_int(Y, __LINE__)
#define assert_once_int(Y, X) assert (\
    ([&](){\
    int static_rand = X; \
    auto& map = xlnagla::assert_once_c::seen_map(); \
    if (map[__FILE__].count(static_rand) > 0 && map.at(__FILE__).at(static_rand)) \
    return true; \
    else { \
    assert(Y); \
    map[__FILE__][static_rand] = true; \
    return true; \
    }; \
    }()))
