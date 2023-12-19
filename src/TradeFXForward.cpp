#include "TradeFXForward.h"
#include "PricerFXForward.h"

namespace minirisk {

    ppricer_t TradeFXForward::pricer(const std::string& baseccy) const {
        return std::make_shared<PricerFXForward>(*this, baseccy);
    }

    void TradeFXForward::save_details(my_ofstream& os) const {
        os << m_ccy1 << m_ccy2 << m_strike << m_fixing_date << m_settle_date;
    }

    void TradeFXForward::load_details(my_ifstream& is) {
        is >> m_ccy1 >> m_ccy2 >> m_strike >> m_fixing_date >> m_settle_date;
    }

    void TradeFXForward::print_details(std::ostream& os) const {
        os << format_label("Strike level") << m_strike << std::endl;
        os << format_label("Base Currency") << m_ccy1 << std::endl;
        os << format_label("Quote Currency") << m_ccy2 << std::endl;
        os << format_label("Fixing Date") << m_fixing_date << std::endl;
        os << format_label("Settlement Date") << m_settle_date << std::endl;
    }

} // namespace minirisk