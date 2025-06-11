#include "dask.h"
DiskArm::DiskArm() {
    std::cout << "Please input current Cylinder:";
    std::cin >> CurrentCylinder;
    // 磁头⽅向
    //  0 ：向⼩磁道号⽅向移动
    //  1 ：向⼤磁道号⽅向移动
    std::cout << "Please input current direction (0/1):";
    std::cin >> SeekDirection;
    // 输入磁盘请求数，请求道号
    std::cout << "Please input request numbers:";
    std::cin >> RequestNumber;

    int useRandom;
    std::cout << "Use random generation? (1-Yes, 0-No): ";
    std::cin >> useRandom;

    std::cout << "Please input request cylinder string:";
    Request = new int[sizeof(int) * RequestNumber];
    Cylinder = new int[sizeof(int) * RequestNumber];
    if (useRandom == 1) {
        // 随机生成请求序列
        srand(static_cast<unsigned>(time(0)));
        for (int i = 0; i < RequestNumber; i++) {
            Request[i] = rand() % 200;  // 假设磁道号范围为0-199
        }
        std::cout << "Generated Request cylinder string: ";
        for (int j = 0; j < RequestNumber; j++) {
            std::cout << Request[j] << " ";
        }
        std::cout << std::endl;
    } else {
        // 手动输入请求序列
        std::cout << "Please input Request cylinder string :";
        for (int i = 0; i < RequestNumber; i++) {
            std::cin >> Request[i];
        }
    }
}
DiskArm::~DiskArm() {}
// 初始化道号，寻道记录
void DiskArm::InitSpace(char* MethodName) {
    std::cout << std::endl << MethodName << std::endl;
    SeekNumber = 0;
    SeekChange = 0;
    for (int i = 0; i < RequestNumber; i++)
        Cylinder[i] = Request[i];
}
// 统计报告算法执⾏情况
void DiskArm::Report() {
    std::cout << std::endl;
    std::cout << "Seek Number:" << SeekNumber << std::endl;
    std::cout << "Change Direction:" << SeekChange << std::endl << std::endl;
}
void DiskArm::FCFS() {
    int Current = CurrentCylinder;
    int Direction = SeekDirection;
    InitSpace("FCFS");
    std::cout << Current;
    for (int i = 0; i < RequestNumber; i++) {
        bool needChangeDirection = ((Cylinder[i] >= Current) && !Direction) || ((Cylinder[i] < Current) && Direction);
        if (needChangeDirection) {
            Direction = !Direction;
            SeekChange++;  // 调头数加1
            // 报告当前响应的道号
            std::cout << std::endl << Current << "->" << Cylinder[i];
        } else {
            // 不需要调头，报告当前响应的道号
            std::cout << "->" << Cylinder[i];
        }
        // 累计寻道数，响应过的道号变为当前道号
        SeekNumber += abs(Current - Cylinder[i]);
        Current = Cylinder[i];
    }
    // 报告磁盘调度情况
    Report();
}
void DiskArm::SSTF(void) {
    int Shortest;
    int Distance = 999999;
    int Direction = SeekDirection;
    int Current = CurrentCylinder;
    InitSpace("SSTF");
    std::cout << Current;
    for (int i = 0; i < RequestNumber; i++) {
        for (int j = 0; j < RequestNumber; j++) {
            if (Cylinder[j] == -1)
                continue;

            if (Distance > abs(Current - Cylinder[j])) {
                Distance = abs(Current - Cylinder[j]);
                Shortest = j;
            }
        }
        if (((Cylinder[Shortest] >= Current) && !Direction) || ((Cylinder[Shortest] < CurrentCylinder) && Direction)) {
            SeekChange++;
            Direction = !Direction;
            std::cout << std::endl << Current << " -> " << Cylinder[Shortest];
        } else  // 不需调头，报告当前响应的道号
            std::cout << " -> " << Cylinder[Shortest];
        SeekNumber += abs(Current - Cylinder[Shortest]);
        Current = Cylinder[Shortest];
        Distance = 999999;
        Cylinder[Shortest] = -1;
    }
    Report();
}

void sort(int* a, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (a[j] > a[j + 1]) {
                int temp = a[j + 1];
                a[j + 1] = a[j];
                a[j] = temp;
            }
        }
    }
}

void DiskArm::SCAN(void) {
    int Current = CurrentCylinder;
    int Direction = SeekDirection;
    InitSpace("SCAN");
    int point = 0;
    for (int i = 0; i < RequestNumber; i++) {
        if (Cylinder[i] <= Current)
            point++;
    }  // 标记
    sort(Cylinder, RequestNumber);  // 升序排列
    std::cout << Current << " ";

    if (Direction == 0) {
        for (int i = point - 1; i >= 0; i--) {
            std::cout << "-> " << Cylinder[i] << " ";
        }
        std::cout << "->" << 0;
        SeekChange++;
        SeekNumber += abs(Current - 0);
        std::cout << std::endl;
        for (int i = point; i < RequestNumber; i++) {
            std::cout << "-> " << Cylinder[i] << " ";
        }
        SeekNumber += abs(Cylinder[RequestNumber - 1] - 0);
    }

    else if (Direction == 1) {
        for (int i = point; i < RequestNumber; i++) {
            std::cout << "-> " << Cylinder[i] << " ";
        }
        std::cout << "-> " << 200;
        SeekNumber += abs(200 - Current);
        SeekChange++;
        std::cout << std::endl;
        for (int i = point - 1; i >= 0; i--) {
            std::cout << "-> " << Cylinder[i] << " ";
        }
        SeekNumber += abs(200 - Cylinder[0]);
    }
    Report();
}

void DiskArm::CSCAN(void) {
    int Current = CurrentCylinder;
    int Direction = SeekDirection;
    InitSpace("CSCAN");
    int point = 0;
    for (int i = 0; i < RequestNumber; i++) {
        if (Cylinder[i] <= Current)
            point++;
    }
    sort(Cylinder, RequestNumber);  // 升序排列
    std::cout << Current << " ";

    if (Direction == 0) {
        for (int i = point - 1; i >= 0; i--) {
            std::cout << "-> " << Cylinder[i] << " ";
        }
        std::cout << "-> " << 0;  // 向左到0
        std::cout << std::endl;
        std::cout << "-> " << 200;
        SeekChange++;
        SeekNumber += abs(Current - 0);  // 向左移动到0的距离
        SeekNumber += 200;               // 从0到200
        std::cout << std::endl;
        SeekChange++;
        for (int i = RequestNumber - 1; i >= point; i--) {
            std::cout << "-> " << Cylinder[i] << " ";
        }
        SeekNumber += abs(200 - Cylinder[point + 1]);  // 200到最后一个访问点的距离
    }

    else if (Direction == 1) {
        for (int i = point; i < RequestNumber; i++) {
            std::cout << "-> " << Cylinder[i] << " ";
        }
        std::cout << "-> " << 200;
        std::cout << std::endl;
        std::cout << "-> " << 0 << ' ';
        SeekNumber += abs(200 - Current) + 200;
        SeekChange++;
        // std::cout << std::endl;
        SeekChange++;
        for (int i = 0; i <= point - 1; i++) {
            std::cout << "-> " << Cylinder[i] << " ";
        }
        SeekNumber += abs(Cylinder[point - 1] - 0);
    }
    Report();
}

int main(int argc, char* argv[]) {
    DiskArm* dask = new DiskArm();
    dask->FCFS();
    dask->SSTF();
    dask->SCAN();
    dask->CSCAN();
    dask->LOOK();
}
