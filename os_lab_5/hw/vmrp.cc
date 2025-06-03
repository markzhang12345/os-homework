#include "vmrp.h"

// PageInfo 结构体实现
PageInfo::PageInfo() : pageNum(-1), referenced(false), modified(false) {}

PageInfo::PageInfo(int num) : pageNum(num), referenced(true), modified(false) {}

// Replace 类构造函数
Replace::Replace() {
    int i;

    // 随机生成
    int maxPage;
    cout << "Please input page numbers:";
    cin >> PageNumber;
    cout << "the max page ";
    cin >> maxPage;

    ReferencePage = new int[PageNumber];
    GenerateRandomPages(PageNumber, maxPage);

    cout << "reference page string:";
    for (i = 0; i < PageNumber; i++)
        cout << ReferencePage[i] << " ";
    cout << endl;

    EliminatePage = new int[PageNumber];

    cout << "Please input page frames:";
    cin >> FrameNumber;
    PageFrames = new int[FrameNumber];
    PageFrequency = new int[FrameNumber];
    EnhancedFrames.resize(FrameNumber);
    clockPointer = 0;
}

// Replace 类析构函数
Replace::~Replace() {
    delete[] ReferencePage;
    delete[] EliminatePage;
    delete[] PageFrames;
    delete[] PageFrequency;
}

// 随机生成页面引用序列
void Replace::GenerateRandomPages(int pageCount, int maxPage) {
    srand(time(nullptr));
    for (int i = 0; i < pageCount; i++) {
        ReferencePage[i] = rand() % maxPage + 1;
    }
}

// 设置页面序列
void Replace::SetPageSequence(vector<int>& pages) {
    if (ReferencePage)
        delete[] ReferencePage;
    PageNumber = pages.size();
    ReferencePage = new int[PageNumber];
    for (int i = 0; i < PageNumber; i++) {
        ReferencePage[i] = pages[i];
    }
}

// 初始化空间
void Replace::InitSpace(const char* MethodName) {
    int i;
    cout << endl << MethodName << endl;
    FaultNumber = 0;
    clockPointer = 0;

    for (i = 0; i < PageNumber; i++)
        EliminatePage[i] = -1;

    for (i = 0; i < FrameNumber; i++) {
        PageFrames[i] = -1;
        PageFrequency[i] = 0;
        EnhancedFrames[i] = PageInfo();
    }
}

// 报告算法执行情况
void Replace::Report(void) {
    // 报告淘汰页顺序
    cout << endl << "Eliminate page:";
    for (int i = 0; EliminatePage[i] != -1; i++)
        cout << EliminatePage[i] << " ";
    // 报告缺页数和缺页率
    cout << endl << "Number of page faults=" << FaultNumber << endl;
    cout << setw(6) << setprecision(3);
    cout << "Rate of page faults=" << 100 * (float)FaultNumber / (float)PageNumber << "%" << endl;
}

// LRU 算法实现
void Replace::Lru(void) {
    int i, j, k, l, next;
    InitSpace("LRU");

    for (k = 0, l = 0; k < PageNumber; k++) {
        next = ReferencePage[k];

        // 检查页面是否已在内存中
        for (i = 0; i < FrameNumber; i++) {
            if (next == PageFrames[i]) {
                // 移到栈顶
                next = PageFrames[i];
                for (j = i; j > 0; j--)
                    PageFrames[j] = PageFrames[j - 1];
                PageFrames[0] = next;
                break;
            }
        }

        if (i < FrameNumber) {
            // 页面命中
            for (j = 0; j < FrameNumber; j++)
                if (PageFrames[j] >= 0)
                    cout << PageFrames[j] << " ";
            cout << endl;
        } else {
            // 页面缺失
            FaultNumber++;
            EliminatePage[l] = PageFrames[FrameNumber - 1];

            for (j = FrameNumber - 1; j > 0; j--)
                PageFrames[j] = PageFrames[j - 1];
            PageFrames[0] = next;

            for (j = 0; j < FrameNumber; j++)
                if (PageFrames[j] >= 0)
                    cout << PageFrames[j] << " ";

            if (EliminatePage[l] >= 0)
                cout << "->" << EliminatePage[l++] << endl;
            else
                cout << endl;
        }
    }
    Report();
}

// FIFO 算法实现
void Replace::Fifo(void) {
    int i, j, k, l, next;
    InitSpace("FIFO");

    for (k = 0, j = l = 0; k < PageNumber; k++) {
        next = ReferencePage[k];

        // 检查页面是否已在内存中
        for (i = 0; i < FrameNumber; i++)
            if (next == PageFrames[i])
                break;

        if (i < FrameNumber) {
            // 页面命中
            for (i = 0; i < FrameNumber; i++)
                if (PageFrames[i] >= 0)
                    cout << PageFrames[i] << " ";
            cout << endl;
        } else {
            // 页面缺失
            FaultNumber++;
            EliminatePage[l] = PageFrames[j];
            PageFrames[j] = next;
            j = (j + 1) % FrameNumber;

            for (i = 0; i < FrameNumber; i++)
                if (PageFrames[i] >= 0)
                    cout << PageFrames[i] << " ";

            if (EliminatePage[l] >= 0)
                cout << "->" << EliminatePage[l++] << endl;
            else
                cout << endl;
        }
    }
    Report();
}

