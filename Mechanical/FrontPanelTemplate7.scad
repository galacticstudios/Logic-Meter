$fn = 360 / 32;

difference()
{
    // Outer edge
    polygon([
        [-35, 66.75], 
        [35.25, 66.75], 
        [35.25, 56.75],
        [43, 56.75], 
        [43, -56.75], 
        [35.25, -56.75], 
        [35.25, -66.75],
        [-35.25, -66.75], 
        [-35.25, -56.75],
        [-43, -56.75],
        [-43, 56.75], 
        [-35, 56.76],
        [-35, 66.75]
    ]);
    
    // Mounting holes
    translate([-39, 51.5, 0]) circle(d = 3.1);
    translate([39, 51.5, 0]) circle(d = 3.1);
    translate([-39, 9.5, 0]) circle(d = 3.1);
    translate([39, 9.5, 0]) circle(d = 3.1);
    translate([-39, -50.75, 0]) circle(d = 3.1);
    translate([39, -50.75, 0]) circle(d = 3.1);
    
    // Button centers
    translate([-18, -0.3295, 0]) circle(d = 2);
    translate([-6.1287, -0.3295, 0]) circle(d = 2);
    translate([5.7426, -0.3295, 0]) circle(d = 2);
    translate([17.6138, -0.3295, 0]) circle(d = 2);
    translate([29.4851, -0.3295, 0]) circle(d = 2);
    translate([-31.3669, -14.4651, 0]) circle(d = 2);
    
    // Selector knob center
    translate([0, -32.0451, 0]) circle(d = 2);
    
    // LCD opening
    translate([0.4258 + 4.7625, 31.0446, 0]) square(size = [57.25, 43.18], center = true);
    
    // Header opening
    translate([0, -55.6822, 0]) scale([1.5, 1.5, 1.5]) square(size = [4 * 2.54, 2.54], center = true);
}