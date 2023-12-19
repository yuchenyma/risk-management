#pragma once
#include "ICurve.h"

namespace minirisk {

struct Market;

struct CurveFXSpot : ICurveFXSpot {
    CurveFXSpot(Market *mkt, const Date& today, const string& curve_name);

    virtual string name() const override { return m_name; }
    virtual Date today() const override { return m_today; }
    double spot() const override;  // Return FX spot price

private:
    Date m_today;
    string m_name;
    double m_rate;
};

} // namespace minirisk