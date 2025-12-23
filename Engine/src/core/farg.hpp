#ifndef FARG_H
#define FARG_H

#include <string>

#define FARG(T) const T&
#define FAUTO FARG(auto)

template<typename T>
    using Farg = FARG(T);

using Sarg = Farg<std::string>;

#endif // FARG_H
