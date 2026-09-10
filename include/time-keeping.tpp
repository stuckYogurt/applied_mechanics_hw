#include "time-keeping.h++"
#include <string>

using namespace TimeKeeping;
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
Time<Scale> Time<Scale>::fromCalendar(int year, int month, int day,
                                      int hour, int minute, double second) {
    if (month <= 2) {
        year--;
        month += 12;
    }

    double A = std::floor(year / 100.0);
    double B = 2.0 - A + std::floor(A / 4.0);

    double jd = std::floor(365.25 * (year + 4716))
              + std::floor(30.6001 * (month + 1))
              + day + B - 1524.5;

    double dayFraction = (hour + minute / 60.0 + second / 3600.0) / 24.0;
    jd += dayFraction;

    return Time<Scale>::fromJD(jd);
}


template<TimeScale Scale>
double Time<Scale>::jdInt()     const noexcept {return jdInt_;}

template<TimeScale Scale>
double Time<Scale>::jdFrac()    const noexcept {return jdFrac_;}

template<TimeScale Scale>
double Time<Scale>::jd()        const noexcept {return jdInt_ + jdFrac_; }

template<TimeScale Scale>
double Time<Scale>::mjd()       const noexcept {return jd() - 2400000.5;}


#include "../lib/sofa/include/sofa.h"

template<typename DutContainer>
template<TimeScale To, TimeScale From>
Time<To> TimeConverter<DutContainer>::convert(const Time<From> &from) const {

    auto jd1 = static_cast<double>(from.jdInt());
    double jd2 = from.jdFrac();

    if constexpr (To == From) {
        return Time<To>(from.jdInt(), from.jdFrac());
    }

    // --- UTC <-> UT1 ---
    else if constexpr (To == TimeScale::UT1 && From == TimeScale::UTC) {
        double ut11, ut12;
        double dut1_sec = dut(from.jd()) * 86400.0;
        iauUtcut1(jd1, jd2, dut1_sec, &ut11, &ut12);
        return Time<To>(ut11, ut12);
    }
    else if constexpr (To == TimeScale::UTC && From == TimeScale::UT1) {
        double utc1, utc2;
        double dut1_sec = dut(from.jd()) * 86400.0;
        iauUt1utc(jd1, jd2, dut1_sec, &utc1, &utc2);
        return Time<To>(utc1, utc2);
    }

    // --- UTC <-> TAI ---
    else if constexpr (To == TimeScale::TAI && From == TimeScale::UTC) {
        double tai1, tai2;
        iauUtctai(jd1, jd2, &tai1, &tai2);
        return Time<To>(tai1, tai2);
    }
    else if constexpr (To == TimeScale::UTC && From == TimeScale::TAI) {
        double utc1, utc2;
        iauTaiutc(jd1, jd2, &utc1, &utc2);
        return Time<To>(utc1, utc2);
    }

    // --- TAI <-> TT ---
    else if constexpr (To == TimeScale::TT && From == TimeScale::TAI) {
        double tt1, tt2;
        iauTaitt(jd1, jd2, &tt1, &tt2);
        return Time<To>(tt1, tt2);
    }
    else if constexpr (To == TimeScale::TAI && From == TimeScale::TT) {
        double tai1, tai2;
        iauTttai(jd1, jd2, &tai1, &tai2);
        return Time<To>(tai1, tai2);
    }

    // --- TT <-> TCG ---
    else if constexpr (To == TimeScale::TCG && From == TimeScale::TT) {
        double tcg1, tcg2;
        iauTttcg(jd1, jd2, &tcg1, &tcg2);
        return Time<To>(tcg1, tcg2);
    }
    else if constexpr (To == TimeScale::TT && From == TimeScale::TCG) {
        double tt1, tt2;
        iauTcgtt(jd1, jd2, &tt1, &tt2);
        return Time<To>(tt1, tt2);
    }

    // --- TT <-> TDB ---
    else if constexpr (To == TimeScale::TDB && From == TimeScale::TT) {
        double tdb1, tdb2;
        double dtr = iauDtdb(jd1, jd2, 0.0, 0.0, 0.0, 0.0);
        iauTttdb(jd1, jd2, dtr, &tdb1, &tdb2);
        return Time<To>(tdb1, tdb2);
    }
    else if constexpr (To == TimeScale::TT && From == TimeScale::TDB) {
        double tt1, tt2;
        double dtr = iauDtdb(jd1, jd2, 0.0, 0.0, 0.0, 0.0);
        iauTdbtt(jd1, jd2, dtr, &tt1, &tt2);
        return Time<To>(tt1, tt2);
    }

    // --- TDB <-> TCB ---
    else if constexpr (To == TimeScale::TCB && From == TimeScale::TDB) {
        double tcb1, tcb2;
        iauTdbtcb(jd1, jd2, &tcb1, &tcb2);
        return Time<To>(tcb1, tcb2);
    }
    else if constexpr (To == TimeScale::TDB && From == TimeScale::TCB) {
        double tdb1, tdb2;
        iauTcbtdb(jd1, jd2, &tdb1, &tdb2);
        return Time<To>(tdb1, tdb2);
    }

    // non-direct, going through the graph
    else {
        auto tt_time = this->template convert<TimeScale::TT, From>(from);
        return this->template convert<To, TimeScale::TT>(tt_time);
    }
}

// returns in days fraction
template<TimeScale Scale>
double operator-(const Time<Scale>& l, const Time<Scale>& r) noexcept {
    return (l.jdInt() - r.jdInt()) + (l.jdFrac() - r.jdFrac());
}

template<TimeScale Scale>
Time<Scale> operator+(double secs, const Time<Scale>& r) noexcept {
    double intDays;
    double fracDays = std::modf(secs / 86400.0, &intDays);

    double newFrac = r.jdFrac() + fracDays;

    if (newFrac >= 1.0) {
        newFrac -= 1.0;
        intDays += 1.0;
    } else if (newFrac < 0.0) {
        newFrac += 1.0;
        intDays -= 1.0;
    }

    return Time<Scale>(r.jdInt() + intDays, newFrac);
}

template<TimeScale Scale>
Time<Scale> operator+(const Time<Scale>& l, double secs) noexcept {
    return secs + l;
}

template<TimeScale Scale>
Time<Scale> operator-(const Time<Scale>& l, double secs) noexcept {
    return -secs + l;
}

template<TimeKeeping::TimeScale Scale>
std::ostream& operator<<(std::ostream& os, const TimeKeeping::Time<Scale>& time) noexcept {
    return os << time.toString();
}

template<TimeKeeping::TimeScale Scale>
std::string TimeKeeping::Time<Scale>::toString() const noexcept {
    return std::to_string(jdInt()) + "d-" + std::to_string(jdFrac());
}
