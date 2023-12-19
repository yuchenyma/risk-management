#include "FixingDataServer.h"
#include "Macros.h"
#include "Streamer.h"

namespace minirisk {

    FixingDataServer::FixingDataServer(const string& filename)
    {
        std::ifstream is(filename);
        MYASSERT(!is.fail(), "Could not open file " << filename);
        do {
            string name;
            string ds;
            double value;
            is >> name >> ds >> value;
            if (ds.length() >= 8) {
                unsigned y = std::atoi(ds.substr(0, 4).c_str());
                unsigned m = std::atoi(ds.substr(4, 2).c_str());
                unsigned d = std::atoi(ds.substr(6, 2).c_str());
                auto ins = m_data.emplace(std::make_pair(name, Date(y, m, d)), value);
                MYASSERT(ins.second, "Duplicated fixing: " << name);
            } // check and decode yyyymmdd
        } while (is);
    }

    double FixingDataServer::get(const string& name, const Date& t) const
    {
        auto iter = m_data.find(std::make_pair(name, t));
        MYASSERT(iter != m_data.end(), "Fixing not found: " << name << "," << t);
        return iter->second;
    }

    std::pair<double, bool> FixingDataServer::lookup(const string& name, const Date& t) const
    {
        auto iter = m_data.find(std::make_pair(name, t));
        return (iter != m_data.end())  // found?
            ? std::make_pair(iter->second, true)
            : std::make_pair(std::numeric_limits<double>::quiet_NaN(), false);
    }
}
