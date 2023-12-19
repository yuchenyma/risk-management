#pragma once

#include "Trade.h"
#include <string>

namespace minirisk {

    struct TradeFXForward : public Trade<TradeFXForward>
    {
        static const guid_t m_id;
        static const std::string m_name;

        virtual ppricer_t pricer(const std::string& baseccy) const;

        const std::string& ccy1() const { return m_ccy1; }
        const std::string& ccy2() const { return m_ccy2; }
        const Date& fixing_date() const { return m_fixing_date; }
        const Date& settle_date() const { return m_settle_date; }
        double strike() const { return m_strike; }
        void save_details(my_ofstream& os) const;
        void load_details(my_ifstream& is);
        void print_details(std::ostream& os) const;

        std::string m_ccy1;
        std::string m_ccy2;
        double m_strike;
        Date m_fixing_date;
        Date m_settle_date;
    };

} // namespace minirisk