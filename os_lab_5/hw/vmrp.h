#include <malloc.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

// 页面结构体
struct PageInfo {
    int pageNum;
    bool referenced;  // 引用位
    bool modified;    // 修改位

    PageInfo();
    PageInfo(int num);
};

class Replace {
   public:
    Replace();
    ~Replace();

    // 基本功能
    void InitSpace(const char* MethodName);  // 初始化页号记录
    void Report(void);                       // 报告算法执行情况

    // 页面置换算法
    void Fifo(void);    // 先进先出算法
    void Lru(void);     // 最近最旧未用算法
    void Clock(void);   // 时钟(二次机会)置换算法
    void Eclock(void);  // 增强二次机会置换算法
    void Lfu(void);     // 最不经常使用置换算法
    void Mfu(void);     // 最经常使用置换算法

    // 扩展功能
    void CompareAlgorithms(void);                          // 比较所有算法性能
    void GenerateRandomPages(int pageCount, int maxPage);  // 随机生成页面引用串
    void SetPageSequence(vector<int>& pages);              // 设置页面序列

   private:
    // 数据成员
    int* ReferencePage;               // 存放要访问到的页号
    int* EliminatePage;               // 存放淘汰页号
    int* PageFrames;                  // 存放当前正在实存中的页号
    int* PageFrequency;               // 存放页面使用频率(用于LFU/MFU)
    vector<PageInfo> EnhancedFrames;  // 增强二次机会算法使用的页面信息

    int PageNumber;    // 访问页数
    int FrameNumber;   // 实存帧数
    int FaultNumber;   // 失败页数
    int clockPointer;  // 时钟指针
};