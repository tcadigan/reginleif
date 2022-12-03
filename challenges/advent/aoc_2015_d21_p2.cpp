#include <iostream>
#include <vector>

class Person {
public:
  Person(int hp, int atk, int def)
    : hp(hp)
    , atk(atk)
    , def(def)
  {}

  int getHealth() const { return hp; }
  int getAttack() const { return atk; }

  void hit(int attack) {
    if (def >= attack) {
      --hp;
    } else {
      hp -= (attack - def);
    }
  }

private:
  int hp;
  int atk;
  int def;
};

class Item {
public:
  Item(std::string name, int cost, int damage, int armor)
    : name(name)
    , cost(cost)
    , damage(damage)
    , armor(armor)
  {}

  friend bool operator==(Item const &lhs, Item const &rhs) {
    return (lhs.name == rhs.name);
  }

  std::string getName() const { return name; }
  int getCost() const { return cost; }
  int getDamage() const { return damage; }
  int getArmor() const { return armor; }

private:
  std::string name;
  int cost;
  int damage;
  int armor;
};

bool fight(Person player, Person enemy) {
  // int round = 0;
  // std::cout << "Round " << round << ":"
  // 	    << " player hp: " << player.getHealth()
  // 	    << " enemy hp: " << enemy.getHealth() << std::endl;

  while ((player.getHealth() > 0) && (enemy.getHealth() > 0)) {
    // ++round;
    enemy.hit(player.getAttack());

    if (enemy.getHealth() > 0) {
      player.hit(enemy.getAttack());
    }

    // std::cout << "Round " << round << ":"
    // 	      << " player hp: " << player.getHealth()
    // 	      << " enemy hp: " << enemy.getHealth() << std::endl;
  }

  return (player.getHealth() > 0);
}

int main(int argc, char * argv[])
{
  std::vector<Item> weapons;
  weapons.push_back(Item("Dagger", 8, 4, 0));
  weapons.push_back(Item("Shortsword", 10, 5, 0));
  weapons.push_back(Item("Warhammer", 25, 6, 0));
  weapons.push_back(Item("Longsword", 40, 7, 0));
  weapons.push_back(Item("GreatAxe", 74, 8, 0));

  std::vector<Item> armor;
  armor.push_back(Item("None", 0, 0, 0));
  armor.push_back(Item("Leather", 13, 0, 1));
  armor.push_back(Item("Chainmail", 31, 0, 2));
  armor.push_back(Item("Splintmail", 53, 0, 3));
  armor.push_back(Item("Bandedmail", 75, 0, 4));
  armor.push_back(Item("Platemail", 102, 0, 5));

  std::vector<Item> rings;
  rings.push_back(Item("None", 0, 0, 0));
  rings.push_back(Item("Damage +1", 25, 1, 0));
  rings.push_back(Item("Damage +2", 50, 2, 0));
  rings.push_back(Item("Damage +3", 100, 3, 0));
  rings.push_back(Item("Defense +1", 20, 0, 1));
  rings.push_back(Item("Defense +2", 40, 0, 2));
  rings.push_back(Item("Defense +3", 80, 0, 3));

  std::string content;
  int enemy_hp;
  int enemy_atk;
  int enemy_def;
  std::string junk;

  std::cin >> junk >> junk >> enemy_hp >> junk >> enemy_atk >> junk >> enemy_def;

  std::cout << "Enemy:"
	    << " hp: " << enemy_hp
	    << " atk: " << enemy_atk
	    << " def: " << enemy_def << std::endl;

  int max_cost = 0;
  for (auto const &weapon : weapons) {
    for (auto const &armor : armor) {
      for (auto const &right : rings) {
	for (auto const &left : rings) {
	  if ((right == left) && (right.getName() != "None")) {
	    continue;
	  }

	  int cost = weapon.getCost() + armor.getCost() + right.getCost() + left.getCost();
	  int attack = weapon.getDamage() + armor.getDamage() + right.getDamage() + left.getDamage();
	  int defense = weapon.getArmor() + armor.getArmor() + right.getArmor() + left.getArmor();
	  Person player(100, attack, defense);
	  Person enemy(enemy_hp, enemy_atk, enemy_def);

	  bool victory = fight(player, enemy);

          if (!victory && (cost > max_cost)) {
	    std::cout << "Using: "
		      << " " << weapon.getName()
		      << " " << armor.getName()
		      << " " << right.getName()
		      << " " << left.getName() << std::endl;
            max_cost = cost;
          }
        }
      }
    }
  }

  std::cout << max_cost << std::endl;

  return 0;
}
