#include "CSVReader.h"
#include "CSVWriter.h"
#include "Function.h"
#include <vector>
#include <string>
#include <math.h>
#include <iostream>

#pragma comment( lib, "Utility_d.lib" ) 

const double PI = 3.1415926536;

const int endTime = 2000;
const int interval = 20;

using namespace ynym::util;

class SinWave : public Function{
	double hertz;
public:
	SinWave() {
		hertz = 1;
	}
	
	double func(int mtime) {
		return sin(mtime / 1000.0 * (2 * PI) * hertz);
	}

};

void writeWave(std::string name, std::vector<std::vector<double>> wave) {
	CSVWriter writer(name);

	std::vector<std::string> temp;
	temp.resize(2);

	for (auto l : wave) {
		temp[0] = std::to_string(l[0] / 1000.0);
		temp[1] = std::to_string(l[1]);
		writer.writeLine(temp);
	}
}

std::vector<std::vector<double>> makeOriginalWave() {
	SinWave sinWave;
	std::vector<std::vector<double>> data;

	data.resize(endTime / interval + 1);

	for (int i = 0, mtime = 0; mtime <= endTime; mtime += interval, i++) {
		data[i].resize(2);
		data[i][0] = mtime;
		data[i][1] = sinWave(mtime);
	}
	
	return data;
}


std::vector<std::vector<double>> makeSampleWave(std::vector<std::vector<double>> wave) {
	std::vector<std::vector<double>> data;
	int skip = 10;

	data.resize(wave.size() / skip + 1);
	for (int ori = 0, sam = 0; ori < wave.size(); sam++, ori += skip) {
		data[sam].resize(2);
		data[sam][0] = wave[ori][0];
		data[sam][1] = wave[ori][1];
	}

	return data;
}

std::vector<std::vector<double>> interpolate(std::vector<std::vector<double>> sample) {
	std::vector<std::vector<double>> res;

	res.resize(endTime / interval + 1);
	for (int mtime = 0, c = 0; mtime <= endTime; mtime += interval, c++) {
		double sum = 0.0;
		for (int i = 0; i < sample.size(); i++) {
			double x = (double)(mtime - sample[i][0]) / (interval * 10);
			double sinc = sin(PI * x) / (PI * x);
			sum += sample[i][1] * sinc;
		}
		res[c].resize(2);
		res[c][0] = mtime;
		res[c][1] = sum;
	}
	
	return res;
}

int main() {


	std::vector<std::vector<double>> wave = makeOriginalWave();
	std::vector<std::vector<double>> sampleWave = makeSampleWave(wave);
	std::vector<std::vector<double>> interpolateWave = interpolate(sampleWave);

	try {
		writeWave("original.csv", wave);
		writeWave("sample.csv", sampleWave);
		writeWave("interpolate.csv", interpolateWave);
	
	} catch (std::runtime_error& err) {
		std::cout << "ERROR!\n" << err.what() << std::endl;
	}
	

	return 0;
}


