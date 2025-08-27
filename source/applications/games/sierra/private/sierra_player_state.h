#pragma once

#include <vector>
#include <functional>

namespace ppp
{
    class sierra_player_state
    {
        using player_state_delegate = std::function<void()>;

    public:
        void toggle_placement();
        void try_spawn_tower();
        void start_new_wave();

        void subscribe_start_placement(player_state_delegate callback);
        void subscribe_stop_placement(player_state_delegate callback);
        void subscribe_try_spawn_tower(player_state_delegate callback);
        void subsrcibe_start_new_wave(player_state_delegate callback);
        
        bool is_placing() const { return _is_placing; }

    private:
        std::vector<player_state_delegate> _start_placement_callbacks;
        std::vector<player_state_delegate> _stop_placement_callbacks;
        std::vector<player_state_delegate> _try_spawn_tower_callbacks;
        std::vector<player_state_delegate> _new_wave_callbacks;

        bool _is_placing = false;
    };
}