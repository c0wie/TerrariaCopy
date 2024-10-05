#pragma once
#include "Global.hpp"
#include <functional>

struct item
{
    int ID{0};
    unsigned int maxStackSize{64};
    std::function<void()> OnUse;
};