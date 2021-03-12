#include <cmath>

#include "GauFilter.h"

GauFilter::GauFilter(sc_module_name n) : sc_module(n) {
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  dont_initialize();
  reset_signal_is(i_rst, false);
}

// sobel mask
const double mask[MASK_X][MASK_Y] = {0.077847, 0.123317, 0.077847, 0.123317, 0.195346, 0.123317, 0.077847, 0.123317, 0.077847};

                                          
void GauFilter::do_filter() {
  { wait(); }
  while (true) {

      val_r = 0;
      val_g = 0;
      val_b = 0;
      wait();

    for (unsigned int v = 0; v < MASK_Y; ++v) {
      for (unsigned int u = 0; u < MASK_X; ++u) {
        
        unsigned char red = i_r.read();
        unsigned char green = i_g.read();
        unsigned char blue = i_b.read();
        
        val_r += red * mask[u][v];
        val_g += green * mask[u][v];
        val_b += blue * mask[u][v];
        wait();
        
      }
    }

    o_result_r.write(val_r);
    o_result_g.write(val_g);
    o_result_b.write(val_b);
  }
}
