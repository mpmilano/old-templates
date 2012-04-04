#include "assert_once.h"

using namespace std;

std::map<string, std::map<int, bool> >& xlnagla::assert_once_c::seen_map(){
    static std::map<string, std::map<int, bool> > ret;
    return ret;
}
