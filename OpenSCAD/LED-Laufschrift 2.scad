// Top lid for bulb for pinapple lamp with hole for push button
$fa=0.5; // default minimum facet angle is now 0.5
$fs=0.5; // default minimum facet size is now 0.5 mm
difference()
{
  // Main cube
  cube([57,40,15]);


  // Cubes for ESP32
  translate ([2, 10, 0])
  cube([44,20,25]);
  translate ([0, 9, 12.75])
  cube([46,22,3]);
  translate ([0, 10, 5])
  cube([3,3,20]);
  translate ([0, 27, 5])
  cube([3,3,20]);   
    
  // Outlet for cables
  translate ([40, 10, 0])
  cube([20,20,12]);
 
  // Cubes for slot for display
  translate ([52, 3.5, 8])
  cube([8,33,2]);
  translate ([50, 0, 12])
  cube([12,40,20]);
  translate ([50, 12, 0])
  cube([12,16,20]);
  
  // Fixture in Housing Bottom
  translate ([12.25, 4.5, 5])
  cube([5.5,5.5,2.5]);
  translate ([12.25, 30, 5])
  cube([5.5,5.5,2.5]);
  
  translate ([15, 7.25, 0])
  cylinder(r=1.55,h=13);
  translate ([15, 32.75, 0])
  cylinder(r=1.55,h=13);  
  
   // Fixture in Housing Side
  translate ([5, 4.5, 7.25])
  cube([2.5,5.5,5.5]);
  translate ([5, 30, 7.25])
  cube([2.5,5.5,5.5]);
  
  translate ([0, 7.25, 10])
  rotate ([0,90,0])
  { 
   cylinder(r=1.55,h=15);   
  }   
  translate ([0, 32.75, 10])
  rotate ([0,90,0])
  { 
   cylinder(r=1.55,h=15);   
  }   
}