// CLOCK 算法实现
void Replace::Clock(void) {
    int i, k, l, next;
    vector<bool> referenceBit(FrameNumber, false);
    InitSpace("CLOCK");

    for (k = 0, l = 0; k < PageNumber; k++) {
        next = ReferencePage[k];

        // 检查页面是否已在内存中
        for (i = 0; i < FrameNumber; i++) {
            if (next == PageFrames[i]) {
                referenceBit[i] = true;  // 设置引用位
                break;
            }
        }

        if (i < FrameNumber) {
            // 页面命中
            for (i = 0; i < FrameNumber; i++)
                if (PageFrames[i] >= 0)
                    cout << PageFrames[i] << " ";
            cout << endl;
        } else {
            // 页面缺失
            FaultNumber++;

            // 寻找替换页面
            while (true) {
                if (PageFrames[clockPointer] == -1) {
                    // 空闲帧
                    PageFrames[clockPointer] = next;
                    referenceBit[clockPointer] = true;
                    clockPointer = (clockPointer + 1) % FrameNumber;
                    break;
                } else if (!referenceBit[clockPointer]) {
                    // 找到替换页面
                    EliminatePage[l] = PageFrames[clockPointer];
                    PageFrames[clockPointer] = next;
                    referenceBit[clockPointer] = true;
                    clockPointer = (clockPointer + 1) % FrameNumber;
                    break;
                } else {
                    // 给第二次机会
                    referenceBit[clockPointer] = false;
                    clockPointer = (clockPointer + 1) % FrameNumber;
                }
            }

            for (i = 0; i < FrameNumber; i++)
                if (PageFrames[i] >= 0)
                    cout << PageFrames[i] << " ";

            if (EliminatePage[l] >= 0)
                cout << "->" << EliminatePage[l++] << endl;
            else
                cout << endl;
        }
    }
    Report();
}

// eclock
void Replace::Eclock(void) {
    int k, l, next;
    InitSpace("Enhanced CLOCK");

    for (k = 0, l = 0; k < PageNumber; k++) {
        next = ReferencePage[k];

        // 检查页面是否已在内存中
        int found = -1;
        for (int i = 0; i < FrameNumber; i++) {
            if (EnhancedFrames[i].pageNum == next) {
                found = i;
                EnhancedFrames[i].referenced = true;
                // 模拟修改位
                if (rand() % 3 == 0) {
                    EnhancedFrames[i].modified = true;
                    cout << " modified" << endl;
                }
                break;
            }
        }

        if (found != -1) {
            // 页面命中
            for (int i = 0; i < FrameNumber; i++)
                if (EnhancedFrames[i].pageNum >= 0)
                    cout << EnhancedFrames[i].pageNum << " ";
            cout << endl;
        } else {
            // 页面缺失
            FaultNumber++;

            // 寻找替换页面 - 增强二次机会算法
            int victim = -1;
            int startPointer = clockPointer;

            // 第一轮：寻找 (0,0) - 未引用且未修改
            do {
                if (EnhancedFrames[clockPointer].pageNum == -1) {  // 空闲帧
                    victim = clockPointer;
                    break;
                }
                if (!EnhancedFrames[clockPointer].referenced && !EnhancedFrames[clockPointer].modified) {
                    victim = clockPointer;
                    break;
                }
                clockPointer = (clockPointer + 1) % FrameNumber;
            } while (clockPointer != startPointer);

            // 第二轮：寻找 (0,1) - 未引用但已修改，同时清除引用位
            if (victim == -1) {
                do {
                    if (!EnhancedFrames[clockPointer].referenced && EnhancedFrames[clockPointer].modified) {
                        victim = clockPointer;
                        break;
                    }
                    if (EnhancedFrames[clockPointer].referenced) {
                        EnhancedFrames[clockPointer].referenced = false;
                    }
                    clockPointer = (clockPointer + 1) % FrameNumber;
                } while (clockPointer != startPointer);
            }

            // 第三轮：寻找 (0,0) - 在清除引用位后
            if (victim == -1) {
                do {
                    if (!EnhancedFrames[clockPointer].referenced && !EnhancedFrames[clockPointer].modified) {
                        victim = clockPointer;
                        break;
                    }
                    clockPointer = (clockPointer + 1) % FrameNumber;
                } while (clockPointer != startPointer);
            }

            // 第四轮：寻找 (0,1)
            if (victim == -1) {
                do {
                    if (!EnhancedFrames[clockPointer].referenced) {
                        victim = clockPointer;
                        break;
                    }
                    clockPointer = (clockPointer + 1) % FrameNumber;
                } while (clockPointer != startPointer);
            }

            // 如果还是没找到，选择当前指针位置
            if (victim == -1)
                victim = clockPointer;

            // 执行替换
            if (EnhancedFrames[victim].pageNum >= 0) {
                EliminatePage[l++] = EnhancedFrames[victim].pageNum;
            }

            EnhancedFrames[victim].pageNum = next;
            EnhancedFrames[victim].referenced = true;
            EnhancedFrames[victim].modified = (rand() % 3 == 0);

            clockPointer = (victim + 1) % FrameNumber;

            for (int i = 0; i < FrameNumber; i++)
                if (EnhancedFrames[i].pageNum >= 0)
                    cout << EnhancedFrames[i].pageNum << " ";

            if (l > 0 && EliminatePage[l - 1] >= 0)
                cout << "->" << EliminatePage[l - 1] << endl;
            else
                cout << endl;
        }
    }
    Report();
}

