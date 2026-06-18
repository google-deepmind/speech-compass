package com.google.experimental.interactionlab.speech;

import static java.lang.Math.max;

import com.google.experimental.interactionlab.ai.OpenAi;
import java.util.ArrayList;

import java.util.Vector;
import processing.core.PApplet;

public class SpeechResult
{

    public Vector<Float> angles;
    public Vector<Integer> times;
    public String text;
    float angle;
    float confidence;

    ArrayList<String> lines;
    ArrayList<Integer> lineTimes = new ArrayList<>();

    public static int screenWidth = -1;

    // public final static int RESULTS_FOR_SUMMARY = 10;
    // public static int countdownToSummary = RESULTS_FOR_SUMMARY;
    public static SpeechResult summary = null;

    public static final int END_TRUNCATE_TIME = 1000; //discount angle changes in the last second
    // to account for transcription settling

    public void setAngles(Vector<Float> sessionAngles, Vector<Integer> sessionTimes) {
        this.angles = sessionAngles;
        this.times = sessionTimes;

        // if (true)
        //     return;

        if (times.size() <= 0)
            return;

        //get the last time to consider (removing some time at the end where someone else might have started speaking)
        int tEnd = times.get(times.size() - 1) - END_TRUNCATE_TIME;

        System.out.println(times.get(0) + " ---> " + times.get(times.size()-1) + " ---> " + tEnd);

        if (tEnd <= 0)
            return;

        int ti = -1; //undefined

        //search for the index that gives us the end time
        for (int i = times.size() - 1; i >= 0; i--)
        {
            System.out.println(i + ": " + times.get(i) + "<" + tEnd);
            if (times.get(i) < tEnd) {
                ti = i; //save index
                break;
            }
        }

        System.out.println("ti: " + ti + " " + times.get(ti) + " " + angles.get(ti/2));

        if (ti >= 0) //if we found an index
        {
            this.angle = angles.get(ti / 2); //set this angle to the median for that interval
        }
    }

    public static enum SUMMARY_STATE { NONE, PROCESSING, COMPLETE };
    public static SUMMARY_STATE summaryProgress = SUMMARY_STATE.NONE;

    // --- static members and methods ---

    public static Vector<SpeechResult> results = new Vector<>();
    public static PApplet p;
    static int nLines = 0;
    public static int newLines = 0;
    public static int newLineTime = 0;
    final static int MAX_RESULTS = 20; //how many results to keep in buffer

    Vector<Float> anglesSession = new Vector<Float>();

    public SpeechResult(String text, float angle, float confidence) {
        this.text = text;
        this.angle = angle;
        this.confidence = confidence;
    }

    public void createLines()
    {
        lines = new ArrayList<>();
        int l = 0;

        String[] strings = text.split(" ");
        StringBuffer stringSplit = new StringBuffer("");

        int w = screenWidth > 0 ? screenWidth : p.width;

        for (String s : strings)
        {
            try {
                if (p.textWidth(stringSplit + s) < w * 0.8f) {
                    stringSplit.append(s);
                    stringSplit.append(" ");
                } else {
                    lines.add(stringSplit.toString());
                    if (l <= lineTimes.size())
                        lineTimes.add(p.millis());
                    l++;

                    stringSplit = new StringBuffer(s + " ");
                }
            } catch(Exception e) {
                e.printStackTrace();
            }
        }

        lines.add(stringSplit.toString());
        if (l <= lineTimes.size())
            lineTimes.add(p.millis());
    }

    public float getAngle() {
        return angle;
    }

    public String getText() {
        return text;
    }

    public ArrayList<String> getLines()
    {
        return lines;
    }

    public static void startSpeech() {
    }
    public static void endSpeech() {
        results.lastElement().text = SpeechResult.results.lastElement().getText() + ".";
        results.lastElement().createLines();
    }

    public static void insert(String text, float angle, float confidence)
    {
        SpeechResult s = new SpeechResult(text, angle, confidence);
        s.createLines();

        newLines += s.lines.size();
        insert(s);
    }

    public static void replace(String text, float angle, float confidence)
    {
        SpeechResult s = new SpeechResult(text, angle, confidence);
        SpeechResult toBeReplaced = results.get(results.size()-1);
        s.createLines();
        newLines += max(0, s.lines.size() - toBeReplaced.lines.size());

        results.remove(results.size()-1);
        insert(s);
    }

    public static void insert(SpeechResult s)
    {
        results.add(s);
        if (results.size() > MAX_RESULTS)
            for (int i = 0; i < results.size() - MAX_RESULTS; i++)
                results.remove(0);
    }

    public static void summarize(String API_KEY) {
        summaryProgress = SUMMARY_STATE.PROCESSING;

        new Thread(() -> {
            ArrayList<String> text = new ArrayList<>();
            for (SpeechResult result : results) {
                float a = result.getAngle();
                //          String t = a < 90 ? "Speaker 1: " : "Speaker 2";
                String t = "";
                t += result.getText();
                text.add(t);
            }

            OpenAi ai = new OpenAi(API_KEY);
            String sum = ai.summarize(text);
            if (sum != null) {
                System.out.println(">>>" + sum);
                summary = new SpeechResult(sum, 0, 0);
                summary.createLines();
                summaryProgress = SUMMARY_STATE.COMPLETE;
                // countdownToSummary = RESULTS_FOR_SUMMARY;

            } else {
                // countdownToSummary = 0;
                summaryProgress = SUMMARY_STATE.NONE;
            }
        }).start();
    }

}