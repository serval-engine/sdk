#ifndef SERVAL_SDK__TEXT_HPP_
#define SERVAL_SDK__TEXT_HPP_

#include "../types.hpp"

namespace serval {
    class Text;
}

class serval::Text {
public:
    ASSET("text");
    virtual const char* c_str () = 0;
    virtual std::size_t size () = 0;

    std::string str () {return std::string{c_str(), size()};}
};

#endif