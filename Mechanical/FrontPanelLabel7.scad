
$fn = 360;

buttonDiameter = 3;
shaftDiameter = 0.5 * 25.4;
knobDiameter = 1 * 25.4;
knobPosition = [0, -32.0451, 0];
labelRadius = knobDiameter / 2 + 0.125 * 25.4;

width = 73;
height = 122;

textSize = 3;
textFont = "Arial: style=Regular";

toolNames = [
    ["OFF", "center", "bottom"], 
    ["SERVO", "left", "bottom"], 
    ["PWM", "left", "center"],
    ["UART IN", "left", "center"],
    ["UART OUT", "left", "top"],
    ["SPI IN", "center", "top"],
    ["GPS SIM", "right", "top"],
    ["LOGIC", "right", "center"],
    ["TBD", "right", "center"],
    ["UTILITY", "right", "bottom"]];

label();

module labelBorder()
{
    hull()
    {
        translate([-width / 2 + 1, height / 2 - 1]) circle(2);
        translate([-width / 2 + 1, -height / 2 + 1]) circle(2);
        translate([width / 2 - 1, height / 2 - 1]) circle(2);
        translate([width / 2 - 1, -height / 2 + 1]) circle(2);
        
    }
}

module label()
{
    union()
    {
        difference()
        {
            labelBorder();
            scale(0.99999) labelBorder();
        }
        
        // Button centers
        translate([-18, -0.3295, 0]) circle(d = buttonDiameter);
        translate([-6.1287, -0.3295, 0]) circle(d = buttonDiameter);
        translate([5.7426, -0.3295, 0]) circle(d = buttonDiameter);
        translate([17.6138, -0.3295, 0]) circle(d = buttonDiameter);
        translate([29.4851, -0.3295, 0]) circle(d = buttonDiameter);
        translate([-31.3669, -14.4651, 0]) circle(d = buttonDiameter);
        
        // Selector knob center
        translate(knobPosition) circle(d = shaftDiameter);
        
        // Selector knob labels
        translate(knobPosition)
        {
            for (i = [0 : len(toolNames)])
            {
                angle = 90 - (360 / 10) * i;
                translate([cos(angle) * labelRadius, sin(angle) * labelRadius, 0])
                text(toolNames[i][0], textSize, font = textFont, halign = toolNames[i][1], valign = toolNames[i][2]);
            }
        }
        
        // LCD opening
        translate([0.4258 + 4.7625, 31.0446, 0]) square(size = [57.25, 43.18], center = true);
        
        // Header opening
        translate([0, -55.6822, 0]) scale([1.5, 1.5, 1.5]) square(size = [4 * 2.54, 2.54], center = true);
        
        // Labels for header
        translate([0.15 * 25.4, -58, 0]) text("P", textSize, font = textFont, halign = "center", valign = "top");
        translate([-0.05 * 25.4, -58, 0]) text("A", textSize, font = textFont, halign = "center", valign = "top");
        translate([-0.15 * 25.4, -58, 0]) text("B", textSize, font = textFont, halign = "center", valign = "top");
        // Ground symbol
        translate([0.05 * 25.4, -58, 0]) union()
        {
            translate([0, -textSize * 0.75 / 2, 0]) square([0.4, textSize * 0.75], center = true);
            translate([0, -textSize * 0.75 + 0.2, 0]) square([0.08 * 25.4, 0.4], center = true);
            translate([0, -textSize * 0.75 - 0.5, 0]) square([0.05 * 25.4, 0.4], center = true);
        }
        
        // Label for Help button
        translate([-31.3669, -14.4651 + buttonDiameter + 2, 0]) text("HELP", textSize, font = textFont, halign = "center", valign = "center");
        
    }
}
