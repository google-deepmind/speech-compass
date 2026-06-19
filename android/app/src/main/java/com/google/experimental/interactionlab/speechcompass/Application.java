// Sketch.java
package com.google.experimental.interactionlab.speechcompass;

import static com.google.experimental.interactionlab.graphics.Graphics.isIncluded;

import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.speech.RecognitionListener;
import android.speech.SpeechRecognizer;
import android.view.KeyEvent;

import com.google.experimental.interactionlab.graphics.Graphics;
import com.google.experimental.interactionlab.graphics.Graphics.GLYPH_STYLE;
import com.google.experimental.interactionlab.sensor.SensorBuffer;
import com.google.experimental.interactionlab.serial.SerialPort;
import com.google.experimental.interactionlab.speech.SpeechRecognizerManager;
import com.google.experimental.interactionlab.speech.SpeechRecognizerManager.SPEECH_RECOGNITION_ENGINE;
import com.google.experimental.interactionlab.speech.SpeechResult;

import com.google.experimental.interactionlab.ui.Ui;
import com.google.experimental.interactionlab.ui.UiToggle;
import com.google.experimental.interactionlab.ui.UiToggleListener;
import java.util.ArrayList;

import java.util.Vector;
import processing.core.PApplet;

public class Application extends PApplet implements RecognitionListener, UiToggleListener {

  private static final String LABEL_SPEECH_DEFAULT = "   Default";
  private static final String LABEL_SPEECH_SSBG_AGSA = "   Speech Services by Google or AGSA";
  private static final String LABEL_SPEECH_AIAI = "   AiAi";
  // Speech recognition
  private SpeechRecognizerManager speech;
  boolean listening = false; // track state
  int results = 0;

  // Serial port to connect to SpeechCompass board
  boolean enableSerial = true;
  SerialPort port = new SerialPort();
  float simulatedAngle = 0; // simulate angle if serial port (SpeechCompass board) is not available

  // Track realtime angles
  float angle;
  final float CONFIDENCE_THRESHOLD = 65;

  // Control which sectors to show
  boolean showTop = true;
  boolean showBottom = true;
  boolean showLeft = true;
  boolean showRight = true;

  // Typography
  int verticalPosition = 0;  // track text position
  int th; // text height
  float tdy; // line spacing

  // Toggle debug printing
  boolean printVerbose = false;
  boolean startInSettings = true;
  String debugText = "";
  String debugTextEvent;
  boolean drawHighlightBox = true;
  boolean drawGlyph = false;
  Graphics.GLYPH_STYLE glyphStyle = Graphics.GLYPH_STYLE.ARROW;
  boolean drawEdgeGlyph = false;
  boolean drawCircleMap = false;
  private boolean drawColoredText = false;
  public boolean drawRms = true;

  boolean isFold = false;

  SensorBuffer angles = new SensorBuffer();
  SensorBuffer rms = new SensorBuffer();
  SensorBuffer rmsAngle = new SensorBuffer();

  SensorBuffer rmsShort = new SensorBuffer();
  float rmsValue = -1;
  Ui ui;

  public static final String LABEL_ARROW = "   Arrow";
  public static final String LABEL_DOT = "   Dot";
  public static final String LABEL_LINE = "   Line";

  public static final String LABEL_TEST_DATA = "Test data";
  public static final String LABEL_SIMULATE_TRANSCRIPT = "Simulate transcript";

  private boolean showTranscript = true;

  private boolean doCorrectAngles = false;

  enum APP_STATE { TRANSCRIPT, SETTINGS };
  APP_STATE state = APP_STATE.SETTINGS;

  Vector<Integer> sessionTimes = new Vector<>();
  Vector<Float> sessionAngles = new Vector<>();

  public void setColorMode(int color)
  {
    if (color == this.HSB)
      //hue = 0-360, saturation = 0-100, brightness = 0-100
      colorMode(HSB, 360, 100, 100);
    else
      colorMode(RGB, 255, 255, 255);
  }

  public void draw() {

    rmsShort.add(rmsValue);
    rms.add(rmsShort.getAverage());

    th = height / 20;
    textSize(th);
    tdy = th * 1.2f;
    textAlign(BASELINE);

    debugText = "";
    setColorMode(HSB);

    background(30);

    switch (state)
    {
      case TRANSCRIPT:
        drawTranscript();
        break;
      case SETTINGS:
        drawUiSettings();
        break;
    }

    rmsAngle.add(angle);

    if (drawRms)
      drawRms();
  }

