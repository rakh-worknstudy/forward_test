#pragma once

#include <cstdint>
#include <vector>

namespace world {
    namespace car {
        namespace state {
            constexpr uint8_t NONE = 0x00;
            constexpr uint8_t WRONG_DIRECTION = 0x01;
            constexpr uint8_t CROSS_SOLID = 0x02;
        }   //  state

        class Car {
        public:
            Car(void);
            ~Car(void);

        private:
            float position;
            float angle;
        };
    }   //  car

    namespace road {
        //  CRITICALLY BAD BEHAVIOR IF world::road::LENGTH < world::road::fragment::MAX_LENGTH
        constexpr unsigned LENGTH = 1500;
        namespace fragment {
            constexpr unsigned MIN_LENGTH = 50;
            constexpr unsigned MAX_LENGTH = 150;    //  Actual Max MAX_LENGTH * 3 - 1

            enum type_e {
                begin = 0,
                solid = begin,
                dash,
                count,
                bad = -1
            };
        }

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

    class World {
    public:
        World(void);
        ~World(void);
        uint8_t Drive(unsigned int message);
    private:
        road::Road Road;
        car::Car Car;
        uint8_t carState;
        void CarStateCheck(void);
    };

}   //  world;