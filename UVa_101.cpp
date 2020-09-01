#include <iostream>
#include <list>
#include <map>

using namespace std;

map<int, list<int> > board;

void output() {
    for(unsigned int i = 0; i < board.size(); ++i) {
        cout << i << ":";

        if(!board.at(i).empty()) {
            list<int>::iterator itr;
            for(itr = board.at(i).begin(); itr != board.at(i).end(); ++itr) {
                cout << " " << *itr;
            }
        }

        cout << endl;
    }
}

int find_stack(int block) {
    for(unsigned int i = 0; i < board.size(); ++i) {
        for(list<int>::iterator j = board.at(i).begin(); j != board.at(i).end(); ++j) {
            if(*j == block) {
                return i;
            }
        }
    }

    return -1;
}

void restore(int stack, int value) {
    while(board.at(stack).back() != value) {
        board.at(board.at(stack).back()).push_back(board.at(stack).back());
        board.at(stack).pop_back();
    }
}

void move_onto(int a, int b) {
    if(a == b) {
        return;
    }
    
    int stack_a = find_stack(a);
    int stack_b = find_stack(b);

    if((stack_a == -1) || (stack_b == -1) || (stack_a == stack_b)) {
        return;
    }

    restore(stack_a, a);
    restore(stack_b, b);

    board.at(stack_a).pop_back();
    board.at(stack_b).push_back(a);
}

void move_over(int a, int b) {
    if(a == b) {
        return;
    }

    int stack_a = find_stack(a);
    int stack_b = find_stack(b);

    if((stack_a == -1) || (stack_b == -1) || (stack_a == stack_b)) {
        return;
    }

    restore(stack_a, a);

    board.at(stack_a).pop_back();
    board.at(stack_b).push_back(a);
}

void pile_onto(int a, int b) {
    if(a == b) {
        return;
    }

    int stack_a = find_stack(a);
    int stack_b = find_stack(b);

    if((stack_a == -1) || (stack_b == -1) || (stack_a == stack_b)) {
        return;
    }

    restore(stack_b, b);

    list<int>::iterator itr = board.at(stack_a).end();

    for(itr = board.at(stack_a).begin(); itr != board.at(stack_a).end(); ++itr) {
        if(*itr == a) {
            break;
        }
    }
    
    board.at(stack_b).splice(board.at(stack_b).end(), board.at(stack_a), itr, board.at(stack_a).end());
}

void pile_over(int a, int b) {
    if(a == b) {
        return;
    }

    int stack_a = find_stack(a);
    int stack_b = find_stack(b);

    if((stack_a == -1) || (stack_b == -1) || (stack_a == stack_b)) {
        return;
    }

    list<int>::iterator itr = board.at(stack_a).end();

    for(itr = board.at(stack_a).begin(); itr != board.at(stack_a).end(); ++itr) {
        if(*itr == a) {
            break;
        }
    }
    
    board.at(stack_b).splice(board.at(stack_b).end(), board.at(stack_a), itr, board.at(stack_a).end());
}

int main(int argc, char *argv[])
{

    bool first = true;
    
    while(!cin.eof()) {
        if(first) {
            int blocks;
            cin >> blocks;
    
            for(int i = 0; i < blocks; ++i) {
                list<int> item;

                item.push_back(i);
                board[i] = item;
            }

            first = false;
        }
        else {
            string word_one;
            string word_two;
            int a;
            int b;

            cin >> word_one >> a >> word_two >> b;
            
            if((word_one == "move") && (word_two == "onto")) {
                move_onto(a, b);
            }
            else if((word_one == "move") && (word_two == "over")) {
                move_over(a, b);
            }
            else if((word_one == "pile") && (word_two == "onto")) {
                pile_onto(a, b);
            }
            else if((word_one == "pile") && (word_two == "over")) {
                pile_over(a, b);
            }
            else if(word_one == "quit") {
                output();
                break;
            }
        }
    }
    
    return 0;
}
