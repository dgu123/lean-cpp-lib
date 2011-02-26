/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_REF_COUNTER
#define LEAN_SMART_REF_COUNTER

#include <memory>

#include "deallocate_on_exit.h"

namespace lean
{
namespace smart
{

/// Reference counter class that maintains strong and weak reference counts.
template < class Counter = long, class Allocator = std::allocator<long> >
class ref_counter
{
public:
	/// References structure.
	struct ref_counts
	{
		// Reference counter
		Counter references;

		// Weak reference count
		Counter weakReferences;

		/// Constructor.
		ref_counts(Counter references, Counter weakReferences)
			: references(references),
			weakReferences(weakReferences) { };
	};

	// Allocator
	typedef typename Allocator::template rebind<Counter>::other _allocator_type;
	_allocator_type m_allocator;

	// Reference counts
	ref_counts *m_references;

	/// Acquires and sets the given reference counter.
	inline void AcquireAndSet(References *pReferences)
	{
		m_references = pReferences;
		++m_references->weakReferences;
	}
	/// Releases the current reference counter, leaving the object in an INVALID STATE.
	inline void release()
	{
		--m_references->weakReferences;

		// Clean up, if this is the last reference
		if(m_references->weakReferences < 1)
			delete_ref_counts(m_references);
	}

	/// Allocates and constructs a new internal reference counting object from the given parameters.
	inline ref_counts new_ref_counts(int references, int weakReferences)
	{
		ref_counts *references = m_allocator.allocate(1);

		try
		{
			new (references) ref_counts(references, weakReferences);
		}
		catch(...)
		{
			m_allocator.deallocate(references, 1);
			throw;
		}
	}

	/// Destructs and deallocates the given internal reference counting object.
	inline void delete_ref_counts(const ref_counts *references)
	{
		deallocate_on_exit<ref_counts, _allocator_type> deallocate(references, m_allocator);
		references->~ref_counts();
	}

public:
	/// Type of the allocator used by this class.
	typedef _allocator_type allocator_type;
	
	/// Constructor.
	explicit ref_counter(int references = 1)
		: m_references( new_ref_counts(references, 1) ) { }
	/// Constructor.
	explicit ref_counter(const allocator_type& allocator)
		: m_allocator(allocator),
		m_references( new_ref_counts(1, 1) ) { }
	/// Constructor.
	ref_counter(int references, const allocator_type& allocator)
		: m_allocator(allocator),
		m_references( new_ref_counts(references, 1) ) { }
	/// Copy constructor.
	ref_counter(const ref_counter& refCounter)
		: m_allocator(m_references.allocator),
		m_references(refCounter.m_references)
	{
		++m_references->weakReferences;
	}
	/// Destructor.
	~CReferenceCounter()
	{
		release();
	}

	/// Assignment operator.
	CReferenceCounter& operator =(const CReferenceCounter& refCounter)
	{
		// Don't re-assign the same
		if(m_pReferences != refCounter.m_pReferences)
		{
			Release();
			AcquireAndSet(refCounter.m_pReferences);
		}

		return *this;
	}

	/// Increments the current reference count.
	inline void Increment() const { m_pReferences->iReferences++; };
	/// Decrements the current reference count.
	inline void Decrement() const { m_pReferences->iReferences--; };

	/// Invalidates all references.
	inline void Invalidate(int iReferences = 0) const { m_pReferences->iReferences = iReferences; };

	/// Gets the current reference count.
	inline int GetCount() const { return m_pReferences->iReferences; };
	/// Gets whether the reference-counted object still exists (<==> reference count > 0).
	inline bool IsValid() const { return (m_pReferences->iReferences > 0); };

	/// Increments the current reference count.
	inline CReferenceCounter& operator ++() { ++const_cast<const CReferenceCounter&>(*this); return *this; };
	/// Increments the current reference count.
	inline const CReferenceCounter& operator ++() const { Increment(); return *this; };
	/// Decrements the current reference count.
	inline CReferenceCounter& operator --() { --const_cast<const CReferenceCounter&>(*this); return *this; };
	/// Decrements the current reference count.
	inline const CReferenceCounter& operator --() const { Decrement(); return *this; };
	/// Increments the current reference count.
	inline int operator ++(int) const { int iPrevCount = GetCount(); ++(*this); return iPrevCount; };
	/// Decrements the current reference count.
	inline int operator --(int) const { int iPrevCount = GetCount(); --(*this); return iPrevCount;};

	/// Gets the current reference count.
	inline operator int() const { return GetCount(); };
	/// Gets whether the reference-counted object still exists (<==> reference count > 0).
	inline operator bool() const { return IsValid(); };
};

} // namespace

using smart::ref_counter;

} // namespace

#endif