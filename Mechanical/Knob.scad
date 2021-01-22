include <LCS145.scad>

renderFlange = 0; // 0 or 1

// Set the shaft to the OFF position. How many degrees clockwise should
// the handle be rotated so it points to the OFF label?
shaftToHandle =  360 / 10 * -1.5;

rotarySwitchDiameter = 24;
rotarySwitchHeight = 3.5; // 11; // Actual height is 11, but we don't want to render most of it in version 7

flangeDiameter = 45;

knobHeight = rotarySwitchHeight + 2 - flangeHeight;
knobDiameter = 30;

shaftDiameter = 6.5;
shaftDiameterMinusFlatted = 4.4;
shaftHeight = 0.375 * 25.4;

handleThickness = 8;
handleHeight = shaftHeight + 3;

pointerWidth = 3;
pointerDepth = 1.9;

detentX = 17;
detentCount = 10;
detentBallDiameter = 2 * PI * detentX / detentCount / 2;
detentChannelDepth = detentBallDiameter / 3;
detentChannelWidth = detentBallDiameter;

innerBumpRadius = 2 * PI * (detentX - detentChannelWidth / 2) / detentCount / 2 * 0.7;
outerBumpRadius = 2 * PI * (detentX + detentChannelWidth / 2) / detentCount / 2 * 0.7;

difference()
{
    union()
    {
        // The flange
        if (renderFlange != 0)
        {
            union()
            {
                difference()
                {
                    // The body of the flange
                    cylinder(flangeHeight, d = flangeDiameter);
                    
                    // Subtract the channel for the detents
                    difference()
                    {
                        // The outer ring of the channel
                        cylinder(detentChannelDepth, r = detentX + detentChannelWidth / 2); 
                        
                        // The inner ring of the channel
                        cylinder(detentChannelDepth, r = detentX - detentChannelWidth / 2); 
                    } 
                }
                
                // The bumps for the detents
                for (b = [360 / detentCount / 2 : 360 / detentCount : 360 - (360 / detentCount / 2)])
                {
                    translate([detentX * cos(180 - b), detentX * sin(180 - b), flangeHeight / 2])
                        rotate(a = [0, 0, -b]) rotate(a = [0, -90, 0])
                        hull()
                        {
                            translate([0, 0, -detentChannelWidth / 2 - 1])
                                scale([1, innerBumpRadius * 2 / flangeHeight, 1])
                                    cylinder(0.1, d = flangeHeight, center = true);
                            translate([0, 0, detentChannelWidth / 2 + 1])
                                scale([1, outerBumpRadius * 2 / flangeHeight, 1])
                                    cylinder(0.1, d = flangeHeight, center = true);

                        }
                }
            }
        }
        
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
    rotate([0, 0, -shaftToHandle]) rotate([0, 0, -90]) union()
    {
        difference()
        {
            cylinder(flangeHeight + knobHeight + shaftHeight + 1.5, d = shaftDiameter);
            translate([shaftDiameterMinusFlatted - shaftDiameter / 2, -4, 0]) {cube([12, 12, flangeHeight + knobHeight + handleHeight]);}
        }
        cylinder(rotarySwitchHeight, d = rotarySwitchDiameter);
    }

}
