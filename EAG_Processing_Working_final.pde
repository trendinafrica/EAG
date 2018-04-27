

import processing.serial.*;

Serial myPort;  

int counter = 0;
int inByte = 0;
float x1, y1, x2, y2, y;
float spacing = 3.0;
int width =1200;
int height = 600;
float yoffset = height/2;
int yscaling=30;
  
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
  String portName = Serial.list()[7];
  print("port selected = ");
  println(portName);
  myPort = new Serial(this, portName, 57600);
  background(BLACK);
}

int[] Buffer = new int[17];
int counter2 =0;
boolean havePacket = false;
boolean donePlotting=true;

void serialEvent (Serial myPort) {
  if(donePlotting){
   
  
  String inByte = myPort.readStringUntil('\n');
  if (inByte!=null)
  {
    
  println(inByte);
  //Buffer[counter2]=inByte;
  inByte=trim(inByte);
  Buffer= int (split(inByte, " "));
  
  if(Buffer.length==17){
    
  
  //counter2++;
 // if (counter2==17)
  // { 
     havePacket=true;
    // counter2=0;
     //for(int i=0; i<17; i++){
      // print(Buffer[i]+" ");
     //}
  }
     println(inByte);
    
  }  
   }
  }
  
  //y = height - inByte;
//}

void draw () {
  
  if (havePacket){
    donePlotting=false;
    
// line color:
  stroke(WHITE);
 
if (counter > width/spacing) {
    counter = 0;
    background(BLACK);
   }
   
   //If you are using more channels read the entire buffer i= 4 to 16
   for (int i=4; i<5; i++){
     println(Buffer[i]);
     Buffer[i]*=yscaling; //Scalling the wave
     
   if (counter == 0) {
       x1 = 0;
       y1 =Buffer[i];
   }
   
   if (counter > 0) {
     x2 = counter*spacing;
     y2 = Buffer[i];
     line(x1, y1+yoffset, x2, y2+yoffset);
     x1 = x2;
     y1 = y2;    
    }    
   counter++;
   }
   
   donePlotting=true;
   havePacket=false;
   
   //output.println(x2 + y1);
   
  TableRow newRow = table.addRow();
  newRow.setInt("Sec", table.getRowCount() - 1);
  newRow.setFloat("Reading", y2);
  
  saveTable(table, "EAG_Data.csv");
}

}
