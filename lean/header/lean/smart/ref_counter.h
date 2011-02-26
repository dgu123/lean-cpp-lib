/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_REF_COUNTER
#define LEAN_SMART_REF_COUNTER

#include <memory>
#include "../lean.h"
#include "../platform/atomic.h"

namespace lean
{
namespace smart
{

/// Reference counter class that maintains strong and weak reference counts.
template < class Counter = long, class Allocator = std::allocator<Counter> >
class ref_counter
{
public:
	/// Reference counter structure.
	struct ref_counts
	{
		// Reference counter
		Counter references;

		// Weak reference count
		Counter weakRefrences;

		/// Constructor.
		ref_counts(Counter references, Counter weakRefrences)
			: references(references),
			weakRefrences(weakRefrences) { };
	};

	// Allocator
	typedef typename Allocator::template rebind<Counter>::other _allocator_type;
	_allocator_type m_allocator;

	// Reference counts
	ref_counts *m_references;

	/// Allocates and constructs a new internal reference counting object from the given parameters.
	ref_counts new_ref_counts(Counter references, Counter weakRefrences)
	{
		ref_counts *references = m_allocator.allocate(1);

		try
		{
			new (references) ref_counts(references, weakRefrences);
		}
		catch (...)
		{
			m_allocator.deallocate(references, 1);
			throw;
		}
	}

	/// Destructs and deallocates the given internal reference counting object.
	void delete_ref_counts(const ref_counts *references)
	{
		try
		{
			references->~ref_counts();
		}
		catch (...)
		{
			m_allocator.deallocate(references, 1);
			throw;
		}

		m_allocator.deallocate(references, 1);
	}

	/// Acquires the given internal reference counting object.
	void acquire(ref_counts *references)
	{
		atomic_increment(m_references->weakRefrences);
	}

	/// Releases the given internal reference counting object.
	void release(ref_counts *references)
	{
		// Clean up, if this is the last reference
		if (atomic_decrement(references->weakRefrences))
			delete_ref_counts(references);
	}

public:
	/// Type of the counter used by this class.
	typedef Counter counter_type;
	/// Type of the allocator used by this class.
	typedef _allocator_type allocator_type;
	
	/// Constructor.
	explicit ref_counter(counter_type references = 1)
		: m_references( new_ref_counts(references, 1) ) { }
	/// Constructor.
	explicit ref_counter(const allocator_type& allocator)
		: m_allocator(allocator),
		m_references( new_ref_counts(1, 1) ) { }
	/// Constructor.
	ref_counter(counter_type references, const allocator_type& allocator)
		: m_allocator(allocator),
		m_references( new_ref_counts(references, 1) ) { }
	/// Copy constructor.
	ref_counter(const ref_counter& refCounter)
		: m_allocator(m_references.allocator),
		m_references(refCounter.m_references)
	{
		acquire(m_references);
	}
	/// Destructor.
	~ref_counter()
	{
		release(m_references);
	}

	/// Assignment operator.
	ref_counter& operator =(const ref_counter& refCounter)
	{
		// Don't re-assign the same
		if(m_references != refCounter.m_references)
		{
			acquire(refCounter.m_references);

			ref_counts *prevReferences = m_references;
			m_references = refCounter.m_references;
			
			release(prevReferences);
		}

		return *this;
	}

	/// Increments the current reference count.
	bool increment() const
	{
		for (;;)
		{
			Counter references = (volatile Counter&)m_references->references;
			
			// Make sure reference count has not become 0 yet
			if (references < 1)
				return false;
			// Make sure value has not changed until it has successfully been updated
			else if (atomic_test_and_set(m_references->references, references, references + 1))
				return true;
		}

		LEAN_ASSERT(0);
	}
	/// Decrements the current reference count.
	void decrement() const
	{
		atomic_decrement(references->references)
	}
	/// Overwrites the current reference count with the given value.
	inline void invalidate(counter_type references = 0) const
	{
		atomic_set(m_references->references, references);
	}

	/// Gets the current reference count.
	inline counter_type count() const { return m_references->references; };
	/// Gets whether the reference-counted object still exists (<==> reference count > 0).
	inline bool valid() const { return (m_references->references > 0); };

	/// Increments the current reference count.
	inline ref_counter& operator ++() { ++const_cast<const ref_counter&>(*this); return *this; };
	/// Increments the current reference count.
	inline const ref_counter& operator ++() const { increment(); return *this; };
	/// Decrements the current reference count.
	inline ref_counter& operator --() { --const_cast<const ref_counter&>(*this); return *this; };
	/// Decrements the current reference count.
	inline const ref_counter& operator --() const { decrement(); return *this; };
	/// Increments the current reference count.
	inline counter_type operator ++(counter_type) const { counter_type prevCount = count(); ++(*this); return prevCount; };
	/// Decrements the current reference count.
	inline counter_type operator --(counter_type) const { counter_type prevCount = count(); --(*this); return prevCount;};

	/// Gets the current reference count.
	inline operator int() const { return count(); };
	/// Gets whether the reference-counted object still exists (<==> reference count > 0).
	inline operator bool() const { return valid(); };
};

} // namespace

using smart::ref_counter;

} // namespace

#endif