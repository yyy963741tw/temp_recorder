#ifndef TEMP_GRAPH_H
#define TEMP_GRAPH_H

#include "../tftlcd/tftlcd.h"
#include "../API.h"
#include <stdio.h>
#include <string.h> // for memset
#include <math.h>   // for fabsf
#include <float.h>  // for FLT_MAX

// --- 圖表參數定義 ---
#define GRAPH_X_START   40
#define GRAPH_Y_START   30
#define GRAPH_WIDTH     400
#define GRAPH_HEIGHT    200
#define MAX_TEMP_POINTS 100

// *** 新增 ***: 可繪製的最大曲線數量 (您可以修改這個數字)
#define NUM_TEMP_CURVES 8

#define MIN_Y_AXIS_SPAN 2.0f
#define Y_AXIS_MARGIN   1.0f

// --- 顏色定義 ---
#define AXIS_COLOR      BLACK
#define GRID_COLOR      GRAY
#define TEXT_COLOR      BLACK
#define GRAPH_BG_COLOR  WHITE

// *** 修改 ***: 使用顏色陣列來定義 N 條曲線的顏色
// (注意: 這些顏色會受您 R/B 顛倒問題的影響，但我們暫時忽略)
extern const u32 CURVE_COLORS[NUM_TEMP_CURVES];


// --- 結構定義 ---
typedef struct {
    // *** 修改 ***: 陣列大小改為 NUM_TEMP_CURVES
    float temperature[NUM_TEMP_CURVES];
} TempDataPoint;

typedef struct {
    TempDataPoint history[MAX_TEMP_POINTS];
    int current_index;
    int data_count;
    u16 x_start;
    u16 y_start;
    u16 width;
    u16 height;
    float temp_min; // Y 軸當前顯示的最小值
    float temp_max; // Y 軸當前顯示的最大值
    float history_min; // 記錄所有曲線曾經出現過的絕對最小值
    float history_max; // 記錄所有曲線曾經出現過的絕對最大值
    bool history_range_valid; // 標記 history_min/max 是否已包含有效數據
} TempGraph;

// --- 函數宣告 ---
void Graph_Init(TempGraph *graph, u16 x, u16 y, u16 w, u16 h);
// *** 修改 ***: 函式原型改為接收一個 float 陣列
void Graph_AddData(TempGraph *graph, float* temps_array);
void Graph_Draw(TempGraph *graph);

#endif // TEMP_GRAPH_H
