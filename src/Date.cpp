#include <iomanip>
#include <iostream>
#include "Date.h"
#include <algorithm>
#include <array>

namespace minirisk {

struct DateInitializer : std::array<unsigned, Date::n_years>
{
	DateInitializer()
	{
        for (unsigned i = 0, s = 0, y = Date::first_year; i < size(); ++i, ++y) {
            (*this)[i] = s;
            s += 365 + (Date::is_leap_year(y) ? 1 : 0);
        }
	}
};

const std::array<unsigned, 12> Date::days_in_month = { {31,28,31,30,31,30,31,31,30,31,30,31} };
const std::array<unsigned, 12> Date::days_ytd = { {0,31,59,90,120,151,181,212,243,273,304,334} };
const std::array<unsigned, Date::n_years> Date::days_epoch(static_cast<const std::array<unsigned, Date::n_years>&>(DateInitializer()));

/* The function checks if a given year is a leap year.
    Leap year must be a multiple of 4, but it cannot be a multiple of 100 without also being a multiple of 400.
*/
bool Date::is_leap_year(unsigned year)
{
    return ((year % 4 != 0) ? false : (year % 100 != 0) ? true : (year % 400 != 0) ? false : true);
}

// The function pads a zero before the month or day if it has only one digit.
std::string Date::padding_dates(unsigned month_or_day)
{
    std::ostringstream os;
    os << std::setw(2) << std::setfill('0') << month_or_day;
    return os.str();
}

void Date::check_valid(unsigned y, unsigned m, unsigned d)
{
	MYASSERT(y >= first_year, "The year must be no earlier than year " << first_year << ", got " << y << ".");
	MYASSERT(y < last_year, "The year must be smaller than year " << last_year << ", got " << y << ".");
    MYASSERT(m >= 1 && m <= 12, "The month must be a integer between 1 and 12, got " << m << ".");
    unsigned dmax = days_in_month[m - 1] + ((m == 2 && is_leap_year(y)) ? 1 : 0);
    MYASSERT(d >= 1 && d <= dmax, "The day must be a integer between 1 and " << dmax << ", got " << d << ".");
}
void Date::check_valid(unsigned s) {
	MYASSERT(s >= 0 && s <= 109572, "The serial number should be an integer ranging from 0 (1-Jan-1900) to 109572 (31-Dec-2199), but the value received is " << s << ".");
}

unsigned Date::day_of_year(unsigned y, unsigned m, unsigned d) const
{
    return days_ytd[m - 1] + ((m > 2 && is_leap_year(y)) ? 1 : 0) + (d - 1);
}

std::array<unsigned,3> Date::get_date_string(unsigned s) const{
	std::array<unsigned,3> datetime; // datetime[0] represents the day, datetime[1] represents the month, and datetime[2] represents the year.
	unsigned y = std::upper_bound(days_epoch.begin(), days_epoch.end(), s) - days_epoch.begin() - 1;
	datetime[2] = first_year + y;
	s -= days_epoch[y];
	bool flag = false;
	if (is_leap_year(datetime[2]) && s > 58) {
		s--;
		flag = true;
	}
	unsigned m = std::upper_bound(days_ytd.begin(), days_ytd.end(), s) - days_ytd.begin() - 1;
	datetime[1] = 1 + m;
	datetime[0] = 1 + s - days_ytd[m];
	if (m == 1 && flag) datetime[0] += 1;
	return datetime;
}

//  The function calculates the distance between two Dates. d1 > d2 is allowed, which returns the negative of d2-d1.
long operator-(const Date& d1, const Date& d2)
{
    unsigned s1 = d1.m_serial;
    unsigned s2 = d2.m_serial;
    return static_cast<long>(s1) - static_cast<long>(s2);
}

} // namespace minirisk

