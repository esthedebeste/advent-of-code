#include "shared.h"
#include <algorithm>
#include <variant>

using namespace std;

using worried_t = uint64_t;
struct Monkey {
  struct Add {
    worried_t amount;
    worried_t apply(worried_t x) { return x + amount; }
  };
  struct Multiply {
    worried_t amount;
    worried_t apply(worried_t x) { return x * amount; }
  };
  struct Square {
    worried_t apply(worried_t x) { return x * x; }
  };
  variant<Add, Multiply, Square> operation;
  worried_t apply(worried_t x) {
    return visit([&](auto &op) { return op.apply(x); }, operation);
  }
  int index, testNum, trueThrow, falseThrow;
  worried_t throws = 0;
  int throwTo(worried_t x) { return x % testNum == 0 ? trueThrow : falseThrow; }
};

struct Item {
  int belongsTo;
  worried_t worried = 0;
};

int main() {
  using input = pair<vector<Monkey>, vector<Item>>;
  day(
      [](ifstream &input) {
        vector<Monkey> monkeys;
        vector<Item> items;
        do {
          Monkey monkey;
          input >> "Monkey " >> monkey.index >> ":\n  Starting items: ";
          do {
            worried_t worried;
            input >> worried;
            items.push_back({monkey.index, worried});
          } while (input.peek() != '\n' && (input >> ", "));
          input >> "\n  Operation: new = old ";
          switch (input.get()) {
          case '+':
            input >> " " >> monkey.operation.emplace<Monkey::Add>().amount;
            break;
          case '*':
            input >> " ";
            if (input.peek() == 'o') {
              input >> "old";
              monkey.operation.emplace<Monkey::Square>();
              break;
            } else
              input >> monkey.operation.emplace<Monkey::Multiply>().amount;
            break;
          default:
            throw std::invalid_argument("Invalid operation in input");
          }
          input >> "\n  Test: divisible by " >> monkey.testNum >>
              "\n    If true: throw to monkey " >> monkey.trueThrow >>
              "\n    If false: throw to monkey " >> monkey.falseThrow;
          monkeys.push_back(monkey);
        } while (!input.eof() && (input >> "\n\n"));
        return make_pair(monkeys, items);
      },
      [](input input) {
        auto [monkeys, items] = input;
        for (int i = 0; i < 20; i++)
          for (auto &m : monkeys)
            for (auto &i : items)
              if (i.belongsTo == m.index) {
                i.worried = m.apply(i.worried) / 3;
                i.belongsTo = m.throwTo(i.worried);
                m.throws++;
              }
        ranges::sort(monkeys, greater(), &Monkey::throws);
        return monkeys[0].throws * monkeys[1].throws;
      },
      [](input input) {
        auto [monkeys, items] = input;
        worried_t divisor = 1;
        for (auto &m : monkeys)
          divisor *= m.testNum;
        for (int i = 0; i < 10000; i++)
          for (auto &m : monkeys)
            for (auto &i : items)
              if (i.belongsTo == m.index) {
                i.worried = m.apply(i.worried % divisor) % divisor;
                i.belongsTo = m.throwTo(i.worried);
                m.throws++;
              }
        ranges::sort(monkeys, greater(), &Monkey::throws);
        return monkeys[0].throws * monkeys[1].throws;
      });
}