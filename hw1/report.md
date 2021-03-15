# HW1 Report 蕭詠倫 109061634

## 用 C++ 程式對 bmp 圖檔進行 Gaussian Blur (高斯模糊)

### 結果 

#### 原始圖檔 
![image](https://github.com/lplp9312/Implement-of-Many-Core-System/blob/master/hw1/gau_cpp/lena.bmp)

#### 模糊處理 
![image](https://github.com/lplp9312/Implement-of-Many-Core-System/blob/master/hw1/gau_cpp/lena_gau_blur.bmp)

### C++程式碼重點介紹

#### 用來進行 Gaussian Blur 的3X3矩陣 (gau_blur.cpp)

        const double mask[MASK_X][MASK_Y] = {{0.077847, 0.123317, 0.077847}, {0.123317, 0.195346, 0.123317}, {0.077847, 0.123317, 0.077847}};
    
#### 讀取來源檔，並存入 *image_s 矩陣 (gau_blur.cpp)

        int read_bmp(const char *fname_s)
        {
            fp_s = fopen(fname_s, "rb");
            if (fp_s == nullptr)
            {
                std::cerr << "fopen fp_s error" << std::endl;
                return -1;
            }

            // move offset to 10 to find rgb raw data offset
            fseek(fp_s, 10, SEEK_SET);
            assert(fread(&rgb_raw_data_offset, sizeof(unsigned int), 1, fp_s));

            // move offset to 18 to get width & height;
            fseek(fp_s, 18, SEEK_SET);
            assert(fread(&width, sizeof(unsigned int), 1, fp_s));
            assert(fread(&height, sizeof(unsigned int), 1, fp_s));

            // get bit per pixel
            fseek(fp_s, 28, SEEK_SET);
            assert(fread(&bit_per_pixel, sizeof(unsigned short), 1, fp_s));
            byte_per_pixel = bit_per_pixel / 8;

            // move offset to rgb_raw_data_offset to get RGB raw data
            fseek(fp_s, rgb_raw_data_offset, SEEK_SET);

            size_t size = width * height * byte_per_pixel;
            image_s = reinterpret_cast<unsigned char *>(new void *[size]);
            if (image_s == nullptr)
            {
                std::cerr << "allocate image_s error" << std::endl;
                return -1;
            }

            image_t = reinterpret_cast<unsigned char *>(new void *[size]);
            if (image_t == nullptr)
            {
                std::cerr << "allocate image_t error" << std::endl;
                return -1;
            }

            assert(fread(image_s, sizeof(unsigned char), (size_t)(long)width * height * byte_per_pixel, fp_s));
            fclose(fp_s);

            return 0;
        }

#### 用3X3 Gaussian Blur 矩陣對來源檔的 RGB 三個圖層進行卷積，並存入 *image_t 矩陣 (gau_blur.cpp)

        int gau_blur()
        {
            unsigned int x, y, v, u; // for loop counter
            unsigned char R, G, B;   // color of R, G, B
            double valR = 0.0;
            double valG = 0.0;
            double valB = 0.0;
            int adjustX, adjustY, xBound, yBound;

            for (y = 0; y != height; ++y)
            {
                for (x = 0; x != width; ++x)
                {
                    adjustX = (MASK_X % 2) ? 1 : 0;
                    adjustY = (MASK_Y % 2) ? 1 : 0;
                    xBound = MASK_X / 2;
                    yBound = MASK_Y / 2;

                    valR = 0.0;
                    valG = 0.0;
                    valB = 0.0;
                    for (v = -yBound; v != yBound + adjustY; ++v)
                    {
                        for (u = -xBound; u != xBound + adjustX; ++u)
                        {
                            if (x + u >= 0 && x + u < width && y + v >= 0 && y + v < height)
                            {
                                R = *(image_s + byte_per_pixel * (width * (y + v) + (x + u)) + 2);
                                G = *(image_s + byte_per_pixel * (width * (y + v) + (x + u)) + 1);
                                B = *(image_s + byte_per_pixel * (width * (y + v) + (x + u)) + 0);

                                valR += (R)*mask[u + xBound][v + yBound];
                                valG += (G)*mask[u + xBound][v + yBound];
                                valB += (B)*mask[u + xBound][v + yBound];
                            }
                        }
                    }

                    *(image_t + byte_per_pixel * (width * y + x) + 2) = valR;
                    *(image_t + byte_per_pixel * (width * y + x) + 1) = valG;
                    *(image_t + byte_per_pixel * (width * y + x) + 0) = valB;
                }
            }

            return 0;
        }

#### 利用 *image_t 矩陣資料，輸出一個經過高斯模糊的 bmp 圖檔 (gau_blur.cpp)

        int write_bmp(const char *fname_t)
        {
            unsigned int file_size; // file size

            fp_t = fopen(fname_t, "wb");
            if (fp_t == nullptr)
            {
                std::cerr << "fopen fname_t error" << std::endl;
                return -1;
            }

            // file size
            file_size = width * height * byte_per_pixel + out_rgb_raw_data_offset;
            header[2] = (unsigned char)(file_size & 0x000000ff);
            header[3] = (file_size >> 8) & 0x000000ff;
            header[4] = (file_size >> 16) & 0x000000ff;
            header[5] = (file_size >> 24) & 0x000000ff;

            // width
            header[18] = width & 0x000000ff;
            header[19] = (width >> 8) & 0x000000ff;
            header[20] = (width >> 16) & 0x000000ff;
            header[21] = (width >> 24) & 0x000000ff;

            // height
            header[22] = height & 0x000000ff;
            header[23] = (height >> 8) & 0x000000ff;
            header[24] = (height >> 16) & 0x000000ff;
            header[25] = (height >> 24) & 0x000000ff;

            // bit per pixel
            header[28] = bit_per_pixel;

            // write header
            fwrite(header, sizeof(unsigned char), out_rgb_raw_data_offset, fp_t);

            // write image
            fwrite(image_t, sizeof(unsigned char), (size_t)(long)width * height * byte_per_pixel, fp_t);

            fclose(fp_t);

            return 0;
        }

## 用 systemC 程式對 bmp 圖檔進行 Gaussian Blur (高斯模糊)

### 結果

#### 原始圖檔 
![image](https://github.com/lplp9312/Implement-of-Many-Core-System/blob/master/hw1/gau_systemC/lena_std_short.bmp)

#### 模糊處理 
![image](https://github.com/lplp9312/Implement-of-Many-Core-System/blob/master/hw1/gau_systemC/build/out.bmp)

#### 模擬時間
![image](https://github.com/lplp9312/Implement-of-Many-Core-System/blob/master/hw1/sim_time.jpg)

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
