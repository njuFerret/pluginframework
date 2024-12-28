#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <qcompilerdetection.h>
#include <set>
// #include <tuple>
#include <QHash>
#include <QObject>
#include <QSet>
#include <QStringList>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

namespace Utils {
/////////////////////////
// reverseForeach
/////////////////////////
template <typename Container, typename Op>
inline void reverseForeach(const Container &c, const Op &operation);
/////////////////////////
// findOr
/////////////////////////
template <typename C, typename F>
Q_REQUIRED_RESULT typename C::value_type findOr(const C &container, typename C::value_type other, F function);
template <typename T, typename R, typename S>
Q_REQUIRED_RESULT typename T::value_type findOr(const T &container, typename T::value_type other,
                                                R (S::*function)() const);
template <typename T, typename R, typename S>
Q_REQUIRED_RESULT typename T::value_type findOr(const T &container, typename T::value_type other, R S::*member);
/////////////////////////
// findOrDefault
/////////////////////////
template <typename C, typename F>
Q_REQUIRED_RESULT
    typename std::enable_if_t<std::is_copy_assignable<typename C::value_type>::value, typename C::value_type>
    findOrDefault(const C &container, F function);
template <typename C, typename R, typename S>
Q_REQUIRED_RESULT
    typename std::enable_if_t<std::is_copy_assignable<typename C::value_type>::value, typename C::value_type>
    findOrDefault(const C &container, R (S::*function)() const);
template <typename C, typename R, typename S>
Q_REQUIRED_RESULT
    typename std::enable_if_t<std::is_copy_assignable<typename C::value_type>::value, typename C::value_type>
    findOrDefault(const C &container, R S::*member);
/////////////////////////
// sort
/////////////////////////
template <typename Container>
inline void sort(Container &container);
template <typename Container, typename Predicate>
inline void sort(Container &container, Predicate p);
template <typename Container, typename R, typename S>
inline void sort(Container &container, R S::*member);
template <typename Container, typename R, typename S>
inline void sort(Container &container, R (S::*function)() const);
/////////////////////////
// transform
/////////////////////////
// function without result type deduction:
template <typename ResultContainer, typename SC, typename F>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, F function);
// function with result type deduction:
template <template <typename> class C, typename SC, typename F, typename Value = typename std::decay_t<SC>::value_type,
          typename Result = std::decay_t<std::result_of_t<F(Value &)>>, typename ResultContainer = C<Result>>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, F function);
template <template <typename, typename> class C, typename SC, typename F,
          typename Value = typename std::decay_t<SC>::value_type,
          typename Result = std::decay_t<std::result_of_t<F(Value &)>>,
          typename ResultContainer = C<Result, std::allocator<Result>>>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, F function);
// member function without result type deduction:
template <template <typename...> class C, typename SC, typename R, typename S>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, R (S::*p)() const);
// member function with result type deduction:
template <typename ResultContainer, typename SC, typename R, typename S>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, R (S::*p)() const);
// member without result type deduction:
template <typename ResultContainer, typename SC, typename R, typename S>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, R S::*p);
// member with result type deduction:
template <template <typename...> class C, typename SC, typename R, typename S>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, R S::*p);
// same container types for input and output, const input
// function:
template <template <typename...> class C, typename F, typename... CArgs>
Q_REQUIRED_RESULT decltype(auto) transform(const C<CArgs...> &container, F function);
// same container types for input and output, const input
// member function:
template <template <typename...> class C, typename R, typename S, typename... CArgs>
Q_REQUIRED_RESULT decltype(auto) transform(const C<CArgs...> &container, R (S::*p)() const);
// same container types for input and output, const input
// members:
template <template <typename...> class C, typename R, typename S, typename... CArgs>
Q_REQUIRED_RESULT decltype(auto) transform(const C<CArgs...> &container, R S::*p);
// same container types for input and output, non-const input
// function:
template <template <typename...> class C, typename F, typename... CArgs>
Q_REQUIRED_RESULT decltype(auto) transform(C<CArgs...> &container, F function);
// same container types for input and output, non-const input
// member function:
template <template <typename...> class C, typename R, typename S, typename... CArgs>
Q_REQUIRED_RESULT decltype(auto) transform(C<CArgs...> &container, R (S::*p)() const);
// same container types for input and output, non-const input
// members:
template <template <typename...> class C, typename R, typename S, typename... CArgs>
Q_REQUIRED_RESULT decltype(auto) transform(C<CArgs...> &container, R S::*p);
/////////////////////////
// filtered
/////////////////////////
template <typename C, typename F>
Q_REQUIRED_RESULT C filtered(const C &container, F predicate);
template <typename C, typename R, typename S>
Q_REQUIRED_RESULT C filtered(const C &container, R (S::*predicate)() const);

