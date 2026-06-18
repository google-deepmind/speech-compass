package com.google.experimental.interactionlab.serial;

import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.driver.UsbSerialProber;

import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import androidx.appcompat.app.AppCompatActivity;
import com.google.experimental.interactionlab.speech.SpeechRecognizerManager;
import java.io.IOException;
import java.util.List;
import processing.android.CompatUtils;
import processing.android.PFragment;

public class SerialActivity extends AppCompatActivity {
  private Sketch sketch;
  public SpeechRecognizerManager manager;
  UsbSerialPort port;

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
  public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
    if (sketch != null) {
      sketch.onRequestPermissionsResult(
          requestCode, permissions, grantResults);
    }
    super.onRequestPermissionsResult(requestCode, permissions, grantResults);
  }

  public UsbSerialPort initSerial() {
    // Find all available drivers from attached devices.
    UsbManager manager = (UsbManager) getSystemService(Context.USB_SERVICE);
    List<UsbSerialDriver> availableDrivers = UsbSerialProber.getDefaultProber().findAllDrivers(manager);
    if (availableDrivers.isEmpty()) {
      sketch.println("no drivers");
      return null;
    }

    // Open a connection to the first available driver.
    UsbSerialDriver driver = availableDrivers.get(0);
    UsbDeviceConnection connection = manager.openDevice(driver.getDevice());
    if (connection == null) {
      // add UsbManager.requestPermission(driver.getDevice(), ..) handling here
      sketch.println("no connection");
      return null;
    }

    UsbSerialPort port = driver.getPorts().get(0); // Most devices have just one port (port 0)
    try {
      port.open(connection);
    } catch (IOException e) {
      sketch.println("couldn't open connection");
      throw new RuntimeException(e);
    }
    try {
      port.setParameters(115200, 8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE);
    } catch (IOException e) {
      throw new RuntimeException(e);
    }

    return port;
  }


  @Override
  public void onNewIntent(Intent intent) {
    super.onNewIntent(intent);
    if (sketch != null) {
      sketch.onNewIntent(intent);
    }
  }
}