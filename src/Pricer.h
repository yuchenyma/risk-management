#pragma once

#include "IPricer.h"

namespace minirisk {

struct Pricer : IPricer
{
	Pricer(const string& baseccy) : m_baseccy(baseccy) {}
	string m_baseccy;
};

} 
