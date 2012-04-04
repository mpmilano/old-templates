#include "assert_once.h"

std::map<int, bool>& xlnagla::assert_once_c::seen_map(){
    static std::map<int, bool> ret;
    return ret;
}
