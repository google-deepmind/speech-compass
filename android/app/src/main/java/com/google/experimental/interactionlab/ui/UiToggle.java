package com.google.experimental.interactionlab.ui;

import java.lang.reflect.Field;

public class UiToggle
{
  int x1, y1, x2, y2;
  String label;
  String variable;

  Object theClass;

  Field field = null;
  boolean fieldInternal = false;
  private UiToggleListener listener = null;

  public UiToggle(Object theClass, String label, String variable, int x1, int y1, int x2, int y2) {
    this.x1 = x1;
    this.y1 = y1;
    this.x2 = x2;
    this.y2 = y2;
    this.label = label;
    this.variable = variable;
    this.theClass = theClass;

    if (variable == null)
      return;

    try {
        field = theClass.getClass().getDeclaredField(variable);
        field.setAccessible(true);
      } catch (Exception e) {
        field = null;
        System.err.println("UiToggle(...): " + variable + " not found, creating button instead of toggle.");
        //e.printStackTrace();
      }
  }

  public boolean get()
  {
    if (variable == null)
      return true;

    if (field == null)
      return fieldInternal;

    try {
      return field.getBoolean(theClass);
    } catch (Exception e) {
      System.err.println(variable);
      e.printStackTrace();
    }

    return false;
  }

  public boolean isActive()
  {
    return !(variable == null && listener == null);
  }

  public boolean isButton()
  {
    return variable == null && listener != null;
  }

  public void set(boolean value)
  {
    if (variable == null)
      return;

    if (field == null) {
      fieldInternal = value;
      return;
    }

    try {
      field.setBoolean(theClass, value);
    } catch (Exception e) {

      System.err.println(variable + " " + (field == null) + " " + fieldInternal);
      e.printStackTrace();
    }
  }

  public void onTouch()
  {
    boolean ok = true;

    if (listener != null)
      ok = listener.onUiToggleChange(this);

    if (ok)
      set(!get());
  }

  public void setListener(UiToggleListener listener) {
    this.listener = listener;
  }

  public String getVariable() {
    return variable;
  }

  public String getLabel() {
    return label;
  }
}