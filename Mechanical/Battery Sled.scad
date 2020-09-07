render = "sled";
//render = "batterycover";

$fn = 50;

// Dimensions from PT-11668 data sheet (https://www.budind.com/pdf/hb11630.pdf)
G = 3.040;
H = 5.040;
J = 4.760;
K = 2.755;

// Platform thickness
pthick = 0.125;

// Battery size
bwidth = 1.8;
blength = 2.8;
bheight = 0.3;

binsideWidth = bwidth + 0.3;
binsideLength = blength + 0.3;
binsideHeight = bheight + 0.3;
boutsideWidth = binsideWidth + 0.5;
boutsideLength = binsideLength + 0.5;
boutsideHeight = binsideHeight + 0.1;

if (render == "sled")
    sled();
else if (render == "batterycover")
    batteryCover();

module sled()
{
    union()
    {
        // Build the platform with the spacers. NB: the spacers will not be hollow
        // all the way through, because of the platform. They'll need to be drilled.
        cube([H, G, pthick]);
        translate([(H - J) / 2, (G - K) / 2, 0]) spacer(0.25, 0.1285, 11 / 16);
        translate([H - (H - J) / 2, (G - K) / 2, 0]) spacer(0.25, 0.1285, 11 / 16);
        translate([(H - J) / 2, G - (G - K) / 2, 0]) spacer(0.25, 0.1285, 11 / 16);
        translate([H - (H - J) / 2, G - (G - K) / 2, 0]) spacer(0.25, 0.1285, 11 / 16);
        
        // Build the battery holder
        translate([(H - boutsideLength) / 2 + boutsideLength / 2, 
                   (G - boutsideWidth) / 2 + boutsideWidth / 2, 
                   bheight / 2 + pthick])
        difference()
        {
            cube([boutsideLength, boutsideWidth, boutsideHeight], center = true);
            cube([binsideLength, binsideWidth, boutsideHeight], center = true);
            // Make a notch for the wires
            translate([-boutsideLength / 2, -boutsideWidth / 4, 0]) cube([boutsideWidth - binsideWidth, boutsideWidth / 2, boutsideHeight], center = true);
        }
    }
}

module spacer(outerDiameter, innerDiameter, height)
{
    difference()
    {
        cylinder(h = height, d = outerDiameter);
        cylinder(h = height, d = innerDiameter);
    }
}

module batteryCover()
{
    union()
    {
        cube([boutsideLength, boutsideWidth, pthick], center = true);
        translate([0, 0, pthick]) cube([blength, bwidth, boutsideHeight - binsideHeight], center = true);
    }
}
