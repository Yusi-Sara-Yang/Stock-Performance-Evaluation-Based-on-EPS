#pragma once
#ifndef Stock_h
#define Stock_h


#include<iostream>
#include<string>
#include<vector>
#include <time.h>

using namespace std;

class stock {
public:
	//Parameter constructer
	stock(const string& tick, const string& zero, const double& act, const double& est, const double& perf, const string& start, const string& end);
	stock() {}
	string getTicker() const;
	string getStartDate() const;
	string getEndDate() const;
	double getPerformance() const;
	void pushBack(double num);
	void Info_Look();
	const vector<double> GetPrice();
	int getSeriesSize() const;
	void printStock() const; //Cout Stock Info


private:
	string ticker, dateZero, dateStart, dateEnd;
	double actual, estimate, performance;
	vector<double> Price_Series;


};

#endif // Stock_h

