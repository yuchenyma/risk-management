#include "CurveFXForward.h"
#include "Market.h"
#include "Streamer.h"

namespace minirisk {

    CurveFXForward::CurveFXForward(Market* mkt, const Date& today, const string& curve_name)
        : m_today(today)
        , m_name(curve_name)
        , m_fx_curve(mkt->get_fxspot_curve(fx_spot_name(curve_name.substr(curve_name.length() - 7, 3), curve_name.substr(curve_name.length() - 3, 3))))
        , m_disc_curve_1(mkt->get_discount_curve(ir_curve_discount_name(curve_name.substr(curve_name.length() - 7, 3))))
        , m_disc_curve_2(mkt->get_discount_curve(ir_curve_discount_name(curve_name.substr(curve_name.length() - 3, 3)))) {
    }

    double CurveFXForward::fwd(const Date& t) const {
        double spot = m_fx_curve->spot();
        double df1 = m_disc_curve_1->df(t);
        double df2 = m_disc_curve_2->df(t);
        return spot * df1 / df2;
    }

}