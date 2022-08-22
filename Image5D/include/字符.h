#pragma once
#include <type_traits>
template<typename T>
concept 字符 = std::_Is_any_of_v<T, char, wchar_t>;