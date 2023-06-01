#include <iostream>

class complex {  // sample class for
 private:
  double real;  // 　real part
  double imag;  // 　imagnary part
 public:
  complex() : real(-1.0), imag(-1.0) {  // comsractor
  }
  complex(double a, double b) : real(a), imag(b){};
  double abs() { return sqrt(real * real + imag * imag); }
};

int main() {
  // C++のクラスインスタンス
  // complex *a;
  // complex b[5, -7];
  // a = &b;
  // std::cout << "" << b.abs() << std::endl;
}