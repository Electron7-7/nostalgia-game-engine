#ifndef UI_FWD_H
#define UI_FWD_H

#include <type_traits>

class IUIImplementor;
class UI_Implementor;

template<typename T>
    concept is_UI_Implementor = std::is_base_of_v<UI_Implementor,T>;

class IUISolution;
template<is_UI_Implementor T>
    class UI_Solution;

#endif // UI_FWD_H
