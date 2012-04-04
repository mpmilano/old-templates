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
    static std::map<int, bool>& seen_map();

};
}
#define assert_once(Y) assert_once_int(Y, __LINE__)
#define assert_once_int(Y, X) \
    ([&](){\
    int static_rand = X; \
    auto& map = xlnagla::assert_once_c::seen_map(); \
    if (map.count(static_rand) > 0 && map.at(static_rand)) \
    return true; \
    else { \
    assert(Y); \
    map[static_rand] = true; \
    return true; \
    }; \
    }())
