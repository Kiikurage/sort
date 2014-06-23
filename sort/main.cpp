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
#define SORT_SWITCH_LIMIT 10

#define __DEBUG__
#define __PRINT_ALL__

#ifdef __DEBUG__
long logtime = 0;
int logcount = 0;
void _LOG()
{
	logtime = clock();
}
void _LOG(const char *mes)
{
	long now = clock();
	printf("%9.3lfms >> %s\n", 1.0*(now - logtime)*1000/CLOCKS_PER_SEC, mes);
	logtime = now;
}
void _LOG_COUNT()
{
	logcount++;
}
void _LOG_COUNT(const char *mes)
{
	printf("%9.3d >> %s\n", logcount, mes);
	logcount = 0;
}
#define LOG(mes) _LOG(mes)
#define LOG_COUNT(mes) _LOG_COUNT(mes)
#else
#define LOG(mes)
#define LOG_COUNT(mes)
#endif

typedef struct _record {
	char *value;
	_record *next;
} record;

record *res_list;
record *res_last;
int res_count;

inline void sort(record *list, const int d, const int requireCount);
inline void print(record *list, int requireCount);
inline void margeSort(record **arr1, long *arr2, int length);

int main(int argc, const char *argv[])
{
	LOG();
	int fileSize, pageSize, mmapSize;
	char *addr;
	
	fileSize = (int)lseek(0, 0, SEEK_END);
	pageSize = getpagesize();
	mmapSize = (fileSize + (pageSize - 1)) / pageSize * pageSize;
	addr = (char *)mmap(0, mmapSize, PROT_READ, MAP_PRIVATE, 0, 0);
	
	int requireCount = fileSize / 2100 * atoi(argv[1]);
	
	record **list = (record **)malloc(sizeof(record *) * ALLOCATE_SIZE);
	record **cursor = (record **)malloc(sizeof(record *) * ALLOCATE_SIZE);
	for (int i = 0; i != ALLOCATE_SIZE; i++)
	{
		list[i] = (record *)malloc(sizeof(record));
		cursor[i] = list[i];
	}
	record *item;
	int index;
	
	for (int i = 0; i < fileSize; i += 21)
	{
		item = (record *)malloc(sizeof(record));
		item->value = addr + i;
		
		index = *(item->value) - CHARCODE_ZERO;
		
		cursor[index]->next = item;
		cursor[index] = item;
	}
	
	//連結
	record *listAll = (record *)malloc(sizeof(record));
	record *cursor2 = listAll;
	int req = requireCount;
	for (int i = 0; i != ALLOCATE_SIZE; i++)
	{
		sort(list[i], UNIT_DIGIT, req);
		
		cursor2->next = res_list->next;
		cursor2 = res_last;
		req -= res_count;
		
		if (req <= 0) break;
	}
	cursor2->next = NULL;
	
//	print(listAll, requireCount);
	
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
	
	for (int i = 0; i != ALLOCATE_SIZE; i++)
	{
		sublist[i] = (record *)malloc(sizeof(record));
		cursor[i] = sublist[i];
	}
	
	record *item = list;
	int index;

	while ((item = item->next))
	{
		index = *(item->value+digit) - CHARCODE_ZERO;

		cursor[index]->next = item;
		cursor[index] = item;
		count[index]++;
	}
	
	record *sublistAll = (record *)malloc(sizeof(record));
	record *cursor2 = sublistAll;
	int req = requireCount;
	
	for (int i = 0; i != ALLOCATE_SIZE; i++)
	{
		if (!count[i]) continue;
		
		if (count[i] == 1)
		{
			cursor2->next = sublist[i]->next;
			cursor2 = cursor[i];
			req -= count[i];
		}
//		else if (count[i] < SORT_SWITCH_LIMIT)
//		{
//			//マージソート
//			record **arr = (record**)malloc(sizeof(record*) * count[i]);
//			long *arrVal = (long*)malloc(sizeof(long) * count[i]);
//			
//			item = sublist[i];
//			int j = 0;
//			long val;
//			while ((item = item->next))
//			{
//				val = 0;
//				for (int k = digit+UNIT_DIGIT; k < 20; k++)
//				{
//					val *= 10;
//					val += *(item->value+k)-CHARCODE_ZERO;
//				}
//
//				arr[j] = item;
//				arrVal[j] = val;
//				j++;
//			}
//			
//			margeSort(arr, arrVal, count[i]);
//			
//		}
		else if (count[i] < SORT_SWITCH_LIMIT)
		{
			//挿入ソート
			record *item2;
			record *prev_item = sublist[i];
			record *prev_item2;
			long max = 0;
			long value1;
			long value2;
			
			item = prev_item->next;
			for (int j = digit+UNIT_DIGIT; j < 20; j++)
			{
				max *= 10;
				max += *(item->value+j)-CHARCODE_ZERO;
			}
			prev_item = prev_item->next;

			cursor[i]->next = NULL;
			while ((item = prev_item->next))
			{
				
				value1 = 0;
				for (int j = digit+UNIT_DIGIT; j < 20; j++)
				{
					value1 *= 10;
					value1 += *(item->value+j)-CHARCODE_ZERO;
				}
				
				if (value1 < max)
				{
					item2 = sublist[i];
					do
					{
						prev_item2 = item2;
						item2 = prev_item2->next;
						value2 = 0;
						for (int j = digit+UNIT_DIGIT; j < 20; j++)
						{
							value2 *= 10;
							value2 += *(item2->value+j)-CHARCODE_ZERO;
						}
					} while (value2 < value1);

					if (item2 != item)
					{
						prev_item->next = item->next;
						item->next = item2;
						prev_item2->next = item;
					}
					else
					{
						prev_item = item;
					}
				}
				else
				{
					max = value1;
					prev_item = item;
				}
				
			}

			cursor2->next = sublist[i]->next;
			while (cursor2->next) cursor2 = cursor2->next;
			req -= count[i];
		}
		else
		{
			cursor[i]->next = NULL;
			sort(sublist[i], digit+UNIT_DIGIT, req);
			
			cursor2->next = res_list->next;
			cursor2 = res_last;
			req -= res_count;
		}
		if (req < 1) break;
	}
	cursor2->next = NULL;

	res_list = sublistAll;
	res_last = cursor2;
	res_count = requireCount-req;
	
	return;
}


inline void print(record *list, int requireCount)
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