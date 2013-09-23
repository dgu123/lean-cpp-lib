#pragma once

#include <lean/pimpl/pimpl_ptr.h>
#include <lean/pimpl/opaque_val.h>

DECLARE_OPAQUE_TYPE(SomeInterfaceHandle, void*);
DECLARE_OPAQUE_TYPE(SomeHandle, void*);

class PrivateInterface;

#ifdef HANDLE_KNOWN
	class PrivateInterface
	{
	public:
		const bool trueValue;
		PrivateInterface() : trueValue(true) { }
		bool getTrue() { return true; }
	};

	typedef struct { int unused; } *PrivateHandle;

	DEFINE_OPAQUE_TYPE(SomeInterfaceHandle, PrivateInterface*);
	DEFINE_OPAQUE_TYPE(SomeHandle, PrivateHandle);
#endif

class SomeModule
{
private:
	lean::pimpl_ptr<PrivateInterface> m_interface;
	SomeHandle m_handle;

public:
	SomeModule(int bogusTestHandle);
	~SomeModule();

	SomeInterfaceHandle getInterface();
	SomeHandle getHandle();
};
