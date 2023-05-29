#pragma once
#include <vector>

namespace ad {
    class Feature {
    public:
        virtual void init() = 0;
    };

    static std::vector<Feature*> features;
}
