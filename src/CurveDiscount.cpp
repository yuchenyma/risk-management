#include "CurveDiscount.h"
#include "Market.h"
#include "Streamer.h"

#include <cmath>


namespace minirisk {

    CurveDiscount::CurveDiscount(Market* mkt, const Date& today, const string& curve_name)
        : m_today(today)
        , m_name(curve_name)
        , m_rates(mkt->get_yield(curve_name.substr(ir_curve_discount_prefix.length(), 3)))
    {
    }

    double CurveDiscount::df(const Date& t) const
    {
        unsigned dt = t - m_today;

        MYASSERT((!(t < m_today)), "Curve " << m_name << ", DF not available before anchor date " << m_today << ", requested " << t);
        MYASSERT((!(t - m_today > m_rates.rbegin()->first)), "Curve " << m_name << ", DF not available beyond last tenor date " <<
            Date(m_today.get_serial() + m_rates.rbegin()->first) << ", requested " << t);
        auto rateIter = m_rates.upper_bound(dt);
        unsigned T1 = rateIter->first;
        double rateAtT1 = rateIter->second;
        unsigned T0 = (--rateIter)->first;
        double rateAtT0 = rateIter->second;

        double interpolatedRate = rateAtT0 + (rateAtT1 - rateAtT0) * (dt - T0) / (T1 - T0);
        return std::exp(-interpolatedRate / 365.0);
    }
}