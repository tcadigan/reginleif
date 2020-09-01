#include <iostream>
#include <vector>

using namespace std;

class Polynomial {
public:
    Polynomial() {}
    virtual ~Polynomial() {}

    void set_polynomial(vector<long long> polynomial)
    {
        polynomial_ = polynomial;
    }

    long long evaluate_at(long long value) const
    {
        long long total = 0;
        for(unsigned int i = 0; i < polynomial_.size(); ++i) {
            long long temp = value;

            if(i == 0) {
                temp = 1;
            }
            else {
                for(unsigned int j = 1; j < i; ++j) {
                    temp *= value;
                }
            }

            total += (temp * polynomial_.at(i));
        }

        return total;
    }

    void print_polynomial(bool newline) const
    {
        vector<long long>::const_reverse_iterator itr;

        unsigned int exp = polynomial_.size() - 1;
        for(itr = polynomial_.rbegin(); itr != polynomial_.rend(); ++itr) {
            long long value = *itr;

            if(*itr != 0) {   
                if(itr != polynomial_.rbegin()) {
                    if(value < 0) {
                        value *= -1;
                        cout << " - ";
                    }
                    else {
                        cout << " + ";
                    }
                }
                
                if(exp > 0) {
                    if(*itr != 1) {
                        cout << value << " * ";
                    }
                    
                    cout << "x";
                    
                    if(exp > 1) {
                        cout << " ^ " << exp;
                    }
                }
                else {
                    cout << value;
                }
            }

            --exp;
        }

        if(newline) {
            cout << endl;
        }
    }

    Polynomial operator+(Polynomial const &p) const
    {
        Polynomial result;
        if(polynomial_.empty()) {
            result.set_polynomial(p.polynomial_);
            return result;
        }
        else if(p.polynomial_.empty()) {
            result.set_polynomial(polynomial_);
            return result;
        }
        else {
            vector<long long> sum;
            if(p.polynomial_.size() > polynomial_.size()) {
                for(unsigned int i = 0; i < p.polynomial_.size(); ++i) {
                    long long value = p.polynomial_.at(i);
                    if(i < polynomial_.size()) {
                        value += polynomial_.at(i);
                    }
                    
                    sum.push_back(value);
                }
            }
            else {
                for(unsigned int i = 0; i < polynomial_.size(); ++i) {
                    long long value = polynomial_.at(i);
                    if(i < p.polynomial_.size()) {
                        value += p.polynomial_.at(i);
                    }
                    
                    sum.push_back(value);
                }
            }

            result.set_polynomial(sum);
        }

        return result;
    }

    Polynomial operator*(long long constant) const
    {
        Polynomial result;

        vector<long long> prod;
        for(unsigned int i = 0; i < polynomial_.size(); ++i) {
            prod.push_back(constant * polynomial_.at(i));
        }

        result.set_polynomial(prod);

        return result;
    }

    Polynomial operator*(Polynomial const &p) const
    {
        Polynomial result;

        vector<long long> prod;
        prod.resize(polynomial_.size() + p.polynomial_.size() - 1);
        for(unsigned int i = 0; i < polynomial_.size(); ++i) {
            for(unsigned int j = 0; j < p.polynomial_.size(); ++j) {
                prod.at(i + j) += (polynomial_.at(i) * p.polynomial_.at(j));
            }
        }

        result.set_polynomial(prod);

        return result;
    }

    Polynomial operator-(Polynomial const &p) const
    {
        Polynomial rhs;
        rhs.set_polynomial(p.polynomial_);

        Polynomial lhs;
        lhs.set_polynomial(polynomial_);

        Polynomial result(rhs * -1);
        result = lhs + result;

        return result;
    }

    Polynomial operator/(long long divisor) const
    {
        Polynomial result;

        vector<long long> values;

        for(unsigned int i = 0; i < polynomial_.size(); ++i) {
            values.push_back(polynomial_.at(i) / divisor);
        }

        result.set_polynomial(values);

        return result;
    }

private:
    vector<long long> polynomial_;
};

class Fraction {
public:
    Fraction() {}
    virtual ~Fraction() {}
    
