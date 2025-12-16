#include "temp_graph.h"

// 假設全域變數
extern _tftlcd_data tftlcd_data;
extern u32 FRONT_COLOR;
extern u32 BACK_COLOR;

// *** 新增 ***: 定義 N 條曲線的顏色
// (您可以自行增減或修改這些顏色)
const u32 CURVE_COLORS[NUM_TEMP_CURVES] = {
    BLUE,
    RED,
    GREEN,
    CYAN,
    MAGENTA,
    YELLOW,
    (u32)0xFFFFA500, // 橘色
    (u32)0xFF800080  // 紫色
};


// --- 內部輔助函數 ---
static u16 MapTempToY(TempGraph *graph, float temp)
{
    // (此函數不需要修改)
    if (!graph || graph->temp_max <= graph->temp_min) return graph->y_start + graph->height;
    if (temp > graph->temp_max) temp = graph->temp_max;
    if (temp < graph->temp_min) temp = graph->temp_min;
    float range = graph->temp_max - graph->temp_min;
    if (range < 0.01f) return graph->y_start + graph->height / 2;
    float scaled_pos = ((temp - graph->temp_min) / range);
    u16 y_pixel = graph->y_start + graph->height - (u16)(scaled_pos * graph->height);
    if (y_pixel < graph->y_start) y_pixel = graph->y_start;
    if (y_pixel > graph->y_start + graph->height) y_pixel = graph->y_start + graph->height;
    return y_pixel;
}

static u16 MapIndexToX(TempGraph *graph, int index_in_buffer, int current_point_index,
                           int first_valid_global_index, int valid_data_count_global)
{
    if (!graph || graph->data_count <= 0 || first_valid_global_index == -1 || valid_data_count_global <= 0) {
        return graph->x_start;
    }

    // *** 修改 ***: 檢查 N 條曲線中是否有任一曲線有效
    bool current_is_valid = false;
    for (int i = 0; i < NUM_TEMP_CURVES; i++) {
        float temp = graph->history[index_in_buffer].temperature[i];
        if (fabsf(temp) > 0.01f && temp > -270.0f) {
            current_is_valid = true;
            break;
        }
    }

    // (以下 X 軸映射邏輯不需修改)
    int order_from_first_valid;
    if (graph->data_count < MAX_TEMP_POINTS) {
         if (!current_is_valid || index_in_buffer < first_valid_global_index) {
              return graph->x_start;
         }
         order_from_first_valid = index_in_buffer - first_valid_global_index;
    } else {
         int diff = (index_in_buffer - first_valid_global_index + MAX_TEMP_POINTS) % MAX_TEMP_POINTS;
         if (!current_is_valid || diff >= valid_data_count_global) {
              return graph->x_start;
         }
         order_from_first_valid = diff;
    }

    u16 x_pixel = graph->x_start;
    if (valid_data_count_global > 1) {
         x_pixel += (u16)(((float)order_from_first_valid / (valid_data_count_global - 1)) * graph->width);
    } else {
         x_pixel = graph->x_start;
    }

    if (x_pixel < graph->x_start) x_pixel = graph->x_start;
    if (x_pixel > graph->x_start + graph->width) x_pixel = graph->x_start + graph->width;

    return x_pixel;
}

