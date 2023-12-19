#include "PricerFXForward.h"
#include "TradeFXForward.h"
#include "CurveDiscount.h"
#include "CurveFXSpot.h"
#include "CurveFXForward.h"

namespace minirisk {

PricerFXForward::PricerFXForward(const TradeFXForward& trd, const string& baseccy)
    : Pricer(baseccy)
	, m_amt(trd.quantity())
	, m_ft(trd.fixing_date())
	, m_st(trd.settle_date())
    , m_s(trd.strike())
	, m_ccy1(trd.ccy1())
	, m_ccy2(trd.ccy2())
{
}

double PricerFXForward::price(Market& mkt, const FixingDataServer& fds) const
{
	ptr_disc_curve_t disc = mkt.get_discount_curve(ir_curve_discount_name(m_ccy2));
	double df = disc->df(m_st); // this throws an exception if m_st < today or m_st > max tenor

	ptr_fxspot_curve_t fxspot = mkt.get_fxspot_curve(fx_spot_name(m_ccy2, m_baseccy));
	double sp = fxspot->spot();

	double fw;
	if (mkt.today() > m_ft) {   //use fixing data when m_ft < today
		fw = fds.get(fx_spot_name(m_ccy1, m_ccy2), m_ft);
	}
	else {
		if (mkt.today() == m_ft) {   //use fixing data first otherwise market data when m_ft == today 
			auto ins = fds.lookup(fx_spot_name(m_ccy1, m_ccy2), m_ft);
			if (ins.second)
				fw = ins.first;
			else {
				ptr_fxspot_curve_t fxs = mkt.get_fxspot_curve(fx_spot_name(m_ccy1, m_ccy2));
				fw = fxs->spot();
			}
		}
		else {    //use market data when m_ft > today
			ptr_fxforward_curve_t fwdc = mkt.get_fxforward_curve(fx_forward_name(m_ccy1, m_ccy2));
			fw = fwdc->fwd(m_ft);	
		}
	}

    return m_amt * df * (fw - m_s) * sp;
}

} // namespace minirisk


