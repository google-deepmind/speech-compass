package com.google.experimental.interactionlab.speechcompass;

import java.util.ArrayList;

public class SimulatedTranscript {

  String talker;
  String id;
  double start;
  double end;
  String transcript;

  float angle;

  int time;

  public SimulatedTranscript(String talker, String id, double start, double end,
      String transcript) {
    this.talker = talker;
    this.id = id;
    this.start = start;
    this.end = end;

    float variation = (float)(Math.random() * 20 - 10);

    int words = transcript.split(" ").length + 1;
    time = 250 * words;


    this.angle = talker.equals("User") ? 45 + variation : 135 + variation;

    this.transcript = (talker.equals("User") ? "Left says: " : "Right says: ") + transcript;

//    this.transcript += " " + this.angle;
  }

  public static ArrayList<SimulatedTranscript> transcripts = new ArrayList<>();

  public static void push(String talker, String id, double start, double end, String transcript)
  {
    transcripts.add(new SimulatedTranscript(talker, id, start, end, transcript));
    System.out.println(transcript);
  }

  static int t = -1;
  public static int TIME_BETWEEN_TRANSCRIPTS = 1000;

  public static SimulatedTranscript pop(int millis)
  {
//    System.out.println(t + " " + millis);

    if (transcripts.size() == 0)
      return null;

    if (t < 0)
    {
      t = millis;
      return null;
    }

    if (millis - t < TIME_BETWEEN_TRANSCRIPTS)
      return null;

    t = millis;

    SimulatedTranscript transcript = transcripts.get(0);
    TIME_BETWEEN_TRANSCRIPTS = transcript.time;
    transcripts.remove(0);
    return transcript;
  }


}
