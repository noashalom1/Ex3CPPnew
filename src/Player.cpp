#include "Player.hpp"
#include "exceptions.hpp"
#include "Game.hpp"

namespace coup
{

    Player::Player(Game &game, const std::string &name)
        : game(game), name(name), coins(0)
    {
    }

    Player::~Player() {}

    const std::string &Player::get_name() const
    {
        return name;
    }

    

    void Player::check_turn() const
    {
        if (game.turn() != name)
            throw NotYourTurnException();
        ;
    }

    void Player::revive()
    {
        if (!is_eliminated())
            throw TargetNotEliminatedException();
        eliminated = false;
    }

    void Player::gather()
    {
        check_turn();
        if (must_coup())
            throw MustPerformCoupException();
        if (is_sanctioned() == true)
            throw SanctionedException();
        coins++;
        game.next_turn();
    }

    void Player::tax()
    {
        check_turn();
        if (must_coup())
            throw MustPerformCoupException();
        if (is_sanctioned() == true)
            throw SanctionedException();
        coins += 2;
        game.get_action_history().emplace_back(name, "tax", game.get_current_round());
        game.get_tax_turns()[name] = game.get_global_turn_index();
        game.next_turn();
    }

    void Player::bribe()
    {
        check_turn();
        if (must_coup())
            throw MustPerformCoupException();
        if (coins < 4)
            throw NotEnoughCoinsException(4, coins);
        coins -= 4;
        extra_turns = 2;
        mark_used_bribe();
        game.next_turn();
    }

    void Player::arrest(Player &target)
    {
        check_turn();
        if (must_coup())
            throw MustPerformCoupException();
        if (is_disable_to_arrest())
            throw ArrestBlockedException();
        if (target.get_name() == game.get_last_arrested_name())
            throw DuplicateArrestException();
        if (target.is_eliminated())
            throw TargetIsEliminatedException();
        if (target.get_name() == name)
            throw CannotTargetYourselfException();
        if (target.role() == "General")
        {
            if (target.get_coins() <= 0)
                throw TargetNoCoinsException();
            coins++;
        }
        else if (target.role() == "Merchant")
        {
            if (target.get_coins() <= 1)
                throw TargetNoCoinsException();
            target.coins -= 2; // Merchant loses 2 coins
        }
        else
        {
            if (target.get_coins() <= 0)
                throw TargetNoCoinsException();
            target.coins--;
            coins++;
        }
        game.set_last_arrested_name(target.get_name());
        game.next_turn();
    }

    void Player::sanction(Player &target)
    {
        check_turn();
        if (must_coup())
            throw MustPerformCoupException();
        if (target.is_eliminated())
            throw TargetIsEliminatedException();
        if (target.get_name() == name)
            throw CannotTargetYourselfException();
        if (target.is_sanctioned() == true)
            throw AlreadySanctionedException();
        if (target.role() == "Baron")
            target.increase_coins(1); // Baron gets 1 coin back
        if ((target.role() == "Judge" && coins < 4) || coins < 3)
            throw NotEnoughCoinsException(target.role() == "Judge" ? 4 : 3, coins);
        target.role() == "Judge" ? coins -= 4 : coins -= 3; // Judge costs 4, others cost 3
        target.mark_sanctioned(name);
        game.next_turn();
    }

    void Player::coup(Player &target)
    {
        check_turn();
        if (coins < 7)
            throw NotEnoughCoinsException(7, coins);
        if (target.is_eliminated())
            throw TargetIsAlreadyEliminated();
        if (target.get_name() == name)
            throw CannotTargetYourselfException();
        game.remove_player(&target);
        game.add_to_coup(name, target.get_name());
        coins -= 7;
        game.next_turn();
    }

    void Player::decrease_coins(int amount)
    {
        if (coins < amount)
        {
            throw NotEnoughCoinsException(amount, coins);
        }
        coins -= amount;
    }

    void Player::increase_coins(int amount)
    {
        coins += amount;
    }

    void Player::mark_eliminated()
    {
        eliminated = true;
    }

    bool Player::is_eliminated() const
    {
        return eliminated;
    }

    void Player::mark_sanctioned(const std::string &by_whom)
    {
        sanctioned = true;
        sanctioned_by = by_whom;
    }

    void Player::clear_sanctioned()
    {
        sanctioned = false;
        sanctioned_by.clear();
    }

    bool Player::is_sanctioned() const
    {
        return sanctioned;
    }

    void Player::set_must_coup(bool value)
    {
        mustPerformCoup = value;
    }

    bool Player::must_coup() const
    {
        return mustPerformCoup;
    }

}