#include "MathDriving.h"
#include "WindowKeyState.h"

#include <cstdlib>
#include <cmath>

namespace world {
//  ----------------------------------------------------------------
//                           CAR CLASS
//  ----------------------------------------------------------------
    Car::Car(void) {
        length = CAR_LENGTH_BASE;
        width = CAR_WIDTH_BASE;
        x_pos = CAR_X_POS_BASE;
        y_pos = CAR_Y_POS_BASE;
        y_times_last = 0;
        angle = CAR_ANGLE_PHASE + CAR_ANGLE_BASE;
        sinLast = std::sinf(angle);
        cosLast = std::cosf(angle);
        speed = CAR_SPEED_BASE;
        position_changed = false;
    }
    Car::~Car(void) {}

    float Car::GetLength(void) const {
        return length;
    }
    float Car::GetWidth(void) const {
        return width;
    }

    float Car::GetX(void) const {
        return x_pos;
    }
    float Car::GetY(void) const {
        return y_pos;
    }

    float Car::GetAngle(void) const {
        return angle - CAR_ANGLE_PHASE;
    }

    //  Пришлось жертвовать формулами трения-скольжения для момента
    //  вращения и рывка, чтобы получить первичный готовый вариант

    static inline float get_rear_acceleration(float speed) {
        static constexpr float R_ACCELERATION_MAX = 0.005f;
        static constexpr float R_ACCELERATION_LOSS = 0.001f;

        float acceleration = R_ACCELERATION_MAX;
        if (speed > 0.0f) {
            acceleration -= (speed * R_ACCELERATION_LOSS);
            if (acceleration < 0.0f)
                acceleration = 0.0f;
        }
        return acceleration;
    }

    static inline float get_forward_acceleration(float speed) {
        static constexpr float F_ACCELERATION_MAX = 0.005f;
        static constexpr float F_ACCELERATION_LOSS = 0.001f;

        float acceleration = F_ACCELERATION_MAX;
        if (speed > 0.0f) {
            acceleration -= (speed * F_ACCELERATION_LOSS);
            if (acceleration < 0.0f)
                acceleration = 0.0f;
        }
        return acceleration;
    }

    void Car::Drive(unsigned message, unsigned ms_elapsed) {
        position_changed = false;
        static constexpr unsigned TICK_MS = 20;
        static unsigned ms_missed = 0;

        if ((message & windowKeyState::UP) && (message & windowKeyState::DOWN))
            message ^ (windowKeyState::UP | windowKeyState::DOWN);

        if ((message & windowKeyState::LEFT) && (message & windowKeyState::RIGHT))
            message ^ (windowKeyState::LEFT | windowKeyState::RIGHT);

        ms_missed += ms_elapsed;
        while (ms_missed >= TICK_MS) {
            if (message & windowKeyState::UP)
                SpeedUp();
            else if (message & windowKeyState::DOWN)
                SpeedDown();
            else
                SpeedLoss();

            Move(message);

            ms_missed -= TICK_MS;
        }
    }

    void Car::SpeedUp(void) {
        float acceleration = get_forward_acceleration(speed);
        speed += acceleration;
    }
    void Car::SpeedDown(void) {
        float acceleration = get_rear_acceleration(speed);
        speed -= acceleration;
    }
    void Car::SpeedLoss(void) {
        static constexpr float SPEED_LOSS = 0.002f;
        if (speed > SPEED_LOSS)         //  Positive and Big
            speed -= SPEED_LOSS;
        else if (speed < -SPEED_LOSS)   //  Negative and Big
            speed += SPEED_LOSS;
        else                            //  Any and Small
            speed = 0.0f;
    }

    void Car::Move(unsigned message) {
        y_times_last = 0;
        if (speed == 0.0f)
            return;

        if (message & windowKeyState::LEFT) {
            Turn(CAR_TURN_RATE);
        } else if (message & windowKeyState::RIGHT) {
            Turn(-CAR_TURN_RATE);
        } else {
            Turn(0.0f);
        }

        while (y_pos > 1.0f) {
            y_pos -= 1.0f;
            ++y_times_last;
        }
        while (y_pos < -1.0f) {
            y_pos += 1.0f;
            --y_times_last;
        }

        position_changed = true;
    }

    static inline float get_new_angle(float angle, float delta) {
        angle += delta;
        if (angle < 0.0f)
            angle += WORLD_M_2PI;
        else if (angle > WORLD_M_2PI)
            angle -= WORLD_M_2PI;
        return angle;
    }

    void Car::Turn(float turn_rate) {
        if (turn_rate == 0.0f) {
            y_pos += speed * sinLast;
            x_pos += speed * cosLast;
        } else {
            float radius = speed / turn_rate;
            if (radius < 0.0f)
                radius = -radius;

            float angle_new;
            if (speed > 0.0f)
                angle_new = get_new_angle(angle, turn_rate);
            else
                angle_new = get_new_angle(angle, -turn_rate);

            float sin_new = std::sinf(angle_new);
            float cos_new = std::cosf(angle_new);

            y_pos += (radius * (sin_new - sinLast));
            x_pos += (radius * (cos_new - cosLast));

            sinLast = sin_new;
            cosLast = cos_new;
            angle = angle_new;
        }
    }

