//
//  main.cpp
//  sort
//
//  Created by KikuraYuichirou on 2014/06/02.
//  Copyright (c) 2014年 KikuraYuichirou. All rights reserved.
//

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>

#define LINE_LENGTH 21
#define CHARCODE_ZERO 48
#define ALLOCATE_SIZE 10
#define UNIT_DIGIT 1

#define __DEBUG__
//#define __PRINT_ALL__

#ifdef __DEBUG__
long logtime = 0;
void _LOG()
{
	logtime = clock();
}
void _LOG(const char *mes)
{
	long now = clock();
	printf("%8ldms >> %s\n", (now - logtime)*1000/CLOCKS_PER_SEC, mes);
	logtime = now;
}
#define LOG(mes) _LOG(mes)
#else
#define LOG(mes)
#endif

typedef struct _record {
	char *value;
	_record *next;
} record;

record *res_list;
record *res_last;
int res_count;

inline void sort(record *list, const int d, const int requireCount);

void print(record *list, int requireCount);

int main(int argc, const char *argv[])
{
	int fileSize, pageSize, mmapSize;
	char *addr;
	
	fileSize = (int)lseek(0, 0, SEEK_END);
	pageSize = getpagesize();
	mmapSize = (fileSize + (pageSize - 1)) / pageSize * pageSize;
	addr = (char *)mmap(0, mmapSize, PROT_READ, MAP_PRIVATE, 0, 0);
	
	int requireCount = fileSize / 100 * atoi(argv[1]) / 21;
	
	record **list = (record **)malloc(sizeof(record *) * ALLOCATE_SIZE);
	record **cursor = (record **)malloc(sizeof(record *) * ALLOCATE_SIZE);
	for (int i = 0; i < ALLOCATE_SIZE; i++)
	{
		list[i] = (record *)malloc(sizeof(record));
		list[i]->next = NULL;
		cursor[i] = list[i];
	}
	record *item;
	int index;
	
	LOG("メモリ確保完了");
	
	for (int i = 0; i < fileSize; i += 21)
	{
		item = (record *)malloc(sizeof(record));
		item->value = addr + i;
		
		index = 0;
		for (int j = 0; j < UNIT_DIGIT; j++)
		{
			index *= 10;
			index += *(item->value+j)-CHARCODE_ZERO;
		}
		
		cursor[index]->next = item;
		cursor[index] = item;
	}
	LOG("読み込み完了");
	
	//連結
	record *listAll = (record *)malloc(sizeof(record));
	record *cursor2 = listAll;
	int req = requireCount;
	for (int i = 0; i < ALLOCATE_SIZE; i++)
	{
		if (!list[i]->next) continue;
		
		sort(list[i], UNIT_DIGIT, req);
		
		cursor2->next = res_list->next;
		cursor2 = res_last;
		req -= res_count;
		
		if (req <= 0) break;
	}
	cursor2->next = NULL;
	
	LOG("終了");
	return 0;
}

/*
 * list: リストへのポインタ
 * digit: 評価の対象とする桁数
 * count: 必要な要素数
 *
 * 戻り値: SortResult
 */
inline void sort(record *list, const int digit, const int requireCount) {
	
	record **sublist = (record **)malloc(sizeof(record *) * ALLOCATE_SIZE);
	record **cursor = (record **)malloc(sizeof(record *) * ALLOCATE_SIZE);
	int *count = (int *)calloc(ALLOCATE_SIZE, sizeof(int));
	
	for (int i = 0; i < ALLOCATE_SIZE; i++)
	{
		sublist[i] = (record *)malloc(sizeof(record));
		sublist[i]->next = NULL;
		cursor[i] = sublist[i];
	}
	
	record *item = list;
	int index;
	
	while ((item = item->next))
	{
		if (item == item->next) throw "error";
		
		index = 0;
		for (int j = 0; j < UNIT_DIGIT; j++)
		{
			index *= 10;
			index += *(item->value+digit+j)-CHARCODE_ZERO;
		}
		cursor[index]->next = item;
		cursor[index] = item;
		count[index]++;
	}
	
	record *sublistAll = (record *)malloc(sizeof(record));
	record *cursor2 = sublistAll;
	int req = requireCount;
	
	for (int i = 0; i < ALLOCATE_SIZE; i++)
	{
		if (count[i] == 0) continue;
		
		if (digit+UNIT_DIGIT < 20 && count[i] > 1)
		{
			cursor[i]->next = NULL;
			
			sort(sublist[i], digit+UNIT_DIGIT, req);
			
			cursor2->next = res_list->next;
			cursor2 = res_last;
			req -= res_count;
			if (req <= 0) break;
		}
		else
		{
			cursor2->next = sublist[i]->next;
			cursor2 = cursor[i];
			req -= count[i];
			if (req <= 0) break;
		}
	}
	cursor2->next = NULL;
	
	res_list = sublistAll;
	res_last = cursor2;
	res_count = requireCount-req;

	return;
}

void print(record *list, int requireCount)
{
	int i = 0;
	record *cursor = list;
	while ((cursor = cursor->next))
	{
#ifdef __PRINT_ALL__
		int s = 0;
		while (s < 20)
		{
			printf("%c", (*(cursor->value + s)));
			s++;
		}
		printf("\n");
#endif
		i++;
		if (i >= requireCount) break;
	}
	printf("%d個\n", i);
}