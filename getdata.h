#pragma once
#ifndef getdata_h
#define getdata_h

#include "stdafx.h"
#include <stdio.h>
#include <string> 
#include <iostream>
#include <sstream>  
#include <vector>
#include <locale>
#include <iomanip>
#include "curl.h"

#include <fstream>
#include <stack>
#include <map>
#include "stock.h"



using namespace std;

struct MemoryStruct {
	char *memory;
	size_t size;
};

//Deal with possible realloc() that doesn't like reallocting  NULL pointers
void *myrealloc(void *ptr, size_t size) 
{
	if (ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}

int write_data2(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)data;

	mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}

string getTimeinSeconds(string Time) 
{
	std::tm t = { 0 };
	std::istringstream ssTime(Time);
	char time[100];
	// set memset 000...000
	memset(time, 0, 100);
	if (ssTime >> std::get_time(&t, "%Y/%m/%dT%H:%M:%S"))
	{
		//cout << std::put_time(&t, "%c %Z") << "\n"
		//<< std::mktime(&t) << "\n";
		sprintf_s(time, "%lld", mktime(&t));
		// longlong int
		//cout << time;
		return string(time);
	}
	else
	{
		cout << "Parse failed\n";
		return "";
	}
}


string getTimeinDate(string Time)
{
	std::tm t = { 0 };
	std::istringstream ssTime(Time);
	char time[100];
	memset(time, 0, 100);
	if (ssTime >> std::get_time(&t, "%Y/%m/%d"))
	{
		char a[128];
		strftime(a, 128, "%Y-%m-%d", &t); 

		string b = a;
		return b;
	}
	else
	{
		cout << "Parse failed\n";
		return "";
	}
}


void readESPData(vector<stock>& highVec, vector<stock>& midVec, vector<stock>& lowVec) 
{
	string line;
	ifstream file("test1.csv"); 
	if (file.is_open()) {
		getline(file, line);
		while (getline(file, line)) {
			// size_t: unsigned int type
			size_t c1 = line.find(',');
			size_t c2 = line.find(',', c1 + 1);
			size_t c3 = line.find(',', c2 + 1);
			size_t c4 = line.find(',', c3 + 1);
			size_t c5 = line.find(',', c4 + 1);
			size_t c6 = line.find(',', c5 + 1);
			size_t a = 0;
			// use stock objects to store their information, stod: string to double
			stock tempStock(line.substr(0, c1), line.substr(c1 + 1, c2 - c1 - 1), stod(line.substr(c2 + 1, c3 - c2 - 1)), stod(line.substr(c3 + 1, c4 - c3 - 1)), stod(line.substr(c4 + 1, c5 - c4 - 2)), line.substr(c5 + 1, c6 - c5 - 1), line.substr(c6 + 1));
			
			// sort into groups
			if (tempStock.getPerformance() > 6.5) //6.5%
				highVec.push_back(tempStock);
			else if (tempStock.getPerformance() < 0)
				lowVec.push_back(tempStock);
			else
				midVec.push_back(tempStock);
			//free(&tempStock);
			//free(&line);
		}

		file.close();
	}
	else
		cerr << "Unable to open file" << endl;
}

//Pass in the stock vector to get the yahoo data for those stocks
void GetYahooData(vector<stock>& stVec, map<string, stock>& stMap)
{
	vector<stock>::iterator itr = stVec.begin();

	struct MemoryStruct data;
	data.memory = NULL;
	data.size = 0;
	FILE *fp;

	CURL *handle;
	CURLcode result;

	curl_global_init(CURL_GLOBAL_ALL);
	handle = curl_easy_init();
	
	if (handle)
	{
		string sCookies, sCrumb;
		fp = fopen("cookies.txt", "r");
		char cCookies[100];
		if (fp) 
		{
			while (fscanf(fp, "%s", cCookies) != EOF);
			fclose(fp);
		}
		else
			cerr << "cookies.txt does not exists" << endl;

		sCookies = cCookies;

		while (1)
		{
			sCookies.clear();
			sCrumb.clear();

			curl_easy_setopt(handle, CURLOPT_URL, "https://finance.yahoo.com/quote/AMZN/history");
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
			curl_easy_setopt(handle, CURLOPT_COOKIEJAR, "cookies.txt");

			curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");
			curl_easy_setopt(handle, CURLOPT_ENCODING, "");

			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);
			result = curl_easy_perform(handle);

			if (result != CURLE_OK)
			{
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				free(data.memory);
				data.memory = NULL;
				data.size = 0;
				continue;
			}
			else
			{
				char cKey[] = "CrumbStore\":{\"crumb\":\"";
				char *ptr1 = strstr(data.memory, cKey);
				char *ptr2 = ptr1 + strlen(cKey);
				char *ptr3 = strstr(ptr2, "\"}");
				if (ptr3 != NULL)
					*ptr3 = NULL;

				sCrumb = ptr2;

				free(data.memory);
				data.memory = NULL;
				data.size = 0;
				break;
			}
		}

		while (true)
		{
			if (itr == stVec.end())
				break;

			string urlA = "https://query1.finance.yahoo.com/v7/finance/download/";
			string symbol = itr->getTicker();
			//cout << itr->getStartDate() << "," << itr->getEndDate() << endl;
			string startTime = getTimeinSeconds(itr->getStartDate() + "T16:00:00");
			string endTime = getTimeinSeconds(itr->getEndDate() + "T16:00:00");
			string urlB = "?period1=";
			string urlC = "&period2=";
			string urlD = "&interval=1d&events=history&crumb=";
			string url = urlA + symbol + urlB + startTime + urlC + endTime + urlD + sCrumb;
			const char * cURL = url.c_str();
			const char * cookies = sCookies.c_str();
			curl_easy_setopt(handle, CURLOPT_COOKIE, cookies);
			curl_easy_setopt(handle, CURLOPT_URL, cURL);

			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);
			result = curl_easy_perform(handle);

			if (result != CURLE_OK)
			{	
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				return;
			}

			stringstream sData;
			sData.str(data.memory);
			string line;
			vector<double> priceVec;
			getline(sData, line);
			//cout << line << endl;
			for (; getline(sData, line);) {
				if (line.find('D') != -1) 
					break;
				size_t pos = line.find(',', 5);
				size_t end = line.find_last_of(',');
				itr->pushBack(stod(line.substr(pos + 1, end - pos - 1))); 
				//cout << line << endl;
			}
			//save price to stMap
			stMap.insert(pair<string, stock>(symbol, *itr)); 
			itr++;

			free(data.memory);
			data.memory = NULL;
			data.size = 0;
		}
		free(data.memory);
		data.size = 0;
	}

	else
	{
		fprintf(stderr, "Curl init failed!\n");
		return;
	}
	
	curl_easy_cleanup(handle);
	curl_global_cleanup();
}



 //Get SPY Data
