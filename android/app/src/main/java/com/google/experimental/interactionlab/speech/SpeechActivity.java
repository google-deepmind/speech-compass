package com.google.experimental.interactionlab.speech;
/*
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;

public class MainActivity extends AppCompatActivity {

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
  }
}

*/

import android.view.KeyEvent;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import processing.android.PFragment;
import processing.android.CompatUtils;

public class SpeechActivity extends AppCompatActivity {
  private Sketch sketch;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    FrameLayout frame = new FrameLayout(this);
    frame.setId(CompatUtils.getUniqueViewId());
    setContentView(frame, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
        ViewGroup.LayoutParams.MATCH_PARENT));

    sketch = new Sketch(this);

    PFragment fragment = new PFragment(sketch);
    fragment.setView(frame, this);
  }

  @Override
  public boolean onKeyDown(int keyCode, KeyEvent event) {
/*
    switch (keyCode) {
      case KeyEvent.KEYCODE_VOLUME_DOWN:
        sketch.println("down");
        if (listening) {
          listening = false;
          speechRecognizer.stopListening();
        }
        return true;

      case KeyEvent.KEYCODE_VOLUME_UP:
        sketch.println("up");
        if (!listening) {
          listening = true;
          speechRecognizer.startListening(sketch.speechRecognizerIntent);
        }
        return true;
    }
    return super.onKeyDown(keyCode, event);
  */
    return true;
  }

  @Override
  public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
    super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    if (sketch != null) {
      sketch.onRequestPermissionsResult(
          requestCode, permissions, grantResults);
    }
  }

  @Override
  public void onNewIntent(Intent intent) {
    super.onNewIntent(intent);
    if (sketch != null) {
      sketch.onNewIntent(intent);
    }
  }
}