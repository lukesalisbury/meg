#if defined _helper_included
  #endinput
#endif
#define _helper_included

#include <core>
#include <mokoi>

//title Helper Functions
//description These are define in the helper.inc header, so to use these functions, #include &lt;helper&gt; must be include at the top of the script.


///Timer Functions
/***
<function name="TimerCountdown">
	<summary>Returns true when time value is 0.</summary>
	<param name="&amp;time" default="" type="n">Time counter varable</param>
	<return>True when time reaches 0.</return>
</function>
<function name="TimerCountdownWithReset">
	<summary>Returns true when time value is 0, then resets time.</summary>
	<param name="&amp;time" default="" type="n">Time counter varable</param>
	<param name="reset" default="" type="n">reset value.</param>
	<return>True when time reaches 0.</return>
</function>
<function name="TimerGetRandomValue">
	<summary>Generates a Random number after a set time.</summary>
	<param name="randomness" default="" type="n">max random value.</param>
	<param name="&amp;random_value" default="" type="n">Varable to store the random number.</param>
	<param name="&amp;time" default="" type="n">Time counter varable</param>
	<param name="timer_reset" default="1000" type="n">reset value.</param>
	<return>True if random_value is updated.</return>
</function>
***/
stock TimerCountdown(&time)
{
	time -= GameFrame();
	return (time > 0 ? false : true);
}

stock TimerCountdownWithReset(&time, reset)
{
	time -= GameFrame();
	if ( time > 0 )
		return false;
	else
	{
		time = reset;
		return true;
	}
}

stock TimerGetRandomValue(randomness, &random_value, &time, timer_reset = 1000 )
{
	if ( CountTimer(time, timer_reset) )
	{
		value = NumberRandom(randomness);
		return true;
	}
	return false;
}


///Sound Helper Functions
stock SoundPlayOnce(&value, file[])
{
	if ( !value )
	{
		AudioPlaySound(file, 0, 0);
		value = true;
	}
}

stock SoundPlayRepeat(&value, file[])
{
	if ( value < 0)
		value = AudioPlaySound(file, 0, 0);
	else
		value -= GameFrame();
}




/***
<function name="SoundPlayOnce">
	<summary>Play a audio file once.</summary>
	<param name="&amp;value" default="" type="n">Variable to store if audio has been played.</param>
	<param name="file[]" default="" type="a">Audio File.</param>
</function>
<function name="SoundPlayRepeat">
	<summary>Trys to repeat playing audio file.</summary>
	<param name="&amp;value" default="" type="n">Variable to store if audio has been played.</param>
	<param name="file[]" default="" type="a">Audio File.</param>
</function>
***/



///Bits Functions
stock SetBit( &v, n )
{
	v |= (1 << n);
}

stock ClearBit( &v, n )
{
	v &= ~(1 << n);
}

stock ToggleBit( &v, n )
{
	v ^= (1 << n);
}

stock bool:TestBit( v, n )
{
	return !!(v & (1 << n));
}

stock SetBits( &v, n, p, s )
{
	n = clamp( n, 0, (1<<s)-1);
	v |= (n << p);
}

stock GetBits( v, p, s )
{
	return (v >> p) & ((1<<s)-1);
}


/***
<function name="SetBit">
	<summary>Changes a bit value</summary>
	<param name="&amp;v" default="" type="n">Variable to use.</param>
	<param name="position" default="" type="n">Bit Position to change.</param>
</function>
<function name="ClearBit">
	<summary>Clears a bit value</summary>
	<param name="&amp;v" default="" type="n">Variable to use.</param>
	<param name="position" default="" type="n">Bit Position to change.</param>
</function>
<function name="ToggleBit">
	<summary>Toggles a  bit value</summary>
	<param name="&amp;v" default="" type="n">Variable to use.</param>
	<param name="position" default="" type="n">Bit Position to change.</param>
</function>
<function name="TestBit">
	<summary>Return the bit value</summary>
	<param name="&amp;v" default="" type="n">Variable to use.</param>
	<param name="position" default="" type="n">Bit Position to change.</param>
	<return type="bool">bit value</return>
</function>
<function name="SetBits">
	<summary>Sets a range of bits.</summary>
	<param name="&amp;v" default="" type="n">Variable to use.</param>
	<param name="value" default="" type="n">Value.</param>
	<param name="position" default="" type="n">Bit position.</param>
	<param name="size" default="" type="n">Number of bits to use.</param>
</function>
<function name="GetBits">
	<summary>Sets a range of bits.</summary>
	<param name="value" default="" type="n">Variable to use.</param>
	<param name="position" default="" type="n">Bit position.</param>
	<param name="size" default="" type="n">Number of bits to use.</param>
	<return>value of bits</return>
</function>

***/