  public void drawTranscript()
  {
    verticalReset();

    float minHeight = drawRms ? height * 0.1f : 0;

    float[] angle_confidence = readSerial();

    SimulatedTranscript transcript = SimulatedTranscript.pop(millis());
    if (transcript != null)
    {
      SpeechResult.insert(transcript.transcript, transcript.angle, 100);
    }

    //add to buffer if:
    // 1) new data received
    // 2) above confidence
    // 3) we are currently getting speech (listening)
    if (angle_confidence != null && angle_confidence[1] > CONFIDENCE_THRESHOLD && listening) {
      angles.add(angle_confidence[0]);
      sessionAngles.add(angle_confidence[0]);
      sessionTimes.add(millis());
    }

    float yaw = angles.getAverage();

    if (angle_confidence == null) {
      simulatedAngle += 1;
      if (simulatedAngle >= 360)
        simulatedAngle = 0;

      yaw = simulatedAngle;
      yaw = 5;
    }

    angle = yaw;

    pushMatrix();

        if (drawCircleMap) {
          translate(0, 0);
          Graphics.drawCircleMap(this, yaw, showLeft, showRight, showTop, showBottom);
        }

        if (drawEdgeGlyph) {
  //        yaw = 170;
          Graphics.drawEdgeGlyph(this, yaw, width/16 + width/64 * rmsShort.getAverage());
        }

        fill(100);

        float animationTime = 150;

        if (SpeechResult.newLines > 0) { //if there are new lines
          if (SpeechResult.newLineTime == 0) { //if at the start of new line fading in
            SpeechResult.newLineTime = millis(); //set the current time stamp
          } else {
            if (millis() - SpeechResult.newLineTime >= animationTime) { //time elapsed >= animation time?
              SpeechResult.newLines = max(0, SpeechResult.newLines - 1); //decrease number of lines to fade in
              SpeechResult.newLineTime = 0; //set time = 0
            }
          }
        }

        float previousAngle = -1;

        //calculate t = [0..1] for text line fading in
        float t = min(1, (millis() - SpeechResult.newLineTime)/animationTime);
        if (SpeechResult.newLineTime == 0) //...but if time is 0 (reset)
          t = 0; //set t = 0, to not do anything

        //translate to bottom of window (height), come back 1 line (-tdy) and then fade in
        //remaining lines (tdy * newLines), subtract the fraction for the new line fading in (-t * tdy)
        verticalFeed(height - 0.5f * tdy + tdy * (SpeechResult.newLines - t), minHeight);

        //indent
        translate(width / 20, 0);
        //start with the most recent result and work upwards
        for (int i = SpeechResult.results.size() - 1; i >= 0; i--) {
          SpeechResult r = SpeechResult.results.get(i);
          if (r == null)
            continue;

          float angle = r.getAngle();
          ArrayList<String> lines = r.getLines();

          //skip results that fall into excluded quadrants
          if (!isIncluded(angle, showLeft, showRight, showTop, showBottom))
            continue;

          boolean mostRecent = i == SpeechResult.results.size() - 1;

          if (mostRecent) {
            fill(255); // emphasize the latest result
          } else
            fill(150); // deemphasize older

          float yStart = verticalPosition;

          //for each speech result, unpack multiple lines if they exist
          for (int j = lines.size() - 1; j >= 0; j--) {
            colorMode(RGB);
            stroke(200);
            colorMode(HSB);
            fill(angle, 40, 100);
            strokeWeight(5);

            float rr = tdy / 2;

            pushMatrix();

            if (drawGlyph && showTranscript) {
              translate(rr, 0);

              //draw glyph for first line (only)
              if (j == 0) {
                Graphics.drawGlyph(this, angle, tdy, glyphStyle);
              }
            }

            if (!drawColoredText) {
              fill(0, 0, 100);
            }

            if (showTranscript)
              text(lines.get(j), 0, 0);

            popMatrix();

            boolean proceed = verticalFeed(-tdy, minHeight);
            if (!proceed)
              break;
          }

          float yEnd = verticalPosition;

          noFill();

          if (drawHighlightBox && showTranscript) {
            strokeWeight(5);

            int colorBg = Color.DKGRAY;
            stroke(colorBg);
            float p[] = {-tdy/3, 0, width - tdy/2, -(yEnd - yStart) + tdy / 2};
            rectMode(CORNERS);
            rect(p[0], p[1], p[2], p[3]);
            //float[] Graphics.getIntersection(p[0], p[1], p[2], p[3], angle);
            float angleWidth = 15;
            int colorFg = color(angle, 40, 100);
            Graphics.drawRectangleHighlightLine(this, p[0], p[1], p[2], p[3], 180 + angle - angleWidth / 2, angleWidth, colorBg, colorFg);
            Graphics.drawRectangleHighlightLine(this, p[0], p[1], p[2], p[3], 180 + angle + angleWidth / 2, angleWidth, colorFg, colorBg);
          }

          ///TODO: not working yet
          //check that previousAngle has been initialized and if the diff more than 45 degrees, add linebreak
          //if (previousAngle >= 0 && abs(previousAngle - r.angle) >= 45)

          boolean proceed = verticalFeed(-0.5f * tdy, minHeight);
          if (!proceed) {
            break;
          }

          previousAngle = angle;
        }

        popMatrix();

      ps("vertical position: " + verticalPosition + " | " + height);

      if (printVerbose) {
        th /= 2;
        textSize(th);
        tdy = th * 1.2f;
        text(debugText, 10, 60);
        text(debugTextEvent, 10, 600 + tdy);
      }

    }