void getSPYData(map<string, size_t>& mpPosition, vector<double>& priceVec)
{
	string symbol = "SPY";
	string startTime = getTimeinSeconds("2016/10/1T16:00:00");
	string endTime = getTimeinSeconds("2018/4/20T16:00:00");

	struct MemoryStruct data;
	data.memory = NULL;
	data.size = 0;
	FILE *fp;

	CURL *handle;
	CURLcode result;

	curl_global_init(CURL_GLOBAL_ALL);
	handle = curl_easy_init();

	if (handle)
	{
		string sCookies, sCrumb;
		if (sCookies.length() == 0 || sCrumb.length() == 0)
		{
			curl_easy_setopt(handle, CURLOPT_URL, "https://finance.yahoo.com/quote/AMZN/history");
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
			curl_easy_setopt(handle, CURLOPT_COOKIEJAR, "cookies.txt");
			curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");
			curl_easy_setopt(handle, CURLOPT_ENCODING, "");

			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);

			result = curl_easy_perform(handle);

			if (result != CURLE_OK)
			{
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				return;
			}

			char cKey[] = "CrumbStore\":{\"crumb\":\"";
			char *ptr1 = strstr(data.memory, cKey);
			char *ptr2 = ptr1 + strlen(cKey);
			char *ptr3 = strstr(ptr2, "\"}");
			if (ptr3 != NULL)
				*ptr3 = NULL;

			sCrumb = ptr2;

			fp = fopen("cookies.txt", "r");
			char cCookies[100];
			if (fp) {
				while (fscanf(fp, "%s", cCookies) != EOF);
				fclose(fp);
			}
			else
				cerr << "cookies.txt does not exists" << endl;

			sCookies = cCookies;
			free(data.memory);
			data.memory = NULL;
			data.size = 0;
		}

		string urlA = "https://query1.finance.yahoo.com/v7/finance/download/";
		string urlB = "?period1=";
		string urlC = "&period2=";
		string urlD = "&interval=1d&events=history&crumb=";
		string url = urlA + symbol + urlB + startTime + urlC + endTime + urlD + sCrumb;
		const char * cURL = url.c_str();
		const char * cookies = sCookies.c_str();
		curl_easy_setopt(handle, CURLOPT_COOKIE, cookies);
		curl_easy_setopt(handle, CURLOPT_URL, cURL);

		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);
		result = curl_easy_perform(handle);
		
		if (result != CURLE_OK)
		{	 
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
			return;
		}

		stringstream sData;
		sData.str(data.memory);
		string line;
		size_t i = -1;
		getline(sData, line);
		for (; getline(sData, line);) {
			string date = line.substr(0, line.find(','));
			mpPosition.insert(pair<string, size_t>(date, i));
			size_t pos = line.find(',', 5);
			size_t end = line.find_last_of(',');
			priceVec.push_back(stod(line.substr(pos + 1, end - pos - 1))); 
			i++;
			//cout << line << endl;
		}
		free(data.memory);
		data.size = 0;
	}

	else
	{
		fprintf(stderr, "Curl init failed!\n");
		return;
	}
	curl_easy_cleanup(handle);
	curl_global_cleanup();
}


#endif // getdata_h
