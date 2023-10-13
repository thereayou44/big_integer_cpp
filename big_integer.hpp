#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

enum Signs { ZERO = 0, PLUS = 1, MINUS = -1 };
class BigInt {
 private:
  const int kWidth = 9;
  const int kBase = 1e9;
  int sign_ = 0;
  std::vector<long long> parts_;

 public:
  void RemoveLeadingZeroes();

  BigInt() = default;

  ~BigInt();

  BigInt(const std::string&);

  BigInt(const int64_t&);

  BigInt(const BigInt&);

  BigInt& operator=(BigInt);

  void PlusAndFix(BigInt);

  BigInt& operator+=(BigInt);

  friend BigInt operator+(BigInt, BigInt);

  friend BigInt operator-(BigInt, BigInt);

  void MinusAndFix(BigInt);

  BigInt& operator-=(BigInt);

  BigInt& operator-();

  BigInt MultSingle(long long, size_t) const;

  friend BigInt operator*(BigInt, BigInt);

  BigInt& operator*=(BigInt);

  friend BigInt operator*(BigInt, long long);

  BigInt operator*=(const long long&);

  BigInt& Abs();

  static long long BinarySearch(BigInt, BigInt);

  void ShiftByOne();

  friend BigInt operator/(BigInt, BigInt);

  BigInt& operator/=(const BigInt&);

  friend BigInt operator%(BigInt&, const BigInt&);

  BigInt& operator%=(BigInt&);

  friend bool operator==(BigInt, BigInt);

  friend bool operator!=(BigInt, BigInt);

  friend bool operator>(BigInt, BigInt);

  friend bool operator>=(BigInt, BigInt);

  friend bool operator<(BigInt, BigInt);

  friend bool operator<=(BigInt, BigInt);

  BigInt& operator++();

  BigInt operator++(int);

  BigInt operator--(int);

  BigInt& operator--();

  friend std::ostream& operator<<(std::ostream&, const BigInt&);

  friend std::istream& operator>>(std::istream&, BigInt&);
};

