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



typedef struct _record {
	char *ptr;
	_record *next;
} record;

record *sort(record* bin, int digit, bool isCutoff);
record **firstRecord, **lastRecord;
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

	firstRecord = (record **)calloc(100, sizeof(record *));
	lastRecord = (record **)calloc(100, sizeof(record *));
	count = (int *)calloc(100, sizeof(int));
		
	//最上位の桁で基数ソート
	int value;
	for (int i = 0; i < fileSize; i += LINE_LENGTH)
	{
		value = (addr[i] - CHARCODE_ZERO)*10 + (addr[i+1] - CHARCODE_ZERO);

		record* r = (record *)malloc(sizeof(record));
		r->ptr = addr + i;
		
		if (firstRecord[value] == NULL) {
			firstRecord[value] = r;
		}
		else
		{
			lastRecord[value]->next = r;
		}

		lastRecord[value] = r;
		count[value]++;
	}

	//リストの連結
	record* bin = firstRecord[0];
	recordCount = count[0];
	firstRecord[0] = NULL;
	lastRecord[0]->next = NULL;

	for (int i = 1; i < 100; i++)
	{
		if (firstRecord[i] == NULL) continue;

		if (recordCount < requireCount) {
			recordCount += count[i];
			lastRecord[i-1]->next = firstRecord[i];
		}
		
		firstRecord[i] = NULL;
		lastRecord[i]->next = NULL;
	}
	
	for (int digit = 2; digit < DIGIT_NUMBER; digit+=2)
	{
		int oldRecordCount = recordCount;
		bin = sort(bin, digit, true);

		if (oldRecordCount == recordCount)
		{
			break;
		}
	}

	for (int digit = DIGIT_NUMBER-2; digit >= 0; digit-=2)
	{
		bin = sort(bin, digit, false);
	}
	
	record* cursor = bin;
	long t_end = clock()*1000/CLOCKS_PER_SEC;
	
//	for (int i = 0; i < requireCount; i++) {
//
//		char *ptr = cursor->ptr;
//
//		while (ptr[0] != CHARCODE_NEWLINE) {
//			printf("%c", ptr[0]);
//			ptr++;
//		}
//		
//		printf("\n");
//		cursor = cursor->next;
//		if (cursor == NULL) break;
//	}
	
	long t_end_with_print = clock()*1000/CLOCKS_PER_SEC;

	printf("\nfinish!\n");
	printf("time             : %ld(ms)\n", t_end);
	printf("time(with print) : %ld(ms)\n", t_end_with_print);
	free(firstRecord);
	free(lastRecord);
	free(count);
	
	return 0;
}

record *sort(record* bin, int digit, bool flagCutoff)
{
	record* cursor = bin;
	int value;

	while (cursor != NULL)
	{
		value = (cursor->ptr[digit] - CHARCODE_ZERO)*10 + (cursor->ptr[digit+1] - CHARCODE_ZERO);
		
		if (firstRecord[value] == NULL) {
			firstRecord[value] = cursor;
		}
		else
		{
			lastRecord[value]->next = cursor;
		}

		lastRecord[value] = cursor;
		count[value]++;

		cursor = cursor->next;
	}
	
	//リストの連結
	record* newbin = firstRecord[0];
	recordCount = count[0];
	count[0] = 0;
	firstRecord[0] = NULL;
	lastRecord[0]->next = NULL;

	if (flagCutoff)
	{
		for (int i = 1; i < 100; i++)
		{
			if (firstRecord[i] == NULL) continue;

			if (recordCount < requireCount) {
				lastRecord[i-1]->next = firstRecord[i];
				recordCount += count[i];
			}

			count[i] = 0;
			firstRecord[i] = NULL;
			lastRecord[i]->next = NULL;
		}
	}
	else
	{
		for (int i = 1; i < 100; i++)
		{
			if (firstRecord[i] == NULL) continue;
			
			lastRecord[i-1]->next = firstRecord[i];
			recordCount += count[i];

			count[i] = 0;
			firstRecord[i] = NULL;
			lastRecord[i]->next = NULL;
		}
	}
	
	return newbin;
}