    void set_numerator(Polynomial const &p)
    {
        numerator_ = p;
    }

    void set_denominator(long long d) {
        denominator_ = d;
    }

    void print_fraction(bool newline) const
    {
        cout << "(";
        numerator_.print_polynomial(false);
        cout << ") / " << denominator_;

        if(newline) {
            cout << endl;
        }
    }

    Polynomial reduce() const
    {
        return numerator_ / denominator_;
    }

    Fraction operator+(Fraction const &f) const
    {
        long long lcm = denominator_ / gcd(denominator_, f.denominator_);
        lcm *= f.denominator_;

        Polynomial rhs(numerator_ * (lcm / denominator_));
        Polynomial lhs(f.numerator_ * (lcm / f.denominator_));

        Fraction result;
        result.set_numerator(rhs + lhs);
        result.set_denominator(lcm);

        return result;
    }

    Fraction operator*(long long constant) const
    {
        Fraction result;
        result.set_numerator(numerator_ * constant);
        result.set_denominator(denominator_);

        return result;
    }

    Fraction operator-(Fraction const &f) const
    {
        Fraction rhs;
        rhs.set_numerator(f.numerator_);
        rhs.set_denominator(f.denominator_);

        rhs = rhs * -1;

        Fraction result;
        result.set_numerator(numerator_ + rhs.numerator_);
        result.set_denominator(denominator_ + rhs.denominator_);

        return result;
    }

private:
    long long gcd(long long a, long long b) const
    {
        while(b != 0) {
            long long temp = b;
            b = a % b;
            a = temp;
        }

        return a;
    }

    Polynomial numerator_;
    long long denominator_;
};

vector<Polynomial> generate_polynomial(int max_value, int skip_value)
{
    vector<Polynomial> result;

    for(int i = 1; i <= max_value; ++i) {
        if(i != skip_value) {
            vector<long long> v;
            v.push_back(-i);
            v.push_back(1);

            Polynomial p;
            p.set_polynomial(v);

            result.push_back(p);
        }
    }

    return result;
}

int main(int argc, char *argv[])
{
    vector<long long> base_data;
    base_data.push_back(1);
    base_data.push_back(-1);
    base_data.push_back(1);
    base_data.push_back(-1);
    base_data.push_back(1);
    base_data.push_back(-1);
    base_data.push_back(1);
    base_data.push_back(-1);
    base_data.push_back(1);
    base_data.push_back(-1);
    base_data.push_back(1);

    Polynomial base;
    base.set_polynomial(base_data);
    
    cout << "base := ";
    base.print_polynomial(true);

    vector<long long> points;
    
    for(unsigned int i = 1; i < base_data.size(); ++i) {
        points.push_back(base.evaluate_at(i));

        if(i != 1) {
            cout << ", ";
        }
        cout << points.at(i - 1);
    }
    cout << endl;

    long long bop = 1;
    for(unsigned int i = 2; i < base_data.size(); ++i) {
        vector<Fraction> fracs;

        for(unsigned int j = 1; j <= i; ++j) {
            vector<Polynomial> sample(generate_polynomial(i, j));

            Polynomial total = sample.at(0);
            for(unsigned int k = 1; k < sample.size(); ++k) {
                total = total * sample.at(k);
            }

            Fraction f;
            f.set_numerator(total);
            f.set_denominator(total.evaluate_at(j));

            fracs.push_back(f);
        }

        for(unsigned int j = 0; j < i; ++j) {
            fracs.at(j) = fracs.at(j) * points.at(j);
        }
        
        Fraction result = fracs.at(0);

        for(unsigned int j = 1; j < fracs.size(); ++j) {
            result = result + fracs.at(j);
        }
        
        Polynomial simplified(result.reduce());
        cout << "simplified := ";
        simplified.print_polynomial(true);

        for(unsigned int j = 0; j < i; ++j) {
            if(j != 0) {
                cout << ", ";
            }

            cout << points.at(j);
        }
        
        cout << ", " << simplified.evaluate_at(i + 1) << endl;

        bop += simplified.evaluate_at(i + 1);
    }

    cout << "bop -> " << bop << endl;
    
    return 0;
}
