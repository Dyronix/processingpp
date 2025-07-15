#pragma once

#include "a-star-grid-cell.h"
#include "a-star-path-node-costs.h"

#include <string>

namespace ppp
{
    class igrid;

    class path_node : public grid_cell
    {
    public:
        path_node(const igrid* source, int x, int y);
        ~path_node() override = default;

        path_node(const path_node& other) = default;
        path_node(path_node&& other) noexcept = default;

        path_node& operator=(const path_node& other) = default;
        path_node& operator=(path_node&& other) noexcept = default;

    public:
        std::string to_string() const override;

    private:
        path_node* m_parent;
        path_node_costs* m_costs;

        bool m_bis_traversable;
    };
}
