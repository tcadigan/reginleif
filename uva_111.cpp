#include <iostream>
#include <vector>

using namespace std;

void compute(vector<int> &master, vector<int> &student) {
    vector<vector<int> > table;

    for(unsigned int i = 0; i < master.size(); ++i) {
        vector<int> v;
        v.resize(student.size(), 0);
        table.push_back(v);
    }

    for(unsigned int i = 1; i < master.size(); ++i) {
        for(unsigned int j = 1; j < student.size(); ++j) {
            if(master.at(i) == student.at(j)) {
                table.at(i).at(j) = table.at(i - 1).at(j  - 1) + 1;
            }
            else {
                if(table.at(i).at(j - 1) < table.at(i - 1).at(j)) {
                    table.at(i).at(j) = table.at(i - 1).at(j);
                }
                else {
                    table.at(i).at(j) = table.at(i).at(j - 1);
                }
            }
        }
    }

    cout << table.at(master.size() - 1).at(student.size() - 1) << endl;
}

int main(int argc, char *argv[]) {
    int events;

    cin >> events;

    vector<int> master;
    master.resize(events + 1, 0);
    for(int i = 1; i <= events; ++i) {
        int number;

        cin >> number;

        master[number] = i;
    }

    while(true) {
        vector<int> student;
        student.resize(events + 1, 0);
        for(int i = 1; i <= events; ++i) {
            int number;
            cin >> number;

            if(cin.eof()) {
                return 0;
            }

            student[number] = i;
        }

        compute(master, student);
    }
    
    return 0;
}
