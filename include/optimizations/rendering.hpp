#pragma once
#include "base.hpp"

namespace ad {
    class RenderingOptimization : public Optimization {
    private:
        static bool _enabled;

    public:
        static inline bool getEnabled() { return _enabled; }

        virtual void init() override;
        virtual void load(std::istream&) override;
        virtual void save(std::ostream&) override;
        virtual void initMegahack(MegaHackExt::Window& window) override;
    };
}
