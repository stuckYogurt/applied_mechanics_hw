#include "time-keeping.h++"
#include <string>




namespace TimeKeeping {
    template<TimeScale Scale>
    Time<Scale> Time<Scale>::fromJD(double jd) noexcept {
        return Time(std::floor(jd), jd - std::floor(jd));
    }

    template<TimeScale Scale>
    Time<Scale> Time<Scale>::fromMJD(double mjd) noexcept {
        return Time(std::floor(mjd) + 2400000, mjd - std::floor(mjd) + 0.5);
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
    CalendarDate Time<Scale>::toCalendar() const {
        double jd = this->jd();

        // Shift JD to start at noon so the integer part is the Julian Day Number
        double jdShifted = jd + 0.5;
        double Z = std::floor(jdShifted);      // integer Julian day number
        double F = jdShifted - Z;              // fractional part of the day


        double alpha = std::floor((Z - 1867216.25) / 36524.25);
        double A = Z + 1.0 + alpha - std::floor(alpha / 4.0);


        double B = A + 1524.0;
        double C = std::floor((B - 122.1) / 365.25);
        double D = std::floor(365.25 * C);
        double E = std::floor((B - D) / 30.6001);

        // Day with fractional part
        double dayWithFraction = B - D - std::floor(30.6001 * E) + F;
        int day = static_cast<int>(std::floor(dayWithFraction));

        int month = (E < 14.0) ? static_cast<int>(E - 1.0)
                               : static_cast<int>(E - 13.0);
        int year  = (month > 2) ? static_cast<int>(C - 4716.0)
                                : static_cast<int>(C - 4715.0);

        // Convert fractional day to h:m:s
        double frac = dayWithFraction - day;   // [0, 1)
        double totalSeconds = frac * 86400.0;

        int hour   = static_cast<int>(totalSeconds / 3600.0);
        int minute = static_cast<int>((totalSeconds - hour * 3600.0) / 60.0);
        double second = totalSeconds - hour * 3600.0 - minute * 60.0;

        // Guard against floating-point artefacts like 59.9999999999 -> roll over
        if (second >= 60.0 - 1e-9) {
            second = 0.0;
            if (++minute == 60) { minute = 0; if (++hour == 24) hour = 0; }
        }

        return { year, month, day, hour, minute, second };
    }


    template<TimeScale Scale>
    double Time<Scale>::jdInt()     const noexcept {return jdInt_;}

    template<TimeScale Scale>
    double Time<Scale>::jdFrac()    const noexcept {return jdFrac_;}

    template<TimeScale Scale>
    double Time<Scale>::jd()        const noexcept {return jdInt_ + jdFrac_; }

    template<TimeScale Scale>
    double Time<Scale>::mjd()       const noexcept {
        return (jdInt() - 2400000) + (jdFrac() - 0.5);
    }


    #include "../lib/sofa/include/sofa.h"

    template<typename DutContainer>
    template<TimeScale To, TimeScale From>
    Time<To> TimeConverter<DutContainer>::convert(const Time<From> &from) const {
        auto jd1 = static_cast<double>(from.jdInt());
        double jd2 = from.jdFrac();

        // 1. Handle identity conversion immediately
        if constexpr (To == From) {
            return Time<To>(from.jdInt(), from.jdFrac());
        }

        double tt1 = 0.0, tt2 = 0.0;

        // 2. Convert From -> TT (Canonical Intermediate)
        if constexpr (From == TimeScale::TT) {
            tt1 = jd1;
            tt2 = jd2;
        }
        else if constexpr (From == TimeScale::TAI) {
            iauTaitt(jd1, jd2, &tt1, &tt2);
        }
        else if constexpr (From == TimeScale::TCG) {
            iauTcgtt(jd1, jd2, &tt1, &tt2);
        }
        else if constexpr (From == TimeScale::TDB) {
            double dtr = iauDtdb(jd1, jd2, 0.0, 0.0, 0.0, 0.0);
            iauTdbtt(jd1, jd2, dtr, &tt1, &tt2);
        }
        else if constexpr (From == TimeScale::TCB) {
            double tdb1, tdb2;
            iauTcbtdb(jd1, jd2, &tdb1, &tdb2);
            double dtr = iauDtdb(tdb1, tdb2, 0.0, 0.0, 0.0, 0.0);
            iauTdbtt(tdb1, tdb2, dtr, &tt1, &tt2);
        }
        else if constexpr (From == TimeScale::UTC) {
            double tai1, tai2;
            iauUtctai(jd1, jd2, &tai1, &tai2);
            iauTaitt(tai1, tai2, &tt1, &tt2);
        }
        else if constexpr (From == TimeScale::UT1) {
            double dut1_sec = dut(from.jd()) * 86400.0;
            double utc1, utc2;
            iauUt1utc(jd1, jd2, dut1_sec, &utc1, &utc2);
            double tai1, tai2;
            iauUtctai(utc1, utc2, &tai1, &tai2);
            iauTaitt(tai1, tai2, &tt1, &tt2);
        }
        else {
            static_assert(sizeof(From) == 0, "Unsupported From TimeScale");
        }

        // 3. Convert TT -> To
        if constexpr (To == TimeScale::TT) {
            return Time<To>(tt1, tt2);
        }
        else if constexpr (To == TimeScale::TAI) {
            double tai1, tai2;
            iauTttai(tt1, tt2, &tai1, &tai2);
            return Time<To>(tai1, tai2);
        }
        else if constexpr (To == TimeScale::TCG) {
            double tcg1, tcg2;
            iauTttcg(tt1, tt2, &tcg1, &tcg2);
            return Time<To>(tcg1, tcg2);
        }
        else if constexpr (To == TimeScale::TDB) {
            double tdb1, tdb2;
            double dtr = iauDtdb(tt1, tt2, 0.0, 0.0, 0.0, 0.0);
            iauTttdb(tt1, tt2, dtr, &tdb1, &tdb2);
            return Time<To>(tdb1, tdb2);
        }
        else if constexpr (To == TimeScale::TCB) {
            double tdb1, tdb2;
            double dtr = iauDtdb(tt1, tt2, 0.0, 0.0, 0.0, 0.0);
            iauTttdb(tt1, tt2, dtr, &tdb1, &tdb2);
            double tcb1, tcb2;
            iauTdbtcb(tdb1, tdb2, &tcb1, &tcb2);
            return Time<To>(tcb1, tcb2);
        }
        else if constexpr (To == TimeScale::UTC) {
            double tai1, tai2;
            iauTttai(tt1, tt2, &tai1, &tai2);
            double utc1, utc2;
            iauTaiutc(tai1, tai2, &utc1, &utc2);
            return Time<To>(utc1, utc2);
        }
        else if constexpr (To == TimeScale::UT1) {
            double tai1, tai2;
            iauTttai(tt1, tt2, &tai1, &tai2);
            double utc1, utc2;
            iauTaiutc(tai1, tai2, &utc1, &utc2);
            double dut1_sec = dutContainer_.dut(utc1 + utc2) * 86400.0;
            double ut11, ut12;
            iauUtcut1(utc1, utc2, dut1_sec, &ut11, &ut12);
            return Time<To>(ut11, ut12);
        }
        else {
            static_assert(sizeof(To) == 0, "Unsupported To TimeScale");
        }
    }


}

// returns in days fraction
template<TimeKeeping::TimeScale Scale>
double operator-(const TimeKeeping::Time<Scale>& l, const TimeKeeping::Time<Scale>& r) noexcept {
    return (l.jdInt() - r.jdInt()) + (l.jdFrac() - r.jdFrac());
}

template<TimeKeeping::TimeScale Scale>
TimeKeeping::Time<Scale> operator+(double secs, const TimeKeeping::Time<Scale>& r) noexcept {
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

template<TimeKeeping::TimeScale Scale>
TimeKeeping::Time<Scale> operator+(const TimeKeeping::Time<Scale>& l, double secs) noexcept {
    return secs + l;
}

template<TimeKeeping::TimeScale Scale>
TimeKeeping::Time<Scale> operator-(const TimeKeeping::Time<Scale>& l, double secs) noexcept {
    return -secs + l;
}

template<TimeKeeping::TimeScale Scale>
std::ostream& operator<<(std::ostream& os, const TimeKeeping::Time<Scale>& time) noexcept {
    return os << time.toString();
}

template<TimeKeeping::TimeScale Scale>
std::string TimeKeeping::Time<Scale>::toString() const noexcept {
    auto calendar = this->toCalendar();
    return  std::to_string(calendar.year) + "y-" + \
            std::to_string(calendar.month) + "m-" + \
            std::to_string(calendar.day) + "d-" + \
            std::to_string(calendar.hour) + "h-" + \
            std::to_string(calendar.min) + "m-" + \
            std::to_string(calendar.sec) + "s";
    ;
}