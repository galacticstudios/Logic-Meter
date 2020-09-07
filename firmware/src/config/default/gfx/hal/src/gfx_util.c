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


#include "gfx/hal/inc/gfx_util.h"
#include "gfx/hal/inc/gfx_layer.h"

typedef GFX_Point (*GFX_Orient_FnPtr)(const GFX_Point*, const GFX_Rect*);

static GFX_Point orient_0(const GFX_Point* pnt, const GFX_Rect* rect);
static GFX_Point orient_90(const GFX_Point* pnt, const GFX_Rect* rect);
static GFX_Point orient_180(const GFX_Point* pnt, const GFX_Rect* rect);
static GFX_Point orient_270(const GFX_Point* pnt, const GFX_Rect* rect);

static GFX_Orient_FnPtr orient_ptrs[] =
{
    &orient_0,
    &orient_90,
    &orient_180,
    &orient_270
};

static GFX_Point orient_0(const GFX_Point* pnt, const GFX_Rect* rect)
{ 
    GFX_Point res;
    
    res = *pnt;
    
    return res;
}

static GFX_Point orient_90(const GFX_Point* pnt, const GFX_Rect* rect)
{
    GFX_Point res;

    res.y = pnt->x;
	res.x = rect->width - 1 - pnt->y;
    
    return res;
}

static GFX_Point orient_180(const GFX_Point* pnt, const GFX_Rect* rect)
{
    GFX_Point res;
    
    res.x = rect->width - 1 - pnt->x;
    res.y = rect->height - 1 - pnt->y;
    
    return res;
}

static GFX_Point orient_270(const GFX_Point* pnt, const GFX_Rect* rect)
{
    GFX_Point res;

    res.y = rect->height - 1 - pnt->x;
    res.x = pnt->y;
    
    return res;
}

GFX_Point GFX_UtilOrientPoint(const GFX_Point* pnt,
                              const GFX_Rect* rect,
                              GFX_Orientation ori)
{
    return orient_ptrs[ori](pnt, rect);    
}

typedef GFX_Point (*GFX_Mirror_FnPtr)(const GFX_Point*, const GFX_Rect*);

static GFX_Point mirror_0(const GFX_Point* pnt, const GFX_Rect* rect);
static GFX_Point mirror_90(const GFX_Point* pnt, const GFX_Rect* rect);
static GFX_Point mirror_180(const GFX_Point* pnt, const GFX_Rect* rect);
static GFX_Point mirror_270(const GFX_Point* pnt, const GFX_Rect* rect);

static GFX_Mirror_FnPtr mirror_ptrs[] =
{
    &mirror_0,
    &mirror_90,
    &mirror_180,
    &mirror_270
};

static GFX_Point mirror_0(const GFX_Point* pnt, const GFX_Rect* rect)
{ 
    GFX_Point res;
    
    res.x = rect->width - 1 - pnt->x;
    res.y = pnt->y;
    
    return res;
}

static GFX_Point mirror_90(const GFX_Point* pnt, const GFX_Rect* rect)
{
    GFX_Point res;

    res.x = pnt->x;
    res.y = rect->height - 1 - pnt->y;
    
    return res;
}

static GFX_Point mirror_180(const GFX_Point* pnt, const GFX_Rect* rect)
{
    GFX_Point res;
    
    res.x = rect->width - 1 - pnt->x;
    res.y = pnt->y;

    return res;
}

static GFX_Point mirror_270(const GFX_Point* pnt, const GFX_Rect* rect)
{
    GFX_Point res;

    res.x = pnt->x;
    res.y = rect->height - 1 - pnt->y;
    
    return res;
}

GFX_Point GFX_UtilMirrorPoint(const GFX_Point* pnt,
                              const GFX_Rect* rect,
                              GFX_Orientation ori)
{
    return mirror_ptrs[ori](pnt, rect);    
}

