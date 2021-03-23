# HW2 Report 蕭詠倫 109061634

## part1 用 systemC 程式對 bmp 圖檔進行 Gaussian Blur (高斯模糊)

### 結果

#### 原始圖檔 
![image](https://github.com/lplp9312/Implement-of-Many-Core-System/blob/master/hw2/gua_systemC_1/lena_std_short.bmp)

#### 模糊處理 
![image](https://github.com/lplp9312/Implement-of-Many-Core-System/blob/master/hw2/gua_systemC_1/build/out.bmp)

#### 模擬時間
![image](https://github.com/lplp9312/Implement-of-Many-Core-System/blob/master/hw2/gua_systemC_1/part_1_sim_time.jpg)

### systemC 架構
![image](https://github.com/lplp9312/Implement-of-Many-Core-System/blob/master/hw1/systemC.jpg)

### systemC程式碼重點介紹

#### 用來進行 Gaussian Blur 的3X3矩陣 (GauFilter.cpp)

        const double mask[MASK_X][MASK_Y] = {0.077847, 0.123317, 0.077847, 0.123317, 0.195346, 0.123317, 0.077847, 0.123317, 0.077847};

#### Testbench,GauFilter,FIFO_channel,clk,rst 的建立和連接 (main.cpp)

        Testbench tb("tb");
        GauFilter gau_filter("gau_filter");
        sc_clock clk("clk", CLOCK_PERIOD, SC_NS);
        sc_signal<bool> rst("rst");
        
        sc_fifo<unsigned char> r;
        sc_fifo<unsigned char> g;
        sc_fifo<unsigned char> b;
        sc_fifo<int> result_r;
        sc_fifo<int> result_g;
        sc_fifo<int> result_b;

        tb.i_clk(clk);
        tb.o_rst(rst);

        gau_filter.i_clk(clk);
        gau_filter.i_rst(rst);
        tb.o_r(r);
        tb.o_g(g);
        tb.o_b(b);

        tb.i_result_r(result_r);
        tb.i_result_g(result_g);
        tb.i_result_b(result_b);

        gau_filter.i_r(r);
        gau_filter.i_g(g);
        gau_filter.i_b(b);
        
        gau_filter.o_result_r(result_r);
        gau_filter.o_result_g(result_g);
        gau_filter.o_result_b(result_b);

#### Testbench <sc_module> 6條 FIFO_channel 的連接 (Testbench.h)

        sc_fifo_out<unsigned char> o_r;
        sc_fifo_out<unsigned char> o_g;
        sc_fifo_out<unsigned char> o_b;

        sc_fifo_in<int> i_result_r;
        sc_fifo_in<int> i_result_g;
        sc_fifo_in<int> i_result_b;

#### GauFilter <sc_module> 6條 FIFO_channel 的連接 (GauFilter.h)

        sc_fifo_in<unsigned char> i_r;
        sc_fifo_in<unsigned char> i_g;
        sc_fifo_in<unsigned char> i_b;

        sc_fifo_out<int> o_result_r;
        sc_fifo_out<int> o_result_g;
        sc_fifo_out<int> o_result_b;

#### Testbench <sc_module> 中，讀取原始圖檔 RGB，並分別寫入 r,g,b <sc_fifo> (Testbench.cpp)

        for (y = 0; y != height; ++y)
        {
            for (x = 0; x != width; ++x)
            {
                adjustX = (MASK_X % 2) ? 1 : 0; // 1
                adjustY = (MASK_Y % 2) ? 1 : 0; // 1
                xBound = MASK_X / 2;            // 1
                yBound = MASK_Y / 2;            // 1

                for (v = -yBound; v != yBound + adjustY; ++v)
                { //-1, 0, 1
                    for (u = -xBound; u != xBound + adjustX; ++u)
                    { //-1, 0, 1
                        if (x + u >= 0 && x + u < width && y + v >= 0 && y + v < height)
                        {
                            R = *(source_bitmap + bytes_per_pixel * (width * (y + v) + (x + u)) + 2);
                            G = *(source_bitmap + bytes_per_pixel * (width * (y + v) + (x + u)) + 1);
                            B = *(source_bitmap + bytes_per_pixel * (width * (y + v) + (x + u)) + 0);
                        }
                        else
                        {
                            R = 0;
                            G = 0;
                            B = 0;
                        }
                        o_r.write(R);
                        o_g.write(G);
                        o_b.write(B);
                    }
                }

            //......

            }
        }


#### GauFilter <sc_module> 中，從 r,g,b <sc_fifo> 讀取資料，並用 3X3 Gaussian Blur 矩陣對其進行卷積，並分別寫入 result_r,result_g,result_b <sc_fifo> (GauFilter.cpp)

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

#### Testbench <sc_module> 中，從 result_r,result_g,result_b <sc_fifo> 讀取資料，並寫入 *target_bitmap 矩陣，作為輸出圖檔的矩陣 (Testbench.cpp)

        for (y = 0; y != height; ++y) {
            for (x = 0; x != width; ++x) {

                //.....

                total_r = i_result_r.read();
                total_g = i_result_g.read();
                total_b = i_result_b.read();

                *(target_bitmap + bytes_per_pixel * (width * y + x) + 2) = total_r;
                *(target_bitmap + bytes_per_pixel * (width * y + x) + 1) = total_g;
                *(target_bitmap + bytes_per_pixel * (width * y + x) + 0) = total_b;

        }


## part2 用 systemC 程式對 bmp 圖檔進行 Gaussian Blur (高斯模糊)

### 結果

#### 原始圖檔 
![image](https://github.com/lplp9312/Implement-of-Many-Core-System/blob/master/hw2/gua_systemC_2/lena_std_short.bmp)

#### 模糊處理 
![image](https://github.com/lplp9312/Implement-of-Many-Core-System/blob/master/hw2/gua_systemC_2/build/out.bmp)

#### 模擬時間
![image](https://github.com/lplp9312/Implement-of-Many-Core-System/blob/master/hw2/gua_systemC_2/part_2_sim_time.jpg)

### systemC 架構
![image](https://github.com/lplp9312/Implement-of-Many-Core-System/blob/master/hw1/systemC.jpg)

### systemC程式碼重點介紹

#### 用來進行 Gaussian Blur 的3X3矩陣 (GauFilter.cpp)

        const double mask[MASK_X][MASK_Y] = {0.077847, 0.123317, 0.077847, 0.123317, 0.195346, 0.123317, 0.077847, 0.123317, 0.077847};

#### Testbench,GauFilter,FIFO_channel,clk,rst 的建立和連接 (main.cpp)

        Testbench tb("tb");
        GauFilter gau_filter("gau_filter");
        sc_clock clk("clk", CLOCK_PERIOD, SC_NS);
        sc_signal<bool> rst("rst");

        sc_fifo<unsigned char> r;
        sc_fifo<unsigned char> g;
        sc_fifo<unsigned char> b;
        sc_fifo<int> result_r;
        sc_fifo<int> result_g;
        sc_fifo<int> result_b;
        sc_fifo<int> width;         //用來傳送圖檔size
        sc_fifo<int> height;        //用來傳送圖檔size

        tb.i_clk(clk);
        tb.o_rst(rst);

        gau_filter.i_clk(clk);
        gau_filter.i_rst(rst);
        tb.o_r(r);
        tb.o_g(g);
        tb.o_b(b);

        tb.i_result_r(result_r);
        tb.i_result_g(result_g);
        tb.i_result_b(result_b);

        tb.o_width(width);          //用來傳送圖檔size
        tb.o_height(height);        //用來傳送圖檔size

        gau_filter.i_r(r);
        gau_filter.i_g(g);
        gau_filter.i_b(b);

        gau_filter.o_result_r(result_r);
        gau_filter.o_result_g(result_g);
        gau_filter.o_result_b(result_b);

        gau_filter.i_width(width);  //用來傳送圖檔size
        gau_filter.i_height(height);//用來傳送圖檔size

#### Testbench <sc_module> 8條 FIFO_channel 的連接 (Testbench.h)

        sc_fifo_out<unsigned char> o_r;
        sc_fifo_out<unsigned char> o_g;
        sc_fifo_out<unsigned char> o_b;

        sc_fifo_in<int> i_result_r;
        sc_fifo_in<int> i_result_g;
        sc_fifo_in<int> i_result_b;

        sc_fifo_out<int> o_width;
        sc_fifo_out<int> o_height;

#### GauFilter <sc_module> 8條 FIFO_channel 的連接 (GauFilter.h)

        sc_fifo_in<unsigned char> i_r;
        sc_fifo_in<unsigned char> i_g;
        sc_fifo_in<unsigned char> i_b;
        sc_fifo_out<int> o_result_r;
        sc_fifo_out<int> o_result_g;
        sc_fifo_out<int> o_result_b;

        sc_fifo_in<int> i_width;
        sc_fifo_in<int> i_height;

#### Testbench <sc_module> 中，讀取原始圖檔 RGB 和 size，並分別寫入 r,g,b,width,height <sc_fifo> (Testbench.cpp)

        void Testbench::do_gau_blur()
        {
        int x, y;

        o_width.write(width);
        o_height.write(height);

        o_rst.write(false);
        wait(5);
        o_rst.write(true);
            for (y = 0; y < height; y++)
            {
                for (x = 0; x < width; x++)
                {

                    o_r.write(*(source_bitmap + bytes_per_pixel * (width * y + x + 28) + 2));
                    o_g.write(*(source_bitmap + bytes_per_pixel * (width * y + x + 28) + 1));
                    o_b.write(*(source_bitmap + bytes_per_pixel * (width * y + x + 28) + 0));
                    wait();
                }
            }
            sc_stop();
        }

#### GauFilter <sc_module> 中，從 width,height <sc_fifo> 讀取資料，並建立一個紀錄 r,g,b 資料的矩陣，然後進行初始化 (GauFilter.cpp)

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

#### GauFilter <sc_module> 中，從 r,g,b <sc_fifo> 讀取資料進 array_r,array_g,array_b，並只在第一次讀取時，讀取圖檔的 3 個 Row，之後皆採用平移的方式，只需再讀取圖檔的 1 個 Row (GauFilter.cpp)

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

            //.....

        }

#### 對存放在 array_r,array_g,array_b 的資料利用 3X3 Gaussian Blur 矩陣對其進行卷積，並分別寫入 result_r,result_g,result_b <sc_fifo> (GauFilter.cpp)

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

#### Testbench <sc_module> 中，從 result_r,result_g,result_b <sc_fifo> 讀取資料，並寫入 *target_bitmap 矩陣，作為輸出圖檔的矩陣 (Testbench.cpp)

        void Testbench::do_gau_blur_output_bmp()
        {
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {

                *(target_bitmap + bytes_per_pixel * (width * y + x) + 2) = i_result_r.read();
                *(target_bitmap + bytes_per_pixel * (width * y + x) + 1) = i_result_g.read();
                *(target_bitmap + bytes_per_pixel * (width * y + x) + 0) = i_result_b.read();
                wait();
                }
            }
        }


## 結論

在模擬時間上，確實 part2 的程式花費比較少的時間。而在讀取原始圖檔 RGB ，並分別寫入 r,g,b sc_fifo 中。在 part1 的程式中總共傳送了  
589824 個 pixel，而在 part2 的程式中總共傳送了 65536 個 pixel。所以可以從數據得知，在 part1 的程式每個 pixel 被重複傳送了 9 次  
，而在 part2 的程式每個 pixel 只會被傳送 1 次，並存放在 3 個矩陣中，所以在 r,g,b sc_fifo 的使用時間可以被大幅縮減。在 part2 的  
程式，讀取圖檔和輸出圖檔分別利用 2 個 SC_THREAD 去執行，不像 part1 的程式是共用 1 個 SC_THREAD，是因為在 GauFilter.cpp 中，卷積  
資料並分別寫入 result_r,result_g,result_b ，卻一直沒有進行讀取，造成 sc_fifo 存放太多筆資料，產生 sc_fifo 的堵塞導致程式卡死，後  
來才分別利用 2 個 SC_THREAD 去執行。
