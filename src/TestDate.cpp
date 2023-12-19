#include <iomanip>
#include <iostream>
#include <cstdlib>
#include "Date.h"
using namespace minirisk;

// write an isValidDate function for test1 to use
bool isValidDate(int y, int m, int d) {
    if (y > Date::last_year || y < Date::first_year || m < 1 || m > 12) 
        return false;

    int daysInMonth = (m == 2) ? (28 + Date::is_leap_year(y)) : 31 - (m - 1) % 7 % 2;

    return (d >= 1 && d <= daysInMonth);
}

// Creating an instance of an invalid date should trigger an error.
// This is validated with 1000 intentionally generated invalid dates.
void test1()
{
    int counter = 0;
    int failures = 0;
    srand(static_cast<unsigned int>(time(NULL)));
    
    while (counter < 1000) {
        unsigned seed = rand();
        srand(seed);

        unsigned y = rand() % 500 + 1800; // Year between 1800 and 2299
        unsigned m = rand() % 20;         // Month between 0 and 19
        unsigned d = rand() % 40;         // Day between 0 and 39
        
        if (!isValidDate(y, m, d)) { 
            counter++; 
            try {
                Date(y, m, d);
                // If the date is invalid but no exception is thrown, record failure
                std::cout << "Invalid date " << d << "-" << m << "-" << y << " is not captured. Seed: " << seed << std::endl;
                failures++;
            }
            catch (const std::invalid_argument&) {
                // Correctly caught invalid date, do nothing
            }
        }
    }
    
    std::cout << "Test1 completed " << ((failures == 0) ? "successfully" : "unsuccessfully") << " with " << failures << " exceptions found!" << std::endl;
}


// Ensure that converting a date from calendar format (day, month, year) to serial format,
// and then back to calendar format, results in the original date. This verification loop
// spans the entire range from 1-Jan-1900 to 31-Dec-2199.
void test2()
{
    int failures = 0;
    const std::array<unsigned, 12> days_in_month = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const std::array<unsigned, 12> days_in_month_leap = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    for (unsigned y = 1900; y < 2200; ++y) {
        for (unsigned m = 1; m <= 12; ++m) {
            unsigned dmax = (Date::is_leap_year(y) ? days_in_month_leap[m - 1] : days_in_month[m - 1]);
            for (unsigned d = 1; d <= dmax; ++d) {
                Date ds(y, m, d);   
                std::string ds_str = ds.to_string(); 
                std::string cal = std::to_string(d) + '-' + std::to_string(m) + '-' + std::to_string(y);

                if (cal != ds_str) {
                    failures++;
                }
            }
        }
    }
    std::cout << "Test2 completed " << ((failures == 0) ? "successfully" : "unsuccessfully") << " with " << failures << " failures." << std::endl;
}


// Confirm that the serial numbers generated for two consecutive dates are contiguous.
// For example, 31-Jan-2012 and 1-Feb-2012 are contiguous dates, so the serial numbers
// they produce should only differ by 1. This validation loop covers the entire range
// from 1-Jan-1900 to 31-Dec-2199.
void test3()
{
    int failures = 0;
    const std::array<unsigned, 12> days_in_month = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const std::array<unsigned, 12> days_in_month_leap = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    for (unsigned y = 1900; y < 2200; ++y) {
        for (unsigned m = 1; m <= 12; ++m) {
            unsigned dmax = Date::is_leap_year(y) ? days_in_month_leap[m - 1] : days_in_month[m - 1];
            for (unsigned d = 1; d <= dmax; ++d) {
                Date d1(y, m, d);

                // Calculate next day
                unsigned y_n = (d == dmax && m == 12) ? y + 1 : y;
                unsigned m_n = (d == dmax) ? (m == 12 ? 1 : m + 1) : m;
                unsigned d_n = (d == dmax) ? 1 : d + 1;
                Date d2(y_n, m_n, d_n);

                if (d2 - d1 != 1) {
                    failures++;
                }

                if (y == 2199 && m == 12 && d == 31) {
                    std::cout << "Test3 completed " << ((failures == 0) ? "successfully" : "unsuccessfully") << " with " << failures << " failures." << std::endl;
                    return;
                }
            }
        }
    }
}

int main()
{
    test1();
    test2();
    test3();
    return 0;
}

