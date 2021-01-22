include <LCS145.scad>

platformThickness = 2; // Arbitrarily chosen, but not too thin
pcbThickness = 1.6;
lcdThickness = 2.8; // From the LCD data sheet
batteryThickness = 7;
bottomToPlatform = batteryThickness + 4;
platformToPCB = interiorHeight - bottomToPlatform - platformThickness - 
    max(lcdThickness, flangeHeight) - pcbThickness;
platformToKnobFlange = interiorHeight - flangeHeight - bottomToPlatform;
assert(platformToPCB >= 0, "Not enough height in the enclosure!");
echo("Platform to PCB: ", platformToPCB, "mm");
echo("Platform to knob flange: ", platformToKnobFlange, "mm");

difference()
{
    // Make the platform
    union()
    {
        // Platform
        cube([86, 133.5, platformThickness], center = true);
        
        // Spacers
        for (h = [0 : len(mountingHoleLocations) - 1])
        {
            translate([mountingHoleLocations[h][0], mountingHoleLocations[h][1], platformToPCB / 2 + platformThickness / 2])
                cylinder(platformToPCB, d = 5, center = true);
        }
        
        // Knob ball supports
        for (i = [0 : 1])
        {
            translate([i * 34 - 17, -26.83333, platformThickness / 2])
                difference()
                {
                    // Support
                    cylinder(platformToKnobFlange, d = 8);
                    
                    // Hole for spring. We don't how wide or deep it should be,
                    // so just mark the center and we'll drill it
                    cylinder(platformToKnobFlange, d = 3);
                    
                    // Cradle for ball. Again, we don't know what we want, 
                    // so just drill it
                }
        }
    }
    
    // "Drill" the M3 mounting holes
    for (h = [0 : len(mountingHoleLocations) - 1])
    {
        translate([mountingHoleLocations[h][0], mountingHoleLocations[h][1], 0])
            cylinder(20, d = 3, center = true);
    }
    
    // Cut hole for battery connector
    translate([-15.3857, -52.22, 0]) cube([8, 7, 20], center = true);
    
    // Cut hole for Tag-Connect
    translate([16.47, -9.5, 0]) cube([55, 24, 20], center = true);
}
