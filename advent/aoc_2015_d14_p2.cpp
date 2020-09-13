#include <iostream>
#include <vector>
#include <map>

class Deer {
public:
  std::string name;
  int speed;
  int travel_time;
  int rest_time;

  int timer;
  bool resting;

  Deer(std::string name, int speed, int travel_time, int rest_time)
      : name(name)
      , speed(speed)
      , travel_time(travel_time)
      , rest_time(rest_time)
  {
    resting = false;
    timer = travel_time;
  }

  void move() {
    --timer;
    if (timer == 0) {
      resting = !resting;
      if (resting) {
	timer = rest_time;
      } else {
        timer = travel_time;
      }
    }
  }

  int distance()
  {
    if (resting) {
      return 0;
    } else {
      return speed;
    }
  }
};

int main(int argc, char *argv[])
{
  std::string name;
  std::string junk;
  int speed;
  int travel_time;
  int rest_time;
  std::vector<Deer> deer;
  std::map<std::string, int> distance;
  std::map<std::string, int> points;

  while (std::cin >> name >> junk >> junk >> speed >> junk >> junk >>
         travel_time >> junk >> junk >> junk >> junk >> junk >> junk >>
         rest_time >> junk) {
    deer.push_back(Deer(name, speed, travel_time, rest_time));
    distance[name] = 0;
    points[name] = 0;
  }

  for (int i = 1; i <= 2503; ++i) {
    int max = 0;
    for (auto &d : deer) {
      distance[d.name] += d.distance();
      d.move();

      if (distance[d.name] > max) {
	max = distance[d.name];
      }
    }

    for (auto const &d : distance) {
      if (d.second == max) {
        ++points[d.first];
      }
    }
  }

  int max = 0;

  for (auto const &d : points) {
    // std::cout << d.first << " " << d.second << std::endl;
    if (d.second > max) {
      max = d.second;
    }
  }

  std::cout << max << std::endl;

  return 0;
}
