#pragma once
#include "ICurve.h"

namespace minirisk {

struct Market;

struct CurveFXForward : ICurveFXForward {
    CurveFXForward(Market *mkt, const Date& today, const string& curve_name);

    virtual string name() const override { return m_name; }
    virtual Date today() const override { return m_today; }

    // Compute FX forward price
    virtual double fwd(const Date& t) const override;

private:
    Date m_today;
    string m_name;
    ptr_fxspot_curve_t m_fx_curve;
    ptr_disc_curve_t m_disc_curve_1;
    ptr_disc_curve_t m_disc_curve_2;
};

} // namespace minirisk