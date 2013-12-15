/* Fixed point arithmetic
 *
 * (c) Copyright 1998-2007, ITB CompuPhase
 * This file is provided as is (no warranties).
 */
#if defined fixed_included
  #endinput
#endif
#define fixed_included


//title Rational Number Support
//description Fixed point numbers are the supported rational number system and allows for 3 decimal places. Floating point numbers are also support.

#include <real>

#pragma rational Fixed(3)

native Fixed:fixed(value);
native Fixed:strfixed(const string[]);
native Fixed:fmul(Fixed:oper1, Fixed:oper2);
native Fixed:fdiv(Fixed:dividend, Fixed:divisor);
native Fixed:ffract(Fixed:value);
native fround(Fixed:value, round_method:method=round_normal);
native Fixed:fpower(Fixed:value, exponent);
native Fixed:fsqroot(Fixed:value);
native Fixed:fabs(Fixed:value);

native Fixed:flog(Fixed:value, Fixed:base=10.0); /* Return the logarithm */

/**************************************************/
/* Return the sine, cosine or tangent. The input angle may be in radian, degrees or grades. */
native Fixed:fsin(Fixed:value, anglemode:mode=radian);
native Fixed:fcos(Fixed:value, anglemode:mode=radian);
native Fixed:ftan(Fixed:value, anglemode:mode=radian);
native Fixed:fatan2(Fixed:x, Fixed:y, anglemode:mode=radian );

/**************************************************/
/* user defined operators */
native Fixed:operator*(Fixed:oper1, Fixed:oper2) = fmul;
native Fixed:operator/(Fixed:oper1, Fixed:oper2) = fdiv;
native Fixed:operator=(oper) = fixed;

stock Fixed:operator++(Fixed:oper)
    return oper + fixed(1);

stock Fixed:operator--(Fixed:oper)
    return oper - fixed(1);

stock Fixed:operator*(Fixed:oper1, oper2)
    return Fixed: (_:oper1 * oper2);            /* "*" is commutative */

stock Fixed:operator/(Fixed:oper1, oper2)
    return oper1 / fixed(oper2);

stock Fixed:operator/(oper1, Fixed:oper2)
    return fdiv(fixed(oper1), oper2);

stock Fixed:operator+(Fixed:oper1, oper2)
    return oper1 + fixed(oper2);                /* "+" is commutative */

stock Fixed:operator-(Fixed:oper1, oper2)
    return oper1 - fixed(oper2);

stock Fixed:operator-(oper1, Fixed:oper2)
    return fixed(oper1) - oper2;

stock bool:operator>(Fixed:oper1, oper2)
    return oper1 > fixed(oper2);

stock bool:operator>(oper1, Fixed:oper2)
    return fixed(oper1) > oper2;

stock bool:operator>=(Fixed:oper1, oper2)
    return oper1 >= fixed(oper2);

stock bool:operator>=(oper1, Fixed:oper2)
    return fixed(oper1) >= oper2;

stock bool:operator<(Fixed:oper1, oper2)
    return oper1 < fixed(oper2);

stock bool:operator<(oper1, Fixed:oper2)
    return fixed(oper1) < oper2;

stock bool:operator<=(Fixed:oper1, oper2)
    return oper1 <= fixed(oper2);

stock bool:operator<=(oper1, Fixed:oper2)
    return fixed(oper1) <= oper2;

stock bool:operator==(Fixed:oper1, oper2)       /* "==" is commutative */
    return oper1 == fixed(oper2);

stock bool:operator!=(Fixed:oper1, oper2)       /* "!=" is commutative */
    return oper1 != fixed(oper2);

/* forbidden operations */
/*
forward operator%(Fixed:oper1, Fixed:oper2);
forward operator%(Fixed:oper1, oper2);
forward operator%(oper1, Fixed:oper2);
*/

stock operator%(Fixed:oper1, Fixed:oper2)
	return fround(oper1*1000)%fround(oper2*1000);
stock operator%(Fixed:oper1, oper2)
	return fround(oper1*1000)%(oper2*1000);
stock operator%(oper1, Fixed:oper2)
	return (oper1*1000)%fround(oper2*1000);



/***
<function name="RealString">
	<summary>Converts a String into Rational number.</summary>
	<param name="string[]" default="" type="a"></param>
	<return>New Rational number</return>
</function>
<function name="RealRound">
	<summary>Converts a Rational number into a Whole number</summary>
	<param name="Real:value" default="" type="n"></param>
	<param name="round_method" default="round_normal" type="n"></param>
	<return>new whole number.</return>
</function>
<function name="RealSqroot">
	<summary>Returns the square root of a value.</summary>
	<param name="Real:value" default="" type="n"></param>
	<return>square root of a value.</return>
</function>
<function name="RealPower">
	<summary>Returns the value raised to the power of exponent.</summary>
	<param name="Real:value" default="" type="n"></param>
	<param name="exponent" default="" type="n"></param>
	<return>the value raised to the power of exponent</return>
</function>
<function name="RealAbs">
	<summary>Returns Absolute value of 'value'.</summary>
	<param name="Real:value" default="" type="n"></param>
</function>
<function name="RealLog">
	<summary>Return the logarithm </summary>
	<param name="Real:value" default="" type="n"></param>
	<param name="Real:base" default="10.0" type="n"></param>
</function>
<function name="RealSin">
	<summary>Return the sine. The input angle may be in radian, degrees or grades.</summary>
	<param name="Real:value" default="" type="n"></param>
	<param name="anglemode:mode" default="radian" type="n"></param>
	<return>sine of the angle.</return>
</function>
<function name="RealCos">
	<summary>Return the cosine. The input angle may be in radian, degrees or grades.</summary>
	<param name="Real:value" default="" type="n"></param>
	<param name="anglemode:mode" default="radian" type="n"></param>
	<return>cosine of the angle.</return>
</function>
<function name="RealTan">
	<summary>Return the tangent. The input angle may be in radian, degrees or grades.</summary>
	<param name="Real:value" default="" type="n"></param>
	<param name="anglemode:mode" default="radian" type="n"></param>
	<return>tangent of the angle.</return>
</function>
<function name="RealAtan2">
	<summary>Return the arctangent of x/y. The input angle may be in radian, degrees or grades.</summary>
	<param name="Real:value" default="" type="n"></param>
	<param name="anglemode:mode" default="radian" type="n"></param>
	<notes>Uses fatan2 for Fixed Point Number. Uses floatatan2 for Floating Point Number. </notes>
	<return>arc tangent of x/y.</return>
</function>
<function name="RealClamp">
	<summary>Return value thats has been limited between min and max.</summary>
	<param name="Real:value" default="" type="n"></param>
	<param name="Real:min" default="" type="n"></param>
	<param name="Real:max" default="" type="n"></param>
</function>

***/