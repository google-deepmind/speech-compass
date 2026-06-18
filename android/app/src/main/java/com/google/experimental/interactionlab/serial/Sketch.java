// Sketch.java
package com.google.experimental.interactionlab.serial;
import android.content.Context;
import processing.core.PApplet;

public class Sketch extends PApplet {

  int th;
  float tdy;

  String[] ports;

  SerialPort port = new SerialPort();

  public Sketch(Context context)
  {
    port.initialize(context);
  }

  public void draw() {
    String t = port.read();
    background(30);

    pushMatrix();

    translate(0, height - tdy);

    text("ports", 0, 0);
    translate(0, 0-tdy);
    if (t != null)
      text(t, 0, 0);

    popMatrix();
  }




  public void settings() {

  }

  public void setup() {
    th = height / 25;
    textSize(th);
    tdy = th * 1.2f;
    print(width + " " + height);

//    ports = Serial.list(this);
/*
    print("opening port...");
    port = activity.initSerial();
    println(port == null ? "[NOK]" : "[OK]");
*/
  }


}