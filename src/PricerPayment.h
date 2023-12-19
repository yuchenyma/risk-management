#pragma once

#include "Pricer.h"
#include "TradePayment.h"

namespace minirisk {

struct PricerPayment : Pricer
{
    PricerPayment(const TradePayment& trd, const string& baseccy);

    virtual double price(Market& m, const FixingDataServer& fds) const;

private:
    double m_amt;
    Date   m_dt;
    string m_ir_curve;
    string m_fx_ccy;
};

} // namespace minirisk

