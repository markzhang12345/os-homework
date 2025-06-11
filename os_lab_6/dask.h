#include <malloc.h>
#include <iomanip>
#include <iostream>
class DiskArm {
   public:
    DiskArm();
    ~DiskArm();
    void InitSpace(char* MethodName);  // 初始化寻道记录
    void Report();                     // 报告算法执⾏情况
    void FCFS();
    void SSTF();
    void SCAN();
    void CSCAN();
    void LOOK();

   private:
    int* Request;         // 磁盘请求道号
    int* Cylinder;        // ⼯作柱⾯道号
    int RequestNumber;    // 磁盘请求数
    int CurrentCylinder;  // 当前道号
    int SeekDirection;    // 磁头⽅向
    int SeekNumber;       // 移臂总数
    int SeekChange;       // 磁头调头数
};