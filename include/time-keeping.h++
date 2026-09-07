//
// Created by  Владимир Малахов on 08.09.2026.
//

#ifndef APPLIED_MECHS_TIME_KEEPING_H
#define APPLIED_MECHS_TIME_KEEPING_H

enum class TimeScale {
    UTC, UT1
};

template<TimeScale Scale>
class Time {
    double jdInt;
    double jdFrac;
public:
    Time(double jdInt = 0, double jdFrac = 0) noexcept: jdInt(jdInt), jdFrac(jdFrac) {};

    Time static fromJD      (double jd)     noexcept;
    Time static fromMJD     (double mjd)    noexcept;
    Time static fromCalendar(int year, ...)         ;

    
};

#endif //APPLIED_MECHS_TIME_KEEPING_H