// LFU 算法实现
void Replace::Lfu(void) {
    int i, k, l, next, victim;
    InitSpace("LFU");

    for (k = 0, l = 0; k < PageNumber; k++) {
        next = ReferencePage[k];

        // 检查页面是否已在内存中
        for (i = 0; i < FrameNumber; i++) {
            if (next == PageFrames[i]) {
                PageFrequency[i]++;
                break;
            }
        }

        if (i < FrameNumber) {
            // 页面命中
            for (i = 0; i < FrameNumber; i++)
                if (PageFrames[i] >= 0)
                    cout << PageFrames[i] << " ";
            cout << endl;
        } else {
            // 页面缺失
            FaultNumber++;

            // 寻找空闲帧或最少使用的页面
            victim = 0;
            for (i = 0; i < FrameNumber; i++) {
                if (PageFrames[i] == -1) {
                    victim = i;
                    break;
                }
                if (PageFrequency[i] < PageFrequency[victim]) {
                    victim = i;
                }
            }

            if (PageFrames[victim] >= 0) {
                EliminatePage[l++] = PageFrames[victim];
            }

            PageFrames[victim] = next;
            PageFrequency[victim] = 1;

            for (i = 0; i < FrameNumber; i++)
                if (PageFrames[i] >= 0)
                    cout << PageFrames[i] << " ";

            if (l > 0 && EliminatePage[l - 1] >= 0)
                cout << "->" << EliminatePage[l - 1] << endl;
            else
                cout << endl;
        }
    }
    Report();
}

void Replace::Mfu(void) {
    int i, k, l, next, victim;
    InitSpace("MFU");

    for (k = 0, l = 0; k < PageNumber; k++) {
        next = ReferencePage[k];

        // 检查页面是否已在内存中
        for (i = 0; i < FrameNumber; i++) {
            if (next == PageFrames[i]) {
                PageFrequency[i]++;
                break;
            }
        }

        if (i < FrameNumber) {
            // 页面命中
            for (i = 0; i < FrameNumber; i++)
                if (PageFrames[i] >= 0)
                    cout << PageFrames[i] << " ";
            cout << endl;
        } else {
            // 页面缺失
            FaultNumber++;

            // 寻找空闲帧或最常使用的页面
            victim = 0;
            for (i = 0; i < FrameNumber; i++) {
                if (PageFrames[i] == -1) {
                    victim = i;
                    break;
                }
                if (PageFrequency[i] > PageFrequency[victim]) {
                    victim = i;
                }
            }

            if (PageFrames[victim] >= 0) {
                EliminatePage[l++] = PageFrames[victim];
            }

            PageFrames[victim] = next;
            PageFrequency[victim] = 1;

            for (i = 0; i < FrameNumber; i++)
                if (PageFrames[i] >= 0)
                    cout << PageFrames[i] << " ";

            if (l > 0 && EliminatePage[l - 1] >= 0)
                cout << "->" << EliminatePage[l - 1] << endl;
            else
                cout << endl;
        }
    }
    Report();
}

// 比较所有算法性能
void Replace::CompareAlgorithms(void) {
    cout << string(45, '-') << endl;

    vector<string> algorithms = {"FIFO", "LRU", "CLOCK", "Enhanced CLOCK", "LFU"};
    vector<int> faults;

    // 保存原始数据
    int* originalRef = new int[PageNumber];
    int* originalElim = new int[PageNumber];
    for (int i = 0; i < PageNumber; i++) {
        originalRef[i] = ReferencePage[i];
        originalElim[i] = EliminatePage[i];
    }

    // 测试每个算法
    for (int alg = 0; alg < 6; alg++) {
        // 恢复原始数据
        for (int i = 0; i < PageNumber; i++) {
            ReferencePage[i] = originalRef[i];
            EliminatePage[i] = originalElim[i];
        }

        switch (alg) {
            case 0:
                Fifo();
                break;
            case 1:
                Lru();
                break;
            case 2:
                Clock();
                break;
            case 3:
                Eclock();
                break;
            case 4:
                Lfu();
                break;
            case 5:
                Mfu();
                break;
        }

        faults.push_back(FaultNumber);
    }

    delete[] originalRef;
    delete[] originalElim;
}

// 主函数
int main(int argc, char* argv[]) {
    Replace* vmpr = new Replace();

    vmpr->CompareAlgorithms();

    delete vmpr;
    return 0;
}