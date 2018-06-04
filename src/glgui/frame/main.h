#pragma once
#include "../component/parameters.h"

namespace glgui {
namespace frame {
    class Main {
    public:
        bool _is_slam_running;
        component::Parameters* _parameters;
        Main(component::Parameters* parameters);
        ~Main();
        void render();
    };
}
}