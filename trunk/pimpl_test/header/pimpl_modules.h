#pragma once

#include <lean/pimpl/pimpl.h>

class UnsafeModuleTestImpl;
class SafeModuleTestImpl;

class FixedModule
{
private:
	lean::pimpl_ptr<UnsafeModuleTestImpl> m_impl;

public:
	FixedModule(int &ctrCalls, int &dtrCalls);
	~FixedModule();
};

class UnsafeFixedModule
{
private:
	lean::pimpl_ptr<UnsafeModuleTestImpl, lean::unsafe_pimpl_base> m_impl;

public:
	UnsafeFixedModule(int &ctrCalls, int &dtrCalls);
	~UnsafeFixedModule();
};

class SafeFixedModule
{
private:
	lean::pimpl_ptr<SafeModuleTestImpl, lean::safe_pimpl_base> m_impl;

public:
	SafeFixedModule(int &ctrCalls, int &dtrCalls);
	~SafeFixedModule();
};

class SafeHeaderModule
{
private:
	lean::pimpl_ptr<SafeModuleTestImpl, lean::safe_pimpl_base> m_impl;

public:
	SafeHeaderModule(int &ctrCalls, int &dtrCalls);
};

class UnsafeHeaderModule
{
private:
	lean::pimpl_ptr<UnsafeModuleTestImpl, lean::unsafe_pimpl_base> m_impl;

public:
	UnsafeHeaderModule(int &ctrCalls, int &dtrCalls);
};
