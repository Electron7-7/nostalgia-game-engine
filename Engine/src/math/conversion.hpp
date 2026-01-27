#ifndef CONVERSION_H
#define CONVERSION_H

namespace Math
{
    template<typename Lhs, typename Rhs>
        void Convert(Lhs&, Farg<Rhs>) noexcept;
    template<typename Lhs, typename Rhs>
        Lhs Convert(Farg<Rhs>) noexcept;
}

#endif // CONVERSION_H
