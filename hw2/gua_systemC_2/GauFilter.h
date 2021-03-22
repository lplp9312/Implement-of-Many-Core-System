#ifndef GAU_FILTER_H_
#define GAU_FILTER_H_
#include <systemc>
using namespace sc_core;

#include "filter_def.h"

class GauFilter : public sc_module
{
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  sc_fifo_in<unsigned char> i_r;
  sc_fifo_in<unsigned char> i_g;
  sc_fifo_in<unsigned char> i_b;
  sc_fifo_out<int> o_result_r;
  sc_fifo_out<int> o_result_g;
  sc_fifo_out<int> o_result_b;

  sc_fifo_in<int> i_width;
  sc_fifo_in<int> i_height;

  SC_HAS_PROCESS(GauFilter);
  GauFilter(sc_module_name n);
  ~GauFilter() = default;

private:
  int width = 0;
  int height = 0;
  void do_filter();
  int val_r;
  int val_g;
  int val_b;
};
#endif
