// This is an include file used by 'LCS145 Sled.scad' and 'Knob.scad'

// High resolution circles
$fn = 90;

// Measurements for Takachi LCS145-N enclosure

interiorHeight = 21; // From the LCS145-N data sheet
mountHeight = 4.5; // From the LCS145-N data sheet
heightBetweenMounts = interiorHeight - mountHeight * 2;

// These coordinates are relative to the origin of the PCB
mountingHoleLocations = [
    [-39, 51.5], // Top left
    [39, 51.5],
    [39, 9.5],
    [39, -50.75],
    [-39, 9.5],
    [-39, -50.75]
];

// The flange on the knob
flangeHeight = mountHeight - 0.5;
