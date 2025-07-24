#include "a-star-path-node.h"

namespace ppp
{
    //--------------------------------------------------------------------------
    path_node::path_node(const igrid* source, int x, int y)
        : grid_cell(source, x, y)
        , m_parent(nullptr)
        , m_costs(nullptr)
        , m_bis_traversable(true)
    {
    }

    //--------------------------------------------------------------------------
    std::string path_node::to_string() const
    {
        std::string debug_text;

        debug_text += "[";
        debug_text += m_bis_traversable ? "TRUE" : "FALSE";
        debug_text += "]";

        return debug_text;
    }
}