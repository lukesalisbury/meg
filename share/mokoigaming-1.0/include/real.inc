#include <realenums>

#if defined USE_FLOAT
	#tryinclude <float>
#endif


#if defined Float_included
	#define Real:			Float:
	#define RealString(%1)		strfloat(%1)
	#define RealRound(%1,%2)	floatround(%1,%2)
	#define RealSqroot(%1)		floatsqroot(%1)
	#define RealPower(%1,%2)	floatpower(%1,%2)
	#define RealAbs(%1)		floatabs(%1)
	#define RealLog(%1,%2)		floatlog(%1,%2)

	#define RealSin(%1,%2)		floatsin(%1,%2)
	#define RealCos(%1,%2)		floatcos(%1,%2)
	#define RealTan(%1,%2)		floattan(%1,%2)
	#define RealAtan2(%1,%2,%3)	floatatan2(%1,%2,%3)
#else
	#include <fixed>
	#define Real:			Fixed:
	#define RealString(%1)		strfixed(%1)
	#define RealRound(%1,%2)	fround(%1,%2)
	#define RealSqroot(%1)		fsqroot(%1)
	#define RealPower(%1,%2)	fpower(%1,%2)
	#define RealAbs(%1)		fabs(%1)
	#define RealLog(%1,%2)		flog(%1,%2)

	#define RealSin(%1,%2)		fsin(%1,%2)
	#define RealCos(%1,%2)		fcos(%1,%2)
	#define RealTan(%1,%2)		ftan(%1,%2)
	#define RealAtan2(%1,%2,%3)	fatan2(%1,%2,%3)
#endif

stock Real:RealClamp(Real:value, Real:min, Real:max)
{
	if ( value > max)
		return max;
	else if ( value < min)
		return min;
	else
		return value;
}



