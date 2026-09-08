
#ifndef APPLIED_MECHS_TIME_KEEPING_H
#define APPLIED_MECHS_TIME_KEEPING_H

#include "vector"

namespace TimeKeeping {
    enum class TimeScale {
        TAI, UTC, UT1, TT, TCG, TCB, TDB
    };

    template<TimeScale Scale>
    class Time {
        double jdInt_;
        double jdFrac_;
    public:
        explicit Time(double jdInt = 0, double jdFrac = 0) noexcept: jdInt_(jdInt), jdFrac_(jdFrac) {
            if (jdFrac > 1) {
                auto whole_part = std::floor(jdFrac_);
                jdInt_ += whole_part;
                jdFrac_ -= whole_part;
            }
        };

        Time static fromJD(double jd) noexcept;

        Time static fromMJD(double mjd) noexcept;

        Time static fromCalendar(int year, int month, int day,
                                 int hour = 0, int minute = 0, double second = 0.0);

        [[nodiscard]] double jdInt() const noexcept;

        [[nodiscard]] double jdFrac() const noexcept;

        [[nodiscard]] double jd() const noexcept;

        [[nodiscard]] double mjd() const noexcept;

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

#endif //APPLIED_MECHS_TIME_KEEPING_H
