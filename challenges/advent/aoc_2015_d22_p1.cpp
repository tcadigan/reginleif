#include <iostream>
#include <vector>

class Effect {
public:
  Effect(std::string name, int turns, int dmg, int def, int mana)
    : name(name)
    , turns(turns)
    , dmg(dmg)
    , def(def)
    , mana(mana)
  {}

  std::string getName() const { return name; }
  int getDmg() const { return dmg; }
  int getDef() const { return def; }
  int getMana() const { return mana; }
  int getTurns() const { return turns; }

  void spendTurn() { --turns; }

private:
  std::string name;
  int turns;
  int dmg;
  int def;
  int mana;
};

class Person {
public:
  Person(std::string name, bool isPlayer, int hp, int atk, int def, int mana)
    : name(name)
    , isPlayer(isPlayer)
    , hp(hp)
    , atk(atk)
    , def(def)
    , mana(mana)
  {}

  std::string getName() const { return name; }
  int getHp() const { return hp; }
  int getAtk() const { return atk; }
  int getMana() const { return mana; }

  void addHp(int add) {
    hp += add;
  }

  void addMana(int add) {
    mana += add;
  }

  void addDef(int add) { def += add; }

  void damage(int atk, bool direct) {
    if (direct) {
      hp -= atk;
    } else if (def >= atk) {
      --hp;
    } else {
      hp -= (atk - def);
    }
  }

  void applyEffect(Effect const &effect) {
    if ((effect.getName() == "Shield") && isPlayer) {
      if (effect.getTurns() == 6) {
        def += 7;
      }
    } else if ((effect.getName() == "Recharge") && isPlayer) {
      mana += 101;
    } else if ((effect.getName() == "Poison") && !isPlayer) {
      hp -= 3;
    }
  }

  void print() {
    std::cout << "- " << name << " has"
	      << " " << hp << " hit points,"
	      << " " << atk << " attack,"
	      << " " << def << " armor,"
	      << " " << mana << " mana." << std::endl;
  }

private:
  std::string name;
  bool isPlayer;
  int hp;
  int atk;
  int def;
  int mana;
};

int min = 1000000000;

class State {
public:
  State(Person player, Person enemy, std::vector<Effect> effects, std::vector<std::string> actions, int used, bool playerTurn)
    : player(player)
    , enemy(enemy)
    , effects(effects)
    , actions(actions)
    , playerTurn(playerTurn)
    , complete(false)
    , victory(false)
    , used(used)
  {}

  void processEffects() {
    std::vector<Effect> active;
    for (auto &effect : effects) {
      player.applyEffect(effect);
      enemy.applyEffect(effect);
      effect.spendTurn();

      if (effect.getTurns() > 0) {
        active.push_back(effect);
      } else if (effect.getName() == "Shield") {
	player.addDef(-7);
      }
    }

    effects = active;
  }

  void takeTurn() {
    if (used > min) {
      complete = true;
      victory = false;

      return;
    }

    processEffects();

    if ((player.getHp() <= 0) || (enemy.getHp() <= 0)) {
      complete = true;
      victory = (player.getHp() > 0);


      if (victory && (used < min)) {
	min = used;
	std::cout << "Victory: Used: " << used << std::endl;
      }

      return;
    }

    if (!playerTurn) {
      Person self = player;
      Person other = enemy;
      std::vector<Effect> effs = effects;
      std::vector<std::string> strs = actions;
      strs.push_back("Attack");
      self.damage(other.getAtk(), false);
      State state(self, other, effs, strs, used, !playerTurn);
      state.takeTurn();
    } else {
      if (player.getMana() < 53) {
	complete = true;
	victory = false;

	return;
      }

      if (player.getMana() >= 53) {
        Person self = player;
        Person other = enemy;
	std::vector<Effect> effs = effects;
	std::vector<std::string> strs = actions;
	strs.push_back("Magic Missile");
        self.addMana(-53);
        other.damage(4, true);
	State state(self, other, effs, strs, 53 + used, !playerTurn);
	state.takeTurn();
      }

      if (player.getMana() >= 73) {
	Person self = player;
	Person other = enemy;
	std::vector<Effect> effs = effects;
	std::vector<std::string> strs = actions;
	strs.push_back("Drain");
	self.addMana(-73);
	self.addHp(2);
	other.addHp(-2);
	State state(self, other, effs, strs, 73 + used, !playerTurn);
	state.takeTurn();
      }

      if (player.getMana() >= 113) {
	Effect effect("Shield", 6, 0, 7, 0);

	bool active = false;
	for (auto const &eff : effects) {
	  if (effect.getName() == eff.getName()) {
	    active = true;
	    break;
	  }
	}

        if (!active) {
          Person self = player;
          Person other = enemy;
	  std::vector<Effect> effs = effects;
	  std::vector<std::string> strs = actions;
	  strs.push_back("Shield");
	  self.addMana(-113);
          effs.push_back(effect);
	  State state(self, other, effs, strs, 113 + used, !playerTurn);
	  state.takeTurn();
        }
      }

      if (player.getMana() >= 173) {
	Effect effect("Poison", 6, 3, 0, 0);

	bool active = false;
	for (auto const &eff : effects) {
	  if (effect.getName() == eff.getName()) {
	    active = true;
	    break;
	  }
	}

        if (!active) {
          Person self = player;
          Person other = enemy;
	  std::vector<Effect> effs = effects;
	  std::vector<std::string> strs = actions;
	  strs.push_back("Poison");
	  self.addMana(-173);
          effs.push_back(effect);
	  State state(self, other, effs, strs, 173 + used, !playerTurn);
	  state.takeTurn();
        }
      }

      if (player.getMana() >= 229) {
	Effect effect("Recharge", 5, 0, 0, 101);

	bool active = false;
	for (auto const &eff : effects) {
	  if (effect.getName() == eff.getName()) {
	    active = true;
	    break;
	  }
	}

        if (!active) {
          Person self = player;
          Person other = enemy;
	  std::vector<Effect> effs = effects;
	  std::vector<std::string> strs = actions;
	  strs.push_back("Recharge");
          self.addMana(-229);
          effs.push_back(effect);
	  State state(self, other, effs, strs, 229 + used, !playerTurn);
	  state.takeTurn();
        }
      }
    }
  }

private : Person player;
  Person enemy;
  std::vector<Effect> effects;
  std::vector<std::string> actions;
  bool playerTurn;
  bool complete;
  bool victory;
  int used;
};

int main(int arc, char *argv[])
{
  std::string junk;
  int enemy_hp;
  int enemy_dmg;

  std::cin >> junk >> junk >> enemy_hp >> junk >> enemy_dmg;

  Person player("Player", true, 50, 0, 0, 500);
  Person enemy("Enemy", false, enemy_hp, enemy_dmg, 0, 0);
  // Person player("Player", true, 10, 0, 0, 250);
  // Person enemy("Enemy", false, 14, 8, 0, 0);
  std::vector<Effect> effects;
  std::vector<std::string> actions;
  State state(player, enemy, effects, actions, 0, true);

  state.takeTurn();

  std::cout << min << std::endl;

  return 0;
}