//////////////////
// reverseForeach
/////////////////
template <typename Container, typename Op>
inline void reverseForeach(const Container &c, const Op &operation) {
  auto rend = c.rend();
  for (auto it = c.rbegin(); it != rend; ++it)
    operation(*it);
}

/////////////////
// helper code for transform to use back_inserter and thus push_back for everything and insert for QSet<>
// SetInsertIterator, straight from the standard for insert_iterator
// just without the additional parameter to insert
template <class Container>
class SetInsertIterator : public std::iterator<std::output_iterator_tag, void, void, void, void> {
protected:
  Container *container;

public:
  using container_type = Container;
  explicit SetInsertIterator(Container &x) : container(&x) {}
  SetInsertIterator<Container> &operator=(const typename Container::value_type &value) {
    container->insert(value);
    return *this;
  }
  SetInsertIterator<Container> &operator=(typename Container::value_type &&value) {
    container->insert(std::move(value));
    return *this;
  }
  SetInsertIterator<Container> &operator*() { return *this; }
  SetInsertIterator<Container> &operator++() { return *this; }
  SetInsertIterator<Container> operator++(int) { return *this; }
};

// for QMap / QHash, inserting a std::pair / QPair
template <class Container>
class MapInsertIterator : public std::iterator<std::output_iterator_tag, void, void, void, void> {
protected:
  Container *container;

public:
  using container_type = Container;
  explicit MapInsertIterator(Container &x) : container(&x) {}
  MapInsertIterator<Container> &
  operator=(const std::pair<const typename Container::key_type, typename Container::mapped_type> &value) {
    container->insert(value.first, value.second);
    return *this;
  }
  MapInsertIterator<Container> &
  operator=(const QPair<typename Container::key_type, typename Container::mapped_type> &value) {
    container->insert(value.first, value.second);
    return *this;
  }
  MapInsertIterator<Container> &operator*() { return *this; }
  MapInsertIterator<Container> &operator++() { return *this; }
  MapInsertIterator<Container> operator++(int) { return *this; }
};

// inserter helper function, returns a std::back_inserter for most containers
// and is overloaded for QSet<> and other containers without push_back, returning custom inserters
template <typename C>
inline std::back_insert_iterator<C> inserter(C &container) {
  return std::back_inserter(container);
}

template <typename X>
inline SetInsertIterator<QSet<X>> inserter(QSet<X> &container) {
  return SetInsertIterator<QSet<X>>(container);
}

template <typename K, typename C, typename A>
inline SetInsertIterator<std::set<K, C, A>> inserter(std::set<K, C, A> &container) {
  return SetInsertIterator<std::set<K, C, A>>(container);
}

template <typename K, typename H, typename C, typename A>
inline SetInsertIterator<std::unordered_set<K, H, C, A>> inserter(std::unordered_set<K, H, C, A> &container) {
  return SetInsertIterator<std::unordered_set<K, H, C, A>>(container);
}

template <typename K, typename V, typename C, typename A>
inline SetInsertIterator<std::map<K, V, C, A>> inserter(std::map<K, V, C, A> &container) {
  return SetInsertIterator<std::map<K, V, C, A>>(container);
}

template <typename K, typename V, typename H, typename C, typename A>
inline SetInsertIterator<std::unordered_map<K, V, H, C, A>> inserter(std::unordered_map<K, V, H, C, A> &container) {
  return SetInsertIterator<std::unordered_map<K, V, H, C, A>>(container);
}

template <typename K, typename V>
inline MapInsertIterator<QMap<K, V>> inserter(QMap<K, V> &container) {
  return MapInsertIterator<QMap<K, V>>(container);
}

