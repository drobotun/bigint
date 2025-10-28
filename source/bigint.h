/****************************************************************************
**
** Copyright (C) 2025 Evgeny Drobotun, Anna Boykova
** Contact: evgeny@drobotun.su
**
** This file (along with the "bigint.cpp" file) is module that implements
** arithmetic and logical operation on long numbers.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and associated documentation files (the “Software”),
** to deal in the Software without restriction, including without limitation
** the right to use, copy, modify, merge, publish, distribute, sublicense,
** and/or sell copies of the Software, and to permit persons to whom the
** Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LABILITY, WHETHER IN AND ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
**
****************************************************************************/
#ifndef BIGINT_H
#define BIGINT_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <string>
#include <algorithm>
#include <complex>
#include <climits>

enum class ExceptionCode {
    Unknown,
    DivisionByZero,
    InvalidRootDegree,
    InvalidValue
};

class BigIntException: public std::exception
{
public:
    BigIntException(const std::string &message);
    BigIntException(const ExceptionCode code);
    BigIntException(const std::string &message, const ExceptionCode code);
    const char* what() const noexcept override;
    ExceptionCode getExceptionCode() const noexcept;
private:
    std::string m_message{""};
    ExceptionCode m_code{ExceptionCode::Unknown};
};

enum class MulMethod {
    Karatsuba,
    FastFourierTransform
};

using digit_t = unsigned char;

const double PI = std::acos(-1);
const size_t LLONG_SIZE = 18;

class BigInt
{
private:
    std::vector<digit_t> m_digits;
    bool m_sign = false;
public:
    BigInt();
    BigInt(const std::string &strValue, const size_t radix = 10);
    BigInt(const char *strValue, const size_t radix = 10);
    BigInt(const long long value);
    BigInt(const int value);
    BigInt(const size_t value);
    BigInt(const BigInt &other);
#if __cplusplus >= 201103L
    BigInt(BigInt &&other);
#endif
    ~BigInt();
    BigInt& operator = (const long long value);
    BigInt& operator = (const BigInt &other);
#if __cplusplus >= 201103L
    BigInt& operator = (BigInt &&other);
#endif

    size_t size() const;
    bool isEven() const;
    unsigned long long numBits();
    std::string toString(const size_t radix = 10, const bool upper = false) const;
    int toInt() const;
    long toLong() const;
    long long toLongLong() const;
    unsigned long toULong() const;
    unsigned long long toULongLong() const;

    void setMulMethod(MulMethod mul_mwethod);

    bool operator == (const BigInt &other) const;
    bool operator == (const long long other) const;
    bool operator != (const BigInt &other) const;
    bool operator != (const long long other) const;
    bool operator > (const BigInt &other) const;
    bool operator > (const long long other) const;
    bool operator < (const BigInt &other) const;
    bool operator < (const long long other) const;
    bool operator >= (const BigInt &other) const;
    bool operator >= (const long long other) const;
    bool operator <= (const BigInt &other) const;
    bool operator <= (const long long other) const;

    BigInt operator << (const unsigned long long shift) const;
    BigInt operator >> (const unsigned long long shift) const;

    BigInt operator + (const BigInt &other) const;
    BigInt operator + (const long long other) const;
    BigInt operator - (const BigInt &other) const;
    BigInt operator - (const long long other) const;
    BigInt operator * (const BigInt &other) const;
    BigInt operator * (const long long other) const;
    BigInt operator / (const BigInt &other) const;
    BigInt operator / (const long long other) const;
    BigInt operator % (const BigInt &other) const;
    BigInt operator % (const long long other) const;

    BigInt operator & (const BigInt &other) const;
    BigInt operator & (const long long other) const;
    BigInt operator | (const BigInt &other) const;
    BigInt operator | (const long long other) const;
    BigInt operator ^ (const BigInt &other) const;
    BigInt operator ^ (const long long other) const;
    BigInt operator ~ () const &;

    BigInt operator -- ();
    BigInt operator ++ ();

    BigInt operator -- (int);
    BigInt operator ++ (int);

    BigInt& operator <<= (const size_t shift);
    BigInt& operator >>= (const size_t shift);

