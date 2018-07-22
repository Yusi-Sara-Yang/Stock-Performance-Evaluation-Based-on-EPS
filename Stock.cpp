#include "stdafx.h"
#include "Stock.h"

using namespace std;

stock::stock(const string& tick, const string& zero, const double& act, const double& est, const double& perf, const string& start, const string& end)
	: ticker(tick), dateZero(zero), dateStart(start), dateEnd(end), actual(act), estimate(est), performance(perf) {}

string stock::getTicker() const {
	return ticker;
}
string stock::getStartDate() const {
	return dateStart;
}
string stock::getEndDate() const {
	return dateEnd;
}
double stock::getPerformance() const {
	return performance;
}
void stock::pushBack(double num) {
	Price_Series.push_back(num);
}
void stock::Info_Look()
{
	for (int i = 0; i <= Price_Series.size() - 1; i++)
	{
		//cout << Price_Series[i] << endl;
	}
}
const vector<double> stock::GetPrice()
{
	return Price_Series;
}
int stock::getSeriesSize() const {
	return Price_Series.size();
}
void stock::printStock() const {
	cout << "Ticker: " << ticker << endl;
	cout << "Start Date: " << dateStart << endl;
	cout << "End Date: " << dateEnd << endl;
	cout << "Actual ESP: " << actual << endl;
	cout << "Estimate ESP: " << estimate << endl;
	cout << "Performance: " << performance << endl;
	for (int i = 0; i <= Price_Series.size() - 1; i++)
	{
		cout << Price_Series[i] << endl;
	}
}