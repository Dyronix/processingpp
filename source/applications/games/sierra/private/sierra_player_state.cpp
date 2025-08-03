#include "sierra_player_state.h"

namespace ppp
{
    //-------------------------------------------------------------------------
    void sierra_player_state::toggle_placement()
    {
        _is_placing = !_is_placing;
        if (_is_placing)
        {
            for (auto& callback : _start_placement_callbacks)
            {
                callback();
            }
        }
        else
        {
            for (auto& callback : _stop_placement_callbacks)
            {
                callback();
            }
        }
    }

    //-------------------------------------------------------------------------
    void sierra_player_state::subscribe_start_placement(player_state_delegates callback)
    {
        _start_placement_callbacks.push_back(callback);
    }
    //-------------------------------------------------------------------------
    void sierra_player_state::subscribe_stop_placement(player_state_delegates callback)
    {
        _stop_placement_callbacks.push_back(callback);
    }
}