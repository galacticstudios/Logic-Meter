include <LCS145.scad>

buttons = [
    [-26.8985, 3.5199],
    [-13.8307, 3.5199],
    [-0.7629, 3.5199],
    [12.3048, 3.5199],
    [25.3726, 3.5199],
    [26.8985, -15.0661]
];

linear_extrude(3)
{
    difference()
    {
            
        // Outline for template
        polygon([[-35.617, 65.3361], [33.383, 65.3361], [33.383, 56.3361],
            [43, 56.3361], [43, -54.6639], [33.383, -54.6639],
            [33.383, -63.6639], [-32.617, -63.6639], [-32.617, -54.6639],
            [-43, -54.6639], [-43, 56.3361], [-35.617, 56.3361]]);

        // Make holes for mounting bosses
        for (hole = mountingHoleLocations)
        {
            translate([hole[0], hole[1], 0]) circle(d = 5);
        }

        // Guide holes for buttons
        for (button = buttons)
        {
            translate([button[0], button[1], 0]) circle(d = 3);
        }

        // Hole for LCD
        translate([-0.9825, 36.7453]) square([59.2, 44.8], center = true);

        // Guide hole for knob
        translate([0, -26.8333, 0]) circle(d = 3);
        
        // Hole for test leads
        translate([0, -54.5983]) square([15, 7], center = true);
    }
}
