#pragma once

#include <vector>
#include <functional>

namespace ppp
{
    class sierra_player_state
    {
        using player_state_delegates = std::function<void()>;

    public:
        void toggle_placement();

        void subscribe_start_placement(player_state_delegates callback);
        void subscribe_stop_placement(player_state_delegates callback);
        
        bool is_placing() const { return _is_placing; }

    private:
        std::vector<player_state_delegates> _start_placement_callbacks;
        std::vector<player_state_delegates> _stop_placement_callbacks;

        bool _is_placing = false;
    };
}