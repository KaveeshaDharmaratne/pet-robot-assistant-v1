#pragma once
#include <WString.h>

namespace utils
{
    template <typename T>
    struct Result
    {
        bool ok;
        T value;
        String error;

        static Result<T> Ok(T v) { return {true, v, String()}; }
        static Result<T> Err(const String &e) { return {false, T(), e}; }
    };
}
