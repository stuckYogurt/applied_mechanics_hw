#include "../include/time-keeping.h++"
#include "cmath"

template<TimeScale Scale>
Time<Scale> Time<Scale>::fromJD(double jd) noexcept {
    return Time(std::floor(jd), jd - std::floor(jd));
}

template<TimeScale Scale>
Time<Scale> Time<Scale>::fromMJD(double mjd) noexcept {
    double jd = mjd + 2400000.5;
    return fromJD(jd);
}

template<TimeScale Scale>
Time<Scale> Time<Scale>::fromCalendar(int year, int month, int day, int hour, int minute, double second) {
    if (month <= 2) {
        year--;
        month += 12;
    }

    // Gregorian adjustment
    double A = (int)(year / 100.0);
    double B = 2.0 - A + (int)(A / 4.0);

    double jd = (int)(365.25 * (year + 4716)) +
                (int)(30.6001 * (month + 1)) +
                day + B - 1524.5;

    double dayFraction = (hour + minute / 60.0 + second / 3600.0) / 24.0;
    jd += dayFraction;

    return fromJD(jd);
}


template<TimeScale Scale>
double Time<Scale>::jdInt()     const noexcept {return jdInt_;}

template<TimeScale Scale>
double Time<Scale>::jdFrac()    const noexcept {return jdFrac_;}

template<TimeScale Scale>
double Time<Scale>::jd()        const noexcept {return jdInt_ + jdFrac_; }

template<TimeScale Scale>
double Time<Scale>::mjd()       const noexcept {return jd() - 2400000.5;}


template<typename DutContainer>
template<TimeScale To, TimeScale From>
Time<To> TimeConverter<DutContainer>::convert(const Time<From> &from) const {
    if constexpr (To == TimeScale::UT1 && From == TimeScale::UTC) {
        return Time<To>(from.jdInt(), from.jdFrac() + dut(from.jd()));
    }
    if constexpr (To == TimeScale::UTC && From == TimeScale::UT1) {
        return Time<To>(from.jdInt(), from.jdFrac() - dut(from.jd()));
    }

}
