#pragma once

#include <vector>
#include <extensions2.h>

namespace ad {
    class Optimization {
    public:
        virtual void init() = 0;
        virtual void load(std::istream&) = 0;
        virtual void save(std::ostream&) = 0;
        virtual void initMegahack(MegaHackExt::Window&) = 0;
    };

    static std::vector<Optimization*> optimizations;
}
