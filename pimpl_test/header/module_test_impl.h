#pragma once

#include <lean/pimpl/unsafe_pimpl_base.h>
#include <lean/pimpl/safe_pimpl_base.h>

class UnsafeModuleTestImpl : public lean::unsafe_pimpl_base
{
	int &m_dtrCalls;

public:
	UnsafeModuleTestImpl(int &ctrCalls, int &dtrCalls)
		: m_dtrCalls(dtrCalls)
	{
		++ctrCalls;
	}
	~UnsafeModuleTestImpl()
	{
		++m_dtrCalls;
	}
};

class SafeModuleTestImpl : public lean::safe_pimpl_base
{
	int &m_dtrCalls;

public:
	SafeModuleTestImpl(int &ctrCalls, int &dtrCalls)
		: m_dtrCalls(dtrCalls)
	{
		++ctrCalls;
	}
	~SafeModuleTestImpl()
	{
		++m_dtrCalls;
	}
};
