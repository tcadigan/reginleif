#include <iostream>
#include <vector>

int goal = 30;
int min = 1000;

class Node
{
public:
  Node(char operation, int num) {
    if (operation == '@') {
      isOperator = false;
    } else {
      isOperator = true;
    }

    op = operation;
    number = num;
  }

  void print() const {
    std::cout << "{"
	      << "OP: \'" << op << "\' "
	      << "NUM: "<< number << "}";
  }

  bool isOperator;
  char op;
  int number;
  std::vector<Node *> neighbors;
};

void travel(Node const &node, int count, std::string path, int val, int limit) {
  // std::cout << "VISITING: ";
  // node.print();
  // std::cout << " COUNT: " << count
  // 	    << " PATH: \'" << path << "\'"
  // 	    << " VAL: " << val << std::endl;

  if ((val < 0)
      || (count > min)
      || ((count > 0) && (node.number == 22))) {
    return;
  }

  if (node.isOperator) {
    path += node.op;
  } else {
    path += std::to_string(node.number);
  }

  if (count == limit) {
    if ((val == goal) && (node.number == 1)) {
      std::cout << "PATH: " << path << std::endl;
      min = count;
    }

    return;
  }

  // Disallow return to lock
  if (node.number == 1) {
    return;
  }

  for (auto const &neighbor : node.neighbors) {
    // std::cout << "NEIGHBOR: ";
    // neighbor->print();
    // std::cout << std::endl;

    if (!neighbor->isOperator) {
      if (node.op == '-') {
	travel(*neighbor, count + 1, path, val - neighbor->number, limit);
      } else if (node.op == '*') {
	travel(*neighbor, count + 1, path, val * neighbor->number, limit);
      } else if (node.op == '+') {
	travel(*neighbor, count + 1, path, val + neighbor->number, limit);
      }
    } else {
      travel(*neighbor, count + 1, path, val, limit);
    }
  }
}


int main(int argc, char *argv[])
{
  Node g_0_0('@', 22);
  Node g_1_0('-', 0);
  Node g_2_0('@', 9);
  Node g_3_0('*', 0);
  Node g_0_1('+', 0);
  Node g_1_1('@', 4);
  Node g_2_1('-', 0);
  Node g_3_1('@', 18);
  Node g_0_2('@', 4);
  Node g_1_2('*', 0);
  Node g_2_2('@', 11);
  Node g_3_2('*', 0);
  Node g_0_3('*', 0);
  Node g_1_3('@', 8);
  Node g_2_3('-', 0);
  Node g_3_3('@', 1);

  // Row 1
  g_0_0.neighbors.push_back(&g_0_1);
  g_0_0.neighbors.push_back(&g_1_0);

  g_1_0.neighbors.push_back(&g_0_0);
  g_1_0.neighbors.push_back(&g_1_1);
  g_1_0.neighbors.push_back(&g_2_0);

  g_2_0.neighbors.push_back(&g_1_0);
  g_2_0.neighbors.push_back(&g_2_1);
  g_2_0.neighbors.push_back(&g_3_0);

  g_3_0.neighbors.push_back(&g_2_0);
  g_3_0.neighbors.push_back(&g_3_1);

  // Row 2
  g_0_1.neighbors.push_back(&g_0_2);
  g_0_1.neighbors.push_back(&g_1_1);
  g_0_1.neighbors.push_back(&g_0_0);

  g_1_1.neighbors.push_back(&g_0_1);
  g_1_1.neighbors.push_back(&g_1_2);
  g_1_1.neighbors.push_back(&g_2_1);
  g_1_1.neighbors.push_back(&g_1_0);

  g_2_1.neighbors.push_back(&g_1_1);
  g_2_1.neighbors.push_back(&g_2_2);
  g_2_1.neighbors.push_back(&g_3_1);
  g_2_1.neighbors.push_back(&g_2_0);

  g_3_1.neighbors.push_back(&g_2_1);
  g_3_1.neighbors.push_back(&g_3_2);
  g_3_1.neighbors.push_back(&g_3_0);

  // Row 2
  g_0_2.neighbors.push_back(&g_0_3);
  g_0_2.neighbors.push_back(&g_1_2);
  g_0_2.neighbors.push_back(&g_0_1);

  g_1_2.neighbors.push_back(&g_0_2);
  g_1_2.neighbors.push_back(&g_1_3);
  g_1_2.neighbors.push_back(&g_2_2);
  g_1_2.neighbors.push_back(&g_1_1);

  g_2_2.neighbors.push_back(&g_1_2);
  g_2_2.neighbors.push_back(&g_2_3);
  g_2_2.neighbors.push_back(&g_3_2);
  g_2_2.neighbors.push_back(&g_2_1);

  g_3_2.neighbors.push_back(&g_2_2);
  g_3_2.neighbors.push_back(&g_3_3);
  g_3_2.neighbors.push_back(&g_3_1);

  // Row 4
  g_0_3.neighbors.push_back(&g_1_3);
  g_0_3.neighbors.push_back(&g_0_2);

  g_1_3.neighbors.push_back(&g_0_3);
  g_1_3.neighbors.push_back(&g_2_3);
  g_1_3.neighbors.push_back(&g_1_2);

  g_2_3.neighbors.push_back(&g_1_3);
  g_2_3.neighbors.push_back(&g_3_3);
  g_2_3.neighbors.push_back(&g_2_2);

  g_3_3.neighbors.push_back(&g_2_3);
  g_3_3.neighbors.push_back(&g_3_2);

  for (unsigned int i = 0; i < 20; ++i) {
    travel(g_0_0, 0, "", g_0_0.number, i);
  }

  return 0;
}
