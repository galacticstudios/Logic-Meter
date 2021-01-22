/* 
 * File:   InputCapture.cpp
 * Author: Bob
 * 
 * Created on June 22, 2020, 11:07 AM
 */

extern "C" 
{
#include "definitions.h"
}
#include "InputCapture.h"
#include "Interrupts.h"

#define IC_INTS(n) \
Implement_InterruptHandler(_INPUT_CAPTURE_##n##_ERROR_VECTOR, InputCaptureInt[n - 1].error) \
Implement_InterruptHandler(_INPUT_CAPTURE_##n##_VECTOR, InputCaptureInt[n - 1].capture)


IC_INTS(1)
IC_INTS(2)
IC_INTS(3)
IC_INTS(4)
IC_INTS(5)
IC_INTS(6)
IC_INTS(7)
IC_INTS(8)
IC_INTS(9)
