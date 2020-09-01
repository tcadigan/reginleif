#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <string>

using namespace std;

class node{
public:
    node *left;
    node *right;
    int value;
    bool used;
    node(){
        used = false;
        value = 0;
        left = NULL;
        right = NULL;
    }

    void toString(){
        cout << value << endl;
    }
};

node *build(int level){
    node *current = new node();
    char nextInput;
    cin >> skipws >> nextInput;

    //cout << "At begin: " << nextInput << endl;

    if(nextInput != '(' && nextInput != ')'){
        cin.putback(nextInput);
        cin >> current->value;
        current->used = true;
        //cout << "new node with: " << current->value << " at level: " << level << endl;

        cin >> nextInput;
        //cout << "At middle #1: " << nextInput << endl;
        current->left = build(level+1);
        //cout << "completed left node of node: " << current->value << endl;
        cin >> nextInput;
        //cout << "At middle #2: " << nextInput << endl;
        current->right = build(level+1);
        //cout << "completed right node of node: " << current->value << endl;
        cin >> nextInput;
    }
    else if(nextInput == ')'){
        //cout << "returning from level: " << level << endl;
        return current;
    }

    //cout << "returning from level: " << level << endl;
    return current;
}

bool findPath(node *root, int sum, int target){
    bool result = false;
    bool result2 = false;
    bool traveled = false;

    if(root != NULL){
        if(root->used){
            sum += root->value;
        }
        //cout << "at " << root->value << " sum " << sum << endl;
        if((root->left && root->left->used)){
            traveled = true;
            result = findPath(root->left, sum, target);
        }
        if(root->right && root->right->used){
            traveled = true;
            result2 = findPath(root->right, sum, target);
        }
        if(!traveled){
            if(sum == target){
                return true;
            }
            else{
                return false;
            }
        }
        else{
            return result || result2;
        }
    }
  
    return false;    
}

void preOrder(node *root){
    if(root != NULL){
        cout << root->value << endl;
        preOrder(root->left);
        preOrder(root->right);
    }
}

int main(int argc, char* argv[]){
    int sum;
    char outer;

    while(cin >> sum){
        //cout << "sum: " << sum << endl;
        cin >> outer;
        node *root = build(0);
        //preOrder(root);
        if(findPath(root, 0, sum) && root->used){
            cout << "yes" << endl;
        }
        else{
            cout << "no" << endl;
        }
    }

    return 0;
}
