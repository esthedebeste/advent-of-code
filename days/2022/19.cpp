#include "shared.h"
#include <deque>
#include <map>

using u8 = uint_fast8_t; // speeds up math a little bit
using namespace std;
struct Inventory {
  u8 ore{}, clay{}, obsidian{}, geode{};
  Inventory(auto ore, auto clay, auto obsidian, auto geode)
      : ore{u8(ore)}, clay{u8(clay)}, obsidian{u8(obsidian)}, geode{u8(geode)} {
  }
};
Inventory operator+(const Inventory &a, const Inventory &b) {
  return Inventory(a.ore + b.ore, a.clay + b.clay, a.obsidian + b.obsidian,
                   a.geode + b.geode);
}
template <typename T>
ostream &operator<<(ostream &output, const Inventory &inventory) {
  return output << "ore = " << +inventory.ore << " clay = " << +inventory.clay
                << " obsidian = " << +inventory.obsidian
                << " geode = " << +inventory.geode;
}
struct Prices {
  struct {
    u8 ore;
    bool buyable(const Inventory &inventory) const {
      return inventory.ore >= ore;
    }
    void buy(Inventory &inventory) const { inventory.ore -= ore; }
  } ore;
  struct {
    u8 ore;
    bool buyable(const Inventory &inventory) const {
      return inventory.ore >= ore;
    }
    void buy(Inventory &inventory) const { inventory.ore -= ore; }
  } clay;
  struct {
    u8 ore;
    u8 clay;
    bool buyable(const Inventory &inventory) const {
      return inventory.ore >= ore && inventory.clay >= clay;
    }
    void buy(Inventory &inventory) const {
      inventory.ore -= ore;
      inventory.clay -= clay;
    }
  } obsidian;
  struct {
    u8 ore;
    u8 obsidian;
    bool buyable(const Inventory &inventory) const {
      return inventory.ore >= ore && inventory.obsidian >= obsidian;
    }
    void buy(Inventory &inventory) const {
      inventory.ore -= ore;
      inventory.obsidian -= obsidian;
    }
  } geode;
};
struct Blueprint {
  u8 index;
  Prices prices;
  u8 max_ore_price;
  Blueprint(u8 index, Prices prices)
      : index{index}, prices{prices}, max_ore_price{max(prices.clay.ore,
                                                        prices.geode.ore,
                                                        prices.obsidian.ore,
                                                        prices.geode.ore)} {}
};
struct GameState {
  const Blueprint &blueprint;
  u8 timer;
  Inventory bots{0, 0, 0, 0};
  Inventory inventory{0, 0, 0, 0};
  vector<GameState> possibilities() {
    if (timer == 0)
      return {};
    GameState clone = *this;
    clone.inventory = inventory + bots;
    clone.timer--;
    if (blueprint.prices.geode.buyable(inventory)) {
      GameState state = clone;
      blueprint.prices.geode.buy(state.inventory);
      state.bots.geode++;
      return {state};
    }
    vector<GameState> result;
    result.reserve(4);
    if (blueprint.prices.obsidian.buyable(inventory) &&
        blueprint.prices.geode.obsidian > bots.obsidian) {
      GameState state = clone;
      blueprint.prices.obsidian.buy(state.inventory);
      state.bots.obsidian++;
      result.push_back(state);
    }
    if (blueprint.prices.clay.buyable(inventory) &&
        blueprint.prices.obsidian.clay > bots.clay) {
      GameState state = clone;
      blueprint.prices.clay.buy(state.inventory);
      state.bots.clay++;
      result.push_back(state);
    }
    if (blueprint.prices.ore.buyable(inventory) &&
        blueprint.max_ore_price > bots.ore) {
      GameState state = clone;
      blueprint.prices.ore.buy(state.inventory);
      state.bots.ore++;
      result.push_back(state);
    }
    result.push_back(clone);
    return result;
  }
};
using input = vector<Blueprint>;

vector<u8> part(input blueprints, u8 minutes) {
  vector<u8> result;
  for (auto &blueprint : blueprints) {
    GameState startState{blueprint, minutes};
    startState.bots.ore = 1;
    vector<GameState> states{startState};
    u8 best = 0;
    while (!states.empty()) {
      GameState state = states.back();
      states.pop_back();
      for (auto &state : state.possibilities()) {
        if (best >
            state.inventory.geode + ((state.bots.geode + state.timer) *
                                     (state.bots.geode + state.timer - 1) / 2))
          continue;
        if (state.timer == 0) {
          if (best < state.inventory.geode)
            best = state.inventory.geode;
          continue;
        }

        states.push_back(state);
      }
    }
    cout << "best for i = " << +blueprint.index << " is " << +best << "    \r";
    result.push_back(best);
  }
  return result;
}

int main() {
  day(
      lines([](istream &input) {
        uint blui, oreore, clayore, obsore, obsclay, geore, geobs;
        input >> "Blueprint " >> blui >> ": Each ore robot costs " >> oreore >>
            " ore. Each clay robot costs " >> clayore >>
            " ore. Each obsidian robot costs " >> obsore >> " ore and " >>
            obsclay >> " clay. Each geode robot costs " >> geore >>
            " ore and " >> geobs >> " obsidian.";
        return Blueprint(blui, Prices{
                                   {u8(oreore)},
                                   {u8(clayore)},
                                   {u8(obsore), u8(obsclay)},
                                   {u8(geore), u8(geobs)},
                               });
      }),
      [](input blueprints) {
        auto res = part(blueprints, 24);
        int total = 0;
        for (uint i = 0; i < res.size(); i++)
          total += res[i] * (i + 1);
        return total;
      },
      [](input blueprints) {
        vector<Blueprint> first3 = {blueprints[0], blueprints[1],
                                    blueprints[2]};
        auto res = part(first3, 32);
        return res[0] * res[1] * res[2];
      });
}