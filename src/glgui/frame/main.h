#pragma once
#include "../header/parameters.h"

namespace glgui {
namespace frame {
    class Main {
    public:
        bool _is_slam_running;
        header::Parameters* _parameters;
        Main(header::Parameters* parameters);
        ~Main();
        void render();
    };
}
}