  // --- Drawing | end ---

  public void drawRms()
  {
    float k1 = 5;
    float k2 = -5;

    setColorMode(HSB);

    pushMatrix();

    translate(0, 50);

    for (int i = 0; i < rms.values.length; i++) {
      int j = (i + rms.getIndex() + 1) % rms.values.length;
      float v = rms.values[j];
      float x = i * 8;
      float yaw = rmsAngle.values[j];

//      float s = (j % 8) / 8.0f;
      float s = v / 10.0f;
//      stroke(100 + 155 * s, 100, 100);

      stroke(yaw, 40, 20 + 70 * s);

      line(x, k1 * v, x, k2 * v);
    }

    popMatrix();
  }

  public void drawUiSettings()
  {
    ui.draw();

    // --- Preview of UI elements ---

    setColorMode(HSB);

    simulatedAngle += 0.25f;
    if (simulatedAngle >= 360)
      simulatedAngle = 0;

    float yaw = simulatedAngle;
    angle = yaw;

    if (drawCircleMap)
      Graphics.drawCircleMap(this, yaw, showLeft, showRight, showTop, showBottom);

    if (drawEdgeGlyph) {
      Graphics.drawEdgeGlyph(this, yaw, width/16 + width/64 * rmsShort.getAverage());
    }

    pushMatrix();

    //indent
    translate(width / 20, height - tdy);
    textSize(th);

    colorMode(RGB);
    stroke(200);
    setColorMode(HSB);
    fill(yaw, 40, 100);
    strokeWeight(5);
    textAlign(BASELINE);

    if (drawHighlightBox) {
      noFill();
      strokeWeight(5);

      // float yStart = tdy * 1.8f;
      // float yEnd = tdy * 2;

      float yStart = tdy;
      float yEnd = 0;

      int colorBg = Color.DKGRAY;
      stroke(colorBg);
      float p[] = {-tdy/3, 0, width - tdy/2, -(yEnd - yStart) + tdy / 2};
      rectMode(CORNERS);
      pushMatrix();
      translate(0, -1.5f * tdy);
      rect(p[0], p[1], p[2], p[3]);
      //popMatrix();
      //float[] Graphics.getIntersection(p[0], p[1], p[2], p[3], angle);
      float angleWidth = 15;
      int colorFg = color(yaw, 40, 100);
      Graphics.drawRectangleHighlightLine(this, p[0], p[1], p[2], p[3], 180 + yaw - angleWidth / 2, angleWidth, colorBg, colorFg);
      Graphics.drawRectangleHighlightLine(this, p[0], p[1], p[2], p[3], 180 + yaw + angleWidth / 2, angleWidth, colorFg, colorBg);
      popMatrix();
    }

    if (drawGlyph) {
      translate(tdy/2, 0);
      pushMatrix();
      translate(0, -tdy/2);
      Graphics.drawGlyph(this, yaw, tdy, glyphStyle);
      popMatrix();
    }

    if (!drawColoredText) {
      fill(0, 0, 100);
    }
    text("SpeechCompass!", 0, -tdy/2);

    popMatrix();

  }


