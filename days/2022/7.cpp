#include "shared.h"
#include <limits>

using namespace std;

string parse_cd(ifstream &input) {
  input.ignore("$ cd "s.length());
  string name;
  getline(input, name);
  return name;
}
struct Directory {
  vector<Directory> entries;
  uint size;

  static Directory empty() { return {.size = 0}; }
  static Directory parse(ifstream &input) {
    input.ignore(numeric_limits<streamsize>::max(), '\n'); // skip `$ ls`
    Directory dir = empty();
    while (!input.eof() && input.peek() != '$') {
      if (input.peek() == 'd')
        // skip directories, will be inferred based off of the later `ls` calls
        input.ignore(numeric_limits<streamsize>::max(), '\n');
      else { // 123 name
        uint size;
        input >> size;
        // name doesn't matter
        input.ignore(numeric_limits<streamsize>::max(), '\n');
        dir.size += size;
      }
    }
    while (!input.eof() && parse_cd(input) != "..") {
      auto pdir = Directory::parse(input);
      dir.entries.push_back(pdir);
      dir.size += pdir.size;
    }

    return dir;
  }
};

template <typename T>
concept DirConsumer = requires(T x, Directory dir) {
  x(dir);
};

void each_entry_recursive(Directory dir, DirConsumer auto visit) {
  visit(dir);
  for (auto &entry : dir.entries)
    each_entry_recursive(entry, visit);
}

int main() {
  day(
      [](ifstream &input) {
        input.ignore(numeric_limits<streamsize>::max(), '\n'); // skip `$ cd /`
        return Directory::parse(input);
      },
      [](Directory root) {
        uint res = 0;
        each_entry_recursive(root, [&](Directory dir) {
          if (dir.size <= 100000)
            res += dir.size;
        });
        return res;
      },
      [](Directory root) {
        const uint TOTAL_DISK_SPACE = 70'000'000;
        const uint UPDATE_SIZE = 30'000'000;
        const uint required_dir_size =
            UPDATE_SIZE - (TOTAL_DISK_SPACE - root.size);

        uint min_size = numeric_limits<uint>::max();
        each_entry_recursive(root, [&](Directory dir) {
          if (dir.size < min_size && dir.size >= required_dir_size)
            min_size = dir.size;
        });
        return min_size;
      });
}