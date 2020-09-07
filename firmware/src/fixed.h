/* 
 * File:   _fixed.h
 * Author: Bob
 *
 * Created on May 7, 2020, 9:07 AM
 */

#ifndef FIXED_H
#define	FIXED_H

/******************************************************************************
CLASS exp10 -- template helper for computing powers of 10
ARGUMENTS
	N	--	exponent
IMPLEMENTATION
	exp10 computes powers of 10 recursively using a specialization for 0 to 
	end recursion
******************************************************************************/
template <typename T, int N> struct exp10 { static const T value = T(10) * exp10<T, N-1>::value; };
template <typename T> struct exp10<T, 0> { static const T value = 1; };

template <typename T, int N>
class _fixed 
{
public:
	static const T scale = exp10<T, N>::value;
    static const int fraction_digits = N;
    
    _fixed() {}
    _fixed(int x) : _val(x * scale) {}
    _fixed(unsigned int x) : _val(T(x * scale)) {}
    _fixed(double x) : _val(T(x * scale + 0.5)) {}
    _fixed(T raw, bool) : _val(raw) {}
    _fixed(const _fixed &orig) : _val(orig._val) {}
    ~_fixed() {}

    _fixed &operator=(const _fixed &rhs) { _val = rhs._val; return *this; }
    
    //	arithmetic operators
	_fixed operator-() const { _fixed rtn; rtn._val = -_val; return rtn;	}
	bool operator!() const { return _val == 0; }
	_fixed &operator+=(const _fixed &rhs) { _val += rhs._val; return *this; }
	_fixed &operator-=(const _fixed &rhs)	{ _val -= rhs._val; return *this; }
	_fixed &operator*=(const _fixed &rhs) 
    {
        if (_val == 0) return *this;
        
        // Make all the numbers positive
        T lVal, rVal;
        bool negResult = false;
        if (rhs._val < 0)
        {
            rVal = -rhs._val;
            negResult = !negResult;
        }
        else
            rVal = rhs._val;
        if (_val < 0)
        {
            lVal = -_val;
            negResult = !negResult;
        }
        else
            lVal = _val;

        // The factors are both multiplied by scale. After the multiplication,
        // we'll need to rescale the product by a power of 10
        int rescaleN = -N;
        
        // To avoid an overflow, shift the rhs right as much as we can before we multiply
        T rem = rVal % 10;
        while (rem == 0)
        {
            rVal /= 10;
            rem = rVal % 10;
            ++rescaleN;
        }
        
        // And shift the lhs right
        rem = lVal % 10;
        while (rem == 0)
        {
            lVal /= 10;
            rem = lVal % 10;
            ++rescaleN;
        }

        _val = (T) (lVal * rVal);
        
        while (rescaleN > 0)
        {
            _val *= 10;
            --rescaleN;
        }
        while (rescaleN < 0)
        {
            _val /= 10;
            ++rescaleN;
        }

        if (negResult)
            _val = -_val;
        return *this; 
    }
	_fixed &operator/=(const _fixed &rhs) 
    {
        if (rhs._val == 0) __builtin_software_breakpoint();
        if (_val == 0) return *this;
        
        // Make all the numbers positive
        T lVal, rVal;
        bool negResult = false;
        if (rhs._val < 0)
        {
            rVal = -rhs._val;
            negResult = !negResult;
        }
        else
            rVal = rhs._val;
        if (_val < 0)
        {
            lVal = -_val;
            negResult = !negResult;
        }
        else
            lVal = _val;

        // The divisor and dividend are both multiplied by scale. After the division,
        // we'll need to rescale the quotient by a power of 10
        int rescaleN = N;
        
        // To avoid an underflow, shift the divisor right as much as we can before we divide
        T rem = rVal % 10;
        while (rem == 0)
        {
            rVal /= 10;
            rem = rVal % 10;
            --rescaleN;
        }
        
        // And shift the dividend left
        T bigger = lVal * 10;
        while (bigger / 10 == lVal)
        {
            lVal = bigger;
            bigger = lVal * 10;
            --rescaleN;
        }

        _val = (T) (lVal / rVal);
        
        while (rescaleN > 0)
        {
            _val *= 10;
            --rescaleN;
        }
        while (rescaleN < 0)
        {
            _val /= 10;
            ++rescaleN;
        }

        if (negResult)
            _val = -_val;
        return *this; 
    }
	_fixed &operator%=(const _fixed &rhs) { _val %= rhs._val; return *this; }

    _fixed &operator*=(int rhs) { operator*=(_fixed(rhs)); return *this; }
	_fixed &operator/=(int rhs) { operator/=(_fixed(rhs)); return *this; }

    const _fixed operator+(const _fixed &rhs) const { _fixed rtn; rtn._val = _val; rtn += rhs; return rtn; }
    const _fixed operator-(const _fixed &rhs) const { _fixed rtn; rtn._val = _val; rtn -= rhs; return rtn; }
    const _fixed operator*(const _fixed &rhs) const { _fixed rtn; rtn._val = _val; rtn *= rhs; return rtn; }
    const _fixed operator/(const _fixed &rhs) const { _fixed rtn; rtn._val = _val; rtn /= rhs; return rtn; }
    const _fixed operator%(const _fixed &rhs) const { _fixed rtn; rtn._val = _val; rtn %= rhs; return rtn; }
    
    //	comparison operators
	bool operator==(const _fixed &rhs) const { return _val == rhs._val; }
	bool operator<(const _fixed &rhs) const { return _val < rhs._val; }
	bool operator!=(const _fixed &rhs) const { return _val != rhs._val; }
	bool operator>(const _fixed &rhs) const { return _val > rhs._val; }
	bool operator<=(const _fixed &rhs) const { return _val <= rhs._val; }
	bool operator>=(const _fixed &rhs) const { return _val >= rhs._val; }
    
    T raw() const {return _val;}
    
    // casts
    explicit operator int() const {return int(_val / scale);}
    explicit operator long() const {return long(_val / scale);}
    explicit operator double() const {return double(_val) / scale;}
    explicit operator bool() const {return _val != 0;}
    
private:
    T _val;
};

template <typename T, int N>
const _fixed<T, N> operator*(int l, _fixed<T, N> r) 
{return r * l;}

template <typename T, int N>
const _fixed<T, N> operator*(double l, _fixed<T, N> r) 
{return r * l;}

template <typename T, int N>
const _fixed<T, N> operator/(int l, _fixed<T, N> r) 
{return _fixed<T, N>(l) / r;}

template <typename T, int N>
const _fixed<T, N> operator/(unsigned int l, _fixed<T, N> r) 
{return _fixed<T, N>(l) / r;}

template <typename T, int N>
const _fixed<T, N> operator/(double l, _fixed<T, N> r) 
{return _fixed<T, N>(l) / r;}

typedef _fixed<long long, 9> fixed;

#endif	/* FIXED_H */

