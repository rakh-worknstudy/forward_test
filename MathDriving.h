#pragma once

#include <cstdint>
#include <vector>

namespace world {
    namespace road {
        //  CRITICALLY BAD BEHAVIOR IF LENGTH < MAX_LENGTH OR MAX_LENGTH < MIN_LENGTH
        constexpr unsigned LENGTH = 1500;
        namespace fragment {
            constexpr unsigned MIN_LENGTH = 50;
            constexpr unsigned MAX_LENGTH = 150;    //  Actual Max MAX_LENGTH * 3 - 1 (+ Cycle repeat)

            enum type_e {
                begin = 0,
                solid = begin,
                dash,
                count,
                bad = -1
            };
        }

        constexpr float width = 1.0f;

        class Road {
        public:
            Road(void);
            ~Road(void);
        private:
            //  Solid/Dash road
            std::vector<std::pair<unsigned, uint8_t>> Fragment;
            std::vector<std::pair<unsigned, uint8_t>>::iterator CurrentFragment;
            void RandFragments(void);
        };
    }   //  road

    namespace car {
        namespace state {
            constexpr uint8_t NONE = 0x00;
            constexpr uint8_t WRONG_DIRECTION = 0x01;
            constexpr uint8_t CROSS_SOLID = 0x02;
        }   //  state

        constexpr float relativeWidth = 6.0f;  //  Road relation
        constexpr float width = road::width / relativeWidth;  //  Car width relative to road
        constexpr float relativeLength = 2.0f;
        constexpr float length = width * relativeLength;
        constexpr float relativeInitPosition = 5.0f;  //  Car initial position relation
        constexpr float initPosition = width * relativeInitPosition;  //  Car inital position
        constexpr float initAngle = 0.0f;  //  Starting angle

        class Car {
        public:
            Car(void);
            ~Car(void);
            float GetPosition(void) const;
            float GetAngle(void) const;
        private:
            float position;
            float angle;
        };
    }   //  car

    class World {
    public:
        World(void);
        ~World(void);
        //  Car
        float GetCarPosition(void) const;
        float GetCarAngle(void) const;
        uint8_t Drive(unsigned int message);
        //  Road
    private:
        road::Road Road;
        car::Car Car;
        uint8_t carState;
        void CarStateCheck(void);
    };

}   //  world;