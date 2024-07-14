#pragma once

#include <cstdint>
#include <vector>

namespace world {
    static constexpr float WORLD_SCALE_BASE = 1.0f;

    static constexpr float WORLD_M_PI = 3.14159265358979323846f;
    static constexpr float WORLD_M_2PI = WORLD_M_PI * 2;
    static constexpr float WORLD_M_PI_2 = 1.57079632679489661923f;
    static constexpr float WORLD_M_PI_4 = 0.785398163397448309616f;

    static constexpr uint8_t CAR_STATE_NONE = 0x00;
    static constexpr uint8_t CAR_STATE_WRONG_DIRECTION = 0x01;
    static constexpr uint8_t CAR_STATE_SOLID_CROSSED = 0x02;

    static constexpr uint8_t DELIM_TYPE_SOLID = 0x00;
    static constexpr uint8_t DELIM_TYPE_DASH = 0x01;

    typedef std::pair<unsigned, uint8_t> fragment_t;

    class Road {
    public:
        Road(void);
        ~Road(void);
        void GetFragmentNumber(unsigned& current, unsigned& position, int delta);
    private:
        //  Critically BAD behavior if ROAD_LENGTH < MAX_FRAGMENT_LENGTH 
        //  or MAX_FRAGMENT_LENGTH < MIN_FRAGMENT_LENGTH
        static constexpr unsigned ROAD_LENGTH_U = 1500;
        static constexpr unsigned MIN_FRAGMENT_LENGTH_U = 50;
        static constexpr unsigned MAX_FRAGMENT_LENGTH_U = 150; //  Note: [0], [N-1] and [N-2] might be same type
        
        //  Рудименты. Пришлось вырезать.
        std::vector<fragment_t> delim;
        void RandFragments(void);
    };

    class Car {
    public:
        Car(void);
        ~Car(void);

        float GetLength(void) const;
        float GetWidth(void) const;
        float GetX(void) const;
        float GetY(void) const;
        float GetAngle(void) const;

        void Drive(unsigned message, unsigned ms_elapsed);
        void Bump(float x_lim);
        int GetYTimesLast(void) const;

        bool StateChanged(void) const;
    private:
        static constexpr float CAR_LENGTH_BASE   = WORLD_SCALE_BASE / 3.0f;
        static constexpr float CAR_WIDTH_BASE    = WORLD_SCALE_BASE / 6.0f;
        float length;
        float width;

        static constexpr float CAR_X_POS_BASE = WORLD_SCALE_BASE / 5.0f;
        static constexpr float CAR_Y_POS_BASE = WORLD_SCALE_BASE * 10.0f;
        float x_pos;
        float y_pos;

        static constexpr float CAR_SPEED_BASE = 0.0f;
        float speed;

        static constexpr float CAR_ANGLE_PHASE = WORLD_M_PI_4;
        static constexpr float CAR_ANGLE_BASE = 0.0f;
        static constexpr float CAR_TURN_RATE = 0.05f;
        float angle;
        float sinLast;
        float cosLast;

        void SpeedUp(void);
        void SpeedDown(void);
        void SpeedLoss(void);
        void Turn(float turn_rate);
        void Move(unsigned message);
        bool position_changed;

        int y_times_last;
    };

    class World {   
    public:
        World(void);
        ~World(void);
        float GetCarLength(void) const;
        float GetCarWidth(void) const;
        float GetCarPositionX(void) const;
        float GetCarPositionY(void) const;
        float GetCarAngle(void) const;
        uint8_t Drive(unsigned message, unsigned ms_elapsed);
        bool StateChanged(void) const;
        
        int GetCarYTimesLast(void) const;
    private:
        Road Road;
        Car Car;
        uint8_t CarStateCheck(void);
        unsigned currentRoadFragment;
        unsigned currentCarPosition;
    };

}   //  world;