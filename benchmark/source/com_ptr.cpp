#include "stdafx.h"
#include <lean/smart/com_ptr.h>

class com_obj
{
	int m_ref;

public:
	com_obj() : m_ref(1) { }
	com_obj(const com_obj&) : m_ref(1) { }
	com_obj& operator =(const com_obj&) { return *this; }

	__declspec(noinline) void AddRef() { ++m_ref; }
	__declspec(noinline) void Release() { --m_ref; }
	int GetRefCount() { return m_ref; }
};

LEAN_NOLTINLINE com_obj* create_com_obj()
{
	return new com_obj();
}

LEAN_NOLTINLINE bool com_create_obj(com_obj **pp)
{
	if (pp)
	{
		*pp = new com_obj();
		return true;
	}
	else
		return false;
}

LEAN_NOLTINLINE void correctly_optimized()
{
	lean::com_ptr<com_obj> ptr;
	ptr = create_com_obj();
	ptr = nullptr;
	ptr = create_com_obj();
	ptr = nullptr;
}

LEAN_NOLTINLINE void poorly_optimized()
{
	lean::com_ptr<com_obj> ptr = create_com_obj();
	ptr = nullptr;
	ptr = create_com_obj();
	ptr = nullptr;
}

LEAN_NOLTINLINE void com_obtain()
{
	lean::com_ptr<com_obj> ptr;
	com_create_obj(ptr.rebind());
	ptr = nullptr;
	com_create_obj(ptr.rebind());
	ptr = nullptr;
}

LEAN_NOLTINLINE void com_ptr_benchmark()
{
	// Nothing benched in here, functions called to allow for code generation analysis
	correctly_optimized();
	poorly_optimized();
	com_obtain();
}
