#pragma once

#include <cpp-state-machine-framework/utils/typestring.hpp>
#include <tuple>

namespace TupleTypeIDExtractor {

template <class T, class Tuple>
struct Index;

template <class T, class... Types>
struct Index<T, std::tuple<T, Types...>> {
    static const std::size_t value = 0;
};

template <class T, class U, class... Types>
struct Index<T, std::tuple<U, Types...>> {
    static const std::size_t value = 1 + Index<T, std::tuple<Types...>>::value;
};


template <typename T, typename = void>
struct has_NameTS : std::false_type{};

template <typename T>
struct has_NameTS<T, std::enable_if_t<TypeString::is_ss_v<typename T::NameTS>> > : std::true_type {};

template <typename T, typename TupleT, bool V>
struct type_id_ts_helper;

template <typename T, typename TupleT>
struct type_id_ts_helper<T, TupleT, true> {
    using ts = typename T::NameTS;
};

using namespace TypeStringLiteralExploder;
template <typename T, typename TupleT>
struct type_id_ts_helper<T, TupleT, false> {
    using ts = TypeString::concat<TS("Type#"), TypeString::i_to_ss<Index<T, TupleT>::value> >;
};

template<typename T, typename TypesTupleT>
using type_id_ts = typename type_id_ts_helper<T, TypesTupleT, has_NameTS<T>::value>::ts;

}