template <typename K, typename V>
inline MapInsertIterator<QHash<K, V>> inserter(QHash<K, V> &container) {
  return MapInsertIterator<QHash<K, V>>(container);
}
// Helper code for container.reserve that makes it possible to effectively disable it for specific cases
// default: do reserve
// Template arguments are more specific than the second version below, so this is tried first
template <template <typename...> class C, typename... CArgs, typename = decltype(&C<CArgs...>::reserve)>
void reserve(C<CArgs...> &c, typename C<CArgs...>::size_type s) {
  c.reserve(s);
}

// containers that don't have reserve()
template <typename C>
void reserve(C &, typename C::size_type) {}

//////////////////
// findOr
/////////////////
template <typename C, typename F>
Q_REQUIRED_RESULT typename C::value_type findOr(const C &container, typename C::value_type other, F function) {
  typename C::const_iterator begin = std::begin(container);
  typename C::const_iterator end = std::end(container);

  typename C::const_iterator it = std::find_if(begin, end, function);
  return it == end ? other : *it;
}

template <typename T, typename R, typename S>
Q_REQUIRED_RESULT typename T::value_type findOr(const T &container, typename T::value_type other,
                                                R (S::*function)() const) {
  return findOr(container, other, std::mem_fn(function));
}

template <typename T, typename R, typename S>
Q_REQUIRED_RESULT typename T::value_type findOr(const T &container, typename T::value_type other, R S::*member) {
  return findOr(container, other, std::mem_fn(member));
}

//////////////////
// findOrDefault
//////////////////
// Default implementation:
template <typename C, typename F>
Q_REQUIRED_RESULT
    typename std::enable_if_t<std::is_copy_assignable<typename C::value_type>::value, typename C::value_type>
    findOrDefault(const C &container, F function) {
  return findOr(container, typename C::value_type(), function);
}

template <typename C, typename R, typename S>
Q_REQUIRED_RESULT
    typename std::enable_if_t<std::is_copy_assignable<typename C::value_type>::value, typename C::value_type>
    findOrDefault(const C &container, R (S::*function)() const) {
  return findOr(container, typename C::value_type(), std::mem_fn(function));
}

template <typename C, typename R, typename S>
Q_REQUIRED_RESULT
    typename std::enable_if_t<std::is_copy_assignable<typename C::value_type>::value, typename C::value_type>
    findOrDefault(const C &container, R S::*member) {
  return findOr(container, typename C::value_type(), std::mem_fn(member));
}
//////////////////
// sort
/////////////////
template <typename Container>
inline void sort(Container &container) {
  std::sort(std::begin(container), std::end(container));
}

template <typename Container, typename Predicate>
inline void sort(Container &container, Predicate p) {
  std::sort(std::begin(container), std::end(container), p);
}

// pointer to member
template <typename Container, typename R, typename S>
inline void sort(Container &container, R S::*member) {
  auto f = std::mem_fn(member);
  using const_ref = typename Container::const_reference;
  std::sort(std::begin(container), std::end(container), [&f](const_ref a, const_ref b) { return f(a) < f(b); });
}

// pointer to member function
template <typename Container, typename R, typename S>
inline void sort(Container &container, R (S::*function)() const) {
  auto f = std::mem_fn(function);
  using const_ref = typename Container::const_reference;
  std::sort(std::begin(container), std::end(container), [&f](const_ref a, const_ref b) { return f(a) < f(b); });
}

// --------------------------------------------------------------------
// Different containers for input and output:
// --------------------------------------------------------------------

// different container types for input and output, e.g. transforming a QList into a QSet

// function without result type deduction:
template <typename ResultContainer,        // complete result container type
          typename SC,                     // input container type
          typename F>                      // function type
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, F function) {
  ResultContainer result;
  reserve(result, typename ResultContainer::size_type(container.size()));
  std::transform(std::begin(container), std::end(container), inserter(result), function);
  return result;
}

// function with result type deduction:
template <template <typename> class C,        // result container type
          typename SC,                        // input container type
          typename F,                         // function type
          typename Value, typename Result, typename ResultContainer>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, F function) {
  return transform<ResultContainer>(std::forward<SC>(container), function);
}

template <template <typename, typename> class C,        // result container type
          typename SC,                                  // input container type
          typename F,                                   // function type
          typename Value, typename Result, typename ResultContainer>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, F function) {
  return transform<ResultContainer>(std::forward<SC>(container), function);
}

