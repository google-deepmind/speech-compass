package com.google.experimental.interactionlab.speech;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ResolveInfo;
import android.os.Build.VERSION;
import android.os.Build.VERSION_CODES;
import android.os.Handler;
import android.speech.RecognitionListener;
import android.speech.RecognitionService;
import android.speech.RecognizerIntent;
import android.speech.SpeechRecognizer;
import java.util.List;
import java.util.Locale;

public class SpeechRecognizerManager {

  Intent speechRecognizerIntent;
  SpeechRecognizer speechRecognizer;

  Context context;

  public enum SPEECH_RECOGNITION_ENGINE { DEFAULT, AIAI, SSBG_AGSA };

  // https://g3doc.corp.google.com/speech/soda/g3doc/android/recognition_service/index.md?cl=head#raw-soda-events
  /* Optional boolean to indicate if the client wants to receive raw SODA events.  */
  public static final String EXTRA_REQUEST_SODA_EVENTS = "com.google.recognition.extra.REQUEST_SODA_EVENTS";

  public SpeechRecognizerManager(Context context, RecognitionListener listener)
  {
    this(context, listener, SPEECH_RECOGNITION_ENGINE.DEFAULT);
  }

  public SpeechRecognizerManager(Context context, RecognitionListener listener, SPEECH_RECOGNITION_ENGINE engine) {
    this.context = context;

    speechRecognizerIntent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);
    speechRecognizerIntent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL,
        RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);
    speechRecognizerIntent.putExtra(RecognizerIntent.EXTRA_LANGUAGE, Locale.getDefault());
    speechRecognizerIntent.putExtra(RecognizerIntent.EXTRA_PARTIAL_RESULTS, true);

//    speechRecognizerIntent.putExtra(RecognizerIntent.EXTRA_PREFER_OFFLINE, true);
//    speechRecognizerIntent.putExtra(RecognizerIntent.EXTRA_REQUEST_WORD_TIMING, true);

    // speechRecognizerIntent.putExtra(EXTRA_REQUEST_SODA_EVENTS, true);


    setEngine(engine, listener);
  }

  public void setEngine(SPEECH_RECOGNITION_ENGINE newEngine, RecognitionListener listener) {

    Handler mainHandler = new Handler(context.getMainLooper());

    final SPEECH_RECOGNITION_ENGINE engine = newEngine;

    Runnable myRunnable = new Runnable() {
      @Override
      public void run() {

        if (speechRecognizer != null) {
          stop();
          speechRecognizer.cancel();
          speechRecognizer.destroy();
        }

        switch (engine) {
          case DEFAULT:
            speechRecognizer = SpeechRecognizer.createSpeechRecognizer(context);
            System.out.println("default");
            break;

          case SSBG_AGSA:
            speechRecognizer = SpeechRecognizer.createSpeechRecognizer(context,
                getRecognitionServiceComponent(context));
            System.out.println("ssbg_agsa");

            break;

          case AIAI:
            speechRecognizer = SpeechRecognizer.createOnDeviceSpeechRecognizer(context);
            System.out.println("aiai");
            break;
        }

        if (speechRecognizer != null) {
          speechRecognizer.setRecognitionListener(listener);
          start();
        }

      }
    };
    mainHandler.post(myRunnable);

  }


 /**
    * Gets the recognition service component provided by Google, which will be Speech Services By
   * Google (SSBG) or AGSA if SSBG doesn't provide recognition service.
      */
  public static ComponentName getRecognitionServiceComponent(Context context) {
    List<ResolveInfo> resolveInfos =
        context
            .getPackageManager()
            .queryIntentServices(new Intent(RecognitionService.SERVICE_INTERFACE), /* flags= */ 0);
    ComponentName agsaComponent = null;
    for (ResolveInfo resolveInfo : resolveInfos) {
      if (resolveInfo.serviceInfo.packageName.equals("com.google.android.tts")
          && VERSION.SDK_INT >= VERSION_CODES.S) {
        return new ComponentName(resolveInfo.serviceInfo.packageName, resolveInfo.serviceInfo.name);
      } else if (resolveInfo.serviceInfo.packageName.equals("com.google.android.googlequicksearchbox")) {
        agsaComponent =
            new ComponentName(resolveInfo.serviceInfo.packageName, resolveInfo.serviceInfo.name);
      }
    }
    if (agsaComponent != null) {
      return agsaComponent;
    } else {
      System.err.println("No speech services provided by Google.");
      return null;
    }
  }

  public void start()
  {
    speechRecognizer.startListening(speechRecognizerIntent);
  }

  public void stop()
  {
//    speechRecognizer.stopListening();
  }

}
