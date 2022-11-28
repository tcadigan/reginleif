#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

int main(int argc, char *argv[]){
    int tests;

    cin >> tests;

    vector<int> grades;
    while(tests){
        int students;

        cin >> students;

        int grade;
    
        double total = 0;
        for(int i = 0; i < students; ++i){
            cin >> grade;
            total += grade;
            grades.push_back(grade);
        }

        total = total/students;
        int above_avg = 0;
        for(int i = 0; i < students; ++i){
            if(grades[i] > total){
                ++above_avg;
            }
        }

        cout << fixed << setprecision(3) << int(above_avg / (double)students * 100000 + 0.5) / 1000.0 << "%" << endl;

        grades.clear();
        --tests;
    }

    return 0;
}
