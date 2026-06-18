package com.google.experimental.interactionlab.speechcompass;

import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import androidx.appcompat.app.AppCompatActivity;
import processing.android.CompatUtils;
import processing.android.PFragment;

/**
 * @noinspection SpellCheckingInspection
 */
public class SpeechCompassActivity extends AppCompatActivity {
  private Application sketch;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    FrameLayout frame = new FrameLayout(this);

    View decorView = getWindow().getDecorView();
// Hide the status bar.
    int uiOptions = View.SYSTEM_UI_FLAG_FULLSCREEN;
    decorView.setSystemUiVisibility(uiOptions);
// Remember that you should never show the action bar if the
// status bar is hidden, so hide that too if necessary.
//     ActionBar actionBar = getActionBar();
//     actionBar.hide();

    frame.setId(CompatUtils.getUniqueViewId());
    setContentView(frame, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
        ViewGroup.LayoutParams.MATCH_PARENT));


    sketch = new Application(this);

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
  public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
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