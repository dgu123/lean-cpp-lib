#include "stdafx.h"

#define HANDLE_KNOWN
#include "pimpl_values.h"

SomeModule::SomeModule(int bogusTestHandle)
	: m_handle( reinterpret_cast<PrivateHandle>(bogusTestHandle) ),
	m_interface( new PrivateInterface() )
{
}

SomeModule::~SomeModule()
{
}

SomeInterfaceHandle SomeModule::getInterface()
{
	return m_interface.getptr();
}

SomeHandle SomeModule::getHandle()
{
	return m_handle;
}
