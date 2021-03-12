# HW1 Report 蕭詠倫 109061634

## 用 C++ 程式對 bmp 圖檔進行 Gaussian Blur (高斯模糊)

### 結果 

#### 原始圖檔 
![image](https://github.com/lplp9312/Implement-of-Many-Core-System/blob/master/hw1/gau_cpp/lena.bmp)

#### 模糊處理 
![image](https://github.com/lplp9312/Implement-of-Many-Core-System/blob/master/hw1/gau_cpp/lena_gau_blur.bmp)

### C++程式碼重點介紹

#### 用來進行 Gaussian Blur 的3X3矩陣

        const double mask[MASK_X][MASK_Y] = {{0.077847, 0.123317, 0.077847}, {0.123317, 0.195346, 0.123317}, {0.077847, 0.123317, 0.077847}};
    

