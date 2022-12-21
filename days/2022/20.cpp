#include "shared.h"
#include <memory>

using namespace std;

template <typename T> struct linked {
  T value = 0;
  linked<T> *prev = nullptr, *next = nullptr;
  linked(T value) : value(value) {}
  linked(T value, linked<T> *prev, linked<T> *next)
      : value(value), prev(prev), next(next) {}
  void move_after(linked<T> *other) {
    if (other == this || other->next == this)
      return;
    assert(next != nullptr && prev != nullptr && other->next != nullptr);
    next->prev = prev;
    prev->next = next;
    next = other->next;
    prev = other;
    other->next = this;
    next->prev = this;
  }
  void move_before(linked<T> *other) {
    if (other == this || other->prev == this)
      return;
    assert(next != nullptr && prev != nullptr && other->next != nullptr);
    // remove self
    next->prev = prev;
    prev->next = next;
    // move self
    prev = other->prev;
    next = other;
    other->prev = this;
    prev->next = this;
  }
  linked<T> *add(ptrdiff_t n) {
    if (n > 0) {
      assert(this->next != nullptr);
      auto iter = this->next;
      while (--n > 0)
        iter = iter->next;
      return iter;
    }
    if (n < 0) {
      assert(this->prev != nullptr);
      auto iter = this->prev;
      while (++n < 0)
        iter = iter->prev;
      return iter;
    }
    return this;
  }
};
using li = linked<int64_t>;
using input = vector<li *>;
void mix(input &numbers) {
  for (auto item : numbers) {
    auto val = item->value;
    if (val > 0)
      item->move_after(item->add(val));
    else if (val < 0)
      item->move_before(item->add(val));
  }
}

// temp includes for wip sleep
#include <chrono>
#include <thread>
int main() {
  //  there's something wrong with mixing i think? it's very close to
  //  the correct order but it's not quite right. todo fix :)
  cerr << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"
          "           Day 20 is WIP!           \n"
          " (does not produce correct answers) \n"
          "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"
       << endl;
  for (int i = 10; i > 0; i--) {
    cerr << "\rSleeping for " << i << " seconds...";
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  day(
      [](istream &stream) {
        input numbers;
        int64_t first;
        stream >> first;
        numbers.push_back(new li(first));
        for (int64_t val; stream >> val;) {
          auto n = new li(val, numbers.back(), nullptr);
          numbers.back()->next = n;
          numbers.push_back(n);
        }
        numbers.back()->next = numbers[0];
        numbers[0]->prev = numbers.back();
        return numbers;
      },
      [](input numbers) -> int64_t {
        mix(numbers);
        auto iter = *find_if(numbers.begin(), numbers.end(),
                             [](auto item) { return item->value == 0; });
        do {
          cout << iter->value << "\n";
          iter = iter->add(1);
        } while (iter->value != 0);
        cout << '\n';
        for (auto item : numbers)
          if (item->value == 0) {
            auto onet = item->add(1000), twot = item->add(2000),
                 threet = item->add(3000);
            cout << "1: " << onet->value << " 2: " << twot->value
                 << " 3: " << threet->value << endl;
            return onet->value + twot->value + threet->value;
          }
        cerr << "no 0 found" << endl;
        return 0;
      },
      [](input numbers) {
        constexpr auto decryption_key = 811589153;
        for (auto &item : numbers)
          item->value *= decryption_key;
        for (int i = 0; i < 10; i++)
          mix(numbers);
        for (auto item : numbers)
          if (item->value == 0) {
            auto onet = item->add(1000), twot = item->add(2000),
                 threet = item->add(3000);
            return onet->value + twot->value + threet->value;
          }
        throw runtime_error("no 0 found");
      });
}