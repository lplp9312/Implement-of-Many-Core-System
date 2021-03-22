#include <cmath>

#include "GauFilter.h"

GauFilter::GauFilter(sc_module_name n) : sc_module(n)
{
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  dont_initialize();
  reset_signal_is(i_rst, false);
}

// sobel mask
const double mask[MASK_X][MASK_Y] = {0.077847, 0.123317, 0.077847, 0.123317, 0.195346, 0.123317, 0.077847, 0.123317, 0.077847};

void GauFilter::do_filter()
{
  {
    wait();
  }
  int test = 0;
  width = i_width.read();
  height = i_height.read();

  int array_r[3][width + 2];
  int array_g[3][width + 2];
  int array_b[3][width + 2];

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < width + 2; j++)
    {
      array_r[i][j] = 0;
      array_g[i][j] = 0;
      array_b[i][j] = 0;
    }
  }

  int counter = 0;

  while (true)
  {

    if (counter == 0)
    {
      for (int i = 0; i < 3; i++)
      {
        for (int j = 0; j < width; j++)
        {
          array_r[i][j] = i_r.read();
          array_g[i][j] = i_g.read();
          array_b[i][j] = i_b.read();
        }
      }
      counter = 3;
    }
    else
    {
      for (int i = 0; i < 2; i++)
      {
        for (int j = 0; j < width; j++)
        {
          array_r[i][j] = array_r[i + 1][j];
          array_g[i][j] = array_g[i + 1][j];
          array_b[i][j] = array_b[i + 1][j];
        }
      }
      for (int k = 0; k < width; k++)
      {
        array_r[2][k] = i_r.read();
        array_g[2][k] = i_g.read();
        array_b[2][k] = i_b.read();
      }
      counter++;
    }

    wait();

    for (int s = 0; s < width; s++)
    {
      val_r = 0;
      val_g = 0;
      val_b = 0;
      for (int v = 0; v < MASK_Y; v++)
      {
        for (int u = 0; u < MASK_X; u++)
        {
          val_r += array_r[u][v + s] * mask[u][v];
          val_g += array_g[u][v + s] * mask[u][v];
          val_b += array_b[u][v + s] * mask[u][v];
          wait();
        }
      }
      o_result_r.write(val_r);
      o_result_g.write(val_g);
      o_result_b.write(val_b);
    }
  }
}
