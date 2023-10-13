#include "big_integer.hpp"

void BigInt::RemoveLeadingZeroes() {
  while (parts_.size() > 1 && parts_.back() == 0) {
    parts_.pop_back();
  }
  if (parts_.back() == 0) {
    sign_ = ZERO;
  }
}

BigInt::~BigInt() { parts_.clear(); }

BigInt::BigInt(const std::string& to_copy) {
  parts_.clear();
  if (to_copy == "0" || to_copy == "-0") {
    sign_ = 0;
  } else {
    sign_ = (to_copy[0] == '-' ? MINUS : PLUS);
    size_t length = (to_copy.length() - 1) / kWidth + 1;
    int part;
    for (size_t iter = 0; iter < length; iter++) {
      int end = to_copy.length() - iter * kWidth;
      int start = std::max((int)(sign_ == MINUS), end - kWidth);
      part = std::stoi(to_copy.substr(start, end - start));
      parts_.push_back(part);
    }
  }
}

BigInt::BigInt(const int64_t& to_copy) {
  int64_t tmp = llabs(to_copy);
  if (to_copy == 0) {
    sign_ = ZERO;
    parts_ = {0};
  } else {
    sign_ = (to_copy > 0 ? PLUS : MINUS);
    do {
      parts_.push_back(llabs(tmp % kBase));
      tmp /= kBase;
    } while (tmp != 0);
  }
}

BigInt::BigInt(const BigInt& to_assign) {
  sign_ = to_assign.sign_;
  parts_ = to_assign.parts_;
}

BigInt& BigInt::operator=(BigInt to_assign) {
  sign_ = to_assign.sign_;
  parts_ = to_assign.parts_;
  return *this;
}

void BigInt::PlusAndFix(BigInt to_add) {
  int size = (int)std::min(parts_.size(), (to_add.parts_.size()));
  int i;
  for (i = 0; i < size; i++) {
    parts_[i] += to_add.parts_[i];
    if (parts_[i] >= kBase && i != (int)(parts_.size() - 1)) {
      int remainder = parts_[i] / kBase;
      parts_[i] %= kBase;
      parts_[i + 1] += remainder;
    }
  }
  for (; i < (int)(to_add.parts_.size()); i++) {
    int remainder = 0;
    if (parts_.back() >= kBase) {
      remainder = parts_.back() / kBase;
      parts_.back() %= kBase;
    }
    parts_.push_back(to_add.parts_[i] + remainder);
  }
  if (parts_.back() >= kBase) {
    int remainder = parts_.back() / kBase;
    parts_.back() %= kBase;
    parts_.push_back(remainder);
  }
}

BigInt& BigInt::operator+=(BigInt to_add) {
  if (sign_ == ZERO || to_add.sign_ == ZERO) {
    *this = (sign_ == ZERO ? to_add : *this);
    return *this;
  }
  if (to_add.sign_ == MINUS) {
    to_add.sign_ = PLUS;
    *this = *this - to_add;
    to_add.sign_ = MINUS;
    return *this;
  }
  if (sign_ == to_add.sign_) {
    this->PlusAndFix(to_add);
    return *this;
  }
  to_add.sign_ = PLUS;
  sign_ = PLUS;
  BigInt result = (to_add.sign_ == MINUS ? (*this - to_add) : (to_add - *this));
  *this = result;
  return *this;
}

BigInt operator+(BigInt first, BigInt second) {
  if (first.sign_ == ZERO || second.sign_ == ZERO) {
    return (first.sign_ == ZERO ? second : first);
  }
  BigInt result(first);
  if (second.sign_ == MINUS) {
    second.sign_ = PLUS;
    result -= second;
    return result;
  }
  result += second;
  return result;
}

