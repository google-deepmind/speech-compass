package com.google.experimental.interactionlab.ui;

import java.util.ArrayList;
import processing.core.PApplet;

public class Ui {

  private final PApplet p;
  int textSize;
  int x;
  int y;
  int tdy;
  boolean visible = true;
  ArrayList<UiToggle> components = new ArrayList<>();

  int cy = 0;

  public boolean toggleVisibility() {
    setVisible(!isVisible());
    return isVisible();
  }

  public boolean isVisible() {
    return visible;
  }

  public void setVisible(boolean visible) {
    this.visible = visible;
  }

  public Ui(PApplet p) {
    this.p = p;
    textSize = p.height / 40;
//p.width / 12;
    tdy = (int) (textSize * 1.4f);
    x = p.width / 20;
    y = p.height / 20;
  }

  public UiToggle add(String label, String variable, UiToggleListener listener) {
    int i = components.size();
    UiToggle c = new UiToggle(p, label, variable, 0, cy, p.width, cy + tdy);
    if (listener != null)
      c.setListener(listener);

    cy += tdy;
    p.println(cy);
    components.add(c);
    return c;
  }

  public UiToggle add(String label, String variable) {
    return add(label, variable, null);
  }

  public UiToggle add(String label) {
    return add(label, null, null);
  }

  public UiToggle add(String label, UiToggleListener listener) {
    return add(label, null, listener);
  }


  public UiToggle add(UiToggle c) {
    int i = components.size();
    c.x1 = 0;
    c.y1 = cy;
    c.x2 = p.width;
    c.y2 = cy + tdy;
    cy += tdy;
    p.println(cy);
    components.add(c);
    return c;
  }


  public void draw() {
    if (!visible)
      return;

    p.pushMatrix();

    p.noStroke();
//      fill(0, 230);
    p.fill(0);
    p.rect(0, 0, p.width, p.height);

    p.translate(x, y);

    p.colorMode(p.HSB);

    for (int i = 0; i < components.size(); i++) {
      UiToggle c = components.get(i);
      p.textSize(textSize);
      p.textAlign(p.LEFT, p.TOP);

      p.colorMode(p.RGB, 255, 255, 255);


      p.rectMode(p.CORNERS);
      p.stroke(255);
      // fill(c.get() ? 50 : 100);
      // rect(c.x1,c.y1, c.x2, c.y2);
      if (c.isActive())
        p.fill(c.get() ? 255 : 100);
      else
        p.fill(190, 150, 150);

      String label = c.isButton() ? c.label + " »" : c.label;
      p.text(label, c.x1 + tdy / 4, c.y1 + tdy / 4);
    }

    p.popMatrix();
  }

  public boolean onTouch(int x, int y) {
    if (!visible)
      return false;

    x -= this.x;
    y -= this.y;
    int index = (y / tdy);

    // p.println(index);

    if (index >= 0 && index < components.size()) {
      UiToggle c = components.get(index);
      c.onTouch();

      saveSettings();
    }

    return true;
  }

  public UiToggle get(String label)
  {
    for (UiToggle t : components)
      if (t.label.equals(label))
        return t;

    return null;
  }

  public boolean getValue(String label)
  {
    for (UiToggle t : components)
      if (t.label.equals(label))
        return t.get();

    return false;
  }

  public void setValue(String label, boolean value)
  {
    for (UiToggle t : components)
      if (t.label.equals(label))
        t.set(value);
  }

  public void saveSettings() {
    String[] strings = new String[components.size()];

    for (int i = 0; i < components.size(); i++)
      strings[i] = components.get(i).variable + "=" + components.get(i).get();

    p.saveStrings("speechcompass.settings.txt", strings);
  }

  public void loadSettings() {
    String[] strings = p.loadStrings("speechcompass.settings.txt");

    if (strings == null)
      return;

    for (int i = 0; i < components.size(); i++) {
      for (String s : strings) {
        String[] setting = s.split("=");

        if (setting != null && setting.length == 2) {
          if (setting[0].equals(components.get(i).variable)) {
            try {
              boolean value = Boolean.parseBoolean(setting[1]);
              boolean current = components.get(i).get();
              if (value != current)
                components.get(i).onTouch();
            } catch (Exception e) {
              e.printStackTrace();
            }
          }
        }
      }
    }
  }
}