  // --- Set up | start ---
  public void settings() {
    size(width, height, P3D);
    fullScreen();
  }

  public void setup() {
    ps(width + " " + height);
    requestPermission("android.permission.RECORD_AUDIO");
    requestPermission("android.permission.INTERNET");

    //Pixel 7
    //1080x2264

    //Pixel Fold
    //1080x1959 --> closed_portrait
    //1959x1080 --> closed_landscape
    //1840x2208 --> open_portrait
    //2208x1840 --> open_landscape

    println(width + " " + height);

    if ((width == 1840 && height == 2208) || (height == 1840 && width == 2208))
    {
      SpeechResult.screenWidth = width / 2;
      println("Pixel Fold");
      isFold = true;
    }

    angles.init(50);
    rms.init(width/8);
    rmsAngle.init(width/8);
    rmsShort.init(50);

    ui = new Ui(this);

    ui.add("VISUALIZATIONS _______________________ ");
    ui.add("Color text", "drawColoredText");
    ui.add("Direction indicator", "drawGlyph");
    ui.add(LABEL_ARROW, "useArrow", this);
//    ui.add(LABEL_DOT, "useDot", this);
    ui.add(LABEL_LINE, "useLine", this);
    ui.add("Live edge indicator", "drawEdgeGlyph");
    ui.add("Mini map", "drawCircleMap");
    ui.add("Highlight box", "drawHighlightBox");
    ui.add("RMS (volume)", "drawRms");
    ui.add("Transcript", "showTranscript");

    ui.add("");
    ui.add("SETUP / TEST / DEBUGGING  ____________________ ");
    ui.add("Start with settings?", "startInSettings");
    ui.add(LABEL_TEST_DATA, this);
    ui.add(LABEL_SIMULATE_TRANSCRIPT, this);
    ui.add("Correct angles", "doCorrectAngles");
    ui.add("Print verbose", "printVerbose");

    ui.add("");
    ui.add("RECOGNITION [needs restart] __________ ");
    ui.add(LABEL_SPEECH_DEFAULT, "default",this);
    ui.add(LABEL_SPEECH_SSBG_AGSA, "ssbg_agsa", this);
    ui.add(LABEL_SPEECH_AIAI, "aiai", this);

    ui.setVisible(true);

    ui.loadSettings();

    this.state = startInSettings ? APP_STATE.SETTINGS : APP_STATE.TRANSCRIPT;

    SPEECH_RECOGNITION_ENGINE engine = SPEECH_RECOGNITION_ENGINE.DEFAULT;

    if (ui.getValue(LABEL_SPEECH_SSBG_AGSA))
      engine = SPEECH_RECOGNITION_ENGINE.SSBG_AGSA;
    else if (ui.getValue(LABEL_SPEECH_AIAI))
      engine = SPEECH_RECOGNITION_ENGINE.AIAI;

    this.speech = new SpeechRecognizerManager(getContext(), this, engine);
  }