BigInt operator-(BigInt first, BigInt second) {
  if (second.sign_ == ZERO || first.sign_ == ZERO) {
    return (first.sign_ == ZERO ? -second : first);
  }
  BigInt result(first);
  if (second.sign_ == MINUS) {
    second.sign_ = PLUS;
    return result += second;
  }
  result -= second;
  result.RemoveLeadingZeroes();
  return result;
}
void BigInt::MinusAndFix(BigInt to_sub) {
  int size = (int)std::min(parts_.size(), (to_sub.parts_.size()));
  for (int i = 0; i < size; i++) {
    parts_[i] -= to_sub.parts_[i];
  }
  for (size_t i = 0; i < parts_.size(); i++) {
    if (parts_[i] < 0 && i != parts_.size() - 1) {
      parts_[i] += kBase;
      parts_[i + 1]--;
    }
  }
  if (parts_[parts_.size() - 1] < 0) {
    parts_[parts_.size() - 2] += parts_[parts_.size() - 1];
    parts_.pop_back();
  }
  this->RemoveLeadingZeroes();
}

BigInt& BigInt::operator-=(BigInt to_sub) {
  if (sign_ == ZERO || to_sub.sign_ == ZERO) {
    *this = (sign_ == ZERO ? -to_sub : *this);
    return *this;
  }
  if (to_sub.sign_ == MINUS) {
    to_sub.sign_ = PLUS;
    *this = *this + to_sub;
    return *this;
  }
  if (sign_ == to_sub.sign_) {
    if (to_sub > *this) {
      BigInt result = to_sub - *this;
      result.sign_ *= -1;
      *this = result;
      return *this;
    }
    this->MinusAndFix(to_sub);
    return *this;
  }
  sign_ = PLUS;
  BigInt result(*this + to_sub);
  result.sign_ = MINUS;
  result.RemoveLeadingZeroes();
  *this = result;
  return *this;
}

BigInt& BigInt::operator-() {
  sign_ *= -1;
  return *this;
}

BigInt BigInt::MultSingle(long long num, size_t shift) const {
  BigInt result;
  result.sign_ = sign_;
  long long remainder = 0;

  for (const auto& item : parts_) {
    long long mul_value = item * num + remainder;
    remainder = mul_value / kBase;
    result.parts_.push_back(mul_value % kBase);
  }

  if (remainder != 0) {
    result.parts_.push_back(remainder);
  }

  if (shift != 0) {
    for (size_t i = 0; i < shift; i++) {
      result.ShiftByOne();
    }
  }
  return result;
}

BigInt operator*(BigInt first, BigInt to_mul) {
  BigInt result;
  if (first.sign_ == ZERO || to_mul.sign_ == ZERO) {
    result.sign_ = ZERO;
    result.parts_ = {0};
    return result;
  }
  for (size_t i = 0; i < to_mul.parts_.size(); i++) {
    BigInt layer(first.MultSingle(to_mul.parts_[i], i));
    result += layer;
  }
  result.sign_ = (first.sign_ == to_mul.sign_) ? PLUS : MINUS;

  result.RemoveLeadingZeroes();
  return result;
}

BigInt& BigInt::operator*=(BigInt to_mul) {
  BigInt tmp = *this * to_mul;
  *this = tmp;
  return *this;
}

BigInt operator*(BigInt my_int, long long to_mul) {
  BigInt tmp(to_mul);
  return my_int * tmp;
}

BigInt BigInt::operator*=(const long long& to_mul) {
  BigInt tmp(*this * to_mul);
  *this = tmp;
  return *this;
}

BigInt& BigInt::Abs() {
  sign_ = PLUS;
  return *this;
}

long long BigInt::BinarySearch(BigInt devisor, BigInt current) {
  const long long kMax = 1e9;
  long long lowest = 0;
  long long bigger = kMax;
  long long mid;
  long long x = 0;
  while (lowest <= bigger) {
    BigInt tmp(devisor);
    mid = (lowest + bigger) / 2;
    tmp *= mid;
    if (tmp == current) {
      return mid;
    }
    if (tmp <= current) {
      lowest = mid + 1;
      x = mid;
    } else {
      bigger = mid - 1;
    }
  }
  return x;
}

void BigInt::ShiftByOne() {
  parts_.push_back(0);
  std::rotate(parts_.rbegin(), parts_.rbegin() + 1, parts_.rend());
}

