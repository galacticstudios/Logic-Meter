// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END


#include <math.h>

#include "gfx/hal/inc/gfx_math.h"

#define COSINETABLEENTRIES 90

#if GFX_DRAW_PIPELINE_ENABLED

const int16_t   _CosineTable[COSINETABLEENTRIES+1] =
{
        256, 256, 256, 256, 255, 255, 255, 254, 254, 253,
        252, 251, 250, 249, 248, 247, 246, 245, 243, 242,
        241, 239, 237, 236, 234, 232, 230, 228, 226, 224,
        222, 219, 217, 215, 212, 210, 207, 204, 202, 199,
        196, 193, 190, 187, 184, 181, 178, 175, 171, 168,
        165, 161, 158, 154, 150, 147, 143, 139, 136, 132,
        128, 124, 120, 116, 112, 108, 104, 100, 96,  92,
        88,  83,  79,  75,  71,  66,  62,  58,  53,  49,
        44,  40,  36,  31,  27,  22,  18,  13,  9,   4,
        0
};

int16_t GFX_Normalize360(int16_t t)
{
    if (t >= 360)
    {
        t -= 360;
    }

    if (t < 0)
    {
        t += 360;
    }
    
    return t;
}

int16_t GFX_SineCosineGet(int16_t v, GFX_TRIG_FUNCTION_TYPE type)
{
    // if angle is neg, convert to pos equivalent
    if (v < 0) 
    	v += 360;   					
           
    if(v > COSINETABLEENTRIES * 3)
    {
        v -= (COSINETABLEENTRIES * 3);
        if (type == GFX_TRIG_SINE_TYPE)
            return (-_CosineTable[v]);
        else
            return (_CosineTable[COSINETABLEENTRIES - v]);
    }
    else if(v > COSINETABLEENTRIES * 2)
    {
        v -= (COSINETABLEENTRIES * 2);
        if (type == GFX_TRIG_SINE_TYPE)
            return (-_CosineTable[(COSINETABLEENTRIES - v)]);
        else
            return (-_CosineTable[v]);
    }
    else if(v > COSINETABLEENTRIES)
    {
        v -= (COSINETABLEENTRIES);
        if (type == GFX_TRIG_SINE_TYPE)
            return (_CosineTable[v]);
        else
            return (-_CosineTable[COSINETABLEENTRIES - v]);
    }
    else
    {
        if (type == GFX_TRIG_SINE_TYPE)
            return (_CosineTable[COSINETABLEENTRIES - v]);
        else
            return (_CosineTable[v]);
    }
}

GFX_Result GFX_PolarToXY(int32_t r, int32_t a, GFX_Point* p)
{
    p->x = (r * GFX_SineCosineGet(a, GFX_TRIG_COSINE_TYPE)) / 256;
    p->y = (r * GFX_SineCosineGet(a, GFX_TRIG_SINE_TYPE)) / 256;
    
    return GFX_SUCCESS;
}

GFX_Result GFX_EllipsePoint(int32_t t, int32_t a, int32_t b, int32_t theta, GFX_Point* p)
{
    p->x = (a * GFX_SineCosineGet(t, GFX_TRIG_COSINE_TYPE) * GFX_SineCosineGet(theta, GFX_TRIG_COSINE_TYPE) / (256 * 256))
            - (b * GFX_SineCosineGet(t, GFX_TRIG_SINE_TYPE) * GFX_SineCosineGet(theta, GFX_TRIG_SINE_TYPE) / (256 * 256));
    p->y = (a * GFX_SineCosineGet(t, GFX_TRIG_COSINE_TYPE) * GFX_SineCosineGet(theta, GFX_TRIG_SINE_TYPE) / (256 * 256))
            + (b * GFX_SineCosineGet(t, GFX_TRIG_SINE_TYPE) * GFX_SineCosineGet(theta, GFX_TRIG_COSINE_TYPE) / (256 * 256));
    
    return GFX_SUCCESS;
}

