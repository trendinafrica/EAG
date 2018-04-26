

import processing.serial.*;

Serial myPort;  

int counter = 0;
int inByte = 0;
float x1, y1, x2, y2, y;
float spacing = 1.0;
  
//PrintWriter output;
Table table;

//color RED = color(255, 0, 0);
//color GREEN = color(0, 255, 0);
color BLUE = color(12, 16, 255);
color WHITE = color(255, 255, 255);
color BLACK = color(0, 0, 0);

void setup () {
size(1200, 600); // window size
 
 // **** List available serial ports **** //
  String[] ports = Serial.list();
  print("No. ports = ");
  println(ports.length);
  for (int i = 0; i < ports.length; i++) {
    print("[" + i + "]");
  println(ports[i]);
  
  //output = createWriter("positions.txt"); 
  table = new Table();
  
  table.addColumn("Sec");
  table.addColumn("Reading");
  
  //TableRow newRow = table.addRow();
  //newRow.setInt("Sec", table.getRowCount() - 1);
  //newRow.setFloat("Reading", y2);
  
  saveTable(table, "2day.csv");
  }

// **** Enter selected port number here **** //
  String portName = Serial.list()[3];
  print("port selected = ");
  println(portName);
  myPort = new Serial(this, portName, 9600);
  background(BLACK);
}

void serialEvent (Serial myPort) {
  int inByte = myPort.read();
  println(inByte);
  y = height - inByte;
}

void draw () {
// line color:
  stroke(WHITE);
 
if (counter > width/spacing) {
    counter = 0;
    background(BLACK);
   }
   
   if (counter == 0) {
       x1 = 0;
       y1 = y;
   }
   
   if (counter > 0) {
     x2 = counter*spacing;
     y2 = y;
     line(x1, y1, x2, y2);
     x1 = x2;
     y1 = y2;    
    }    
   counter++;
   
   //output.println(x2 + y1);
   
  TableRow newRow = table.addRow();
  newRow.setInt("Sec", table.getRowCount() - 1);
  newRow.setFloat("Reading", y2);
  
  saveTable(table, "EAG_Data.csv");
}
