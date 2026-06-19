package com.google.experimental.interactionlab.serial;

import android.content.Context;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.driver.UsbSerialProber;
import java.io.IOException;
import java.util.List;

public class SerialPort {

  private UsbSerialPort port;
  private byte[] response = new byte[128];

  public static UsbSerialPort init(Context context) {
    // Find all available drivers from attached devices.
    UsbManager manager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
    List<UsbSerialDriver> availableDrivers = UsbSerialProber.getDefaultProber().findAllDrivers(manager);
    if (availableDrivers.isEmpty()) {
      System.err.println("no drivers");
      return null;
    }

    // Open a connection to the first available driver.
    UsbSerialDriver driver = availableDrivers.get(0);
    UsbDeviceConnection connection = manager.openDevice(driver.getDevice());
    if (connection == null) {
      // add UsbManager.requestPermission(driver.getDevice(), ..) handling here
      System.err.println("no connection");
      return null;
    }

    UsbSerialPort port = driver.getPorts().get(0); // Most devices have just one port (port 0)
    try {
      port.open(connection);
    } catch (IOException e) {
      System.err.println("couldn't open serial port: " + e.getMessage());
      return null;
    }
    try {
      port.setParameters(115200, 8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE);
    } catch (IOException e) {
      System.err.println("couldn't configure serial port: " + e.getMessage());
      return null;
    }

    return port;
  }
  
  public void initialize(Context context)
  {
    this.port = init(context);
  }

  StringBuffer s = new StringBuffer();

  public String read()
  {
    int len = 0;

    if (port == null)
      return null;

    // p("reading...");
    try {
      len = port.read(response, 10);
    } catch (Exception e) {
      System.err.println("serial read error: " + e.getMessage());
      return null;
    }

    // pn("[OK]");
    return new String(response, 0, len);
  }

  public void close() {
    if (port != null) {
      try {
        port.close();
      } catch (IOException e) {
        System.err.println("error closing serial port: " + e.getMessage());
      }
      port = null;
    }
  }

  public void p(String text)
  {
    System.out.print(text);
  }

  public void pn(String text)
  {
    System.out.println(text);
  }


}
