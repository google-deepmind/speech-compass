// Sketch.java
package com.google.experimental.interactionlab.ai;
import android.view.KeyEvent;
import java.util.ArrayList;
import processing.core.PApplet;

public class Sketch extends PApplet {

  int th;
  float tdy;

  OpenAi ai;

  public void draw() {
    background(30);

    pushMatrix();

    translate(0, height - tdy);

    translate(0, 0-tdy);

    popMatrix();
  }


  public void keyPressed()
  {
    switch (keyCode) {
      case KeyEvent.KEYCODE_SPACE:
      case KeyEvent.KEYCODE_VOLUME_UP:

        String[] dialogue = new String[]{"Lisa: Hi, I wonder if you could recommend a local restaurant?",
            "Anna: I guess it depends on how far you are willing to go?",
            "Lisa: I was thinking Santa Cruz, Capitola, Aptos and Soquel, but not farther than that",
            "Anna: Oh, I see. Let me think for a moment.",
            "Lisa: No worries, take your time!",
            "Anna: I think I would recommend Mentone in Aptos, if you like Italian food.",
            "Lisa: What are their specialties?",
            "Anna: They have pasta and pizza",
            "Lisa: How about drinks?",
            "Anna: Very good cocktails",
            "Lisa: Hmmm... how about something more American?",
            "Anna: Like traditional or modern?",
            "Lisa: Modern!",
            "Anna: Oh, then you may want to check out Persephone or the Home restaurant in Soquel",
            "Lisa: Do they serve local ingredients?",
            "Anna: Yes, Home even has its own garden with vegetables in the back!",
            "Lisa: Oh, that sounds amazing. Thank you very much!"};

        ArrayList<String> strings = new ArrayList<>();
        for (String s : dialogue)
          strings.add(s);

        try {
          ai.summarize(strings);
        }
        catch (Exception e)
        {e.printStackTrace(); }
        break;

      case KeyEvent.KEYCODE_C:
        println("c");
        break;
    }
  }

  public void settings() {

  }

  public void setup() {

    String API_KEY = "";
    ai = new OpenAi(API_KEY);

    th = height / 25;
    textSize(th);
    tdy = th * 1.2f;
    print(width + " " + height);
  }


}