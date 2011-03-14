#include "stdafx.h"
#include "pimpl_modules.h"
#include "module_test_impl.h"

FixedModule::FixedModule(int &ctrCalls, int &dtrCalls)
	: m_impl( new UnsafeModuleTestImpl(&ctrCalls, &dtrCalls) ) { }

FixedModule::~FixedModule() { }

UnsafeFixedModule::UnsafeFixedModule(int &ctrCalls, int &dtrCalls)
	: m_impl( new UnsafeModuleTestImpl(&ctrCalls, &dtrCalls) ) { }

UnsafeFixedModule::~UnsafeFixedModule() { }

SafeFixedModule::SafeFixedModule(int &ctrCalls, int &dtrCalls)
	: m_impl( new SafeModuleTestImpl(&ctrCalls, &dtrCalls) ) { }

SafeFixedModule::~SafeFixedModule() { }

SafeHeaderModule::SafeHeaderModule(int &ctrCalls, int &dtrCalls)
	: m_impl( new SafeModuleTestImpl(&ctrCalls, &dtrCalls) ) { }

UnsafeHeaderModule::UnsafeHeaderModule(int &ctrCalls, int &dtrCalls)
	: m_impl( new UnsafeModuleTestImpl(&ctrCalls, &dtrCalls) ) { }