// --- 核心繪圖函數 ---
void Graph_Draw(TempGraph *graph)
{
    if (!graph) return;
    char label[10];
    u32 original_front_color = FRONT_COLOR;
    u32 original_back_color = BACK_COLOR;

    // --- 0. 根據 history_min/max 設定當前的 Y 軸顯示範圍 ---
    // (此部分邏輯不需修改)
    if (graph->history_range_valid) {
        graph->temp_min = graph->history_min - Y_AXIS_MARGIN;
        graph->temp_max = graph->history_max + Y_AXIS_MARGIN;
        if ((graph->temp_max - graph->temp_min) < MIN_Y_AXIS_SPAN) {
            float mid = (graph->history_min + graph->history_max) / 2.0f;
            graph->temp_min = mid - MIN_Y_AXIS_SPAN / 2.0f;
            graph->temp_max = mid + MIN_Y_AXIS_SPAN / 2.0f;
        }
        if (graph->temp_max <= graph->temp_min) {
             graph->temp_max = graph->temp_min + MIN_Y_AXIS_SPAN;
        }
    } else {
        graph->temp_min = 20.0f;
        graph->temp_max = 25.0f;
        if ((graph->temp_max - graph->temp_min) < MIN_Y_AXIS_SPAN) {
            graph->temp_max = graph->temp_min + MIN_Y_AXIS_SPAN;
        }
    }

    // --- 1. 清除圖表背景 ---
    // (保持不變)
    FRONT_COLOR = GRAPH_BG_COLOR;
    LCD_Fill(graph->x_start - 35, graph->y_start, graph->x_start + graph->width, graph->y_start + graph->height + 18, GRAPH_BG_COLOR);

    // --- 2. 繪製座標軸和標籤 ---
    // (保持不變)
    FRONT_COLOR = AXIS_COLOR;
    BACK_COLOR = GRAPH_BG_COLOR;
    LCD_DrawLine(graph->x_start, graph->y_start, graph->x_start, graph->y_start + graph->height); // Y
    LCD_DrawLine(graph->x_start, graph->y_start + graph->height, graph->x_start + graph->width, graph->y_start + graph->height); // X
    FRONT_COLOR = TEXT_COLOR;
    sprintf(label, "%.1f", graph->temp_max);
    LCD_ShowString(graph->x_start - 35, graph->y_start, 30, 12, 12, (u8*)label);
    sprintf(label, "%.1f", graph->temp_min);
    LCD_ShowString(graph->x_start - 35, graph->y_start + graph->height - 12, 30, 12, 12, (u8*)label);
    float mid_temp = (graph->temp_max + graph->temp_min) / 2.0f;
    u16 mid_y = MapTempToY(graph, mid_temp);
    sprintf(label, "%.1f", mid_temp);
    LCD_ShowString(graph->x_start - 35, mid_y - 6, 30, 12, 12, (u8*)label);
    FRONT_COLOR = GRID_COLOR;
    LCD_DrawLine(graph->x_start + 1, mid_y, graph->x_start + graph->width, mid_y);
    FRONT_COLOR = TEXT_COLOR;
    BACK_COLOR = GRAPH_BG_COLOR;
    LCD_ShowString(graph->x_start, graph->y_start + graph->height + 5, 20, 12, 12, (u8*)"0");

    // *** 修改 ***: X 軸標籤顯示有效點數 (檢查 N 條曲線)
    int first_valid_global_index_draw = -1;
    int valid_data_count_global_draw = 0;
     for (int i = 0; i < graph->data_count; i++) {
        int check_index = (graph->data_count < MAX_TEMP_POINTS) ? i : (graph->current_index + 1 + i) % MAX_TEMP_POINTS;

        // *** 修改 ***: 檢查 N 條曲線中是否有任一曲線有效
        bool any_valid = false;
        for (int j = 0; j < NUM_TEMP_CURVES; j++) {
             float temp = graph->history[check_index].temperature[j];
             if (fabsf(temp) > 0.01f && temp > -270.0f) {
                 any_valid = true;
                 break;
             }
        }

        if (any_valid) { // 任一有效
            if (first_valid_global_index_draw == -1) first_valid_global_index_draw = check_index;
            valid_data_count_global_draw++;
        }
    }
    sprintf(label, "%d", valid_data_count_global_draw > 0 ? valid_data_count_global_draw -1 : 0);
    LCD_ShowString(graph->x_start + graph->width - 30, graph->y_start + graph->height + 5, 30, 12, 12, (u8*)label);


    // --- 3. 繪製曲線 (*** 重點修改 ***) ---
    if (graph->data_count >= 1 && graph->history_range_valid) {

        // *** 修改 ***: 為 N 條曲線建立追蹤變數
        int prev_buffer_index[NUM_TEMP_CURVES];
        u16 prev_x[NUM_TEMP_CURVES];
        u16 prev_y[NUM_TEMP_CURVES];

        for (int j = 0; j < NUM_TEMP_CURVES; j++) {
            prev_buffer_index[j] = -1;
            prev_x[j] = 0;
            prev_y[j] = 0;
        }

        if (first_valid_global_index_draw != -1 && valid_data_count_global_draw >= 1) {

            // 遍歷所有有效的 X 軸時間點
            for (int i = 0; i < valid_data_count_global_draw; i++) {
                 int current_buffer_index;
                 if(graph->data_count < MAX_TEMP_POINTS) {
                       current_buffer_index = first_valid_global_index_draw + i;
                 } else {
                       current_buffer_index = (first_valid_global_index_draw + i) % MAX_TEMP_POINTS;
                 }

                // 獲取當前的 X 座標 (所有曲線共用)
                u16 current_x = MapIndexToX(graph, current_buffer_index, graph->current_index, first_valid_global_index_draw, valid_data_count_global_draw);

                // *** 修改 ***: 在這個時間點上，遍歷 N 條曲線並各自繪製
                for (int j = 0; j < NUM_TEMP_CURVES; j++) {

                    float temp_j = graph->history[current_buffer_index].temperature[j];
                    bool is_valid_j = (fabsf(temp_j) > 0.01f && temp_j > -270.0f);

                    if (is_valid_j) {
                        u16 current_y_j = MapTempToY(graph, temp_j);
                        if (prev_buffer_index[j] != -1) {
                             FRONT_COLOR = CURVE_COLORS[j]; // 使用 N 條曲線的顏色
                             LCD_DrawLine(prev_x[j], prev_y[j], current_x, current_y_j);
                        }
                        prev_buffer_index[j] = current_buffer_index;
                        prev_x[j] = current_x;
                        prev_y[j] = current_y_j;
                    } else {
                        prev_buffer_index[j] = -1; // 數據無效，斷開此曲線
                    }
                } // 結束 N 條曲線的迴圈
            } // 結束 X 軸時間點的迴圈
        }
    }

    // --- 恢復原始顏色 ---
    FRONT_COLOR = original_front_color;
    BACK_COLOR = original_back_color;
}


