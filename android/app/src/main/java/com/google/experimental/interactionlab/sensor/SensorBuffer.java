package com.google.experimental.interactionlab.sensor;

public class SensorBuffer
{
  //  FloatList list = new FloatList();
  protected boolean statistics = true;

  public float[] values;
  protected int index = 0;

  //statistics
  public float[] average;
  public float[] median;
  public float[] max;
  public float[] min;
  public float[] sum;
  
  protected float latest = 0;

  public static final int DEFAULT_N_SAMPLES = 30; 

  public static int m = DEFAULT_N_SAMPLES;
  
  public int first = -1;

  //n = number of values in buffer
  public void init(int n)
  {
    values = new float[n];

    average = new float[n];
    median = new float[n];
    max = new float[n];
    min = new float[n];
    sum = new float[n];

    //zero buffer
    for (int i = 0; i < values.length; i++)
    {
      values[i] = 0;
      average[i] = 0;
      max[i] = 0;
      min[i] = 0;
      sum[i] = 0;
    }
  }

  public int getFirst()
  {
	  return first;
  }

  public int getIndex()
  {
	  return index;
  }
  
  public float getAverage()
  {
	  return average[index];
  }  
  
  public void add(float v)
  {
    index = (index + 1) % values.length;
    values[index] = v;
    
    first = (index + 1) % values.length;

    if (!statistics)
      return;

    int j = (index - m + values.length) % values.length;    

    sum[index] = 0;
    average[index] = 0;

    float mx = Float.MIN_VALUE;
    float mn = Float.MAX_VALUE;

    for (int i = 0; i < m; i++)
    {
      int o = (index - m + i + values.length) % values.length;

      sum[index] += values[o];
      average[index] += values[o];       
      //      list.append(values[index]);

      if (values[o] > mx)
        max[index] = values[o];

      if (values[o] < mn)
        min[index] = values[o];
    } 

    average[index] = average[index]/m;
    
    //  list.sort();
    //  median[index] = list.get(m/2);
    //    min[index] = list.min();    
    //    max[index] = list.max();    

    //    p.println(index + " " + values[index]);
  }

}