
#ifndef APPLIED_MECHS_TIME_KEEPING_H
#define APPLIED_MECHS_TIME_KEEPING_H

#include "ostream"
#include <string>
#include <cmath>

#include <utils.h++>

namespace TimeKeeping {
    enum class TimeScale {
        TAI, UTC, UT1, TT, TCG, TCB, TDB
    };

    struct CalendarDate {
        int year;
        int month;
        int day;
        int hour;
        int min;
        double sec;
    };

    template<TimeScale Scale>
    class Time {
        double jdInt_;
        double jdFrac_;
    public:
        explicit Time(double jdInt = 0, double jdFrac = 0) noexcept: jdInt_(0), jdFrac_(0) {
            
            if (jdFrac >= 1) {
                LOG_WARNING("WARNING: jdFrac >= 1 at Time(). Integer part is to be ignored");
                jdFrac -= std::trunc(jdFrac);
            }
            if (isFractional(jdInt)) {
                LOG_WARNING("WARNING: Fractional jdInt at Time(). Fractional part is to be ignored");
                jdInt = std::trunc(jdInt);
            }

            jdInt_ = jdInt;
            jdFrac_ = jdFrac;

        };

        Time static fromJD(double jd) noexcept;

        Time static fromMJD(double mjd) noexcept;

        Time static fromCalendar(int year, int month, int day,
                                 int hour = 0, int minute = 0, double second = 0.0);

        [[nodiscard]] double jdInt() const noexcept;

        [[nodiscard]] double jdFrac() const noexcept;

        [[nodiscard]] double jd() const noexcept;

        [[nodiscard]] double mjd() const noexcept;

        [[nodiscard]] std::string toString() const noexcept;

        [[nodiscard]] CalendarDate toCalendar() const;

        auto operator<=>(const Time &rhs) const noexcept = default;

    };


    template<typename DutContainer>
    class TimeConverter {
        DutContainer dutContainer_;
    public:
        explicit TimeConverter(const DutContainer &dutContainer) : dutContainer_(dutContainer) {};

        template<TimeScale To, TimeScale From>
        Time<To> convert(const Time<From> &from) const;

    };




}

template<TimeKeeping::TimeScale Scale>
double operator-(const TimeKeeping::Time<Scale>& l, const TimeKeeping::Time<Scale>& r) noexcept;

template<TimeKeeping::TimeScale Scale>
TimeKeeping::Time<Scale> operator+(double secs, const TimeKeeping::Time<Scale>& r) noexcept;

template<TimeKeeping::TimeScale Scale>
TimeKeeping::Time<Scale> operator+(const TimeKeeping::Time<Scale>& l, double secs) noexcept;

template<TimeKeeping::TimeScale Scale>
TimeKeeping::Time<Scale> operator-(const TimeKeeping::Time<Scale>& l, double secs) noexcept;

template<TimeKeeping::TimeScale Scale>
std::ostream& operator<<(std::ostream& os, const TimeKeeping::Time<Scale>& time) noexcept;

namespace TimeKeeping {
    ;
}

#include "time-keeping.tpp"

#endif //APPLIED_MECHS_TIME_KEEPING_H
