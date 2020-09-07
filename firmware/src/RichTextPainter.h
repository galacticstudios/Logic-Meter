/* 
 * File:   RichTextPainter.h
 * Author: Bob
 *
 * Created on May 1, 2020, 11:19 AM
 */

#ifndef RICHTEXTPAINTER_H
#define	RICHTEXTPAINTER_H

#include <vector>
#include <string>
#include <stdint.h>
#include "SurfacePainter.h"

struct GFXU_FontAsset_t;
typedef struct GFXU_FontAsset_t GFXU_FontAsset;

struct RichTextBlock
{
    std::string text;
    GFXU_FontAsset *font;
    uint16_t textColor, backgroundColor;
    int16_t verticalOffset;
};

class RichTextPainter : public SurfacePainter
{
public:
    RichTextPainter(int32_t xOffset = 0, int32_t yOffset = 0) : SurfacePainter(xOffset, yOffset), _just(Left) {}
    virtual ~RichTextPainter();
    
    std::vector<RichTextBlock> &RichTextBlocks() {return _richText;}
    typedef enum {Left, Center, Right} Justification;
    void SetJustify(Justification just) {_just = just;}
    
    bool OnDraw(SurfaceWrapper *surface, GFX_Rect *bounds);
    
private:
    RichTextPainter(const RichTextPainter& orig);
    
    std::vector<RichTextBlock> _richText;
    Justification _just;
};

#endif	/* RICHTEXTPAINTER_H */

