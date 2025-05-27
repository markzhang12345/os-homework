# Makefile for Smoker Problem
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
OBJECTS = common.o

all: main smoker supplier

# 主控制程序
main: main.o $(OBJECTS)
	$(CC) $(CFLAGS) -o main main.o $(OBJECTS)

# 抽烟者进程
smoker: smoker.o $(OBJECTS)
	$(CC) $(CFLAGS) -o smoker smoker.o $(OBJECTS)

# 供应者进程
supplier: supplier.o $(OBJECTS)
	$(CC) $(CFLAGS) -o supplier supplier.o $(OBJECTS)

# 编译目标文件
main.o: main.c smoker.h
	$(CC) $(CFLAGS) -c main.c

smoker.o: smoker.c smoker.h
	$(CC) $(CFLAGS) -c smoker.c

supplier.o: supplier.c smoker.h
	$(CC) $(CFLAGS) -c supplier.c

common.o: common.c smoker.h
	$(CC) $(CFLAGS) -c common.c

# 运行程序
run: all
	@echo "启动抽烟者问题演示..."
	./main

# 清理编译文件
clean:
	rm -f *.o main smoker supplier

# 清理 IPC 资源 (如果程序异常退出)
clean-ipc:
	@echo "清理残余的 IPC 资源..."
	-ipcrm -S 2001 2>/dev/null || true
	-ipcrm -S 2002 2>/dev/null || true  
	-ipcrm -S 2003 2>/dev/null || true
	-ipcrm -S 2004 2>/dev/null || true
	-ipcrm -S 2005 2>/dev/null || true
	-ipcrm -M 1001 2>/dev/null || true
	-ipcrm -M 1002 2>/dev/null || true
	@echo "IPC 资源清理完成"

# 显示当前 IPC 状态
show-ipc:
	@echo "=== 当前信号量状态 ==="
	ipcs -s
	@echo "=== 当前共享内存状态 ==="
	ipcs -m

.PHONY: all run clean clean-ipc show-ipc