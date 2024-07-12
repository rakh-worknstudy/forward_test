#include "MathDriving.h"
#include <cstdlib>
#include <cmath>

//  ----------------------------------------------------------------

world::car::Car::Car(void) : position(0),
                 angle(0) {
}
world::car::Car::~Car(void) {}


//  ----------------------------------------------------------------
static inline unsigned rand_fragment_length(void) {
    return world::road::fragment::MIN_LENGTH + (unsigned) std::rand() %
        (world::road::fragment::MAX_LENGTH - world::road::fragment::MIN_LENGTH);
}
static inline uint8_t road_fragment_type_swap(uint8_t current_fragment_type) {
    if (current_fragment_type == world::road::fragment::type_e::solid)
        return world::road::fragment::type_e::dash;
    return world::road::fragment::type_e::solid;
}

void world::road::Road::RandFragments(void) {
    unsigned road_length_left = world::road::LENGTH;
    unsigned current_length = 0;
    uint8_t current_type = std::rand() % world::road::fragment::type_e::count;

    while (road_length_left != 0) {
        current_length = rand_fragment_length();
        if (current_length > road_length_left) {
            current_length = road_length_left;
            current_type = Fragment[0].second;
        } else {
            current_type = road_fragment_type_swap(current_type);
        }
        Fragment.push_back(std::make_pair(current_length, current_type));
        road_length_left -= current_length;
    }
}

world::road::Road::Road(void) {
    RandFragments();
    CurrentFragment = Fragment.begin();
}
world::road::Road::~Road(void) {}

//  ----------------------------------------------------------------

world::World::World(void) : Road(), Car() {
    carState = car::state::NONE;
}
world::World::~World(void) {
    Road.~Road();
    Car.~Car();
}

uint8_t world::World::Drive(unsigned int message) {
    CarStateCheck();
    return carState;
}

static inline uint8_t car_check_cross_solid(void) {
    if (true)
        return world::car::state::CROSS_SOLID;
    return world::car::state::NONE;
}

static inline uint8_t car_check_wrong_direction(void) {
    if (true)
        return world::car::state::WRONG_DIRECTION;
    return world::car::state::NONE;
}

void world::World::CarStateCheck(void) {
    carState = car_check_wrong_direction();
}