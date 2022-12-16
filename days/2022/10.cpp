#include "shared.h"
#include <variant>

using namespace std;

using Instruction = variant<int, monostate>; // variant<addx, noop>
int main() {
  using input = vector<Instruction>;
  day(
      lines([](istream &input) -> Instruction {
        if (input.peek() == 'a') {
          int amount;
          input >> "addx " >> amount;
          return amount;
        } else
          return monostate{};
      }),
      [](input &input) {
        int curr_cycle = 0, signal = 0, x = 1;
        auto cycle = [&]() {
          curr_cycle++;
          if (curr_cycle <= 220 && (curr_cycle - 20) % 40 == 0)
            signal += x * curr_cycle;
        };
        for (auto &ins : input) {
          cycle();
          if (holds_alternative<int>(ins)) {
            cycle();
            x += get<int>(ins);
          }
        }
        return signal;
      },
      [](input &input) {
        stringstream screen;
        int curr_cycle = 0, x = 1;
        auto cycle = [&]() {
          if (curr_cycle % 40 == 0)
            screen << '\n';
          screen << (abs(x - (curr_cycle % 40)) <= 1 ? '#' : ' ');
          curr_cycle++;
        };
        for (auto &ins : input) {
          cycle();
          if (holds_alternative<int>(ins)) {
            cycle();
            x += get<int>(ins);
          }
        }
        screen << '\n';
        return screen.str();
      });
}