    BigInt& operator += (const BigInt &other);
    BigInt& operator += (const long long other);
    BigInt& operator -= (const BigInt &other);
    BigInt& operator -= (const long long other);
    BigInt& operator *= (const BigInt &other);
    BigInt& operator *= (const long long other);
    BigInt& operator /= (const BigInt &other);
    BigInt& operator /= (const long long other);
    BigInt& operator %= (const BigInt &other);
    BigInt& operator %= (const long long other);
    BigInt& operator |= (const BigInt &other);
    BigInt& operator |= (const long long other);
    BigInt& operator &= (const BigInt &other);
    BigInt& operator &= (const long long other);
    BigInt& operator ^= (const BigInt &other);
    BigInt& operator ^= (const long long other);

    BigInt operator -() const &;
    BigInt operator +() const &;

    friend BigInt strToBigInt(const std::string &value, const size_t radix);

    friend std::ostream& operator << (std::ostream &out, const BigInt &value);
    friend std::istream& operator >> (std::istream &in, BigInt &value);

    friend BigInt shr(const BigInt &value, const size_t shift);
    friend BigInt shl(const BigInt &value, const size_t shift);

    friend BigInt sqr(const BigInt &value);
    friend BigInt pow(const BigInt &value, const long long exp);
    friend BigInt pow(const BigInt &value, const BigInt exp);

    friend BigInt powMod(const BigInt &value, const long long exp, const BigInt &mod);
    friend BigInt powMod(const BigInt &value, const BigInt exp, const BigInt &mod);

    friend BigInt sqrt(const BigInt &value);
    friend BigInt root(const BigInt &value, const long long exp);
    friend BigInt root(const BigInt &value, const BigInt &exp);

    friend BigInt abs(const BigInt &value);

    friend BigInt gcd(const BigInt &value_1, const BigInt &value_2);
    friend BigInt gcdExt(const BigInt &value_1, const BigInt &value_2,
                          BigInt &factor_1, BigInt &factor_2);

    friend int getLegendreSymbol(const BigInt &value_a, const BigInt &value_p);
    friend int getJacobiSymbol(const BigInt &value_a, const BigInt &value_p);


private:
    BigInt shlBitsBase(const size_t shift) const;
    BigInt shrBitsBase(const size_t shift) const;

    BigInt shlDigitsBase(const size_t shift) const;
    BigInt shrDigitsBase(const size_t shift) const;

    BigInt orBase(const BigInt &value) const;
    BigInt andBase(const BigInt &value) const;
    BigInt xorBase(const BigInt &value) const;
    BigInt notBase() const;

    BigInt addBase(const BigInt &value) const;
    BigInt subBase(const BigInt &value) const;
    BigInt mulNative(const BigInt &value) const;
    BigInt mulKaratsuba(const BigInt &value) const;
    BigInt mulFft(const BigInt &value) const;
    BigInt mulBase(const BigInt &value) const;
    BigInt divNative(const BigInt &value) const;
    BigInt divNewtonRaphson(const BigInt &value) const;
    BigInt divBase(const BigInt &value) const;
    BigInt remBase(const BigInt &value) const;

    BigInt powBase(long long exp) const;
    BigInt powBase(const BigInt &exp) const;
    BigInt powModBase(const long long exp, const BigInt &mod) const;
    BigInt powModBase(const BigInt &exp, const BigInt &mod) const;
    BigInt rootBase(const long long exp) const;
    BigInt rootBase(const BigInt &exp) const;

    void normalize();

    MulMethod m_mulMethod{MulMethod::Karatsuba};
};

BigInt strToBigInt(const std::string &strValue, const size_t radix = 10);
BigInt strToBigInt(const char *strValue, size_t radix = 10);

std::ostream& operator << (std::ostream &out, const BigInt &value);
std::istream& operator >> (std::istream &in, BigInt &value);

BigInt shr(const BigInt &value, const size_t shift);
BigInt shl(const BigInt &value, const size_t shift);

BigInt sqr(const BigInt &value);
BigInt pow(const BigInt &value, const long long exp);
BigInt pow(const BigInt &value, const BigInt exp);

BigInt powMod(const BigInt &value, const long long exp, const BigInt &mod);
BigInt powMod(const BigInt &value, const BigInt exp, const BigInt &mod);

BigInt sqrt(const BigInt &value);
BigInt root(const BigInt &value, const long long exp);
BigInt root(const BigInt &value, const BigInt &exp);

BigInt abs(const BigInt &value);

BigInt gcd(const BigInt &value_1, const BigInt &value_2);
BigInt gcdExt(const BigInt &value_1, const BigInt &value_2,
               BigInt &factor_1, BigInt &factor_2);

int getLegendreSymbol(const BigInt &value_a, const BigInt &value_p);
int getJacobiSymbol(const BigInt &value_a, const BigInt &value_p);

#endif // BIGINT_H