// --- Graph_Init ---
void Graph_Init(TempGraph *graph, u16 x, u16 y, u16 w, u16 h)
{
    // (此函數不需修改)
    if (!graph) return;
    memset(graph->history, 0, sizeof(graph->history));
    graph->current_index = -1;
    graph->data_count = 0;
    graph->x_start = x;
    graph->y_start = y;
    graph->width = w;
    graph->height = h;
    graph->history_min = FLT_MAX;
    graph->history_max = -FLT_MAX;
    graph->history_range_valid = false;
    graph->temp_min = 20.0f;
    graph->temp_max = 25.0f;
    if ((graph->temp_max - graph->temp_min) < MIN_Y_AXIS_SPAN) {
         graph->temp_max = graph->temp_min + MIN_Y_AXIS_SPAN;
    }
    Graph_Draw(graph);
}


// *** 用於更新 Y 軸範圍的輔助函數 ***
static void update_history_range(TempGraph *graph, float temp)
{
    // (此函數不需要修改)
    if (fabsf(temp) > 0.01f && temp > -270.0f) {
        if (!graph->history_range_valid) {
            graph->history_min = temp;
            graph->history_max = temp;
            graph->history_range_valid = true;
        } else {
            if (temp < graph->history_min) {
                graph->history_min = temp;
            }
            if (temp > graph->history_max) {
                graph->history_max = temp;
            }
        }
    }
}

// --- Graph_AddData (*** 重點修改 ***) ---
void Graph_AddData(TempGraph *graph, float* temps_array)
{
    if (!graph) return;

    // 添加數據到環形緩衝區
    graph->current_index = (graph->current_index + 1) % MAX_TEMP_POINTS;

    // *** 修改 ***: 迴圈儲存 N 個溫度
    for (int i = 0; i < NUM_TEMP_CURVES; i++) {
         graph->history[graph->current_index].temperature[i] = temps_array[i];
    }

    if (graph->data_count < MAX_TEMP_POINTS) {
        graph->data_count++;
    }

    // *** 修改 ***: 迴圈更新 N 個溫度的 Y 軸範圍
    for (int i = 0; i < NUM_TEMP_CURVES; i++) {
        update_history_range(graph, temps_array[i]);
    }
}
