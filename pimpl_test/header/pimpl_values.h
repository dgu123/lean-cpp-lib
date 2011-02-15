#pragma once

#include "pimpl_modules.h"

DECLARE_OPAQUE_TYPE(ModuleHandle, void*);

#ifdef MODULE_KNOWN
	class SubModule;

	DEFINE_OPAQUE_TYPE(ModuleHandle, SubModule*);
#endif

class SubModule
{
private:
	ModuleHandle m_module;

public:
	SubModule(int n);
	~SubModule();

	ModuleHandle getHandle();
};
