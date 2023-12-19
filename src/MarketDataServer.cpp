#include "MarketDataServer.h"
#include "Macros.h"
#include "Streamer.h"

#include <limits>

namespace minirisk {

MarketDataServer::MarketDataServer(const string& filename)
{
    std::ifstream is(filename);
    MYASSERT(!is.fail(), "Could not open file " << filename);
    do {
        string name;
        double value;
        is >> name >> value;
        auto ins = m_data.emplace(name, value);
        MYASSERT(ins.second, "Duplicated risk factor: " << name);
    } while (is);
}

double MarketDataServer::get(const string& name) const
{
    auto iter = m_data.find(name);
    MYASSERT(iter != m_data.end(), "Market data not found: " << name);
    return iter->second;
}

std::pair<double, bool> MarketDataServer::lookup(const string& name) const
{
    auto iter = m_data.find(name);
    return (iter != m_data.end())  // found?
            ? std::make_pair(iter->second, true)
            : std::make_pair(std::numeric_limits<double>::quiet_NaN(), false);
}

std::vector<std::string> MarketDataServer::match(const std::string& expr) const
{
    std::regex r(expr);
	std::vector<std::string> names;
	for (const auto& it : m_data) {
		if (std::regex_match(it.first, r)) 
			names.push_back(it.first);
	}
	return names;
}

} // namespace minirisk

