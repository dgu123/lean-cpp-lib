#include "stdafx.h"

#define MODULE_KNOWN

#include "pimpl_values.h"

SubModule::SubModule(int n)
	: m_module(this)
{
}

SubModule::~SubModule()
{
}

ModuleHandle SubModule::getHandle()
{
	return m_module;
}
