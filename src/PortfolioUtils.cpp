#include "Global.h"
#include "PortfolioUtils.h"
#include "TradePayment.h"
#include "TradeFXForward.h"
#include "Market.h"

#include <map>
#include <numeric>

namespace minirisk {

void print_portfolio(const portfolio_t& portfolio)
{
    std::for_each(portfolio.begin(), portfolio.end(), [](auto& pt){ pt->print(std::cout); });
}

std::vector<ppricer_t> get_pricers(const portfolio_t& portfolio, const string& baseccy)
{
    std::vector<ppricer_t> pricers(portfolio.size());
    std::transform(portfolio.begin(), portfolio.end(), pricers.begin()
                  , [baseccy](auto &pt) -> ppricer_t { return pt->pricer(baseccy); } );
    return pricers;
}

portfolio_values_t compute_prices(const std::vector<ppricer_t>& pricers, Market& mkt, const FixingDataServer& fds) {
    portfolio_values_t prices(pricers.size()); // Holds calculated prices for each pricer

    // Calculate prices for each pricer, handle exceptions
    std::transform(pricers.begin(), pricers.end(), prices.begin(), [&mkt, &fds](const auto &pp) {
        try {
            return std::make_pair(pp->price(mkt, fds), ""); // Calculate price
        } catch (const std::exception& e) {
            return std::make_pair(std::numeric_limits<double>::quiet_NaN(), e.what()); // Handle price calculation errors
        }
    });
    return prices;
}

portfolio_total_t portfolio_total(const portfolio_values_t& values) {
    portfolio_total_t total; // Holds the total value and error messages
    total.first = 0.0; // Initialize total value to 0

    // Sum valid prices and collect error messages for NaN values
    for (size_t i = 0; i < values.size(); ++i) {
        if (std::isnan(values[i].first)) {
            total.second.push_back(std::make_pair(i, values[i].second)); // Collect error message
        } else {
            total.first += values[i].first; // Sum valid price
        }
    }
    return total;
}
std::vector<std::pair<string, portfolio_values_t>> compute_pv01_bucketed(const std::vector<ppricer_t>& pricers, const Market& mkt, const FixingDataServer& fds) {
    std::vector<std::pair<string, portfolio_values_t>> pv01;  // Holds PV01 for each trade
    const double bump_size = 0.01 / 100;  // Bump size for interest rate

    auto base = mkt.get_risk_factors(ir_rate_prefix + "\\d+[DWMY].[A-Z]{3}");  // Filter IR related risk factors
    Market tmpmkt(mkt);  // Local copy of Market for applying bumps

    pv01.reserve(base.size());
    for (const auto& d : base) {
        std::vector<std::pair<string, double>> bumped(1, d);
        pv01.emplace_back(d.first, portfolio_values_t(pricers.size()));

        // Apply bump down, compute prices
        bumped[0].second -= bump_size;
        tmpmkt.set_risk_factors(bumped);
        auto pv_dn = compute_prices(pricers, tmpmkt, fds);

        // Apply bump up, compute prices
        bumped[0].second += 2 * bump_size;
        tmpmkt.set_risk_factors(bumped);
        auto pv_up = compute_prices(pricers, tmpmkt, fds);

        // Restore original market state
        bumped[0].second = d.second;
        tmpmkt.set_risk_factors(bumped);

        // Compute derivative estimator via central finite differences
        double dr = 2.0 * bump_size;
        std::transform(pv_up.begin(), pv_up.end(), pv_dn.begin(), pv01.back().second.begin(), [dr](const auto& hi, const auto& lo) {
            return isnan(hi.first) ? std::make_pair(std::numeric_limits<double>::quiet_NaN(), hi.second)
                                   : std::make_pair((hi.first - lo.first) / dr, "");
        });
    }
    return pv01;
}

std::vector<std::pair<string, portfolio_values_t>> compute_pv01_parallel(const std::vector<ppricer_t>& pricers, const Market& mkt, const FixingDataServer& fds) {
    std::vector<std::pair<string, portfolio_values_t>> pv01;  // Holds PV01 for each trade
    const double bump_size = 0.01 / 100;  // Bump size for interest rate

    auto base = mkt.get_risk_factors(ir_rate_prefix + "\\d+[DWMY].[A-Z]{3}");  // Filter IR related risk factors
    Market tmpmkt(mkt);  // Local copy of Market for applying bumps

    std::map<string, Market::vec_risk_factor_t> base_map;
    for (auto& b : base) {
        base_map[ir_rate_prefix + b.first.substr(b.first.length() - 3, 3)].push_back(b);
    }

    pv01.reserve(base_map.size());
    for (const auto& bm : base_map) {
        std::vector<std::pair<string, double>> bumped;
        pv01.emplace_back(bm.first, portfolio_values_t(pricers.size()));

        // Apply bumps and compute prices
        for (auto& b : bumped = bm.second) {
            b.second -= bump_size;  // bump down
        }
        tmpmkt.set_risk_factors(bumped);
        auto pv_dn = compute_prices(pricers, tmpmkt, fds);

        for (auto& b : bumped) {
            b.second += 2 * bump_size;  // offset down bump and apply up bump
        }
        tmpmkt.set_risk_factors(bumped);
        auto pv_up = compute_prices(pricers, tmpmkt, fds);

        // Restore original market state
        for (auto& b : bumped) {
            b.second = bm.second[&b - &bumped[0]].second;
        }
        tmpmkt.set_risk_factors(bumped);

        // Compute derivative estimator via central finite differences
        double dr = 2.0 * bump_size;
        std::transform(pv_up.begin(), pv_up.end(), pv_dn.begin(), pv01.back().second.begin(), [dr](const auto& hi, const auto& lo) {
            return isnan(hi.first) ? std::make_pair(std::numeric_limits<double>::quiet_NaN(), hi.second)
                                   : std::make_pair((hi.first - lo.first) / dr, "");
        });
    }
    return pv01;
}


std::vector<std::pair<string, portfolio_values_t>> compute_fx_delta(const std::vector<ppricer_t>& pricers, const Market& mkt, const FixingDataServer& fds) {
    std::vector<std::pair<string, portfolio_values_t>> pv01;  // Holds FX Delta for each trade
    const double bump_size = 0.1 / 100;  // Bump size for FX rates

    auto base = mkt.get_risk_factors(fx_spot_prefix + "[A-Z]{3}");  // Filter FX spot related risk factors
    Market tmpmkt(mkt);  // Local copy of Market for applying bumps

    pv01.reserve(base.size());
    for (const auto& d : base) {
        std::vector<std::pair<string, double>> bumped(1, d);
        pv01.emplace_back(d.first, portfolio_values_t(pricers.size()));

        // Apply bump down, compute prices
        bumped[0].second *= 1 - bump_size;
        tmpmkt.set_risk_factors(bumped);
        auto pv_dn = compute_prices(pricers, tmpmkt, fds);

        // Apply bump up, compute prices
        bumped[0].second = d.second * (1 + bump_size);
        tmpmkt.set_risk_factors(bumped);
        auto pv_up = compute_prices(pricers, tmpmkt, fds);

        // Restore original market state
        bumped[0].second = d.second;
        tmpmkt.set_risk_factors(bumped);

        // Compute derivative estimator via central finite differences
        double dr = 2.0 * bump_size * d.second;
        std::transform(pv_up.begin(), pv_up.end(), pv_dn.begin(), pv01.back().second.begin(), [dr](const auto& hi, const auto& lo) {
            return isnan(hi.first) ? std::make_pair(std::numeric_limits<double>::quiet_NaN(), hi.second)
                                   : std::make_pair((hi.first - lo.first) / dr, "");
        });
    }
    return pv01;
}

ptrade_t load_trade(my_ifstream& is)
{
    string name;
    ptrade_t p;

    // read trade identifier
    guid_t id;
    is >> id;

	if (id == TradePayment::m_id)
		p.reset(new TradePayment);
	else if (id == TradeFXForward::m_id)
		p.reset(new TradeFXForward);
	else
        THROW("Unknown trade type:" << id);

    p->load(is);

    return p;
}

void save_portfolio(const string& filename, const std::vector<ptrade_t>& portfolio)
{
    // test saving to file
    my_ofstream of(filename);
    for( const auto& pt : portfolio) {
        pt->save(of);
        of.endl();
    }
    of.close();
}

std::vector<ptrade_t> load_portfolio(const string& filename)
{
    std::vector<ptrade_t> portfolio;

    // test reloading the portfolio
    my_ifstream is(filename);
    while (is.read_line())
        portfolio.push_back(load_trade(is));

    return portfolio;
}

void print_price_vector(const string& name, const portfolio_values_t& values)
{
    std::cout
        << "========================\n"
        << name << ":\n"
        << "========================\n"
        << "Total: " << portfolio_total(values).first
		<< "\nErrors: " << portfolio_total(values).second.size()
        << "\n\n========================\n";

	for (size_t i = 0, n = values.size(); i < n; ++i) {
		if (std::isnan(values[i].first))
			std::cout << std::setw(5) << i << ": " << values[i].second << "\n";
		else
			std::cout << std::setw(5) << i << ": " << values[i].first << "\n";
	}

    std::cout << "========================\n\n";
}

} // namespace minirisk
