#include <iostream>

#include "include/time-keeping.h++"

int main() {
    TimeKeeping::Time<TimeKeeping::TimeScale::TT> curr_jd(2461293);

    auto jesus = TimeKeeping::Time<TimeKeeping::TimeScale::TT>::fromCalendar(0, 0,0);

    std::cout << jesus - curr_jd << std::endl;
    return 0;
}
