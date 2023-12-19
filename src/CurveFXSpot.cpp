#include "CurveFXSpot.h"
#include "Market.h"
#include "Streamer.h"

namespace minirisk {

// Last 7 characters of the curve name should be in "CCY1.CCY2" format, representing the prices of CCY1 in CCY2
CurveFXSpot::CurveFXSpot(Market *mkt, const Date& today, const string& curve_name)
    : m_today(today), m_name(curve_name), m_rate(mkt->get_fx_spot(curve_name.substr(curve_name.length() - 7, 3), curve_name.substr(curve_name.length() - 3, 3))) {
}

double CurveFXSpot::spot() const {
    return m_rate;
}

} // namespace minirisk