  @Override
  public boolean onUiToggleChange(UiToggle uiToggle) {
    println(uiToggle.getLabel());

    if (uiToggle.getLabel().equals(LABEL_SPEECH_DEFAULT)) {
      ui.setValue(LABEL_SPEECH_AIAI, false);
      ui.setValue(LABEL_SPEECH_SSBG_AGSA, false);
    }
    else if (uiToggle.getLabel().equals(LABEL_SPEECH_AIAI)) {
      ui.setValue(LABEL_SPEECH_DEFAULT, false);
      ui.setValue(LABEL_SPEECH_SSBG_AGSA, false);
    }
    else if (uiToggle.getLabel().equals(LABEL_SPEECH_SSBG_AGSA)) {
      ui.setValue(LABEL_SPEECH_AIAI, false);
      ui.setValue(LABEL_SPEECH_DEFAULT, false);
    }
    else if (uiToggle.getLabel().equals(LABEL_TEST_DATA)) {
      SpeechResult.insert("Barbara here! I am right across from you!", 90, 100);
      SpeechResult.insert("On your right side, it's me, Charlotte", 170, 100);
      SpeechResult.insert("Nice to meet you all, this is my own speech", 270, 100);
      SpeechResult.insert("Hi, my name is Alice and I am sitting to the left", 0, 100);
    } else if (uiToggle.getLabel().equals(LABEL_SIMULATE_TRANSCRIPT)) {
      SimulatedTranscript.push("User", "Chiong Lai", 3.357, 4.709, "I have an idea for a new app!");
      SimulatedTranscript.push("World Talker", "Bhanu Guda", 5.8, 6.34, "What is it?");
      SimulatedTranscript.push("User", "Chiong Lai", 7.211, 10.4,
          "It's a social media app for people who are passionate about food.");
      SimulatedTranscript.push("World Talker", "Bhanu Guda", 11.397, 15.4,
          "That sounds interesting. What would make it different from other social media apps?");
      SimulatedTranscript.push("User", "Chiong Lai", 15.9, 23.7,
          "Well, it would be focused on food and cooking. Users would be able to share recipes, photos of their food, and cooking tips.");
      SimulatedTranscript.push("World Talker", "Bhanu Guda", 24.734, 29.6,
          "That sounds like a great idea! I love food, so I would definitely use an app like that.");
      SimulatedTranscript.push("User", "Chiong Lai", 30.483, 36.501,
          "Me too! I think it would be a really cool and engaging way to connect with other food lovers.");
      SimulatedTranscript.push("World Talker", "Bhanu Guda", 37.7, 39.9,
          "I agree. So, what are the next steps?");
      SimulatedTranscript.push("User", "Chiong Lai", 40.625, 47.703,
          "I think we need to start by creating a prototype of the app. We can use the whiteboard to sketch out the user interface and some of the features.");
      SimulatedTranscript.push("World Talker", "Bhanu Guda", 48.813, 50.896,
          "Cool! I am excited to get started.");
    }
    else if (uiToggle.getLabel().equals(LABEL_ARROW) && !uiToggle.get())
    {
      ui.setValue(LABEL_LINE, false);
      ui.setValue(LABEL_DOT, false);
      glyphStyle = GLYPH_STYLE.ARROW;
    }
    else if (uiToggle.getLabel().equals(LABEL_DOT) && !uiToggle.get())
    {
      ui.setValue(LABEL_LINE, false);
      ui.setValue(LABEL_ARROW, false);
      glyphStyle = GLYPH_STYLE.DOT;
    }
    else if (uiToggle.getLabel().equals(LABEL_LINE) && !uiToggle.get())
    {
      ui.setValue(LABEL_ARROW, false);
      ui.setValue(LABEL_DOT, false);
      glyphStyle = GLYPH_STYLE.LINE;
    }
    else
      return false;

    return true;
  }

  public Application(Context context) {

    SpeechResult.p = this;

    if (enableSerial)
      this.port.initialize(context);
  }
  // --- Set up | end ---

  // --- Serial | start ---

  public float[] readSerial()
  {
    float a = 0; //angle
    float c = 0; //confidence

    String data = port.read();
    // println("[" + data + "]");

    if (data == null) //not connected
      return null;

    if (data.length() > 0)
    {
      try {
        String[] v = data.trim().split(",");
        c = Float.parseFloat(v[0]);
        a = Float.parseFloat(v[1]);
        // println(a + " " + c);
      }
      catch (Exception e)
      {
        e.printStackTrace();
      }
    }

    return new float[]{a, c};
  }
  // --- Serial | end ---

  // --- Interaction | start ---
  public void mousePressed()
  {
    //skip further handling if UI matched
    if (state == APP_STATE.SETTINGS && ui.onTouch(mouseX, mouseY))
      return;

    if (mouseY < height * 0.2)
      showTop = !showTop;
    else if (mouseY > height * 0.8)
      showBottom = !showBottom;
    else if (mouseX < width * 0.2)
      showLeft = !showLeft;
    else if (mouseX > width * 0.8)
      showRight = !showRight;

    ps(showTop + " " + showRight + " " + showBottom + " " + showLeft);
  }

