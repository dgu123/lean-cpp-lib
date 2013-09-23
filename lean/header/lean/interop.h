/*****************************************************/
/* lean                         (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef _LEAN_INTEROP_H_
#define _LEAN_INTEROP_H_

#include "lean.h"

namespace std
{
	template <class E, class A> class vector;
	template <class E, class T, class A> class basic_string;
}

namespace lean
{
namespace interop
{

template <class Interface, class Type>
struct ref_type_map;

template <class T>
class vector {
	LEAN_INTERFACE_BEHAVIOR(vector)
public:
	virtual T* resize(size_t size) = 0;
	virtual range<T const*> data() const = 0;
};
template <class Container, class Element = typename Container::value_type>
struct vector_ref : vector<Element>
{
	Container& container;

	vector_ref(Container& container) : container(container) { }
	Element* resize(size_t size) { container.resize(size); return &container[0]; }
	range<Element const*> data() const { Element const* p = container.data(); return range<Element const*>(p, p + container.size()); }
};
template <class E, class A>
struct ref_type_map< vector<E>, std::vector<E, A> > { typedef vector_ref< std::vector<E, A> > type; };
template <class E, class T, class A>
struct ref_type_map< vector<E>, std::basic_string<E, T, A> > { typedef vector_ref< std::basic_string<E, T, A> > type; };

template <class Type>
struct byte_storage { typedef Type value_type; char bytes[sizeof(Type)]; };

template <class Ref>
struct ref_param
{
	Ref &ref;
	ref_param(Ref& ref) : ref( ref ) { }

	template <class Type>
	struct ref_storage : byte_storage<typename ref_type_map<Ref, Type>::type> { };
	template <class T>
	ref_param(T& ref, ref_storage<T> &storage = ref_storage<T>())
		: ref( *new(storage.bytes) typename ref_storage<T>::value_type(ref) ) { }

private:
	ref_param(ref_param const&);
	ref_param& operator =(ref_param const&);
};

} // namespace

} // namespace

#endif