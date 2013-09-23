/*****************************************************/
/* lean                         (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef _LEAN_CORE_H_
#define _LEAN_CORE_H_

/// @addtogroup GlobalMacros Global macros
/// @see GlobalSwitches
/// @see CPP0X
/// @{

/// Appends token b to token a.
#define LEAN_JOIN(a, b) a##b
/// Appends token b to token a, evaluating macros first.
#define LEAN_JOIN_VALUES(a, b) LEAN_JOIN(a, b)

/// Quotes the given expression.
#define LEAN_QUOTE(expr) #expr
/// Quotes the given value, evaluating macros first.
#define LEAN_QUOTE_VALUE(value) LEAN_QUOTE(value)

/// @}

#include "platform.h"
#include "cpp0x.h"
#include <cstddef> // maximum bloat for size_t & offsetof :-/

/// @addtogroup Libray Lean cpp library
/// For a better overview, see <a href="namespaces.html">Namespaces</a>.
/// @see <a href="namespaces.html">Namespaces</a>
/// @{

/// Main namespace of the lean library.
namespace lean
{

/// Allows for the construction of uninitialized objects.
enum uninitialized_t {
	uninitialized ///< Do not initialize.
};
/// Allows for explicitly consuming construction (aka move).
enum consume_t {
	consume ///< Consume the contents of the given object.
};

/// Returns the smaller of both arguments.
template <class Type>
LEAN_INLINE const Type& min(const Type &cand, const Type &keep) {
	return (cand < keep) ? cand : keep; }
/// Returns the larger of both arguments.
template <class Type>
LEAN_INLINE const Type& max(const Type &cand, const Type &keep) {
	return (cand > keep) ? cand : keep; }

/// Returns the address of the given reference.
template <class Type>
LEAN_INLINE Type* addressof(Type& value) {
	// Use C-style cast as const_casting would only make it worse
	return reinterpret_cast<Type*>( &(char&)value ); }

/// Returns the next iterator.
template <class Iterator>
LEAN_INLINE Iterator next(Iterator iterator) {
	return ++iterator; }
/// Returns the previous iterator.
template <class Iterator>
LEAN_INLINE Iterator prev(Iterator iterator) {
	return --iterator; }

/// Returns a pointer to the end of the given array.
template <class T, size_t Size>
LEAN_INLINE T* arrayend(T (&a)[Size]) { return &a[0] + Size; }
/// Returns a pointer to the end of the given null-terminated array.
template <class T, size_t Size>
LEAN_INLINE T* ntarrayend(T (&a)[Size]) { return &a[Size - 1]; }

namespace detail
{
	template <class Type, size_t Size>
	char const (&arraylen_helper(Type const (&)[Size]))[Size];
}

/// Gets the length of the given array.
template <class Type, size_t Size>
size_t arraylen(Type const (&)[Size]) { return Size; }
/// Gets the length of the given null-terminated array.
template <class Type, size_t Size>
size_t ntarraylen(Type const (&)[Size]) { return Size - 1; }

} // namespace

/// Gets the length of the given array.
#define LEAN_ARRAYLEN(x) sizeof(::lean::detail::arraylen_helper(x))
/// Gets the length of the given null-terminated array.
#define LEAN_NTARRAYLEN(x) (sizeof(::lean::detail::arraylen_helper(x)) - 1)

/// @}

/// @addtogroup GlobalMacros
/// @{

#ifndef LEAN_INTERFACE_BEHAVIOR

#ifndef LEAN_OPTIMIZE_DEFAULT_DESTRUCTOR
	/// Makes the given class behave like an interface.
	#define LEAN_INTERFACE_BEHAVIOR(name) \
			protected: \
				LEAN_INLINE name& operator =(const name&) noexcept { return *this; } \
				LEAN_INLINE ~name() noexcept { } \
			private:
#else
	#define LEAN_INTERFACE_BEHAVIOR(name) \
			protected: \
				LEAN_INLINE name& operator =(const name&) noexcept { return *this; } \
			private:
#endif
	/// Makes the given class behave like an interface supporting shared ownership.
	#define LEAN_SHARED_INTERFACE_BEHAVIOR(name) \
			public: \
				virtual ~name() noexcept { } \
			protected: \
				LEAN_INLINE name& operator =(const name&) noexcept { return *this; } \
			private:
	/// Makes the given class behave like a static interface (abstract w/o virtual methods).
	#define LEAN_STATIC_INTERFACE_BEHAVIOR(name) LEAN_INTERFACE_BEHAVIOR(name) \
			protected: \
				LEAN_INLINE name() noexcept { } \
				LEAN_INLINE name(const name&) noexcept { } \
			private:

#ifndef LEAN_OPTIMIZE_DEFAULT_DESTRUCTOR
	/// Makes the given class behave like a base class.
	#define LEAN_BASE_BEHAVIOR(name) \
			protected: \
				LEAN_INLINE ~name() noexcept { } \
			private:
#else
	#define LEAN_BASE_BEHAVIOR(name) private:
#endif
	/// Makes the given class behave like a base class supporting shared ownership.
	#define LEAN_SHARED_BASE_BEHAVIOR(name) \
			public: \
				virtual ~name() noexcept { } \
			private:

	/// Defines delegating default construction.
	#define LEAN_BASE_DELEGATE_CONS(name, base) \
		LEAN_INLINE name() : base() { }	
	/// Defines delegating copy construction.
	#define LEAN_BASE_DELEGATE_COPY(name, base) \
		LEAN_INLINE name(const name &right) : base(right) { }
	/// Defines delegating assignment.
	#define LEAN_BASE_DELEGATE_ASSIGNMENT(name, base) \
		LEAN_INLINE name& operator =(const name &right) { this->base::operator =(right); return *this; }

	/// Defines delegating copy construction and assignment.
	#define LEAN_BASE_DELEGATE_COPYASSIGNMENT(name, base) \
		LEAN_BASE_DELEGATE_COPY(name, base) \
		LEAN_BASE_DELEGATE_ASSIGNMENT(name, base)
	/// Defines delegating default/copy construction and assignment.
	#define LEAN_BASE_DELEGATE(name, base) \
		LEAN_BASE_DELEGATE_CONS(name, base) \
		LEAN_BASE_DELEGATE_COPY(name, base) \
		LEAN_BASE_DELEGATE_ASSIGNMENT(name, base)
#endif

/// @}

namespace lean
{

/// Defines value as Value.
template <class Type, Type Value>
struct literal_constant {
	static const Type value = Value;
};
/// Helper struct that may be used to pass constant booleans (compile-time literals).
template <bool Value>
struct literal_bool : literal_constant<bool, Value> { };
/// Helper struct that may be used to pass constant integers (compile-time literals).
template <int Value>
struct literal_int : literal_constant<int, Value> { };
/// Defines value as false.
struct false_type : literal_bool<false> { void* buffer[1]; };
/// Defines value as true.
struct true_type : literal_bool<true> { void* buffer[2]; };
/// Defines a false literal, ignoring any template arguments.
template <class T>
struct dependent_false : false_type { };
/// Defines a false literal, ignoring any template arguments.
template <int N>
struct int_dependent_false : false_type { };

#define LEAN_IS_TRUE_TYPE_SIZE(x) (sizeof(x) == sizeof(::lean::true_type))

/// True if types are equal, false otherwise.
template <class Type1, class Type2>
struct is_equal : false_type { };
#ifndef DOXYGEN_SKIP_THIS
	template <class Type>
	struct is_equal<Type, Type> : true_type { };
#endif

/// Redefines the given type.
template <class Type>
struct identity {
	/// Type.
	typedef Type type;
};
/// Redefines the given type if true, empty otherwise.
template <bool Condition, class Type>
struct enable_if : identity<Type> { };
/// Redefines TrueType if condition fulfilled, FalseType otherwise.
template <bool Condition, class TrueType, class FalseType>
struct conditional_type : identity<TrueType> { };
/// Redefines Type1 if not void, else Type2 if not void, nothing otherwise.
template <class Type1, class Type2>
struct first_non_void : identity<Type1> { };
#ifndef DOXYGEN_SKIP_THIS
	template <class Type>
	struct enable_if<false, Type> { };
	template <class TrueType, class FalseType>
	struct conditional_type<false, TrueType, FalseType> : identity<FalseType> { };
	template <class Type2>
	struct first_non_void<void, Type2> : identity<Type2> { };
	template <>
	struct first_non_void<void, void> { };
#endif

/// Removes a reference from the given type.
template <class Type>
struct remove_reference : identity<Type>, false_type { };
#ifndef DOXYGEN_SKIP_THIS
	template <class Type>
	struct remove_reference<Type&> : identity<Type>, true_type { };
	#ifndef LEAN0X_NO_RVALUE_REFERENCES
	template <class Type>
	struct remove_reference<Type&&> : identity<Type>, true_type { };
	#endif
#endif
/// Removes a const modifier from the given type.
template <class Type>
struct remove_const : identity<Type>, false_type { };
/// Removes a volatile modifier from the given type.
template <class Type>
struct remove_volatile : identity<Type>, false_type { };
#ifndef DOXYGEN_SKIP_THIS
	template <class Type>
	struct remove_const<Type const> : identity<Type>, true_type { };
	template <class Type>
	struct remove_volatile<Type volatile> : identity<Type>, true_type { };
#endif
/// Removes a cv modifier from the given type.
template <class Type>
struct remove_cv : identity<typename remove_volatile<typename remove_const<Type>::type>::type> { };
/// Removes a pointer from the given type.
template <class Type, class Stripped = typename remove_cv<Type>::type>
struct remove_pointer : identity<Type>, false_type { };
#ifndef DOXYGEN_SKIP_THIS
	template <class Type, class Stripped>
	struct remove_pointer<Type, Stripped*> : identity<Stripped>, true_type { };
#endif

template <class Type>
LEAN_INLINE Type&& forward(typename identity<Type>::type& arg) {
	return const_cast<Type&&>(arg); }
template <class Type>
LEAN_INLINE typename remove_reference<Type>::type&& move(Type&& arg) {
	return const_cast<typename remove_reference<Type>::type&&>(arg); }

/// Returns an rval expression of type T.
template <class T>
T make_rval();
/// Returns an lval expression of type T.
template <class T>
T& make_lval();

/// Checks if the given integer type is unsigned.
template <class Integer>
struct is_unsigned : literal_bool<(Integer(-1) > Integer(0))> { };
/// Checks if the given type is an integer type.
template <class Integer>
struct is_integral : false_type { };
/// Converts the given signed integer type to the corresponding unsigned integer type.
template <class Integer>
struct make_unsigned;
/// Converts the given unsigned integer type to the corresponding signed integer type.
template <class Integer>
struct make_signed;
#ifndef DOXYGEN_SKIP_THIS
	#define LEAN_MAKE_INTEGER(integer) \
		template <> struct is_integral<unsigned integer> : true_type { }; \
		template <> struct is_integral<signed integer> : true_type { }; \
		template <> struct make_unsigned<unsigned integer> { typedef unsigned integer type; }; \
		template <> struct make_unsigned<signed integer> { typedef unsigned integer type; }; \
		template <> struct make_signed<unsigned integer> { typedef signed integer type; }; \
		template <> struct make_signed<signed integer> { typedef signed integer type; };
	LEAN_MAKE_INTEGER(char)
	LEAN_MAKE_INTEGER(short)
	LEAN_MAKE_INTEGER(int)
	LEAN_MAKE_INTEGER(long)
	LEAN_MAKE_INTEGER(long long)
#endif

namespace detail
{
	namespace checks
	{
		template <class Source, class Destination>
		struct is_convertible {
			static true_type sfinae_check(Destination);
			static false_type sfinae_check(...);
			static bool const value = LEAN_IS_TRUE_TYPE_SIZE(
				is_convertible::sfinae_check( make_rval<Source>() )
			);
		};
		template <class EmptyType>
		struct is_empty {
			// MONITOR: Make use of empty base class optimization
			struct derived_with_int : private EmptyType { int i; };
			static bool const value = (sizeof(derived_with_int) == sizeof(int));
		};
	} // namespace
} // namespace

/// True if Src is convertible to Dest, false otherwise.
template <class Src, class Dest>
struct is_convertible : literal_bool<detail::checks::is_convertible<Src, Dest>::value> { };
/// True if Type is derived from Base, false otherwise.
template <class Type, class Base>
struct is_derived : is_convertible<Type*, Base*> { };
/// True if Type is an empty class.
template <class Type>
struct is_empty : literal_bool<detail::checks::is_empty<Type>::value> { };

/// True if Type defines the given type, false otherwise.
#define LEAN_DEFINE_HAS_TYPE(TypeName)                                                 \
	template <class Type>                                                                \
	struct detect_type_##TypeName {                                                      \
		template <class T>                                                                 \
		static ::lean::true_type sfinae_check(T*, typename T::TypeName* = nullptr);        \
		static ::lean::false_type sfinae_check(...);                                       \
		static bool const value = LEAN_IS_TRUE_TYPE_SIZE(                                  \
				detect_type_##TypeName::sfinae_check( ::lean::make_rval<Type*>() )             \
			);                                                                               \
	};																																									 \
	template <class Type>                                                                \
	struct has_type_##TypeName : ::lean::literal_bool<detect_type_##TypeName<Type>::value> { };

/// True if Type defines the given type as X, false otherwise.
#define LEAN_DEFINE_HAS_TYPE_AND_X(TypeName, X)                                                 \
	LEAN_DEFINE_HAS_TYPE(TypeName)                                                                \
	template <class Type, bool HasType = has_type_##TypeName<Type>::value>                        \
	struct has_type_and_##X##TypeName : ::lean::literal_bool<(Type::TypeName::value == (X))> { }; \
	template <class Type>                                                                         \
	struct has_type_and_##X##TypeName<Type, false> : ::lean::false_type { };

/// True if Type defines the given type as true, false otherwise.
#define LEAN_DEFINE_HAS_TYPE_AND_TRUE(TypeName) LEAN_DEFINE_HAS_TYPE_AND_X(TypeName, true)
/// True if Type defines the given type as false, false otherwise.
#define LEAN_DEFINE_HAS_TYPE_AND_FALSE(TypeName) LEAN_DEFINE_HAS_TYPE_AND_X(TypeName, false)

LEAN_DEFINE_HAS_TYPE(iterator_category);

/// Checks if the given type is an iterator.
template <class Type>
struct is_iterator : literal_bool<
	has_type_iterator_category<Type>::value ||
	remove_pointer<Type>::value
> { };

} // namespace

// NOTE: <iterator> includes loads of cruft
namespace std { template <class Iterator> struct iterator_traits; }

namespace lean
{

template <class Type>
struct iterator_types {
	typedef typename std::iterator_traits<Type>::value_type value_type;
	typedef typename std::iterator_traits<Type>::difference_type difference_type;
	typedef typename std::iterator_traits<Type>::pointer pointer;
	typedef typename std::iterator_traits<Type>::reference reference;
};
// note: pointer shortcut w/o STL <iterator> cruft
template<class Type>
struct iterator_types<Type*> {
	typedef Type value_type;
	typedef ptrdiff_t difference_type;
	typedef Type* pointer;
	typedef Type& reference;
};
template<class Type>
struct iterator_types<Type const*> {
	typedef Type value_type;
	typedef ptrdiff_t difference_type;
	typedef Type const* pointer;
	typedef Type const& reference;
};

template <class Type, bool HasDeref = is_iterator<Type>::value>
struct iterator_types_or_none_base : iterator_types<Type> { };
template <class Type>
struct iterator_types_or_none_base<Type, false> { };
// note: pointer shortcut w/o SFINAE iterator detection
template <class Type>
struct iterator_types_or_none : iterator_types_or_none_base<Type> { };
template <class Type>
struct iterator_types_or_none<Type*> : iterator_types<Type*> { };

template < class Type, bool HasDeref = is_iterator<Type>::value >
struct iterator_types_or_type_base : iterator_types<Type> { };
template <class Type>
struct iterator_types_or_type_base<Type, false> {
	typedef Type value_type;
	typedef Type difference_type;
	typedef Type* pointer;
	typedef Type& reference;
};
// note: pointer shortcut w/o SFINAE iterator detection
template <class Type>
struct iterator_types_or_type : iterator_types_or_type_base<Type> { };
template <class Type>
struct iterator_types_or_type<Type*> : iterator_types<Type*> { };

/// Range type.
template <class Iterator>
struct range
{
	typedef Iterator iterator;
	typedef Iterator const_iterator;

	typedef typename iterator_types_or_type<iterator>::difference_type difference_type;
	typedef typename make_unsigned<difference_type>::type size_type;
	typedef typename iterator_types_or_type<iterator>::reference reference;

	iterator first, last;

	range() : first(), last() { }
	range(iterator f, iterator l) : first((iterator&&) f), last((iterator&&) l) { }
	template <class Range>
	explicit range(Range& r) : first(r.begin()), last(r.end()) { }
	template <class Range>
	explicit range(Range const& r) : first(r.begin()), last(r.end()) { }
	
	void assign(iterator begin, iterator end) { first = begin; last = end; }
	template <class Range>
	void assign(Range &range) { first = range.begin(); last = range.end(); }

	// const& to catch pre-POD usage
	LEAN_INLINE iterator const& begin() const { return first; }
	LEAN_INLINE iterator const& end() const { return last; }
	LEAN_INLINE bool empty() const { return (first == last); }
	LEAN_INLINE difference_type delta() const { return last - first; }
	LEAN_INLINE size_type size() const { return last - first; }
	template <class Index>
	LEAN_INLINE reference operator [](Index i) const { return *(first + i); }

	/// Gets a reference to the first element.
	LEAN_INLINE reference operator *() const { return *first; }
	/// Gets an iterator to the first element.
	LEAN_INLINE iterator operator ->() const { return first; }
	/// Gets whether this range is non-empty.
	LEAN_INLINE operator bool() const { return (first != last); }
};

/// Makes a range from the given pair of iterators.
template <class Iterator>
LEAN_INLINE range<Iterator> make_range(Iterator begin, Iterator end) {
	return range<Iterator>(begin, end); }
/// Makes a range from the given pair of iterators.
template <class Iterator, class Size>
LEAN_INLINE range<Iterator> make_range_n(Iterator begin, Size len) {
	return range<Iterator>(begin, begin + len); }
/// Makes a range from the given range-compatible object.
template <class Range>
LEAN_INLINE range<typename Range::iterator> make_range(Range &r) {
	return range<typename Range::iterator>(r.begin(), r.end()); }
/// Makes a range from the given range-compatible object.
template <class Range>
LEAN_INLINE range<typename Range::const_iterator> make_range(Range const &r) {
	return range<typename Range::const_iterator>(r.begin(), r.end()); }

/// Makes a range from the given range-compatible vector-like container.
template <class Range>
LEAN_INLINE range<typename Range::pointer> data_range(Range &r) {
#ifndef LEAN0X_NO_STL
	return range<typename Range::pointer>(r.data(), r.data() + r.size()); }
#else
	return range<typename Range::pointer>(&r[0], &r[0] + r.size()); }
#endif
/// Makes a range from the given range-compatible vector-like container.
template <class Range>
LEAN_INLINE range<typename Range::const_pointer> data_range(Range const &r) {
#ifndef LEAN0X_NO_STL
	return range<typename Range::const_pointer>(r.data(), r.data() + r.size()); }
#else
	return range<typename Range::const_pointer>(&r[0], &r[0] + r.size()); }
#endif

/// Casts the iterators of the given source range into iterators of the given destination range using const_cast.
template <class DestRange, class Range>
LEAN_INLINE DestRange const_range_cast(const Range &right) {
	return DestRange(
			const_cast<typename DestRange::iterator>(right.begin()),
			const_cast<typename DestRange::iterator>(right.end())
		); }
/// Casts the iterators of the given source range into iterators of the given destination range using reinterpret_cast.
template <class DestRange, class Range>
LEAN_INLINE DestRange reinterpret_range_cast(const Range &right) {
	return DestRange(
			reinterpret_cast<typename DestRange::iterator>(right.begin()),
			reinterpret_cast<typename DestRange::iterator>(right.end())
		); }
/// Casts the iterators of the given source range into iterators of the given destination range using static_cast.
/// WARNING: For pointer types, this may effectively be the same as reinterpret_cast!
template <class DestRange, class Range>
LEAN_INLINE DestRange static_range_cast(const Range &right) {
	return DestRange(
			static_cast<typename DestRange::iterator>(right.begin()),
			static_cast<typename DestRange::iterator>(right.end())
		); }
/// Constructs a range of the given type from the given range.
/// WARNING: For pointer types, this may effectively be the same as reinterpret_cast!
template <class DestRange, class Range>
LEAN_INLINE DestRange range_cast(const Range &right) {
	return DestRange(right.begin(), right.end()); }

} // namespace

#endif