#include "shared.h"
#include <algorithm>
#include <bitset>
#include <map>
#include <queue>
#include <ranges>
#include <set>
#include <unordered_map>
#include <unordered_set>

using namespace std;

using roomid = uint16_t;
roomid to_roomid(const char *id) { return id[0] << 8 | id[1]; }
roomid operator"" _roomid(const char *s, size_t) { return to_roomid(s); }
string to_str(roomid id) { return string{char(id >> 8), char(id & 0xff)}; }
struct room {
  roomid id;
  uint flow_rate;
  map<roomid, uint> costs;
};

using field = map<roomid, room>;

struct s {};
istream &operator>>(istream &input, s) {
  if (input.peek() == 's')
    input >> "s";
  return input;
}

roomid curr_roomid = 0;
istream &operator>>(istream &input, room &v) {
  input >> "Valve ";
  char id[2];
  input.read(id, 2);
  v.id = id[0] << 8 | id[1];
  input >> " has flow rate=" >> v.flow_rate >> "; tunnel" >> s{} >> " lead" >>
      s{} >> " to valve" >> s{} >> " ";
  while (isupper(input.peek())) {
    char id[2];
    input.read(id, 2);
    v.costs[to_roomid(id)] = 1;
    if (input.peek() == ',')
      input >> ", ";
  }
  return input;
}

int maximum_releasing_speed = 0;
field cave;

void search(int64_t &best, int64_t time, roomid room_id = "AA"_roomid,
            set<roomid> open = {}, int64_t score = 0,
            int64_t remaining_pressure = maximum_releasing_speed) {
  if (score > best) [[unlikely]]
    best = score;
  if (best > score + remaining_pressure * (time - 1)) [[unlikely]]
    return; // impossible to beat
  room &room = cave[room_id];
  if (!open.contains(room_id) && time > 1 && room.flow_rate > 0) [[likely]] {
    open.insert(room_id);
    int64_t new_score = score + room.flow_rate * (time - 1);
    int64_t new_remaining = remaining_pressure - room.flow_rate;
    for (auto &[dest, cost] : room.costs)
      if (!open.contains(dest))
        search(best, time - cost - 1, dest, open, new_score, new_remaining);
    open.erase(room_id);
  } else if (room.flow_rate == 0) [[unlikely]]
    for (auto &[dest, cost] : room.costs)
      if (!open.contains(dest))
        search(best, time - cost, dest, open, score, remaining_pressure);
}

// elephant friend :)
void WIP_search_with_friend(
    int64_t &best, int64_t self_time, roomid self_pos = "AA"_roomid,
    int64_t friend_time = -1, roomid friend_pos = "AA"_roomid,
    set<roomid> open = {}, int64_t score = 0,
    int64_t remaining_pressure = maximum_releasing_speed,
    vector<tuple<roomid, roomid>> steps = {}) {
  if (friend_time == -1)
    friend_time = self_time;
  steps.push_back({self_pos, friend_pos});
  if (score > best) [[unlikely]] {
    cerr << "Found new best: " << score;
    cerr << ", enabled: ";
    for (auto &i : open)
      cerr << to_str(cave[i].id) << ", ";
    cerr << "steps: ";
    for (auto &step : steps)
      cerr << to_str(cave[get<0>(step)].id) << "/"
           << to_str(cave[get<1>(step)].id) << ", ";
    cerr << '\n';
    best = score;
  }
  room &room = cave[self_pos];
  int64_t time = max(self_time, friend_time);
  if (!open.contains(self_pos) && time > 1 && room.flow_rate > 0) [[likely]] {
    open.insert(self_pos);
    int64_t new_score = score + room.flow_rate * (time - 1);
    int64_t new_remaining = remaining_pressure - room.flow_rate;
    for (auto &[dest, cost] : room.costs)
      if (!open.contains(dest)) {
        if (time == self_time)
          WIP_search_with_friend(best, self_time - cost - 1, dest, friend_time,
                                 friend_pos, open, new_score, new_remaining,
                                 steps);
        else if (time == friend_time)
          WIP_search_with_friend(best, self_time, self_pos,
                                 friend_time - cost - 1, dest, open, new_score,
                                 new_remaining, steps);
      }
    open.erase(self_pos);
  } else if (room.flow_rate == 0)
    for (auto &[dest, cost] : room.costs)
      if (!open.contains(dest)) {
        if (time == self_time)
          WIP_search_with_friend(best, self_time - cost, dest, friend_time,
                                 friend_pos, open, score, remaining_pressure,
                                 steps);
        else if (time == friend_time)
          WIP_search_with_friend(best, self_time, self_pos, friend_time - cost,
                                 dest, open, score, remaining_pressure, steps);
      }
}
int main() {
  pos start, goal;
  day(
      [](istream &input) {
        input >> std::noskipws;
        auto vec = lines(parse<room>)(input);
        map<roomid, room> rooms;
        for (auto &v : vec) {
          rooms[v.id] = v;
          maximum_releasing_speed += v.flow_rate;
        }
        auto nrooms = rooms;
        for (auto &[id, v] : rooms) {
          for (auto &[id2, v2] : rooms) {
            auto rid = id;
            if (id2 == rid || (v2.flow_rate == 0 && id2 != "AA"_roomid))
              continue;
            nrooms[id].costs[id2] = dijkstra<roomid>(rid, id2, [&](roomid id) {
              return ranges::views::keys(rooms[id].costs);
            });
          }
        }
        rooms = nrooms;

        vector<roomid> removing;
        removing.reserve(rooms.size());
        for (auto &[id, v] : rooms)
          if (v.flow_rate == 0 && id != "AA"_roomid)
            removing.push_back(id);
        for (auto &id : removing) {
          rooms.erase(id);
          for (auto &[_id, v] : rooms)
            v.costs.erase(id);
        }
        for (auto &[id, v] : rooms) {
          cerr << "Room " << to_str(id) << " has flow rate=" << v.flow_rate
               << "; tunnels lead to rooms ";
          for (auto [id, cost] : v.costs)
            cerr << to_str(id) << " (" << cost << "), ";
          cerr << endl;
        }
        cave = rooms;
        return rooms;
      },
      [&](field &input) {
        int64_t best = 0;
        search(best, 30);
        cerr << "\n\n";
        return best;
      },
      [&](field &input) -> int64_t {
        cerr << "Part 2 still WIP\n";
        int64_t best = 0;
        WIP_search_with_friend(best, 26);
        return best;
      });
}