// member function without result type deduction:
template <template <typename...> class C,        // result container type
          typename SC,                           // input container type
          typename R, typename S>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, R (S::*p)() const) {
  return transform<C>(std::forward<SC>(container), std::mem_fn(p));
}

// member function with result type deduction:
template <typename ResultContainer,        // complete result container type
          typename SC,                     // input container type
          typename R, typename S>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, R (S::*p)() const) {
  return transform<ResultContainer>(std::forward<SC>(container), std::mem_fn(p));
}

// member without result type deduction:
template <typename ResultContainer,        // complete result container type
          typename SC,                     // input container
          typename R, typename S>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, R S::*p) {
  return transform<ResultContainer>(std::forward<SC>(container), std::mem_fn(p));
}

// member with result type deduction:
template <template <typename...> class C,        // result container
          typename SC,                           // input container
          typename R, typename S>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, R S::*p) {
  return transform<C>(std::forward<SC>(container), std::mem_fn(p));
}

// same container types for input and output, const input

// function:
template <template <typename...> class C,        // container type
          typename F,                            // function type
          typename... CArgs>                     // Arguments to SC
Q_REQUIRED_RESULT decltype(auto) transform(const C<CArgs...> &container, F function) {
  return transform<C, const C<CArgs...> &>(container, function);
}

// member function:
template <template <typename...> class C,        // container type
          typename R, typename S,
          typename... CArgs>        // Arguments to SC
Q_REQUIRED_RESULT decltype(auto) transform(const C<CArgs...> &container, R (S::*p)() const) {
  return transform<C, const C<CArgs...> &>(container, std::mem_fn(p));
}

// members:
template <template <typename...> class C,        // container
          typename R, typename S,
          typename... CArgs>        // Arguments to SC
Q_REQUIRED_RESULT decltype(auto) transform(const C<CArgs...> &container, R S::*p) {
  return transform<C, const C<CArgs...> &>(container, std::mem_fn(p));
}

// same container types for input and output, non-const input

// function:
template <template <typename...> class C,        // container type
          typename F,                            // function type
          typename... CArgs>                     // Arguments to SC
Q_REQUIRED_RESULT decltype(auto) transform(C<CArgs...> &container, F function) {
  return transform<C, C<CArgs...> &>(container, function);
}

// member function:
template <template <typename...> class C,        // container type
          typename R, typename S,
          typename... CArgs>        // Arguments to SC
Q_REQUIRED_RESULT decltype(auto) transform(C<CArgs...> &container, R (S::*p)() const) {
  return transform<C, C<CArgs...> &>(container, std::mem_fn(p));
}

// members:
template <template <typename...> class C,        // container
          typename R, typename S,
          typename... CArgs>        // Arguments to SC
Q_REQUIRED_RESULT decltype(auto) transform(C<CArgs...> &container, R S::*p) {
  return transform<C, C<CArgs...> &>(container, std::mem_fn(p));
}

// Specialization for QStringList:

template <template <typename...> class C = QList,        // result container
          typename F>                                    // Arguments to C
Q_REQUIRED_RESULT decltype(auto) transform(const QStringList &container, F function) {
  return transform<C, const QList<QString> &>(static_cast<QList<QString>>(container), function);
}

// member function:
template <template <typename...> class C = QList,        // result container type
          typename R, typename S>
Q_REQUIRED_RESULT decltype(auto) transform(const QStringList &container, R (S::*p)() const) {
  return transform<C, const QList<QString> &>(static_cast<QList<QString>>(container), std::mem_fn(p));
}

// members:
template <template <typename...> class C = QList,        // result container
          typename R, typename S>
Q_REQUIRED_RESULT decltype(auto) transform(const QStringList &container, R S::*p) {
  return transform<C, const QList<QString> &>(static_cast<QList<QString>>(container), std::mem_fn(p));
}

//////////////////
// filtered
/////////////////
template <typename C, typename F>
Q_REQUIRED_RESULT C filtered(const C &container, F predicate) {
  C out;
  std::copy_if(std::begin(container), std::end(container), inserter(out), predicate);
  return out;
}

template <typename C, typename R, typename S>
Q_REQUIRED_RESULT C filtered(const C &container, R (S::*predicate)() const) {
  C out;
  std::copy_if(std::begin(container), std::end(container), inserter(out), std::mem_fn(predicate));
  return out;
}

}        // namespace Utils
