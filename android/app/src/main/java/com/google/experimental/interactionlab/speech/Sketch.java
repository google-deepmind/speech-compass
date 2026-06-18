// Sketch.java
package com.google.experimental.interactionlab.speech;

import  android.content.Context;
import android.os.Bundle;
import android.speech.RecognitionListener;
import android.speech.RecognitionPart;
import android.speech.SpeechRecognizer;
import com.google.experimental.interactionlab.speech.SpeechRecognizerManager.SPEECH_RECOGNITION_ENGINE;
import java.util.ArrayList;
import processing.core.PApplet;

public class Sketch extends PApplet implements RecognitionListener {

  String result;
  boolean listening = false;
  private SpeechRecognizerManager speech;

  int th;
  float tdy;

  public void draw() {
    background(30);

    pushMatrix();

    translate(0,height - tdy);

    for (int i = SpeechResult.results.size() - 1; i >= 0; i--) {
      SpeechResult r = SpeechResult.results.get(i);
      if (i == SpeechResult.results.size() - 1)
        fill(255);
      else
        fill(150);
      text(r.text, 10, 0);
      translate(0, -tdy);
    }

    popMatrix();
  }

  public void settings()
  {

  }

  public void setup() {
    th = height/25;
    textSize(th);
    tdy = th * 1.2f;
    print(width + " " + height);
    requestPermission("android.permission.RECORD_AUDIO");
    requestPermission("android.permission.INTERNET");


    this.speech = new SpeechRecognizerManager(getContext(), this, SPEECH_RECOGNITION_ENGINE.DEFAULT);

  }

  //------ Speech callbacks ----
  @Override
  public void onReadyForSpeech(Bundle bundle) {
    println("Ready...");
    listening = true;
  }
  @Override
  public void onBeginningOfSpeech() { println("Listening...");  }
  @Override
  public void onRmsChanged(float v) { }
  @Override
  public void onBufferReceived(byte[] bytes) { }
  @Override
  public void onEndOfSpeech() { println("End..."); }
  @Override
  public void onError(int i) { println("Error...");
    speech.start();
  }
  @Override
  public void onResults(Bundle bundle) {
    ArrayList<String> data = bundle.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION);
    assert data != null;
    println(data.get(0));

//    SpeechResult.insert(data.get(0), 0, 0);

    listening = false;
    speech.start();
  }
  @Override
  public void onPartialResults(Bundle bundle) {
    ArrayList<String> data = bundle.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION);
    assert data != null;
    println("> " + data.get(0));

    // ArrayList<RecognitionPart> p = new ArrayList<RecognitionPart>();
    //
    // Object o = bundle.getParcelable(SpeechRecognizer.RECOGNITION_PARTS, p.getClass());
    //
    // println(">>>>>>" + o + " " + o.getClass());
    //
    final String SODA_EVENT = "soda_event";
    if (bundle.containsKey(SODA_EVENT)) {
      println("SODA EVENT!!!");
      // sodaEvent = ProtoParsers.get(
      //     partialResults, SODA_EVENT, SodaEvent.getDefaultInstance(),
      //     extensionRegistry);
    }

//    ArrayList<RecognitionPart> parts = bundle.get(SpeechRecognizer.RECOGNITION_PARTS);
  }
  @Override
  public void onEvent(int i, Bundle bundle) { }
  //------ end of Speech callbacks ----


  public Sketch(Context context) {
  }

}
