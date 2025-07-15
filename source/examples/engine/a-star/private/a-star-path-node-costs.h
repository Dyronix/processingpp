#pragma once

namespace ppp
{
    struct path_node_costs
    {
    public:
        static constexpr int MAX_G_COST = 10000;
        static constexpr int MAX_H_COST = 10000;

    public:
        int g_cost {0};
        int h_cost {0};

    public:
        int FCost() const { return g_cost + h_cost; }
    };
}