GFX_Point GFX_UtilPointFromOrientedSpace(const GFX_Rect* screenRect,
                                         const GFX_Rect* innerRect,
                                         const GFX_Point* pnt,
                                         GFX_Orientation ori,
                                         GFX_Bool mirrored)
{
    GFX_Point res;
    
	res.x = 0;
	res.y = 0;

	switch (ori)
    {
        case GFX_ORIENTATION_0:
        {
            if(mirrored == GFX_TRUE)
                res.x = screenRect->width - innerRect->width - pnt->x;
            else
                res.x = innerRect->x + pnt->x;
            
            res.y = innerRect->y + pnt->y;
            
            break;
        }
        case GFX_ORIENTATION_90:
        {
            res.y = innerRect->x + innerRect->width - pnt->y;
            
            if(mirrored == GFX_TRUE)
                res.x = screenRect->height - innerRect->height - innerRect->y + pnt->x;
            else
                res.x = innerRect->y + pnt->x;
            
            break;
        }
        case GFX_ORIENTATION_180:
        {         
            res.y = screenRect->height - (innerRect->height + innerRect->y);

            if(mirrored == GFX_TRUE)
                res.x = innerRect->x;
            else
                res.x = screenRect->width - innerRect->width - innerRect->x;

            break;
        }
        case GFX_ORIENTATION_270:
        {
            res.y = (innerRect->height - innerRect->x) + pnt->x;
            
            if(mirrored == GFX_TRUE)
                res.x = innerRect->y + pnt->y;
            else
                res.x = screenRect->height - (innerRect->height + innerRect->y) + pnt->y;

            break;
        }
    }
    
    return res;
}

GFX_Point GFX_UtilPointToOrientedSpace(const GFX_Rect* screenRect,
                                       const GFX_Rect* innerRect,
                                       const GFX_Point* pnt,
                                       GFX_Orientation ori,
                                       GFX_Bool mirrored)
{
    GFX_Point res;

	res.x = 0;
	res.y = 0;

	switch (ori)
    {
        case GFX_ORIENTATION_0:
        {
            if(mirrored == GFX_TRUE)
                res.x = screenRect->width - innerRect->width - pnt->x;
            else
                res.x = pnt->x;

            res.y = pnt->y;
            
            break;
        }
        case GFX_ORIENTATION_90:
        {
            res.x = screenRect->width - innerRect->width - pnt->y;
            
            if(mirrored == GFX_TRUE)
                res.y = screenRect->height - innerRect->height - pnt->x;
            else
                res.y = pnt->x;
                
            break;
        }
        case GFX_ORIENTATION_180:
        {
            if(mirrored == GFX_TRUE)
                res.x = pnt->x;
            else
                res.x = screenRect->width - innerRect->width - pnt->x;

            res.y = screenRect->height - innerRect->height - pnt->y;
            
            break;
        }
        case GFX_ORIENTATION_270:
        {
            res.x = pnt->y;

            if(mirrored == GFX_TRUE)
                res.y = pnt->x;
            else
                res.y = screenRect->height - innerRect->height - pnt->x;
                    
            break;
        }
    }
    
    return res;
}

GFX_Size GFX_UtilSizeFromOrientedSpace(const GFX_Size* size, 
                                       GFX_Orientation ori)
{
    GFX_Size res;

    if(ori == GFX_ORIENTATION_90 || ori == GFX_ORIENTATION_270)
    {
        res.width = size->height;
        res.height = size->width;
    }
    else
    {
        res = *size;
    }
    
    return res;
}

GFX_Size GFX_UtilSizeToOrientedSpace(const GFX_Size* size, 
                                     GFX_Orientation ori)
{
    GFX_Size res;

    if(ori == GFX_ORIENTATION_90 || ori == GFX_ORIENTATION_270)
    {
        res.width = size->height;
        res.height = size->width;
    }
    else
    {
        res = *size;
    }
    
    return res;
}

void GFX_UtilSortPointsX(GFX_Point* p1, GFX_Point* p2)
{
    GFX_Point tpt;
    
    if(p1->x > p2->x)
    {
        tpt = *p1;
        *p2 = *p1;
        *p1 = tpt;
    }
}

void GFX_UtilSortPointsY(GFX_Point* p1, GFX_Point* p2)
{
    GFX_Point tpt;
    
    if(p1->y > p2->y)
    {
        tpt = *p1;
        *p2 = *p1;
        *p1 = tpt;
    }
}

int32_t GFX_UtilGetXGivenYOnLine(GFX_Point p1, GFX_Point p2, int32_t y)
{
    if (p1.y == p2.y)
        return p1.x;
    
    return (p1.x - ((p1.x - p2.x) * (p1.y - y))/(p1.y - p2.y));
}

int32_t GFX_UtilGetYGivenXOnLine(GFX_Point p1, GFX_Point p2, int32_t x)
{
    if (p1.x == p2.x)
        return p1.y;
    
    return (p1.y - ((p1.y - p2.y) * (p1.x - x))/(p1.x - p2.x));
}