double GFX_Atan(double x)
{
    //use math.h atan for now. will optimize with LUT if needed.
    return atan(x);
}

int32_t GFX_Mini(int32_t l, int32_t r)
{
    return l < r ? l : r;
}

int32_t GFX_Maxi(int32_t l, int32_t r)
{
    return l > r ? l : r;
}

float GFX_Minf(float l, float r)
{
    return l < r ? l : r;
}

float GFX_Maxf(float l, float r)
{
    return l > r ? l : r;
}

int32_t GFX_Clampi(int32_t min, int32_t max, int32_t i)
{
    if(i < min)
        return min;
    else if(i > max)
        return max;
        
    return i;
}

float GFX_Clampf(float min, float max, float f)
{
    if(f < min)
        return min;
    else if(f > max)
        return max;
        
    return f;
}

uint32_t GFX_Percent(uint32_t l, uint32_t r)
{
    return (l * 10000) / r;
}

uint32_t GFX_PercentWholeRounded(uint32_t l, uint32_t r)
{
    int32_t per = ((((int32_t)l * 10000) / (int32_t)r) + 50) / 100;
    
    if (per < 0)
        return 0;
    
    return per;
}

uint32_t GFX_PercentOf(uint32_t num, uint32_t percent)
{
    int whl = ((num * 100) * percent) / 10000;
    int dec = (((num * 1000) * percent) / 10000) % 10;
    
    if(dec >= 5)
        whl++;
    
    return whl;
}

void GFX_PercentOfDec(uint32_t num, uint32_t percent, uint32_t* whl, uint32_t* dec)
{
    *whl = ((num * 100) * percent) / 10000;
    *dec = (((num * 1000) * percent) / 1000) % 100;
}

/*int32_t GFX_ScaleInteger(int num, int oldMax, int newMax)
{
    num = GFX_Percent(num, oldMax);
    num = GFX_PercentOf(newMax, num);
    
    return num;
}*/

float roundFloat(float flt);

int32_t GFX_ScaleIntegerSigned(int32_t num, int32_t oldMax, int32_t newMax)
{
    float percent;
    
    percent = (float)num / (float)oldMax;
    percent *= newMax;
    
    return (int32_t)percent;
}


uint32_t GFX_ScaleInteger(uint32_t num, uint32_t oldMax, uint32_t newMax)
{
    float percent;
    
    percent = (float)num / (float)oldMax;
    percent *= newMax;
    //percent = roundFloat(percent);
    
    return (uint32_t)percent;
}

/*void GFX_RebaseValue(int32_t imin, int32_t ival, uint32_t* uval)
{
    int32_t min, max;
    
    if(imin == ival)
    {
        *uval = 0;
        
        return;
    }
    
    *umax = imax - min;
}*/

uint32_t GFX_AbsoluteValue(int32_t val)
{
    uint32_t temp = val >> 31;
    val ^= temp;
    val += temp & 1;
    
    return (uint32_t)val;
}

int32_t GFX_Lerp(int32_t l, int32_t r, uint32_t per)
{
    int32_t imin, imax;
    uint32_t umax, val;
    
    if(l == r)
        return l;
        
    if(per <= 0)
        return l;
    
    if(per >= 100)
        return r;
        
    imin = l;
    imax = r;
    
    if(l > r)
    {
        imin = r;
        imax = l;
        
		umax = imax - imin;

		val = GFX_PercentOf(umax, per);

		return imax - val;
	}
	else
	{
		umax = imax - imin;

		val = GFX_PercentOf(umax, per);

		return imin + val;
	}        
}

int32_t GFX_DivideRounding(int32_t num, int32_t denom)
{
    int32_t lnum, ldenom, lquo;
    
    if(denom == 0)
        return 0;
        
    lnum = num * 100;
    ldenom = denom;// * 100;
    
    lquo = lnum / ldenom;
    
    if(lquo % 100 >= 50)
        return (num / denom) + 1;
    else
        return num / denom;
}

#endif // GFX_DRAW_PIPELINE_ENABLED