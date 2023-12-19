#include "PricerPayment.h"
#include "TradePayment.h"
#include "CurveDiscount.h"
#include "CurveFXSpot.h"

namespace minirisk {

PricerPayment::PricerPayment(const TradePayment& trd, const string& baseccy)
    : Pricer(baseccy)
	, m_amt(trd.quantity())
    , m_dt(trd.delivery_date())
    , m_ir_curve(ir_curve_discount_name(trd.ccy()))
    , m_fx_ccy(trd.ccy() == m_baseccy? "": fx_spot_name(trd.ccy(), m_baseccy))
{
}

double PricerPayment::price(Market& mkt, const FixingDataServer& fds) const
{
    ptr_disc_curve_t disc = mkt.get_discount_curve(m_ir_curve);
    double df = disc->df(m_dt); // this throws an exception if m_st < today or m_st > max tenor

	if (m_fx_ccy.length() == 0) return m_amt * df;

	ptr_fxspot_curve_t fxspot = mkt.get_fxspot_curve(m_fx_ccy);
	double sp = fxspot->spot();

    return m_amt * df * sp;
}

} // namespace minirisk


