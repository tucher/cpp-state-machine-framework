#pragma once
#include <type_traits>
#include <utility>

template<std::size_t I, class T, bool IsClass>
struct tuple_base { T value; };
template<std::size_t I, class T>
struct tuple_base<I, T, true> : T {};

template<std::size_t I, class T>
T& tuple_get(tuple_base<I, T, false>& obj) {
  return obj.value;
}
template<std::size_t I, class T>
T& tuple_get(tuple_base<I, T, true>& obj) {
  return obj;
}

template<std::size_t I, class T>
const T& tuple_get(const tuple_base<I, T, false>& obj) {
  return obj.value;
}
template<std::size_t I, class T>
const T& tuple_get(const tuple_base<I, T, true>& obj) {
  return obj;
}


template<class...> struct tuple_impl;
template<std::size_t... Is, class... Ts>
struct tuple_impl<std::index_sequence<Is...>, Ts...>
  : tuple_base<Is, Ts, std::is_class<Ts>::value>... {
  tuple_impl() = default;
  ~tuple_impl() = default;
  tuple_impl(tuple_impl&&) = default;
  tuple_impl(tuple_impl const&) = default;
#if true || !__cpp_aggregate_bases
  template<class... U> tuple_impl(U&&... u)
    : tuple_base<Is, Ts, std::is_class<Ts>::value>{
      std::forward<U>(u)}... {}
#endif
};

template<class... Ts> struct tuple
  : tuple_impl<std::index_sequence_for<Ts...>, Ts...> {
  tuple() = default;
  ~tuple() = default;
  tuple(tuple&&) = default;
  tuple(tuple const&) = default;
#if true || !__cpp_aggregate_bases
  template<class... U> tuple(U&&... u)
    : tuple::tuple_impl{std::forward<U>(u)...} {}
#endif
};

template<std::size_t I, class T, bool Get> struct nth_base {};
template<std::size_t I, class T>
struct nth_base<I, T, true> { using type = T; };
template<std::size_t, class...> struct nth_impl;
template<std::size_t I, std::size_t... Is, class... Ts>
struct nth_impl<I, std::index_sequence<Is...>, Ts...>
  : nth_base<Is, Ts, I == Is>... {};
template<std::size_t I, class... Ts> using nth =
  nth_impl<I, std::index_sequence_for<Ts...>, Ts...>;

template <class T, class Tuple>
struct TypeIndex;

template <class T, class... Types>
struct TypeIndex<T, tuple<T, Types...>> {
    static const std::size_t value = 0;
};

template <class T, class U, class... Types>
struct TypeIndex<T, tuple<U, Types...>> {
    static const std::size_t value = 1 + TypeIndex<T, tuple<Types...>>::value;
};

namespace std {
    template<class... Ts> class tuple_size<::tuple<Ts...>> : public integral_constant<size_t, sizeof...(Ts)> {};
    template<size_t I, class... Ts>
    class tuple_element<I, ::tuple<Ts...>> : public ::nth<I, Ts...> {};
    
    template<size_t I, class... Ts>
    typename tuple_element<I, ::tuple<Ts...>>::type& get(::tuple<Ts...>& t) {
        return ::tuple_get<I>(t);
    }

    template<size_t I, class... Ts>
    const typename tuple_element<I, ::tuple<Ts...>>::type& get(const ::tuple<Ts...>& t) {
        return ::tuple_get<I>(t);
    }

    template<typename T, class... Ts>
    auto& get(::tuple<Ts...>& t) {
        return ::tuple_get<::TypeIndex<T, ::tuple<Ts...>>::value>(t);
    }

    template<typename T, class... Ts>
    const auto& get(const ::tuple<Ts...>& t) {
        return ::tuple_get<::TypeIndex<T, ::tuple<Ts...>>::value>(t);
    }
}

template <typename T>
inline constexpr size_t tuple_size_v = std::tuple_size<T>::value;

template <typename T, typename F, std::size_t... I>
constexpr auto visit_impl(T& tup, const size_t idx, F fun, std::index_sequence<I...>)
{
    using ResultType = std::invoke_result_t<F, decltype(std::get<0>(tup))>;
    assert(idx < tuple_size_v<T>);
    if constexpr(!std::is_same_v<void, ResultType>) {
        ResultType results[tuple_size_v<T>] = {(I == idx ? fun(std::get<I>(tup)) : ResultType()) ...};
        return results[idx];
    } else {
        ((I == idx ? fun(std::get<I>(tup)) : void()), ...);
        return;
    }
}

template <typename F, typename... Ts, typename Indices = std::make_index_sequence<sizeof...(Ts)>>
constexpr auto visit_at(tuple<Ts...>& tup, const size_t idx, F fun)
{
    return visit_impl(tup, idx, fun, Indices {});
}

template <typename F, typename... Ts, typename Indices = std::make_index_sequence<sizeof...(Ts)>>
constexpr auto  visit_at(const tuple<Ts...>& tup, const size_t idx, F fun)
{
    return visit_impl(tup, idx, fun, Indices {});
}