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

#define DIGIT_NUMBER         20
#define NEWLINECODE_SIZE     1
#define LINE_LENGTH          (DIGIT_NUMBER+NEWLINECODE_SIZE)
#define CHARCODE_ZERO        48
#define CHARCODE_NEWLINE     10
#define UNIT_DIGIT_SIZE      4
#define MEMORY_ALLOCATE_SIZE 10000

typedef struct _record {
	_record *next;
	char *ptr;
} record;

record *sort(int digit);
record *connect();
void print();

record **firstRecord, **lastRecord;
record *bin;
int *count, recordCount, requireCount;

int main(int argc, const char *argv[])
{
	int fileSize, pageSize, mmapSize;
	char *addr;

	fileSize = (int)lseek(0, 0, SEEK_END);
	pageSize = getpagesize();
	mmapSize = (fileSize + (pageSize - 1)) / pageSize * pageSize;
	addr = (char *)mmap(0, mmapSize, PROT_READ, MAP_PRIVATE, 0, 0);
	
	requireCount = fileSize / 100 * atoi(argv[1]) / LINE_LENGTH;

	firstRecord = (record **)malloc(MEMORY_ALLOCATE_SIZE * sizeof(record *));
	lastRecord = (record **)malloc(MEMORY_ALLOCATE_SIZE * sizeof(record *));
	count = (int *)calloc(MEMORY_ALLOCATE_SIZE, sizeof(int));
	
	
	for (int i = 0; i < MEMORY_ALLOCATE_SIZE; i++)
	{
		firstRecord[i] = (record *)calloc(1, sizeof(record));
		lastRecord[i] = firstRecord[i];
	}
	
	//最上位の桁で基数ソート
	int value;
	for (int i = 0; i < fileSize; i += LINE_LENGTH)
	{
		value = 0;
		for (int j = 0; j < UNIT_DIGIT_SIZE; j++) {
			value *= 10;
			value += (addr[i+j] - CHARCODE_ZERO);
		}

		record* r = (record *)malloc(sizeof(record));
		r->ptr = addr + i;
		
		lastRecord[value]->next = r;
		lastRecord[value] = r;
		count[value]++;
	}
	
	bin = connect();
	
	//下から順に基数ソート
	for (int digit = DIGIT_NUMBER - UNIT_DIGIT_SIZE; digit >= 0; digit -= UNIT_DIGIT_SIZE)
		bin = sort(digit);
	
	long t_end = clock()*1000/CLOCKS_PER_SEC;
	
//	print();
	
	printf("\nfinish!\n");
	printf("time             : %ld(ms)\n", t_end);

	free(firstRecord);
	free(lastRecord);
	free(count);
	
	return 0;
}

record *sort(int digit)
{
	record* cursor = bin;
	int value;
	
	while (cursor != NULL)
	{
		value = 0;
		for (int j = 0; j < UNIT_DIGIT_SIZE; j++) {
			value *= 10;
			value += (cursor->ptr[digit+j] - CHARCODE_ZERO);
		}
		
		lastRecord[value]->next = cursor;
		lastRecord[value] = cursor;
		cursor = cursor->next;
	}
	
	return connect();
}

record *connect()
{
	record* newbin = (record *)calloc(1, sizeof(record));
	record* cursor = newbin;
	int recordCount = 0;
	
	for (int i = 0; i < MEMORY_ALLOCATE_SIZE; i++)
	{
		if (firstRecord[i]->next != NULL && recordCount < requireCount) {
			cursor->next = firstRecord[i]->next;
			cursor = lastRecord[i];
			recordCount += count[i];
		}
		
		lastRecord[i] = firstRecord[i];
		firstRecord[i]->next = NULL;
		count[i] = 0;
	}
	cursor->next = NULL;
	
	return newbin->next;
}

void print()
{
	record *cursor = bin;
	while (cursor != NULL) {
		
		char *ptr = cursor->ptr;
		
		while (ptr[0] != CHARCODE_NEWLINE) {
			printf("%c", ptr[0]);
			ptr++;
		}
		printf("\n");
		
		cursor = cursor->next;
	}
}
