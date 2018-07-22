#pragma once
#ifndef Calculate_h
#define Calculate_h


#include "stdafx.h"
#include <string> 
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <map>
#include "Stock.h"
#include "getdata.h"

using namespace std;

/***************************operator overload for vector******************************/

template<class T>
vector<T> operator-(const vector<T> &x, const vector<T> &y)
{
	vector<T> result;
	T temp;
	for (int i = 0; i <= x.size() - 1; i++)
	{
		temp = x[i] - y[i];
		result.push_back(temp);
	}
	return result;
}

void operator+(map<string, vector<vector<double>>> & x, map<string, vector<vector<double>>> & y)
{
	map<string, vector<vector<double>>>::iterator ite1 = x.begin();
	map<string, vector<vector<double>>>::iterator ite2 = y.begin();
	while (ite1 != x.end())
	{
		for (int i = 0; i <= (*ite1).second[1].size() - 1; i++)
		{
			(*ite1).second[1][i] += (*ite2).second[1][i];
			(*ite1).second[0][i] += (*ite2).second[0][i];
		}
		ite1++;
		ite2++;
	}
}


template<class T>
vector<T> operator/(const vector<double> & x, const double & y)
{
	vector<T> result;
	T temp;
	for (int i = 0; i <= x.size() - 1; i++)
	{
		temp = x[i] / y;
		result.push_back(temp);
	}
	return results;
}


/****************************calculate functions*******************************/
vector<double> DailyReturn(vector<double> Stock_Series)
{
	vector<double> result;
	double dr = 0.0;
	for (int i = 1; i <= Stock_Series.size() - 1; i++)//Stock_Series.size() - 1
	{
		dr = (Stock_Series[i] - Stock_Series[i - 1]) / Stock_Series[i - 1];
		if (dr > 10)
			dr = 0;
		result.push_back(dr);
	}
	return result;
}


vector<double> AART(vector<vector<double>> diff_info)
{
	vector<double> result;
	for (int i = 0; i <= diff_info[1].size() - 1; i++) //days
	{
		double temp = 0.0;
		for (int j = 0; j <= diff_info.size() - 1; j++)//calcuate the daily
		{

			temp += diff_info[j][i];
		}
		temp /= diff_info.size();
		result.push_back(temp);
	}
	return result;
}

vector<double> CAAR(vector<double> AART_result)
{
	vector<double> result;
	double temp = 0.0;
	for (int i = 0; i <= AART_result.size() - 1; i++)
	{
		temp += AART_result[i];
		result.push_back(temp);
	}
	return result;
}


// store results in a matrix(3,3)
static int c = 0;
map<string, vector<vector<double>>> MatrixInfo(vector<map<string, stock>> Group_Info, map<string, size_t> S_map, vector<double> SP_ret)
{
	// S_map use stock date as key, calculate results as value; 
	// SP_ret is the vector of daily return of SPY
	map<string, vector<vector<double>>> result;  
	vector<vector<double>> diff_return;
	vector<double> temp1;
	c++;

	vector<double> temp3;
	int stardate = 0;
	double mean = 0;
	//cout << "Daily return for each stock: " << endl;
	for (int j = 0; j <= 2; j++) 
	{
		diff_return.clear();

		map<string, stock>::iterator ite = Group_Info[j].begin(); 
		while (ite != Group_Info[j].end())
		{
			// get daily return of this stock and startdate 
			temp1 = DailyReturn((*ite).second.GetPrice());
			stardate = S_map[getTimeinDate((*ite).second.getStartDate())];

			// match SP time
			vector<double> temp2(SP_ret.begin() + stardate, SP_ret.begin() + stardate + temp1.size());

			// compute abnormal returns
			temp3 = temp1 - temp2;
			diff_return.push_back(temp3);

			ite++;
		}

		temp1.clear();
		temp1 = AART(diff_return); 
		diff_return.clear();
		temp3.clear();
		temp3 = CAAR(temp1);
		diff_return.push_back(temp1); // AAR
		diff_return.push_back(temp3); // CAAR
		temp3.clear();

		switch (j)
		{
		case 0:
			result["low"] = diff_return;
			break;
		case 1:
			result["mid"] = diff_return;
			break;
		case 2:
			result["high"] = diff_return;
			break;
		default:
			break;
		}
		diff_return.clear();
	}
	return result;

}
#endif // Calculate_h