    void Car::Bump(float x_lim) {
        speed = 0.0f;
        x_pos = x_lim;
    }

    bool Car::StateChanged(void) const {
        return position_changed;
    }

    int Car::GetYTimesLast(void) const {
        return y_times_last;
    }

//  ----------------------------------------------------------------
//                           ROAD CLASS
//  ----------------------------------------------------------------
    static inline unsigned rand_from_to(unsigned from, unsigned to) {
        if (from > to) {
            unsigned temp = from;
            from = to;
            to = temp;
        }
        return from + (unsigned)std::rand() % (to - from);
    }

    static inline void swap_delim(uint8_t& delim_type) {
        if (delim_type == DELIM_TYPE_SOLID)
            delim_type = DELIM_TYPE_DASH;
        else
            delim_type = DELIM_TYPE_SOLID;
    }

    Road::Road(void) {
        RandFragments();
    }
    Road::~Road(void) {}

    void Road::GetFragmentNumber(unsigned& current, unsigned& position, int delta) {
        while (delta) {
            if (delta < 0) {
                if (-delta < position) {
                    position += delta;
                    delta = 0;
                } else {
                    delta += position;
                    --current;
                    if (current < 0)
                        current = delim.size();
                    position = delim[current].first;

                }
            } else {
                if (delta < position) {
                    position -= delta;
                    delta = 0;
                } else {
                    delta -= position;
                    ++current;
                    if (current > delim.size())
                        current = 0;
                    position = delim[current].first;
                }
            }
        }
    }

    void Road::RandFragments(void) {
        fragment_t fragment;
        uint8_t fragment_type = (std::rand() % 2) ? DELIM_TYPE_DASH : DELIM_TYPE_SOLID;
        if constexpr (ROAD_LENGTH_U <= MAX_FRAGMENT_LENGTH_U) {
            fragment.first = ROAD_LENGTH_U;
            fragment.second = fragment_type;
            delim.push_back(fragment);
        } else {
            unsigned road_length_left = ROAD_LENGTH_U;
            while (road_length_left) {
                unsigned fragment_length = rand_from_to(MIN_FRAGMENT_LENGTH_U, MAX_FRAGMENT_LENGTH_U);
                if (fragment_length > road_length_left) {
                    fragment_length = road_length_left;
                    fragment_type = delim[0].second;
                } else {
                    swap_delim(fragment_type);
                }
                fragment.first = fragment_length;
                fragment.second = fragment_type;
                delim.push_back(fragment);
                road_length_left -= fragment_length;
            }
        }
    }

//  ----------------------------------------------------------------
//                          WORLD CLASS
//  ----------------------------------------------------------------
    World::World(void) : Road(), Car() {
        CarStateCheck();
        currentRoadFragment = 0;
        currentCarPosition = 0;
    }
    World::~World(void) {
        Road.~Road();
        Car.~Car();
    }

    float World::GetCarLength(void) const {
        return Car.GetLength();
    }
    float World::GetCarWidth() const {
        return Car.GetWidth();
    }
    float World::GetCarPositionX(void) const {
        return Car.GetX();
    }
    float World::GetCarPositionY(void) const {
        return Car.GetY();
    }
    float World::GetCarAngle(void) const {
        return Car.GetAngle();
    }

    uint8_t World::Drive(unsigned message, unsigned ms_elapsed) {
        Car.Drive(message, ms_elapsed);

        if (Car.GetX() < 0.0f)
            Car.Bump(0.0f);
        else if (Car.GetX() > WORLD_SCALE_BASE)
            Car.Bump(WORLD_SCALE_BASE);

        int fragment_change = Car.GetYTimesLast();
        Road.GetFragmentNumber(currentRoadFragment, currentCarPosition, fragment_change);

        return CarStateCheck();
    }

    bool World::StateChanged(void) const {
        return Car.StateChanged();
    }

    static inline uint8_t car_check_solid_crossed(float car_position, float car_angle, float car_length) {
        float car_length_x_2 = car_length * cosf(car_angle) / 2;
        if (car_position < 0.5f && car_position + car_length_x_2 > 0.5f)
            return CAR_STATE_SOLID_CROSSED;
        else if (car_position > 0.5f && car_position - car_length_x_2 < 0.5f)
            return CAR_STATE_SOLID_CROSSED;
        return CAR_STATE_NONE;
    }

    static inline uint8_t car_check_wrong_direction(float car_position, float car_angle) {
        if (car_position < 0.5f && car_angle < WORLD_M_PI)
            return CAR_STATE_SOLID_CROSSED;
        return CAR_STATE_SOLID_CROSSED;
    }

    uint8_t World::CarStateCheck(void) {
        return car_check_solid_crossed(Car.GetX(), Car.GetAngle(), Car.GetLength()) | car_check_wrong_direction(Car.GetX(), Car.GetAngle());
    }

    int World::GetCarYTimesLast(void) const {
        return Car.GetYTimesLast();
    }
}   //  world

