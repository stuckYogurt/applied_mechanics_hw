#include <iostream>
#include "map"

#include "time-keeping.h++"

class MapDUT {
public:
    std::map<TimeKeeping::Time<TimeKeeping::TimeScale::TT>, double> container;
    MapDUT() {std::cout << "MapDUT" << std::endl;};
    double dut(double jd) {
        return 0.01;
    }

};


int main() {
    TimeKeeping::Time<TimeKeeping::TimeScale::UTC> curr_jd(2461293);

    auto jesus = TimeKeeping::Time<TimeKeeping::TimeScale::UTC>::fromCalendar(0, 0,0);

    std::cout << jesus - curr_jd << std::endl;

    auto dutta = MapDUT();
    auto converter = TimeKeeping::TimeConverter<MapDUT>(dutta);

    auto TT_t = converter.convert<TimeKeeping::TimeScale::TT>(curr_jd);

    std::cout << "TT: " << TT_t << "; UTC: " << curr_jd << std::endl;

    return 0;
}
