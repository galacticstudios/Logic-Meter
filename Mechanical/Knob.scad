$fn = 180;

flangeDiameter = 45;
flangeHeight = 6.5;

knobHeight = 6;
knobDiameter = 37;

handleThickness = 8;
handleHeight = 10;

pointerWidth = 3;
pointerDepth = 1.9;

shaftDiameter = 6.5;
shaftDiameterMinusFlatted = 4.4;

rotarySwitchDiameter = 24;
rotarySwitchHeight = 11;

difference()
{
    union()
    {
        // The flange
        cylinder(flangeHeight, d = flangeDiameter);
        
        translate([0, 0, flangeHeight])
        {
            // The dial
            cylinder(knobHeight, d = knobDiameter);
            translate([0, 0, knobHeight]) intersection()
            {
                translate([-knobDiameter / 2, -handleThickness / 2, 0])
                {
                    difference()
                    {
                        // The handle
                        cube([knobDiameter, handleThickness, handleHeight]);
                        // Cut out the pointer notch
                        translate([0, (handleThickness - pointerWidth) / 2, handleHeight - pointerDepth])
                        {
                            cube([knobDiameter / 3, (handleThickness - pointerWidth) / 2, pointerDepth]);
                        }
                        
                    }
                }
                // Intersect with a cylinder to shave off the corners of
                // the rectangular handle
                cylinder(handleHeight, d = knobDiameter);
            }
        }
    }
    
    // Subtract the hole for the shaft and the body of the rotary switch
    rotate([0, 0, -90 + 36 / 2]) union()
    {
        difference()
        {
            cylinder(flangeHeight + knobHeight + handleHeight - pointerDepth, d = shaftDiameter);
            translate([shaftDiameterMinusFlatted - shaftDiameter / 2, -4, 0]) {cube([12, 12, flangeHeight + knobHeight + handleHeight]);}
        }
        cylinder(rotarySwitchHeight, d = rotarySwitchDiameter);
    }

}
