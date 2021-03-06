// Calculation.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <string> 
#include <iostream>
#include <sstream>  
#include <vector>
#include <locale>
#include <iomanip>
#include "curl.h"
#include "Input.h"

#include <fstream>
#include <stack>
#include <map>
#include "Stock.h"
#include "StockGroup.h"
#include "bootstrapping.h"
#include "Calculate.h"
#include "getdata.h"


using namespace std;

int main()
{
	char step;
	char data, menu_again;  
	//Testing
	time_t timeBegin, timeEnd;
	timeBegin = time(NULL);

	vector<stock> high;
	vector<stock> mid;
	vector<stock> low;
	map<string, stock> mpHigh;
	map<string, stock> mpMid;
	map<string, stock> mpLow;
	vector<double> SPYPriceVec;
	vector<double> SPYDailyReturn;
	vector<map<string, stock>> po;
	map<string, size_t>::iterator itr;
	vector < map<string, stock>> groups;
	map<string, size_t> SPYVectorPosition;
	vector<map<string, stock>>::iterator itr2 = groups.begin();
	map<string, vector<vector<double>>> result; //Store the ARR and CRR in this map


	system("cls");
	cout << "Welcome to Group 3 Project!" << endl << endl;
beginning:
	cout << "--------------------------------- MENU --------------------------------" << endl;
	cout << "Our project has the following steps: " << endl;
	cout << "1. Retrieve historical price around Announcement Day for all stocks." << endl;
	cout << "2. Pull information of a certain stock." << endl;
	cout << "3. Compute AAR or CAAR for each group." << endl;
	cout << "4. Graph AAR & CAAR for all 3 groups." << endl;
	cout << "5. Exit." << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	cout << "Please type in the step you would like to conduct: ";
	cin >> step;

	switch (step)
	{
	case '1':
	step1:
		while (true)
		{
			cout << endl << "Now you are in step 1: price retrieving." << endl;
			cout << "This may take a few minutes..." << endl << endl;
			cout << "Please ignore the warning 'could not connect to host server'." << endl;
			cout << "It will not affect our calculation." << endl << endl << endl;

			//getdata.h read .csv data
			readESPData(high, mid, low);

			// Download data fron Yahoo Finance
			StockGroup All_SP500_Info(GetYahooData, high, low, mid);
			groups = All_SP500_Info.GetResult();
			low = All_SP500_Info.GetLow();
			mid = All_SP500_Info.GetMid();
			high = All_SP500_Info.GetHigh();

			if (low[1].GetPrice().size() == 0 || mid[1].GetPrice().size() == 0 || high[1].GetPrice().size() == 0)
			{
				cout << "There is something wrong within the csv files!" << endl;
				system("pause");
				return 0;
			}

			getSPYData(SPYVectorPosition, SPYPriceVec);
			SPYDailyReturn = DailyReturn(SPYPriceVec);
			itr = SPYVectorPosition.begin();
			SPYVectorPosition.erase(itr);
			cout << "Data downlowding finished." << endl;
			cout << "Thanks for your patience." << endl;

			goto beginning;
			break;
		}

	case '2':
	step2:

		cout << endl << endl << "Now you are in step 2: view info of a signle stock." << endl;
		if (low.size() == 0)
		{
			cout << "Sorry, you have not finished step 1. Let's step back now." << endl;
			goto step1;
		}
		else
		{
		Getticker:
			cout << "Please type in the ticker of the stock you would like to retrieve: " << endl;
			string ticker;
			cin >> ticker;
			cout << endl;

			int count = 0;
			char ticker_decision;
			for (int i = 0; i < 3; i++) 
			{
				for (auto it = groups[i].begin(); it != groups[i].end(); it++)
				{
					if (it->second.getTicker() == ticker)
					{
						it->second.printStock();
						count++;
						goto beginning;
						break;
					}
				}
			}
			if (count == 0)
			{
				cout << "Sorry, we cannot find the corresponding stock. Please make sure your stock is included in S&P 500." << endl;
				goto Getticker;
			}
		Getticker2:
			cout << "Do you want to try for another stock? (Y/N)" << endl;
			cin >> ticker_decision;
			if (ticker_decision == 'Y' || ticker_decision == 'y')
				goto Getticker;
			else if (ticker_decision == 'N' || ticker_decision == 'n')
				//break;
				cout << "Continue to step 3." << endl;
			else
			{
				cout << "Illegal input! Please use Y/y for Yes or N/n for No." << endl;
				goto Getticker2;
			}
		}
		

	case '3':
	step3:

		cout << endl << endl << "Now you are in step 3: calculation of AAR and CAAR." << endl;
		cout << "This may take a few seconds..." << endl << endl;
		if (low.size() != 0)
		{
			po = GetSampleGroup(low, mid, high);
			result = MatrixInfo(po, SPYVectorPosition, SPYDailyReturn);

			for (int i = 1; i <= 29; i++) 
			{
				po = GetSampleGroup(low, mid, high);
				map<string, vector<vector<double>>> temp = MatrixInfo(po, SPYVectorPosition, SPYDailyReturn);
				result + temp; 
			}
			
			Divide(result, 30.0); 
			cout << "Calculations finished." << endl;
		display:
			cout << "Please type in the group name you would like to view (high, mid or low): " << endl;
			string group_no;
			cin >> group_no;
			int counter = 1;
			if (group_no == "high" || group_no == "mid" || group_no == "low")
			{
				if (group_no == "high") { cout << "AAR & CAAR for Group 'High'" << endl; }
				else if (group_no == "mid") { cout << "AAR & CAAR for Group 'Mid'" << endl; }
				else { cout << "AAR & CAAR for Group 'Low'" << endl; }

				cout << "AAR         CAAR" << endl;
				for (auto it = result[group_no][0].begin(), it2 = result[group_no][1].begin(); it != result[group_no][0].end(); it++, it2++)
				{
					cout << *it << "     " << *it2 << endl;
				}
			}
			else 
			{
				cout << "Illegal input! Please type specifically the name of a group." << endl;
				goto display;
			}
		display2:

			cout << "Would you like to view another group? (Y/N)" << endl;
			char decision_3;
			cin >> decision_3;
			if (decision_3 == 'Y' || decision_3 == 'y')
				goto display;
			else if (decision_3 == 'N' || decision_3 == 'n')
			{
				cout << "Continue to step 4." << endl << endl;
				//break;
				goto step4;
			}
			else
			{
				cout << "Illegal input! Please use Y/y for Yes or N/n for No." << endl;
				goto display2;
			}
		}
		else 
		{
			cout << "Sorry, you have not finished step 1. Let's retrieve data now." << endl;
			cout << "This may take a few minutes..." << endl << endl;
			cout << "Please ignore the warning 'could not connect to host server'." << endl;
			cout << "It will not affect our calculation." << endl << endl << endl;

			readESPData(high, mid, low);

			// Download data fron Yahoo Finance
			StockGroup All_SP500_Info(GetYahooData, high, low, mid);
			groups = All_SP500_Info.GetResult();
			low = All_SP500_Info.GetLow();
			mid = All_SP500_Info.GetMid();
			high = All_SP500_Info.GetHigh();

			if (low[1].GetPrice().size() == 0 || mid[1].GetPrice().size() == 0 || high[1].GetPrice().size() == 0)
			{
				cout << "There is something wrong within the csv files!" << endl;
				system("pause");
				return 0;
			}

			getSPYData(SPYVectorPosition, SPYPriceVec);
			SPYDailyReturn = DailyReturn(SPYPriceVec);
			itr = SPYVectorPosition.begin();
			SPYVectorPosition.erase(itr);
			cout << "Data downlowding finished." << endl;
			cout << "Thanks for your patience." << endl;

			goto step3;
		}
		goto beginning;
		

	case '4':
	step4:
		
		cout << endl << endl << "Now you are in step 4: graph of AAR and CAAR." << endl;

		if (low.size() != 0 || result["low"].size() != 0)
		{
		plot:
			cout << "Which graph would you like to plot?" << endl;
			cout << "Please type A (for AAR), B (for Both) or C (for CAAR)." << endl;
			string graph_type;
			cin >> graph_type;
			if (graph_type == "A" || graph_type == "a" ||graph_type == "B" || graph_type == "b" || graph_type == "C" || graph_type == "c")
			{
				cout << "We are now starting excel. This may take a few minutes..." << endl;
				if (graph_type == "A" || graph_type == "a") { GraphAAR(result); }
				else if (graph_type == "B" || graph_type == "b") { GraphAAR(result); GraphCAAR(result); }
				else { GraphCAAR(result); }
			}
			else {
				goto plot;
			}
		plot_2:
			cout << "Would you like to see another graph? (Y/N)" << endl;
			char graph_2;
			cin >> graph_2;
			if (graph_2 == 'Y' || graph_2 == 'y')
				goto plot;
			else if (graph_2 == 'N' || graph_2 == 'n')
			{
				cout << "Step 4 finished." << endl;
			}
			else
			{
				cout << "Illegal input! Please use Y/y for Yes or N/n for No." << endl;
				goto plot_2;
			}
		}

		else if (low.size() == 0)
		{
			cout << "Sorry, you have not finished step 1. Let's retrieve data now." << endl;
			cout << "This may take a few minutes..." << endl << endl;
			cout << "Please ignore the warning 'could not connect to host server'." << endl;
			cout << "It will not affect our calculation." << endl << endl << endl;

			readESPData(high, mid, low);

			// Download data fron Yahoo Finance
			StockGroup All_SP500_Info(GetYahooData, high, low, mid);
			groups = All_SP500_Info.GetResult();
			low = All_SP500_Info.GetLow();
			mid = All_SP500_Info.GetMid();
			high = All_SP500_Info.GetHigh();

			if (low[1].GetPrice().size() == 0 || mid[1].GetPrice().size() == 0 || high[1].GetPrice().size() == 0)
			{
				cout << "There is something wrong within the csv files!" << endl;
				system("pause");
				return 0;
			}

			getSPYData(SPYVectorPosition, SPYPriceVec);
			SPYDailyReturn = DailyReturn(SPYPriceVec);
			itr = SPYVectorPosition.begin();
			SPYVectorPosition.erase(itr);
			cout << "Data downlowding finished." << endl;
			cout << "Thanks for your patience." << endl;

			cout << endl << "Now let's go to step 3." << endl;
			goto step3;
		}
		else
		{
			cout << "Sorry, you have not finished step 3. Let's go to step 3 now." << endl;
			goto step3;
		}
		goto beginning;
		break;

	case '5':
		cout << "Successfully exited." << endl;
		cout << "Thanks for using!" << endl;
		break;
		
	default:
		cout << "Please select steps between 1 and 5." << endl;
		goto beginning;
		
	}
	system("pause");
	return 0;

}

