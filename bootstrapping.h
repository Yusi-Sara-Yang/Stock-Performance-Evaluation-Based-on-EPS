#pragma once

//#include "stdafx.h"
#include <vector>
#include <iostream>
#include <map>
#include "Stock.h"
#include <algorithm>
using namespace std;

vector<int> Rand_Generator(vector<stock> &group)
{
	int temp = 0;
	vector<int> arry;
	vector<int>::iterator itr;
	for (int i = 0; i <= 59; i++)
	{
		temp = rand() % (group.size());
		arry.push_back(temp);
		while (1)
		{
			itr = find(arry.begin(), arry.end(), temp);
			if (itr == arry.end()) 
			{
				arry.push_back(temp);
				break;
			}
			else
			{
				temp = rand() % (group.size());
			}
		}
	}
	return arry;
}


vector<map<string, stock>> GetSampleGroup(vector<stock> &low, vector<stock> &mid, vector<stock> &high)
{
	// set group1 for low(miss), group2 for mid(meet), group3 for high(beat)
	vector<map<string, stock>> result;
	map<string, stock> low_group;
	map<string, stock> mid_group;
	map<string, stock> high_group;

	vector<int> Randarry1 = Rand_Generator(low);
	vector<int> Randarry2 = Rand_Generator(mid);
	vector<int> Randarry3 = Rand_Generator(high);

	for (int i = 0; i <= 59; i++)
	{
		low_group[low[Randarry1[i]].getTicker()] = low[Randarry1[i]];
		mid_group[mid[Randarry2[i]].getTicker()] = mid[Randarry2[i]];
		high_group[high[Randarry3[i]].getTicker()] = high[Randarry3[i]];
		
	}

	result.push_back(low_group);
	result.push_back(mid_group);
	result.push_back(high_group);
	return result;

}

void Divide(map<string, vector<vector<double>>> &result, double a)
{

	map<string, vector<vector<double>>>::iterator itr = result.begin();
	while (itr != result.end())
	{
		for (int i = 0; i <= (*itr).second[1].size() - 1; i++)
		{
			(*itr).second[1][i] /= a;
		}
		itr++;
	}
}