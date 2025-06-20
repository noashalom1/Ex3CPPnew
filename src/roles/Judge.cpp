#include "Judge.hpp"
#include "Game.hpp"
#include "exceptions.hpp"
#include <iostream>

namespace coup
{

    Judge::Judge(Game &game, const std::string &name) : Player(game, name) {}

    std::string Judge::undo_bribe(Player &target)
    {
        if (is_eliminated())
        {
            throw PlayerEliminatedException(name);
        }

        if (!can_undo_bribe())
        {
            throw ActionAlreadyUsedThisRoundException(name, "UNDO BRIBE");
        }

        if(!target.is_used_bribe())
        {
            throw UndoNotAllowedException(target.get_name(), " bribe");
        }

        if (&target == this)
        {
            throw CannotUndoOwnActionException(name, "bribe");
        }

        if (target.is_eliminated())
        {
            throw TargetIsEliminatedException();
        }

         std::string msg;

        if (target.get_extra_turns() == 1)
        {
            msg = name + " has canceled " + target.get_name() + "'s bribe.";
            target.set_extra_turns(0);
        }
        else if (target.get_extra_turns() == 0)
        {
            msg = name + " has canceled " + target.get_name() + "'s bribe (after it took effect).";
            game.next_turn();
        }
        else
        {
            throw InvalidBribeUndoException();
        }

        mark_undo_bribe_used();
        return msg;
    }

    std::string Judge::role() const
    {
        return "Judge";
    }
}