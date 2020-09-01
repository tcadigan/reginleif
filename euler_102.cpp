#include <iostream>
#include <vector>

using namespace std;

class Point {
public:
    Point(long long x, long long y, long long z)
    {
        points_.push_back(x);
        points_.push_back(y);
        points_.push_back(z);
    }

    virtual ~Point()
    {
    }

    Point cross(Point const &p) const
    {
        long long x = (points_.at(1) * p.points_.at(2)) - (points_.at(2) * p.points_.at(1));
        long long y = (points_.at(2) * p.points_.at(0)) - (points_.at(0) * p.points_.at(2));
        long long z = (points_.at(0) * p.points_.at(1)) - (points_.at(1) * p.points_.at(0));

        return Point(x, y, z);
    }

    long long dot(Point const &p) const
    {
        long long result = 0;
        for(unsigned int i = 0; i < points_.size(); ++i) {
            result += (points_.at(i) * p.points_.at(i));
        }

        return result;
    }

    Point operator-(Point const &p) const
    {
        long long x = points_.at(0) - p.points_.at(0);
        long long y = points_.at(1) - p.points_.at(1);
        long long z = points_.at(2) - p.points_.at(2);

        return Point(x, y, z);
    }

private:
    vector<long long> points_;
};

int check(Point const &A, Point const &B, Point const &C)
{
    Point Z(0, 0, 0);

    Point R1(A - B);
    Point R2(A - C);
    Point R3(A - Z);

    Point C1(R1.cross(R2));
    Point C2(R1.cross(R3));

    if(C1.dot(C2) >= 0) {
        return true;
    }
    else {
        return false;
    }
}

int main(int argc, char *argv[])
{
    int x1;
    int x2;
    int y1;
    int y2;
    int z1;
    int z2;
    char c;
    int result = 0;
    while(cin >> x1 >> c >> x2 >> c >> y1 >> c >> y2 >> c >> z1 >> c >> z2) {
        Point A(x1, x2, 0);
        Point B(y1, y2, 0);
        Point C(z1, z2, 0);

        if(check(A, B, C) && check(B, C, A) && check(C, A, B)) {
            ++result;
        }
    }

    cout << "result -> " << result << endl;

    return 0;
}
