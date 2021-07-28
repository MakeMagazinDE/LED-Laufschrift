// Top lid for bulb for pinapple lamp with hole for push button
$fa=0.5; // default minimum facet angle is now 0.5
$fs=0.5; // default minimum facet size is now 0.5 mm
difference()
{
  // Main cube
  cube([12,40,8]);


  // Cubes for ESP32
  translate ([0, 10, 3])
  cube([12,20,25]);
    
  // Outlet for cables
  translate ([40, 10, 0])
  cube([20,20,12]);
  
  // Fixture in Housing Bottom
  translate ([3.25, 4.5, 3])
  cube([5.5,5.5,2.5]);
  translate ([3.25, 30, 3])
  cube([5.5,5.5,2.5]);
  
  translate ([6, 7.25, 0])
  cylinder(r=1.55,h=13);
  translate ([6, 32.75, 0])
  cylinder(r=1.55,h=13);  
}