BigInt operator/(BigInt first, BigInt second) {
  BigInt result(0);
  result.parts_.resize(first.parts_.size() + 1);
  result.sign_ = (first.sign_ == second.sign_ ? PLUS : MINUS);
  first.Abs();
  second.Abs();
  if (second > first || first.sign_ == ZERO) {
    result.sign_ = ZERO;
    result.parts_ = {0};
    return result;
  }
  BigInt current;
  current.sign_ = PLUS, current.parts_.resize(1);
  bool flag = false;
  for (int i = first.parts_.size() - 1; i >= 0; i--) {
    if (flag) {
      current.ShiftByOne();
    }
    current.parts_[0] = first.parts_[i];
    current.RemoveLeadingZeroes();
    flag = true;
    long long part = BigInt::BinarySearch(second, current);
    result.parts_[i] = part;
    BigInt mul(second * part);
    mul.RemoveLeadingZeroes();
    current -= mul;
  }
  result.RemoveLeadingZeroes();
  return result;
}

BigInt& BigInt::operator/=(const BigInt& to_div) {
  BigInt tmp(*this / to_div);
  *this = tmp;
  return *this;
}

BigInt operator%(BigInt& first, const BigInt& to_mod) {
  BigInt tmp(first - (first / to_mod) * to_mod);
  tmp.RemoveLeadingZeroes();
  return tmp;
}

BigInt& BigInt::operator%=(BigInt& to_mod) {
  BigInt tmp(*this - (*this / to_mod) * to_mod);
  *this = tmp;
  return *this;
}

bool operator==(BigInt first, BigInt to_compare) {
  if (first.sign_ != to_compare.sign_ ||
      first.parts_.size() != to_compare.parts_.size()) {
    return false;
  }

  for (int i = (int)(first.parts_.size() - 1); i >= 0; i--) {
    if (first.parts_[i] != to_compare.parts_[i]) {
      return false;
    }
  }
  return true;
}

bool operator!=(BigInt first, BigInt to_compare) {
  if (first.sign_ != to_compare.sign_ ||
      first.parts_.size() != to_compare.parts_.size()) {
    return (first.sign_ != to_compare.sign_ ||
            first.parts_.size() != to_compare.parts_.size());
  }

  for (int i = (int)(first.parts_.size() - 1); i >= 0; i--) {
    if (first.parts_[i] != to_compare.parts_[i]) {
      return true;
    }
  }
  return false;
}
bool operator>(BigInt first, BigInt to_compare) {
  if (first.sign_ != to_compare.sign_) {
    return (first.sign_ > to_compare.sign_);
  }
  if (first.parts_.size() != to_compare.parts_.size()) {
    return (first.parts_.size() > to_compare.parts_.size());
  }

  for (int i = (int)(first.parts_.size() - 1); i >= 0; i--) {
    if (first.parts_[i] > to_compare.parts_[i]) {
      return (first.sign_ != MINUS);
    }
    if (first.parts_[i] < to_compare.parts_[i]) {
      return (first.sign_ == MINUS);
    }
  }
  return false;
}

bool operator>=(BigInt first, BigInt to_compare) {
  if (first > to_compare || first == to_compare) {
    return (first > to_compare || first == to_compare);
  }
  return false;
}
bool operator<(BigInt first, BigInt to_compare) {
  return !(first >= to_compare);
}
bool operator<=(BigInt first, BigInt to_compare) {
  return !(first > to_compare);
}

BigInt& BigInt::operator++() {
  *this += 1;
  return (*this);
}

BigInt BigInt::operator++(int) {
  BigInt tmp(*this);
  *this += 1;
  return (tmp);
}

BigInt BigInt::operator--(int) {
  BigInt tmp(*this);
  *this -= 1;
  return tmp;
}

BigInt& BigInt::operator--() {
  *this -= 1;
  return *this;
}

std::ostream& operator<<(std::ostream& os, const BigInt& to_cout) {
  const int kMaxWidth = 9;
  if (to_cout.sign_ == ZERO) {
    os << '0';
    return os;
  }
  if (to_cout.sign_ == MINUS) {
    os << "-";
  }
  os << to_cout.parts_.back();
  for (int iter = (int)(to_cout.parts_.size() - 2); iter >= 0; iter--) {
    os << std::string((double)(kMaxWidth - log10(to_cout.parts_[iter]) - 1),
                      '0')
       << to_cout.parts_[iter];
  }
  return os;
}

std::istream& operator>>(std::istream& is, BigInt& to_cin) {
  std::string buf;
  is >> buf;
  BigInt tmp(buf);
  to_cin = tmp;
  return is;
}
