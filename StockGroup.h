#pragma once
#include "stdafx.h"
#include <string> 
#include <iostream>
#include <vector>
#include <map>
#include "Stock.h"
#include <process.h>
#include <Windows.h>
#include <time.h>
//#include <algorithm>


class StockGroup
{

private:
	struct node
	{
		StockGroup* object;
		int num;
	};

	HANDLE threads[3];
	void(*pfun) (vector<stock>& stVec, map<string, stock>& stMap);
	map<string, stock> mpHigh;
	map<string, stock> mpMid;
	map<string, stock> mpLow;
	vector<stock> high;
	vector<stock> mid;
	vector<stock> low;
	vector<map<string, stock>> groups;
	int flag[3];
	//double timebegin;
	bool sig;

public:
	static unsigned int __stdcall procWork(void  * lpParam)
	{
		node * pthis = (node *)lpParam;
		int mark = 0;

		switch (pthis->num)
		{
		case 0:
			pthis->object->pfun(pthis->object->high, pthis->object->mpHigh);
			mark = 1;
			break;
		case 1:
			pthis->object->pfun(pthis->object->mid, pthis->object->mpMid);
			mark = 1;
			break;
		case 2:
			pthis->object->pfun(pthis->object->low, pthis->object->mpLow);
			mark = 1;
			break;
		default:
			break;
		}
		pthis->object->flag[pthis->num] = 1;
		return pthis->num;
	}


	StockGroup(void(*pfun) (vector<stock>& stVec, map<string, stock>& stMap), vector<stock> &high, vector<stock> &low, vector<stock> &mid)
	{
		sig = true;
		flag[0] = 0; flag[1] = 0; flag[2] = 0;
		//timebegin = time(0);

		this->high = high;
		this->mid = mid;
		this->low = low;
		this->pfun = pfun;
		struct node *temp[3];
		for (int i = 0; i <= 2; i++)
		{
			temp[i] = new node;
			temp[i]->num = i;
			temp[i]->object = this;
		}
		for (int i = 0; i <= 2; i++)
		{
			threads[i] = (HANDLE)_beginthreadex(NULL, 0, procWork, temp[i], 0, NULL);
		}
		
	}


	vector<map<string, stock>> GetResult()
	{
		while (1) //loop forever until break
		{
			int k = flag[0] + flag[1] + flag[2];
			if (k == 3)
			{
				for (int i = 0; i <= 2; i++)
				{
					CloseHandle(threads[i]);
					threads[i] = NULL;
				}
				break;
			}
		}
		vector < map<string, stock>> groups;


		groups.push_back(mpLow);
		groups.push_back(mpMid);
		groups.push_back(mpHigh);

		return groups;
	}

	vector<stock> GetHigh() { return high; }
	vector<stock> GetMid() { return mid; }
	vector<stock> GetLow() { return low; }
	//double TimeCost(){ return (time(0) -timebegin) / 60.000;}

};
