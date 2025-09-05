#include <functional>

using Action = std::function<void()>;

template <typename R>
using Func = std::function<R()>;

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

template <typename T1>
using Action1 = std::function<void(const T1&)>;

template <typename T1, typename T2>
using Action2 = std::function<void(const T1&, const T2&)>;

template <typename T1, typename T2, typename T3>
using Action3 = std::function<void(const T1&, const T2&, const T3&)>;

template <typename T1, typename T2, typename T3, typename T4>
using Action4 = std::function<void(const T1&, const T2&, const T3&, const T4&)>;

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

template <typename R, typename T>
using Func1 = std::function<R(const T&)>;

template <typename R, typename T1, typename T2>
using Func2 = std::function<R(const T1&, const T2&)>;

template <typename R, typename T1, typename T2, typename T3>
using Func3 = std::function<R(const T1&, const T2&, const T3&)>;

template <typename R, typename T1, typename T2, typename T3, typename T4>
using Func4 = std::function<R(const T1&, const T2&, const T3&, const T4&)>;

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

using FuncBool = std::function<bool()>;
using FuncFloat = std::function<float()>;