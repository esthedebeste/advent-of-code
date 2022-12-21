#include "shared.h"
#include <bitset>
#include <deque>
#include <unordered_map>
#include <variant>

using namespace std;
using id = string;
enum op_type : char {
  multiply = '*',
  add = '+',
  divide = '/',
  subtract = '-',
};
struct op;
using input = unordered_map<id, variant<op, int64_t>>;
input monkeys;
int64_t get_value(id id);
struct op {
  op_type type;
  id a, b;
  int64_t run() const {
    auto a = get_value(this->a);
    auto b = get_value(this->b);
    switch (type) {
    case multiply:
      return a * b;
    case add:
      return a + b;
    case divide:
      return a / b;
    case subtract:
      return a - b;
    }
  }
  int64_t a_to_get(int64_t target) {
    auto b = get_value(this->b);
    switch (type) {
    case multiply:
      return target / b;
    case add:
      return target - b;
    case divide:
      return target * b;
    case subtract:
      return target + b;
    }
  }
  int64_t b_to_get(int64_t target) {
    auto a = get_value(this->a);
    switch (type) {
    case multiply:
      return target / a;
    case add:
      return target - a;
    case divide:
      return a / target;
    case subtract:
      return a - target;
    }
  }
};
int64_t get_value(id id) {
  auto &variant = monkeys.at(id);
  if (holds_alternative<int64_t>(variant))
    return get<int64_t>(variant);
  else
    return get<op>(variant).run();
}
bool depends_on(const id &a, const id &b) {
  if (a == b)
    return true;
  if (holds_alternative<int64_t>(monkeys.at(a))) {
    return false;
  } else {
    auto &o = get<op>(monkeys.at(a));
    return depends_on(o.a, b) || depends_on(o.b, b);
  }
}
enum side { A, B };
using path_t = deque<pair<id, side>>;
path_t path(const id &a, const id &b, path_t curr = {}) {
  if (a == b)
    return curr;
  if (holds_alternative<int64_t>(monkeys.at(a))) {
    return {};
  } else {
    auto &o = get<op>(monkeys.at(a));
    if (depends_on(o.a, b)) {
      curr.push_back({a, A});
      return path(o.a, b, curr);
    } else if (depends_on(o.b, b)) {
      curr.push_back({a, B});
      return path(o.b, b, curr);
    } else {
      return {};
    }
  }
}

int64_t part2(int64_t goal, path_t path) {
  if (path.empty())
    return goal;
  auto [id, side] = path.front();
  path.pop_front();
  auto &o = get<op>(monkeys.at(id));
  if (side == A) {
    auto a = o.a_to_get(goal);
    return part2(a, path);
  } else {
    auto b = o.b_to_get(goal);
    return part2(b, path);
  }
}

int main() {
  day(
      [](istream &stream) {
        input monkeys;
        while (stream.peek() != EOF) {
          string id(4, '\0');
          stream >> id >> ": ";
          if (isdigit(stream.peek())) {
            int num;
            stream >> num;
            monkeys[id] = num;
            stream >> skip_until('\n');
            continue;
          }
          // op
          string a(4, '\0'), b(4, '\0');
          op_type type;
          stream >> a >> " " >> reinterpret_cast<char &>(type) >> " " >> b >>
              skip_until('\n');
          monkeys[id] = op{type, a, b};
        }
        ::monkeys = monkeys;
        return monkeys;
      },
      [](input monkeys) -> int64_t {
        auto &root = get<op>(monkeys["root"]);
        auto &humn = get<int64_t>(monkeys["humn"]);
        return root.run();
      },
      [](input monkeys) -> int64_t {
        path_t path = ::path("root", "humn");
        int64_t goal = path[0].second == A
                           ? get_value(get<op>(monkeys["root"]).b)
                           : get_value(get<op>(monkeys["root"]).a);
        path.pop_front();
        return part2(goal, path);
      });
}