  public void keyPressed()
  {
    switch (keyCode) {

      case KeyEvent.KEYCODE_VOLUME_UP:
      case KeyEvent.KEYCODE_LEFT_BRACKET:
        state = APP_STATE.values()[(state.ordinal() + 1) % APP_STATE.values().length];
        break;

      case KeyEvent.KEYCODE_VOLUME_DOWN:
      case KeyEvent.KEYCODE_RIGHT_BRACKET:
        state = APP_STATE.values()[(state.ordinal() - 1 + APP_STATE.values().length) % APP_STATE.values().length];
        break;

      case KeyEvent.KEYCODE_A: simulatedAngle += 1; println(simulatedAngle); break;
      case KeyEvent.KEYCODE_Z: simulatedAngle -= 1; println(simulatedAngle); break;
      case KeyEvent.KEYCODE_U: ui.toggleVisibility(); break;
      case KeyEvent.KEYCODE_C: drawColoredText = !drawColoredText; break;
      case KeyEvent.KEYCODE_R: showTranscript = !showTranscript; break;

      case KeyEvent.KEYCODE_T:
        SimulatedTranscript.push("User","Chiong Lai",3.357,4.709,"I have an idea for a new app!");
        SimulatedTranscript.push("World Talker","Bhanu Guda",5.8,6.34,"What is it?");
        SimulatedTranscript.push("User","Chiong Lai",7.211,10.4,"It's a social media app for people who are passionate about food.");
        SimulatedTranscript.push("World Talker","Bhanu Guda",11.397,15.4,"That sounds interesting. What would make it different from other social media apps?");
        SimulatedTranscript.push("User","Chiong Lai",15.9,23.7,"Well, it would be focused on food and cooking. Users would be able to share recipes, photos of their food, and cooking tips.");
        SimulatedTranscript.push("World Talker","Bhanu Guda",24.734,29.6,"That sounds like a great idea! I love food, so I would definitely use an app like that.");
        SimulatedTranscript.push("User","Chiong Lai",30.483,36.501,"Me too! I think it would be a really cool and engaging way to connect with other food lovers.");
        SimulatedTranscript.push("World Talker","Bhanu Guda",37.7,39.9,"I agree. So, what are the next steps?");
        SimulatedTranscript.push("User","Chiong Lai",40.625,47.703,"I think we need to start by creating a prototype of the app. We can use the whiteboard to sketch out the user interface and some of the features.");
        SimulatedTranscript.push("World Talker","Bhanu Guda",48.813,50.896,"Cool! I am excited to get started.");
        break;
    }
  }

  // --- Interaction | end ---

  //--- Utilities | start ---
  boolean verticalFeed(float dy, float minHeight)
  {
    verticalPosition += dy;
    translate(0, dy);

    return (verticalPosition) > minHeight;
  }

  void verticalReset() { verticalPosition = 0; }

  void ps(Object o) {
    if (printVerbose) println(o);
    debugText += o.toString() + "\n";
  }

  void ps(Object o, boolean event) {
    if (printVerbose) println(o);
    debugTextEvent += o.toString() + "\n";
  }
  //--- Utilities | end ---

  //--- Speech callbacks | start ---
  @Override
  public void onResults(Bundle bundle) {
    ArrayList<String> data = bundle.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION);

    updateResults(data.get(0));

    results = 0;
    listening = false;
    SpeechResult.endSpeech();

    if (doCorrectAngles)
      SpeechResult.results.lastElement().setAngles(sessionAngles, sessionTimes);

    speech.start();
  }
  @Override
  public void onPartialResults(Bundle bundle) {
    ArrayList<String> data = bundle.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION);
    updateResults(data.get(0));
  }

  private void updateResults(String s) {

    if (s.equals(""))
      return;

    float angle = angles.getAverage();

    if (results == 0)
      SpeechResult.insert(s, angle, 0);
    else
      SpeechResult.replace(s, angle, 0);

    results++;
  }

  @Override
  public void onEvent(int i, Bundle bundle) { ps("onEvent" + " " + bundle, true); }
  @Override
  public void onReadyForSpeech(Bundle bundle) { ps("onReadyForSpeech" + " " + bundle, true); }
  @Override
  public void onBeginningOfSpeech() { ps("onBeginningOfSpeech", true); listening = true; }
  @Override
  public void onRmsChanged(float v) { rmsValue = v; /* ps("onRmsChanged" + " " + v); */ }
  @Override
  public void onBufferReceived(byte[] bytes) { ps("onBufferReceived" + " " + bytes, true); }
  @Override
  public void onEndOfSpeech() { ps("onEndOfSpeech", true); listening = false; }
  @Override
  public void onError(int i) { ps("onError" + " " + i, true); listening = false; speech.start();}

  //--- Speech callbacks | end ---

}

