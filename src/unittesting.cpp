#include <queue>
#include <string>
#include <algorithm>
#include <iostream>
#include "gtest/gtest.h"
#include "util.h"
#include "inference.h"
#include "features/features.h"
#include "optparser.h"
#include "globalDefs.h"
#include "ml/random_forest.h"
#include "ml/boosted_svm.h"
#include "posemachine.h"
#include "piotr/piotrfeatures.h"
#include "HDFUtil.h"

static double allowable_prec_error = 1e-6;

using namespace std;
using namespace cv;

#if COMPILE_CUDA
#include "cuda_inference.h"
#include "cuda_util.h"

// Error checking
#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, string file, int line, bool abort=true)
{
	if (code != cudaSuccess)
	{
		cout << "GPUassert: " << cudaGetErrorString(code) << " " << file << " " << line << endl;
		if (abort) exit(code);
	}
}
#endif

string configLocation;

using namespace std;

/*---------------------------------
-------------- util ---------------
-----------------------------------*/
// toLowerCase
TEST(UtilTest, toLowerCase)
{
	EXPECT_EQ("lowercase", toLowerCase("LoWERcAsE"));
	EXPECT_EQ("qwerasdf", toLowerCase("qwerasdf"));
	EXPECT_EQ("yolo!", toLowerCase("YOLO!"));
}

// getFilesInDirectory
TEST(UtilTest, getFilesInDirectory)
{
	vector<string> fileNames;
	vector<string> validExtensions;
	validExtensions.push_back("gtest");
	getFilesInDirectory(configLocation, fileNames, validExtensions);
	EXPECT_EQ(5, fileNames.size());
	EXPECT_TRUE(find(fileNames.begin(), fileNames.end(), configLocation + "file1.gtest") != fileNames.end());
	EXPECT_TRUE(find(fileNames.begin(), fileNames.end(), configLocation + "file2.gtest") != fileNames.end());
	EXPECT_TRUE(find(fileNames.begin(), fileNames.end(), configLocation + "file3.gtest") != fileNames.end());
	EXPECT_TRUE(find(fileNames.begin(), fileNames.end(), configLocation + "file4.gtest") != fileNames.end());
	EXPECT_TRUE(find(fileNames.begin(), fileNames.end(), configLocation + "file5.gtest") != fileNames.end());
}

// scores2scmap
TEST(UtilTest, scores2scmap)
{
	for (int numtimes = 0; numtimes < 100; numtimes++)
	{
		srand(numtimes);
		int numdim1 = 10;
		int xsize1 = 5;
		int ysize1 = 5;
		vector<vector<prec_type> > Y1;
		Size sz1(xsize1, ysize1);
		for (int y = 0; y < ysize1; y++)
		{
			for (int x = 0; x < xsize1; x++)
			{
				vector<prec_type> vec;
				int idx = y*xsize1 + x;
				for (int d = 0; d < numdim1; d++)
				{
					vec.push_back((prec_type)rand() / RAND_MAX);
				}
				Y1.push_back(vec);
			}
		}
		vector<Mat*> mat1 = scores2scmap(Y1, sz1);
		for (int y = 0; y < ysize1; y++)
		{
			for (int x = 0; x < xsize1; x++)
			{
				int idx = y*xsize1 + x;
				for (int d = 0; d < numdim1; d++)
				{
					EXPECT_EQ(mat1[d]->at<float>(y,x), (float)Y1[idx][d]);
				}
			}
		}

		int numdim2 = 23;
		int xsize2 = 23;
		int ysize2 = 11;
		prec_type* Y2 = new prec_type[xsize2*ysize2*numdim2];
		Size sz2(xsize2, ysize2);
		for (int y = 0; y < ysize2; y++)
		{
			for (int x = 0; x < xsize2; x++)
			{
				int idx = y*xsize2 + x;
				for (int d = 0; d < numdim2; d++)
				{
					Y2[idx*numdim2 + d] = (prec_type)rand() / RAND_MAX;
				}
			}
		}
		vector<Mat*> mat2 = scores2scmap(Y2, xsize2*ysize2, numdim2, sz2);
		for (int y = 0; y < ysize2; y++)
		{
			for (int x = 0; x < xsize2; x++)
			{
				int idx = y*xsize2 + x;
				for (int d = 0; d < numdim2; d++)
				{
					EXPECT_EQ(mat2[d]->at<float>(y,x), (float)Y2[idx*numdim2 + d]);
				}
			}
		}
		delete Y2;
	}
}

// parseHierAnnotation
TEST(UtilTest, parseHierAnnotation)
{
  Json::Value config;
  Json::Value anno;
  char configFilepath[1024];
  char annoFilepath[1024];
  strcpy(configFilepath, string(configLocation + "/config.json").c_str());
  strcpy(annoFilepath, string(configLocation + "/anno.json").c_str());
  parseJSONFile(configFilepath, config);
  parseJSONFile(annoFilepath, anno);

  // annotations are all at (0, 0)
  Json::Value anno1 = anno["anno1"];
  vector<vector<double> > h_anno1;
  parseHierAnnotation(h_anno1, anno1, config, 0);
  for (int p = 0; p < h_anno1.size(); p++)
  {
    EXPECT_EQ(0, h_anno1[p][0]);
    EXPECT_EQ(0, h_anno1[p][1]);
  }

  // Now different annotations
  Json::Value anno2 = anno["anno2"];
  vector<vector<double> > h_anno2;
  parseHierAnnotation(h_anno2, anno2, config, 0);
  EXPECT_EQ(1, h_anno2[0][0]);
  EXPECT_EQ(10, h_anno2[0][1]);
  EXPECT_EQ(80, h_anno2[1][0]);
  EXPECT_EQ(40, h_anno2[1][1]);
  EXPECT_EQ(0, h_anno2[2][0]);
  EXPECT_EQ(20, h_anno2[2][1]);
  EXPECT_EQ(30, h_anno2[3][0]);
  EXPECT_EQ(20, h_anno2[3][1]);
  EXPECT_EQ(60, h_anno2[4][0]);
  EXPECT_EQ(50, h_anno2[4][1]);
  EXPECT_EQ(35, h_anno2[5][0]);
  EXPECT_EQ(25, h_anno2[5][1]);
  EXPECT_EQ(45, h_anno2[6][0]);
  EXPECT_EQ(31, h_anno2[6][1]);
  EXPECT_EQ(40, h_anno2[7][0]);
  EXPECT_EQ(5, h_anno2[7][1]);
  EXPECT_EQ(102, h_anno2[8][0]);
  EXPECT_EQ(97, h_anno2[8][1]);
  EXPECT_EQ(36, h_anno2[9][0]);
  EXPECT_EQ(81, h_anno2[9][1]);
  EXPECT_EQ(33, h_anno2[10][0]);
  EXPECT_EQ(17, h_anno2[10][1]);
  EXPECT_EQ(124, h_anno2[11][0]);
  EXPECT_EQ(144, h_anno2[11][1]);
  EXPECT_EQ(43, h_anno2[12][0]);
  EXPECT_EQ(209, h_anno2[12][1]);
  EXPECT_EQ(111, h_anno2[13][0]);
  EXPECT_EQ(1, h_anno2[13][1]);
  h_anno2.clear();
  parseHierAnnotation(h_anno2, anno2, config, 1);
  EXPECT_EQ(35.25, h_anno2[0][0]);
  EXPECT_EQ(30, h_anno2[0][1]);
  EXPECT_EQ(30, h_anno2[1][0]);
  EXPECT_EQ(30, h_anno2[1][1]);
  EXPECT_EQ(40, h_anno2[2][0]);
  EXPECT_EQ(20 + 1/3.0, h_anno2[2][1]);
  EXPECT_EQ(57, h_anno2[3][0]);
  EXPECT_EQ(65, h_anno2[3][1]);
  EXPECT_EQ(92 + 2/3.0, h_anno2[4][0]);
  EXPECT_EQ(118, h_anno2[4][1]);

  // Now different annotations
  anno2 = anno["anno2"];
  vector<bool> vis;
  vector<vector<double> > h_anno3;
  parseHierAnnotation(h_anno3, vis, anno2, config, 0);
  EXPECT_EQ(1, h_anno3[0][0]);
  EXPECT_EQ(10, h_anno3[0][1]);
  EXPECT_EQ(80, h_anno3[1][0]);
  EXPECT_EQ(40, h_anno3[1][1]);
  EXPECT_EQ(0, h_anno3[2][0]);
  EXPECT_EQ(20, h_anno3[2][1]);
  EXPECT_EQ(30, h_anno3[3][0]);
  EXPECT_EQ(20, h_anno3[3][1]);
  EXPECT_EQ(60, h_anno3[4][0]);
  EXPECT_EQ(50, h_anno3[4][1]);
  EXPECT_EQ(35, h_anno3[5][0]);
  EXPECT_EQ(25, h_anno3[5][1]);
  EXPECT_EQ(45, h_anno3[6][0]);
  EXPECT_EQ(31, h_anno3[6][1]);
  EXPECT_EQ(40, h_anno3[7][0]);
  EXPECT_EQ(5, h_anno3[7][1]);
  EXPECT_EQ(102, h_anno3[8][0]);
  EXPECT_EQ(97, h_anno3[8][1]);
  EXPECT_EQ(36, h_anno3[9][0]);
  EXPECT_EQ(81, h_anno3[9][1]);
  EXPECT_EQ(33, h_anno3[10][0]);
  EXPECT_EQ(17, h_anno3[10][1]);
  EXPECT_EQ(124, h_anno3[11][0]);
  EXPECT_EQ(144, h_anno3[11][1]);
  EXPECT_EQ(43, h_anno3[12][0]);
  EXPECT_EQ(209, h_anno3[12][1]);
  EXPECT_EQ(111, h_anno3[13][0]);
  EXPECT_EQ(1, h_anno3[13][1]);
  h_anno3.clear();
  parseHierAnnotation(h_anno3, anno2, config, 1);
  EXPECT_EQ(35.25, h_anno3[0][0]);
  EXPECT_EQ(30, h_anno3[0][1]);
  EXPECT_EQ(30, h_anno3[1][0]);
  EXPECT_EQ(30, h_anno3[1][1]);
  EXPECT_EQ(40, h_anno3[2][0]);
  EXPECT_EQ(20 + 1/3.0, h_anno3[2][1]);
  EXPECT_EQ(57, h_anno3[3][0]);
  EXPECT_EQ(65, h_anno3[3][1]);
  EXPECT_EQ(92 + 2/3.0, h_anno3[4][0]);
  EXPECT_EQ(118, h_anno3[4][1]);
}

// getHOGsize
TEST(UtilTest, getHOGsize)
{
  Json::Value config;
  char configFilepath[1024];
  strcpy(configFilepath, string(configLocation + "/config.json").c_str());
  parseJSONFile(configFilepath, config);

  if(config.get("feat_type", 0).asInt() == 1)
  {
    config["hog_variant"] = 1;
    EXPECT_EQ(36, getHOGsize(config));
    config["hog_variant"] = 2;
    EXPECT_EQ(31, getHOGsize(config));
  }
  else
  {
    config["hog_orientations"] = 9;
    EXPECT_EQ(13, getHOGsize(config));
    cout << getHOGsize(config) << endl;
  }
}

// readMargin
TEST(UtilTest, readMargin)
{
	Json::Value config;
	char configFilepath[1024];
	strcpy(configFilepath, string(configLocation + "/config.json").c_str());
	parseJSONFile(configFilepath, config);
	Json::Value numLabels = config["numLabels"];

	int hier = 0;
	vector<vector<double> > margin_h0 = readMargin(config, hier);
	for (int i = 0; i < numLabels.get(hier, 0).asInt(); i++)
	{
		for (int j = 0; j < numLabels.get(hier, 0).asInt(); j++)
		{
			if (i == j)
				EXPECT_EQ(0, margin_h0[i][j]);
			else
				EXPECT_EQ(1, margin_h0[i][j]);
		}
	}

	hier = 1;
	vector<vector<double> > margin_h1 = readMargin(config, hier);
	for (int i = 0; i < numLabels.get(hier, 0).asInt(); i++)
	{
		for (int j = 0; j < numLabels.get(hier, 0).asInt(); j++)
		{
			if (i == j)
				EXPECT_EQ(0, margin_h1[i][j]);
			else
				EXPECT_EQ(1, margin_h1[i][j]);
		}
	}
}

// argmax
TEST(UtilTest, argmax)
{

	vector<prec_type> v1(100, -2);
	v1[23] = -1.999999;
	v1[65] = -2.000001;
	EXPECT_EQ(23, argmax(v1));

	vector<prec_type> v2(100, 0);
	v2[0] = 1.0;
	v2[34] = 1.0;
	v2[77] = .99;
	EXPECT_EQ(0, argmax(v2));

	vector<prec_type> v3(100, 0.23);
	v3[33] = 0.22;
	v3[63] = -0.23;
	v3[77] = 0.2301;
	EXPECT_EQ(77, argmax(v3));

	vector<prec_type> v4(100, 43.1);
	v4[2] = 0;
	v4[34] = 43.09;
	v4[56] = 43.10001;
	v4[88] = -43.1;
	EXPECT_EQ(56, argmax(v4));

	vector<prec_type> v5(100, -234.3);
	v5[0] = 0;
	v5[26] = 0.000003;
	v5[52] = -0.003;
	v5[88] = 0.000003;
	EXPECT_EQ(26, argmax(v5));
}

// getTrainingScale
TEST(UtilTest, getTrainingScale)
{
	Json::Value config;
	Json::Value anno;
	char configFilepath[1024];
	char annoFilepath[1024];
	strcpy(configFilepath, string(configLocation + "/config.json").c_str());
	strcpy(annoFilepath, string(configLocation + "/anno.json").c_str());
	parseJSONFile(configFilepath, config);
	parseJSONFile(annoFilepath, anno);
	vector<string> fileNames;
	fileNames.push_back("anno1");
	fileNames.push_back("anno2");
	fileNames.push_back("anno3");

	// No scaling to training
	bool scale_to_training = 0;
	vector<double> scales1 = getTrainingScale(config, anno, fileNames, 1);
	vector<double> scales2 = getTrainingScale(config, anno, fileNames, 0);

	// TODO - compare this to MATLAB
}

// getStackingSplits
TEST(UtilTest, getStackingSplits)
{

	Json::Value config;
	Json::Value anno;
	char configFilepath[1024];
	char annoFilepath[1024];
	strcpy(configFilepath, string(configLocation + "/config.json").c_str());
	strcpy(annoFilepath, string(configLocation + "/anno1.json").c_str());
	parseJSONFile(configFilepath, config);
	parseJSONFile(annoFilepath, anno);

	vector<string> fileNames;
	fileNames.push_back("im0001.jpg");
	fileNames.push_back("im0002.jpg");
	fileNames.push_back("im0003.jpg");
	fileNames.push_back("im0004.jpg");
	fileNames.push_back("im0005.jpg");
	fileNames.push_back("im0006.jpg");
	fileNames.push_back("im0007.jpg");
	fileNames.push_back("im0008.jpg");
	fileNames.push_back("im0009.jpg");
	fileNames.push_back("im0010.jpg");

	// pn_flag = 1, num_splits = 10
	config["splits"] = 10;
	vector<int> splits1;
	getStackingSplits(splits1, config, fileNames, 1, anno);
	EXPECT_EQ(1, splits1[0]);
	EXPECT_EQ(2, splits1[1]);
	EXPECT_EQ(3, splits1[2]);
	EXPECT_EQ(4, splits1[3]);
	EXPECT_EQ(5, splits1[4]);
	EXPECT_EQ(6, splits1[5]);
	EXPECT_EQ(7, splits1[6]);
	EXPECT_EQ(8, splits1[7]);
	EXPECT_EQ(9, splits1[8]);
	EXPECT_EQ(0, splits1[9]);

	// pn_flag = 0, num_splits = 10
	config["splits"] = 10;
	vector<int> splits2;
	getStackingSplits(splits2, config, fileNames, 0, anno);
	EXPECT_EQ(0, splits2[0]);
	EXPECT_EQ(1, splits2[1]);
	EXPECT_EQ(2, splits2[2]);
	EXPECT_EQ(3, splits2[3]);
	EXPECT_EQ(4, splits2[4]);
	EXPECT_EQ(5, splits2[5]);
	EXPECT_EQ(6, splits2[6]);
	EXPECT_EQ(7, splits2[7]);
	EXPECT_EQ(8, splits2[8]);
	EXPECT_EQ(9, splits2[9]);

	// pn_flag = 1, num_splits = 2;
	config["splits"] = 2;
	vector<int> splits3;
	getStackingSplits(splits3, config, fileNames, 1, anno);
	EXPECT_EQ(1, splits3[0]);
	EXPECT_EQ(0, splits3[1]);
	EXPECT_EQ(1, splits3[2]);
	EXPECT_EQ(0, splits3[3]);
	EXPECT_EQ(1, splits3[4]);
	EXPECT_EQ(0, splits3[5]);
	EXPECT_EQ(1, splits3[6]);
	EXPECT_EQ(0, splits3[7]);
	EXPECT_EQ(1, splits3[8]);
	EXPECT_EQ(0, splits3[9]);

	// pn_flag = 0, num_splits = 2
	config["splits"] = 2;
	vector<int> splits4;
	getStackingSplits(splits4, config, fileNames, 0, anno);
	EXPECT_EQ(0, splits4[0]);
	EXPECT_EQ(1, splits4[1]);
	EXPECT_EQ(0, splits4[2]);
	EXPECT_EQ(1, splits4[3]);
	EXPECT_EQ(0, splits4[4]);
	EXPECT_EQ(1, splits4[5]);
	EXPECT_EQ(0, splits4[6]);
	EXPECT_EQ(1, splits4[7]);
	EXPECT_EQ(0, splits4[8]);
	EXPECT_EQ(1, splits4[9]);

	// num_splits = -1
	config["splits"] = -1;
	vector<int> splits5;
	getStackingSplits(splits5, config, fileNames, 0, anno);
	EXPECT_EQ(-1, splits5[0]);
	EXPECT_EQ(-1, splits5[1]);
	EXPECT_EQ(-1, splits5[2]);
	EXPECT_EQ(-1, splits5[3]);
	EXPECT_EQ(-1, splits5[4]);
	EXPECT_EQ(-1, splits5[5]);
	EXPECT_EQ(-1, splits5[6]);
	EXPECT_EQ(-1, splits5[7]);
	EXPECT_EQ(-1, splits5[8]);
	EXPECT_EQ(-1, splits5[9]);
}

// getColorMap
TEST(UtilTest, getColorMap)
{
	int numColors = 2;
	vector<Scalar> colorMap1 = getColorMap(numColors);
	EXPECT_EQ(0, colorMap1[0][0]);
	EXPECT_EQ(0, colorMap1[0][1]);
	EXPECT_EQ(255, colorMap1[0][2]);
	EXPECT_EQ(128, colorMap1[1][0]);
	EXPECT_EQ(127, colorMap1[1][1]);
	EXPECT_EQ(127, colorMap1[1][2]);
	numColors = 5;
	vector<Scalar> colorMap2 = getColorMap(numColors);
	EXPECT_EQ(0, colorMap2[0][0]);
	EXPECT_EQ(0, colorMap2[0][1]);
	EXPECT_EQ(255, colorMap2[0][2]);
	EXPECT_EQ(51, colorMap2[1][0]);
	EXPECT_EQ(51, colorMap2[1][1]);
	EXPECT_EQ(204, colorMap2[1][2]);
	EXPECT_EQ(102, colorMap2[2][0]);
	EXPECT_EQ(102, colorMap2[2][1]);
	EXPECT_EQ(153, colorMap2[2][2]);
	EXPECT_EQ(153, colorMap2[3][0]);
	EXPECT_EQ(153, colorMap2[3][1]);
	EXPECT_EQ(102, colorMap2[3][2]);
	EXPECT_EQ(204, colorMap2[4][0]);
	EXPECT_EQ(204, colorMap2[4][1]);
	EXPECT_EQ(51, colorMap2[4][2]);
}

// writeOutput
TEST(UtilTest, writeOutput)
{
	Json::Value output1;
	vector<vector<int> > partLocs1;
	vector<int> pL10;
	pL10.push_back(10); pL10.push_back(36);
	partLocs1.push_back(pL10);
	vector<int> pL11;
	pL11.push_back(57); pL11.push_back(99);
	partLocs1.push_back(pL11);
	vector<int> pL12;
	pL12.push_back(212); pL12.push_back(74);
	partLocs1.push_back(pL12);
	vector<int> pL13;
	pL13.push_back(184); pL13.push_back(146);
	partLocs1.push_back(pL13);
	vector<int> pL14;
	pL14.push_back(14); pL14.push_back(36);
	partLocs1.push_back(pL14);
	writeOutput(output1, partLocs1, 5, "f1");
	Json::Value f1 = output1["f1"];
	for (int i = 0; i < partLocs1.size(); i++)
	{
		Json::Value out = f1.get(i, 0);
		EXPECT_EQ(roundup(partLocs1[i][0]/5.0), out["x"].asInt());
		EXPECT_EQ(roundup(partLocs1[i][1]/5.0), out["y"].asInt());
	}
}

// notInNbrhood
TEST(UtilTest, notInNbrhood)
{
	// TODO test by comparing to MATLAB

	// bool nin = notInNbrhood(int x, int y, sbin, scale, Size, imsz, const vector<vector<int> >, const Json::Value& config);
}

// scalePartLocation
TEST(UtilTest, scalePartLocation)
{
	vector<double> anno1;
	anno1.push_back(1); anno1.push_back(1);
	double scale1 = 10;
	int sbin1 = 16;
	double xs1 = 50;
	double ys1 = 50;
	int xx1, yy1;
	scalePartLocation(anno1, scale1, sbin1, xs1, ys1, xx1, yy1);
	EXPECT_EQ(0, xx1);
	EXPECT_EQ(0, yy1);
	vector<double> anno2;
	anno2.push_back(51); anno2.push_back(123);
	double scale2 = 1.2;
	int sbin2 = 16;
	int xs2 = 100;
	int ys2 = 100;
	int xx2, yy2;
	scalePartLocation(anno2, scale2, sbin2, xs2, ys2, xx2, yy2);
	EXPECT_EQ(3, xx2);
	EXPECT_EQ(8, yy2);
	vector<double> anno3;
	anno3.push_back(321); anno3.push_back(154);
	double scale3 = 2;
	int sbin3 = 2;
	double xs3 = 200;
	double ys3 = 200;
	int xx3, yy3;
	scalePartLocation(anno3, scale3, sbin3, xs3, ys3, xx3, yy3);
	EXPECT_EQ(199, xx3);
	EXPECT_EQ(153, yy3);
	vector<double> anno4;
	anno4.push_back(0); anno4.push_back(0);
	double scale4 = 2;
	int sbin4 = 2;
	double xs4 = 200;
	double ys4 = 200;
	int xx4, yy4;
	scalePartLocation(anno4, scale4, sbin4, xs4, ys4, xx4, yy4);
	EXPECT_EQ(0, xx4);
	EXPECT_EQ(0, yy4);
}

// sortMatrix
TEST(UtilTest, sortMatrix)
{

	srand(0);
	int numdim = 10;
	int numSamples = 1000;
	int ysize = 50;
	vector<vector<prec_type> > Y;
	for (int i = 0; i < numSamples; i++)
	{
		vector<prec_type> vec;
		for (int j = 0; j < ysize; j++)
		{
			vec.push_back((prec_type)rand() / RAND_MAX);
		}
		Y.push_back(vec);
	}
	int N = 100;
	vector<int> locations = sortMatrix(Y, N);
	for (int i = 0; i < numdim; i++)
	{
		for (int j = 1; j < N; j++)
		{
			EXPECT_GE(Y[locations[i*N + j - 1]][i], Y[locations[i*N + j]][i]);
		}
	}
}

// pairCompare
TEST(UtilTest, pairCompare)
{
  pair<double, int> e1(10.3, 23);
  pair<double, int> e2(-34.23, -122);
  pair<double, int> e3(-2.33, 3432);
  pair<double, int> e4(10.3, -123);
  pair<double, int> e5(56.0, 64);
  pair<double, int> e6(-2.33, -15);

  EXPECT_EQ(0, pairCompare(e1, e1));
  EXPECT_EQ(1, pairCompare(e1, e2));
  EXPECT_EQ(1, pairCompare(e1, e3));
  EXPECT_EQ(0, pairCompare(e1, e4));
  EXPECT_EQ(0, pairCompare(e1, e5));
  EXPECT_EQ(1, pairCompare(e1, e6));

  EXPECT_EQ(0, pairCompare(e2, e1));
  EXPECT_EQ(0, pairCompare(e2, e2));
  EXPECT_EQ(0, pairCompare(e2, e3));
  EXPECT_EQ(0, pairCompare(e2, e4));
  EXPECT_EQ(0, pairCompare(e2, e5));
  EXPECT_EQ(0, pairCompare(e2, e6));

  EXPECT_EQ(0, pairCompare(e3, e1));
  EXPECT_EQ(1, pairCompare(e3, e2));
  EXPECT_EQ(0, pairCompare(e3, e3));
  EXPECT_EQ(0, pairCompare(e3, e4));
  EXPECT_EQ(0, pairCompare(e3, e5));
  EXPECT_EQ(0, pairCompare(e3, e6));

  EXPECT_EQ(0, pairCompare(e4, e1));
  EXPECT_EQ(1, pairCompare(e4, e2));
  EXPECT_EQ(1, pairCompare(e4, e3));
  EXPECT_EQ(0, pairCompare(e4, e4));
  EXPECT_EQ(0, pairCompare(e4, e5));
  EXPECT_EQ(1, pairCompare(e4, e6));

  EXPECT_EQ(1, pairCompare(e5, e1));
  EXPECT_EQ(1, pairCompare(e5, e2));
  EXPECT_EQ(1, pairCompare(e5, e3));
  EXPECT_EQ(1, pairCompare(e5, e4));
  EXPECT_EQ(0, pairCompare(e5, e5));
  EXPECT_EQ(1, pairCompare(e5, e6));

  EXPECT_EQ(0, pairCompare(e6, e1));
  EXPECT_EQ(1, pairCompare(e6, e2));
  EXPECT_EQ(0, pairCompare(e6, e3));
  EXPECT_EQ(0, pairCompare(e6, e4));
  EXPECT_EQ(0, pairCompare(e6, e5));
  EXPECT_EQ(0, pairCompare(e6, e6));

}

// getFeatSize
TEST(UtilTest, getFeatSize)
{
  Json::Value config;
  char configFilepath[1024];
  strcpy(configFilepath, string(configLocation + "/config.json").c_str());
  parseJSONFile(configFilepath, config);

  // Check with different values
  config["numHiers"] = 1;
  config["feat_type"] = 1;
  EXPECT_EQ(900, getFeatSize(config, 0, 0));
  EXPECT_EQ(1635, getFeatSize(config, 1, 0));
  EXPECT_EQ(1635, getFeatSize(config, 2, 0));
  config["context_width"] = 10;
  EXPECT_EQ(900, getFeatSize(config, 0, 0));
  EXPECT_EQ(1635, getFeatSize(config, 1, 0));
  config["hog_variant"] = 2;
  EXPECT_EQ(775, getFeatSize(config, 0, 0));
  EXPECT_EQ(1510, getFeatSize(config, 1, 0));
  config["numHiers"] = 2;
  EXPECT_EQ(775, getFeatSize(config, 0, 0));
  EXPECT_EQ(1804, getFeatSize(config, 1, 0));
  EXPECT_EQ(1804, getFeatSize(config, 1, 1));


  config["feat_type"] = 2;
  EXPECT_EQ(5*5*13, getFeatSize(config, 0, 0));
  EXPECT_EQ(5*5*13+1804-775, getFeatSize(config, 1, 0));
  EXPECT_EQ(5*5*13+1804-775, getFeatSize(config, 2, 0));

}

// getNumSamples
TEST(UtilTest, getNumSamples)
{
  Json::Value config;
  char configFilepath[1024];
  strcpy(configFilepath, string(configLocation + "/config.json").c_str());
  parseJSONFile(configFilepath, config);
  int stride, hier = 0;
  stride  = config["stride"].get(hier, 0).asInt();
  cout << "stride = " << stride << endl;
  Size sz1(100, 100);
  EXPECT_EQ(100*100/stride/stride, getNumSamples(config, sz1, 0));
  hier = 1;
  stride  = config["stride"].get(hier, 0).asInt();
  Size sz2(200, 40);
  EXPECT_EQ(200*40/stride/stride, getNumSamples(config, sz2, 0));

}


// vectorToPointer
TEST(UtilTest, vectorToPointer)
{
	for (int count = 0; count < 100; count++)
	{
		int outside_dim = 1037;
		int inside_dim = 73;
		srand(100 + count);
		vector<vector<prec_type>*> vec;
		for (int i = 0; i < outside_dim; i++)
		{
			vector<prec_type>* v = new vector<prec_type>();
			for (int j = 0; j < inside_dim; j++)
			{
				v->push_back((prec_type)rand() / RAND_MAX);
			}
			vec.push_back(v);
		}
		prec_type* ptr = vectorToPointer(vec, outside_dim, inside_dim);
		for (int i = 0; i < outside_dim; i++)
		{
			for (int j = 0; j < inside_dim; j++)
			{
				EXPECT_EQ((*vec[i])[j], ptr[i*inside_dim + j]);
			}
		}
		delete ptr;
	}
}

// copyPointerToVector
TEST(UtilTest, copyPointerToVector)
{
	for (int count = 0; count < 100; count++)
	{
		int outside_dim = 1129;
		int inside_dim = 52;
		srand(1000 + count);
		vector<vector<prec_type> > vec1;
		vector<vector<prec_type>*> vec2;
		prec_type* ptr = new prec_type[outside_dim*inside_dim];
		for (int i = 0; i < outside_dim; i++)
		{
			for (int j = 0; j < inside_dim; j++)
			{
				ptr[i*inside_dim + j] = (prec_type)rand() / RAND_MAX;
			}
		}
		copyPointerToVector(vec1, ptr, outside_dim, inside_dim);
		copyPointerToVector(vec2, ptr, outside_dim, inside_dim);
		for (int i = 0; i < outside_dim; i++)
		{
			for (int j = 0; j < inside_dim; j++)
			{
				EXPECT_EQ(vec1[i][j], ptr[i*inside_dim + j]);
				EXPECT_EQ((*vec2[i])[j], ptr[i*inside_dim + j]);
			}
		}
		delete ptr;
		for (int i = 0; i < outside_dim; i++)
		{
			delete vec2[i];
		}
	}
}

// transferToVectorAndDelete
TEST(UtilTest, transferToVectorAndDelete)
{
	Json::Value config;
	char configFilepath[1024];
	strcpy(configFilepath, string(configLocation + "/config.json").c_str());
	parseJSONFile(configFilepath, config);
	Size imsz(640, 480);
	int stage = 1;
	int numHiers = config.get("numHiers", 0).asInt();
	int numStages = config.get("stages", 0).asInt();
	srand(1000);
	vector<vector<vector<prec_type>*> > Fv;
	vector<vector<vector<vector<prec_type> > > > Yv;
	vector<prec_type*> F_ptr;
	vector<vector<prec_type*> > Y_ptr;
	vector<prec_type*> F_ptr_copy;
	vector<vector<prec_type*> > Y_ptr_copy;
	for (int hier = 0; hier < numHiers; hier++)
	{
		int numSamples = getNumSamples(config, imsz, hier);
		int numFeats = getFeatSize(config, stage, hier);
		int numLabels = config["numLabels"].get(hier, 0).asInt();
		prec_type* F_ptr_h = new prec_type[numSamples*numFeats];
		prec_type* F_ptr_h_copy = new prec_type[numSamples*numFeats];
		prec_type randval;
		for (int i = 0; i < numSamples; i++)
		{
			for (int j = 0; j < numFeats; j++)
			{
				randval = (prec_type)rand() / RAND_MAX;
				F_ptr_h[i*numFeats + j] = randval;
				F_ptr_h_copy[i*numFeats + j] = randval;
			}
		}
		F_ptr.push_back(F_ptr_h);
		F_ptr_copy.push_back(F_ptr_h_copy);
	}
	for (int s = 0; s < numStages; s++)
	{

		vector<prec_type*> Y_ptr_s;
		vector<prec_type*> Y_ptr_s_copy;
		for (int hier = 0; hier < numHiers; hier++)
		{
			int numSamples = getNumSamples(config, imsz, hier);
			int numLabels = config["numLabels"].get(hier, 0).asInt();
			prec_type* Y_ptr_h = new prec_type[numSamples*numLabels];
			prec_type* Y_ptr_h_copy = new prec_type[numSamples*numLabels];
			prec_type randval;
			for (int i = 0; i < numSamples; i++)
			{
				for (int j = 0; j < numLabels; j++)
				{
					randval = (prec_type)rand() / RAND_MAX;
					Y_ptr_h[i*numLabels + j] = randval;
					Y_ptr_h_copy[i*numLabels + j] = randval;
				}
			}
			Y_ptr_s.push_back(Y_ptr_h);
			Y_ptr_s_copy.push_back(Y_ptr_h_copy);
		}
		Y_ptr.push_back(Y_ptr_s);
		Y_ptr_copy.push_back(Y_ptr_s_copy);
	}
	transferToVectorAndDelete(F_ptr, Fv, config, imsz, stage);
	transferToVectorAndDelete(Y_ptr, Yv, config, imsz, numStages);

	// Check
	for (int i = 0; i < Fv.size(); i++)
	{
		for (int j = 0; j < Fv[i].size(); j++)
		{
			for (int k = 0; k < Fv[i][j]->size(); k++)
			{
				EXPECT_EQ((*Fv[i][j])[k], F_ptr_copy[i][j*Fv[i][j]->size() + k]);
			}
		}
	}
	for (int i = 0; i < Yv.size(); i++)
	{
		for (int j = 0; j < Yv[i].size(); j++)
		{
			for (int k = 0; k < Yv[i][j].size(); k++)
			{
				for (int m = 0; m < Yv[i][j][k].size(); m++)
				{
					EXPECT_EQ(Yv[i][j][k][m], Y_ptr_copy[i][j][k*Yv[i][j][k].size() + m]);
				}
			}
		}
	}

	// Delete
	for (int i = 0; i < Fv.size(); i++)
	{
		for (int j = 0; j < Fv[i].size(); j++)
		{
			delete Fv[i][j];
		}
	}

	for (int i = 0; i < F_ptr_copy.size(); i++)
	{
		delete F_ptr_copy[i];
	}
	for (int i = 0; i < Y_ptr_copy.size(); i++)
	{
		for (int j = 0; j < Y_ptr_copy[i].size(); j++)
		{
			delete Y_ptr_copy[i][j];
		}
	}
}

// maxPooling
TEST(UtilTest, maxPooling)
{

	Size sz1(100, 100);
	Mat mati1(sz1, CV_32FC1, Scalar(0));
	Mat mato1 = maxPooling(mati1, 8);
	Size szo1 = mato1.size();
	EXPECT_EQ(sz1.width, szo1.width);
	EXPECT_EQ(sz1.height, szo1.height);
	for (int x = 0; x < sz1.width; x++)
	{
		for (int y = 0; y < sz1.height; y++)
		{
			EXPECT_EQ(0, mato1.at<float>(y, x));
		}
	}
	Size sz2(164, 122);
	Mat mati2(sz2, CV_32FC1, Scalar(0));
	for (int x = 0; x < sz2.width; x += 7)
	{
		for (int y = 0; y < sz2.height; y += 7)
		{
			mati2.at<float>(y, x) = 10;
		}
	}
	Mat mato2 = maxPooling(mati2, 7);
	Size szo2 = mato2.size();
	EXPECT_EQ(sz2.width, szo2.width);
	EXPECT_EQ(sz2.height, szo2.height);
	for (int x = 0; x < sz2.width; x++)
	{
		for (int y = 0; y < sz2.height; y++)
		{
			EXPECT_EQ(10, mato2.at<float>(y, x));
		}
	}
	Size sz3(164, 223);
	Mat mati3(sz3, CV_32FC1, Scalar(-1));
	for (int x = 0; x < sz3.width; x += 3)
	{
		for (int y = 0; y < sz3.height; y += 3)
		{
			mati3.at<float>(y, x) = -1.5;
		}
	}
	Mat mato3 = maxPooling(mati3, 6);
	Size szo3 = mato3.size();
	EXPECT_EQ(sz3.width, szo3.width);
	EXPECT_EQ(sz3.height, szo3.height);
	for (int x = 0; x < sz3.width; x++)
	{
		for (int y = 0; y < sz3.height; y++)
		{
			EXPECT_EQ(-1, mato3.at<float>(y, x));
		}
	}

	// TODO Test with MATLAB generated data
}

// roundup
TEST(UtilTest, roundup)
{
	EXPECT_EQ(5, roundup(5.0));
	EXPECT_EQ(0, roundup(-0.4));
	EXPECT_EQ(100, roundup(99.999999999));
	EXPECT_EQ(19, roundup(19.0000000001));
	EXPECT_EQ(-30, roundup(-30.5));
}

// max
TEST(UtilTest, max)
{
	EXPECT_EQ(100, max(100, 99));
	EXPECT_EQ(-23, max(-99, -23));
	EXPECT_EQ(50, max(49.99999, 50.0));
	EXPECT_EQ(-10, max(-12.1, -10.0000));
}

// min
TEST(UtilTest, min)
{
	EXPECT_EQ(99, min(100, 99));
	EXPECT_EQ(-99, min(-99, -23));
	EXPECT_EQ(49.99999, min(49.99999, 50.0));
	EXPECT_EQ(-12.1, min(-12.1, -10.0000));
}

// median
TEST(UtilTest, median)
{
	vector<double> v1(100, 0);
	for (int i = 0; i < 100; i++)
	{
		v1[i] = i;
	}
	EXPECT_EQ(49.5, median(v1));
	vector<double> v2(214, -34.5);
	EXPECT_EQ(-34.5, median(v2));
	vector<double> v3(91, 0);
	for (int i = 0; i < 91; i++)
	{
		v3[i] = 91-i;
	}
	EXPECT_EQ(46, median(v3));
}

/*---------------------------------
  ------------ features -------------
  -----------------------------------*/
// computeHOGFeatures
TEST(FeaturesTest, computeImageFeatures)
{
  // Set config data
  Json::Value config;
  char configFilepath[1024];
  strcpy(configFilepath, string(configLocation + "/configMATLABeq.json").c_str());
  parseJSONFile(configFilepath, config);

  int numHiers = config.get("numHiers",0).asInt();
  Json::Value patch_size = config["patch_size"];
  Json::Value sbin = config["sbin"];
	Json::Value stride = config["stride"];
	Mat img = imread(configLocation + "im0001.jpg", CV_LOAD_IMAGE_COLOR);
	img = cropToBinMultiple(img, config);

	vector<vector<vector<prec_type>* > > F;
	// Load MATLAB computed features -- only checks Piotr features variant.
  for (int hier = 0; hier < numHiers; ++hier)
	{
		vector<vector<prec_type>* > F_h;
		int w = patch_size.get(hier, 0).asInt();
		F_h = computeImageFeatures(img, config, sbin.get(hier, 0).asInt(), stride.get(hier, 0).asInt(), w, -1);
		F.push_back(F_h);
	}
	writeFToFile(F, "config/unitTest/test_F");
	// // Assert that they are equal
	// computeNormDiff(Fmatlab, F);
	// double diff(0.0);
	// EXPECT_LT(1e-6, diff);
}

// computeFeatures
TEST(FeaturesTest, computeFeatures)
{



	// TODO - figure out some reasonable way of testing this (not sure how exactly)
	//computeFeatures(vector<vector<vector<prec_type>* > >&F, Mat& img, const PoseMachine& poseMachine, const Json::Value& config, const Json::Value& sbin, const Size imsz, const int stage, int split, bool debug_flag);
}

// appendContextFeatures
TEST(FeaturesTest, appendContextFeatures)
{
	// TODO - test with MATLAB data
	// Set config data
  Json::Value config;
  char configFilepath[1024];
  strcpy(configFilepath, string(configLocation + "/configMATLABeq.json").c_str());
  parseJSONFile(configFilepath, config);

	//appendContextFeatures(vector<vector<vector<prec_type>* > >& F, vector<vector<vector<Mat*> > >& scoremaps, vector<vector<vector<vector<prec_type> > > >& Y_out, const Json::Value config, const Size imsz, const PoseMachine& poseMachine, const int split, const int stage);
}

// appendContextFeaturesMining
TEST(FeaturesTest, appendContextFeaturesMining)
{
	// TODO -test with MATLAB data

	//appendContextFeaturesMining(vector<vector<vector<prec_type>* > >& F, vector<vector<vector<Mat*> > >& scoremaps, vector<vector<vector<vector<prec_type> > > >& Y_out, const Json::Value config, const Size imsz, const PoseMachine& poseMachine, const int split, const int stage, int no_context_flag);
}

// appendPatchFeatures
TEST(FeaturesTest, extractPatchFeatures)
{
	// TODO - test with MATLAB data

	//extractPatchFeatures(vector<vector<vecotr<prec_type>* > >& F, const vector<vector<Mat*> > & scoremaps, const Json::Value& config, const Size imsz);
}

/*---------------------------------
------------ HDFUtil --------------
-----------------------------------*/
// Read sizes
TEST(HDFTest, testReadSizes)
{
	int o_num_samples = 1000;
	int o_dim_x = 80;
	int o_dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  cout << "location: " << hdfFilename << endl;
 	remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, o_num_samples, o_dim_x, o_dim_y);
  int num_samples, dim_x, dim_y;
  readSizeXHDF(hdfFilename, num_samples, dim_x);
  EXPECT_EQ(num_samples, o_num_samples);
  EXPECT_EQ(dim_x, o_dim_x);
	readSizeYHDF(hdfFilename, num_samples, dim_y);
	EXPECT_EQ(num_samples, o_num_samples);
  EXPECT_EQ(dim_y, o_dim_y);
}

// Test X read and write
TEST(HDFTest, Xrw)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
 
  // Come up with X data
  vector<const vector<float>*> X_data;
  for (int i = 0; i < num_samples; i++)
  {
  	vector<float>* xrow = new vector<float>();
  	for (int j = 0; j < dim_x; j++)
  	{
  		xrow->push_back((float)rand() / RAND_MAX);
  	}
  	X_data.push_back(xrow);
  }
	writeDataXHDF(hdfFilename, X_data);
	
	// Compare with read
	vector<const vector<float>*> X_comp;
	readDataXHDF(hdfFilename, X_comp);
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < dim_x; j++)
		{
			EXPECT_EQ((*X_data[i])[j], (*X_comp[i])[j]);
		}
	}
	
	// Clean up
	for (int i = 0; i < num_samples; i++)
	{
		delete X_data[i];
		delete X_comp[i];
	}
}

// Test Y read and write
TEST(HDFTest, Xqrw)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
 
  // Come up with X data
  vector<vector<int>*> Xq_data;
  for (int i = 0; i < num_samples; i++)
  {
  	vector<int>* xqrow = new vector<int>();
  	for (int j = 0; j < dim_x; j++)
  	{
  		xqrow->push_back(rand());
  	}
  	Xq_data.push_back(xqrow);
  }
	writeDataXqHDF(hdfFilename, Xq_data);
	
	// Compare with read
	vector<vector<int>*> Xq_comp;
	readDataXqHDF(hdfFilename, Xq_comp);
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < dim_x; j++)
		{
			EXPECT_EQ((*Xq_data[i])[j], (*Xq_comp[i])[j]);
		}
	}
	
	// Clean up
	for (int i = 0; i < num_samples; i++)
	{
		delete Xq_data[i];
		delete Xq_comp[i];
	}
}

// Test Y read and write
TEST(HDFTest, Yrw)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
 
  // Come up with Y data
  vector<const vector<float>*> Y_data;
  for (int i = 0; i < num_samples; i++)
  {
  	vector<float>* yrow = new vector<float>();
  	for (int j = 0; j < dim_y; j++)
  	{
  		yrow->push_back((float)rand() / RAND_MAX);
  	}
  	Y_data.push_back(yrow);
  }
	writeDataYHDF(hdfFilename, Y_data);
	
	// Compare with read
	vector<const vector<float>*> Y_comp;
	readDataYHDF(hdfFilename, Y_comp);
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < dim_y; j++)
		{
			EXPECT_EQ((*Y_data[i])[j], (*Y_comp[i])[j]);
		}
	}
	
	// Clean up
	for (int i = 0; i < num_samples; i++)
	{
		delete Y_data[i];
		delete Y_comp[i];
	}
}

// Test yint read and write
TEST(HDFTest, yintrw)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
 
  // Come up with yint data
  vector<int> yint_data;
  for (int i = 0; i < num_samples; i++)
  {
  	yint_data.push_back(rand());
  }
	writeDatayintHDF(hdfFilename, yint_data);
	
	// Compare with read
	vector<int> yint_comp;
	readDatayintHDF(hdfFilename, yint_comp);
	for (int i = 0; i < num_samples; i++)
	{
		EXPECT_EQ(yint_data[i], yint_comp[i]);
	}
}

// Test w read and write
TEST(HDFTest, wrw)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
 
  // Come up with w data
  vector<double> w_data;
  for (int i = 0; i < num_samples; i++)
  {
  	w_data.push_back((double)rand() / RAND_MAX);
  }
	writeDatawHDF(hdfFilename, w_data);
	
	// Compare with read
	vector<double> w_comp;
	readDatawHDF(hdfFilename, w_comp);
	for (int i = 0; i < num_samples; i++)
	{
		EXPECT_EQ(w_data[i], w_comp[i]);
	}
}

// Test F read and write
TEST(HDFTest, Frw)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
 
  // Come up with F data
  vector<vector<float> > F_data;
  for (int i = 0; i < num_samples; i++)
  {
  	vector<float> frow;
  	for (int j = 0; j < dim_y; j++)
  	{
  		frow.push_back((float)rand() / RAND_MAX);
  	}
  	F_data.push_back(frow);
  }
	writeDataFHDF(hdfFilename, F_data);
	
	// Compare with read
	vector<vector<float> > F_comp;
	readDataFHDF(hdfFilename, F_comp);
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < dim_y; j++)
		{
			EXPECT_EQ(F_data[i][j], F_comp[i][j]);
		}
	}
}

// Test H read and write
TEST(HDFTest, Hrw)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
 
  // Come up with H data
  vector<vector<float> > H_data;
  for (int i = 0; i < num_samples; i++)
  {
  	vector<float> hrow;
  	for (int j = 0; j < dim_y; j++)
  	{
  		hrow.push_back((float)rand() / RAND_MAX);
  	}
  	H_data.push_back(hrow);
  }
	writeDataHHDF(hdfFilename, H_data);
	
	// Compare with read
	vector<vector<float> > H_comp;
	readDataHHDF(hdfFilename, H_comp);
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < dim_y; j++)
		{
			EXPECT_EQ(H_data[i][j], H_comp[i][j]);
		}
	}
}

// Test cc read and write
TEST(HDFTest, ccrw)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
 
  // Come up with cc data
  vector<double> cc_data;
  for (int i = 0; i < num_samples; i++)
  {
  	cc_data.push_back((double)rand() / RAND_MAX);
  }
	writeDataccHDF(hdfFilename, cc_data);
	
	// Compare with read
	vector<double> cc_comp;
	readDataccHDF(hdfFilename, cc_comp);
	for (int i = 0; i < num_samples; i++)
	{
		EXPECT_EQ(cc_data[i], cc_comp[i]);
	}
}

// Test w0 read and write
TEST(HDFTest, w0rw)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
 
  // Come up with w0 data
  vector<double> w0_data;
  for (int i = 0; i < num_samples; i++)
  {
  	w0_data.push_back((float)rand() / RAND_MAX);
  }
	writeDataw0HDF(hdfFilename, w0_data);
	
	// Compare with read
	vector<double> w0_comp;
	readDataw0HDF(hdfFilename, w0_comp);
	for (int i = 0; i < num_samples; i++)
	{
		EXPECT_EQ(w0_data[i], w0_comp[i]);
	}
}

// Test wl read and write
TEST(HDFTest, wlrw)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
 
  // Come up with wl data
  vector<double> wl_data;
  for (int i = 0; i < num_samples; i++)
  {
  	wl_data.push_back((double)rand() / RAND_MAX);
  }
	writeDatawlHDF(hdfFilename, wl_data);
	
	// Compare with read
	vector<double> wl_comp;
	readDatawlHDF(hdfFilename, wl_comp);
	for (int i = 0; i < num_samples; i++)
	{
		EXPECT_EQ(wl_data[i], wl_comp[i]);
	}
}

// Test writeAllwHDF
TEST(HDFTest, writeAllwHDF)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
 
  // Come up with w value
  double w_val = (double)rand() / RAND_MAX;
  writeAllwHDF(hdfFilename, w_val, 0, num_samples, 10);
	
	// Read and compare
	vector<double> w_comp;
	readDatawHDF(hdfFilename, w_comp);
	for (int i = 0; i < num_samples; i++)
	{
		EXPECT_EQ(w_val, w_comp[i]);
	}
}

// Test w, w0, wl accumulate
TEST(HDFTest, sumwValues)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
 
  // Come up with w data
  vector<double> w_data;
  for (int i = 0; i < num_samples; i++)
  {
  	w_data.push_back((float)rand() / RAND_MAX);
  }
	writeDatawHDF(hdfFilename, w_data);
	
	// Come up with w0 data
  vector<double> w0_data;
  for (int i = 0; i < num_samples; i++)
  {
  	w0_data.push_back((double)rand() / RAND_MAX);
  }
	writeDataw0HDF(hdfFilename, w0_data);
	
	// Come up with wl data
  vector<double> wl_data;
  for (int i = 0; i < num_samples; i++)
  {
  	wl_data.push_back((double)rand() / RAND_MAX);
  }
	writeDatawlHDF(hdfFilename, wl_data);
	
	// Calculate sums
	double w_comp_sum = 0;
	double w0_comp_sum = 0;
	double wl_comp_sum = 0;
	for (int i = 0; i < num_samples; i++)
  {
  	w_comp_sum += w_data[i];
  	w0_comp_sum += w0_data[i];
  	wl_comp_sum += wl_data[i];
  }
  	
  // Compare with read w
	double sum = sumwValues(hdfFilename, 0, num_samples, 10);
	EXPECT_EQ(sum, w_comp_sum);
	
	// Compare with read w0
	double sum0 = sumw0Values(hdfFilename, 0, num_samples, 10);
	EXPECT_EQ(sum0, w0_comp_sum);
	
	// Compare with read wl
	double suml = sumwlValues(hdfFilename, 0, num_samples, 10);
	EXPECT_EQ(suml, wl_comp_sum);
}

// Test getClassCounts
TEST(HDFTest, getClassCounts)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
  
  // Come up with yint data
  vector<int> y_int;
  for (int i = 0; i < num_samples; i++)
  {
  	y_int.push_back(rand() % dim_y);
  }
	writeDatayintHDF(hdfFilename, y_int);

	// Manually calculate class counts
	vector<double> vec_class_counts_comp(dim_y, 0.0);
	for (int i = 0 ; i < num_samples ; i++)
	{
		vec_class_counts_comp[y_int[i]]++;
	}

	// Use function
	vector<double> vec_class_counts = getClassCounts(hdfFilename, 0, num_samples, 10, dim_y);

	// Compare values
	EXPECT_EQ(vec_class_counts.size(), vec_class_counts_comp.size());
	for (int i = 0; i < vec_class_counts.size(); i++)
	{
		EXPECT_EQ(vec_class_counts[i], vec_class_counts_comp[i]);
	}
}

// Test getClassCounts
TEST(HDFTest, getClassCountsInt)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
  
  // Come up with yint data
  vector<int> y_int;
  for (int i = 0; i < num_samples; i++)
  {
  	y_int.push_back(rand() % dim_y);
  }
	writeDatayintHDF(hdfFilename, y_int);
	
	// Convert to Y_train
	vector<const vector<float>*> Y_train;
	for (int i = 0; i < num_samples; i++)
	{
		vector<float>* yrow = new vector<float>(dim_y, 0);
		(*yrow)[y_int[i]] = 1;
		Y_train.push_back(yrow);
	}
	writeDataYHDF(hdfFilename, Y_train);
	for (int i = 0; i < num_samples; i++)
	{
		delete Y_train[i];
	}
	

	// Manually calculate class counts
	vector<int> vec_class_counts_comp(dim_y, 0);
	for (int i = 0 ; i < num_samples ; i++)
	{
		vec_class_counts_comp[y_int[i]]++;
	}

	// Use function
	vector<int> vec_class_counts = getClassCountsInt(hdfFilename, 0, num_samples, 10, dim_y);

	// Compare values
	EXPECT_EQ(vec_class_counts.size(), vec_class_counts_comp.size());
	for (int i = 0; i < vec_class_counts.size(); i++)
	{
		EXPECT_EQ(vec_class_counts[i], vec_class_counts_comp[i]);
	}
}

// Test calculateInnerProduct
TEST(HDFTest, calculateInnerProduct)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
  strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
  remove(hdfFilename);
  CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
   
  // Come up with data
  vector<vector<float> > H_data;
  vector<vector<float> > F_data;
  vector<double> w_data;
  for (int i = 0; i < num_samples; i++)
  {
  	vector<float> hrow;
  	vector<float> frow;
  	for (int j = 0; j < dim_y; j++)
  	{
  		hrow.push_back((float)rand() / RAND_MAX);
  		frow.push_back((float)rand() / RAND_MAX);
  	}
  	w_data.push_back((float)rand() / RAND_MAX);
  	H_data.push_back(hrow);
  	F_data.push_back(frow);
  }
	writeDataHHDF(hdfFilename, H_data);
	writeDataFHDF(hdfFilename, F_data);
	writeDatawHDF(hdfFilename, w_data);
  
  // Use function
  double inner_product, h_norm_sq;	
	calculateInnerProduct(hdfFilename, 0, num_samples, 10, inner_product, h_norm_sq);

	// Manually calculate inner product
	double inner_product_comp = 0.0;
	double h_norm_sq_comp = 0.0;
	for (int i = 0; i < num_samples; i++)
	{
		const double w_i = w_data[i];
		const vector<prec_type>& h_i = H_data[i];
		const vector<prec_type>& r_i = F_data[i];
		// <h, residuals>
		inner_product_comp += w_i * std::inner_product(h_i.begin(), h_i.end(), r_i.begin(), 0.0);
		// ||h||^2
		h_norm_sq_comp += w_i * std::inner_product(h_i.begin(), h_i.end(), h_i.begin(), 0.0);
	}
	
	// Compare values
	EXPECT_EQ(inner_product, inner_product_comp);
	EXPECT_EQ(h_norm_sq, h_norm_sq_comp);
}

// Test updateResiduals
TEST(HDFTest, updateResiduals)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
	strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
	remove(hdfFilename);
	CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
   
	// Come up with data
	vector<vector<float> > H_data;
	vector<vector<float> > F_data;
	for (int i = 0; i < num_samples; i++)
	{
		vector<float> hrow;
		vector<float> frow;
		for (int j = 0; j < dim_y; j++)
		{
			hrow.push_back((float)rand() / RAND_MAX);
			frow.push_back((float)rand() / RAND_MAX);
		}
		H_data.push_back(hrow);
		F_data.push_back(frow);
	}
	writeDataHHDF(hdfFilename, H_data);
	writeDataFHDF(hdfFilename, F_data);

	// Use function
	double scalar_project = 0.5;
	updateResiduals(hdfFilename, 0, num_samples, 10, scalar_project);

	// Read new H
	vector<vector<float> > F_comp;
	readDataFHDF(hdfFilename, F_comp);

	// Update Residuals
	for (int i = 0; i < num_samples; i++)
	{
		vector<prec_type>& r_i = F_data[i];
		const vector<prec_type>& h_i = H_data[i];
		for (int k = 0; k < dim_y; k++)
		{
			r_i[k] -= scalar_project * h_i[k];
		}
	}
		
	// Compare values
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < dim_y; j++)
		{
			EXPECT_EQ(F_data[i][j], F_comp[i][j]);
		}
	}
}

// Test updateResiduals
TEST(HDFTest, setFZero)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
	strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
	remove(hdfFilename);
	CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
   
	// Come up with fake data and write it
	vector<vector<float> > F_data;
	for (int i = 0; i < num_samples; i++)
	{
		vector<float> frow;
		for (int j = 0; j < dim_y; j++)
		{
			frow.push_back((float)rand() / RAND_MAX);
		}
		F_data.push_back(frow);
	}
	writeDataFHDF(hdfFilename, F_data);

	// Use function to clear it
	setFZero(hdfFilename, 0, num_samples, 10);

	// Read new F
	vector<vector<float> > F_comp;
	readDataFHDF(hdfFilename, F_comp);
		
	// Compare values
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < dim_y; j++)
		{
			EXPECT_EQ(0, F_comp[i][j]);
		}
	}
}

// Test quantizeX
TEST(HDFTest, quantizeX)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
	strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
	remove(hdfFilename);
	CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
   
	// Come up with X data and write it
	vector<const vector<float>*> X_data;
	for (int i = 0; i < num_samples; i++)
	{
		vector<float>* xrow = new vector<float>();
		for (int j = 0; j < dim_x; j++)
		{
			xrow->push_back((float)rand() / RAND_MAX);
		}
		X_data.push_back(xrow);
	}
	writeDataXHDF(hdfFilename, X_data);
	
	// Use function to calculate Xq
	vector<vector<pair<double, double> > > allBins_dummy;
	quantizeX(hdfFilename, allBins_dummy, 0, num_samples, 10, 20, false, 0.001, 1);

	// Get first part of X
	vector<const vector<float>*> X_data_partial;
	for (int i = 0; i < num_samples / 10; i++)
	{
		X_data_partial.push_back(X_data[i]);
	}	
	
	// Make fsq
	vector<vector<pair<double, double> > > allBins;
	fq::FeatureSpaceQuanztizer quantizer = make_fsq(X_data_partial, 20, false, 0.001, 1);
	
	// Now get full X_quantized
	vector<vector<int>*> Xq_comp;
	get_quantized(quantizer, X_data, Xq_comp, allBins);
	
	// Read Xq
	vector<vector<int>*> Xq_data;
	readDataXqHDF(hdfFilename, Xq_data);
		
	// Compare values
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < dim_x; j++)
		{
			//EXPECT_EQ((*Xq_data[i])[j], (*Xq_comp[i])[j]);
			// Note, I think this value is randomized so we can't compare directly like this
		}
	}
	
	// Clean up
	for (int i = 0; i < num_samples; i++)
	{
		delete X_data[i];
		delete Xq_data[i];
		delete Xq_comp[i];
	}
}

// Test convertYtoInt
TEST(HDFTest, convertYtoInt)
{
	int num_samples = 1000;
	int dim_x = 80;
	int dim_y = 16;
	char hdfFilename[1024];
	strcpy(hdfFilename, string(configLocation + "/testHDF.h5").c_str());
	remove(hdfFilename);
	CreateTrainingDataFileHDF(hdfFilename, num_samples, dim_x, dim_y);
   
	// Come up with Y data and write it
	vector<const vector<float>*> Y_data;
	for (int i = 0; i < num_samples; i++)
	{
		// Come up with random value to use
		int yval = rand() % dim_y;
	
		// Set data
		vector<float>* yrow = new vector<float>();
		for (int j = 0; j < dim_y; j++)
		{
			if (j == yval)
			{
				yrow->push_back(1.0);
			}
			else
			{
				yrow->push_back(0.0);
			}
		}
		Y_data.push_back(yrow);
	}
	writeDataYHDF(hdfFilename, Y_data);
	
	// Use convert yint function
	convertYtoInt(hdfFilename, 0, num_samples, 10);
	
	// Calculate Y_int manually
	vector<int> Y_int_comp;
	for (int i = 0; i < Y_data.size(); i++)
	{
		int label = -1;
		for (int j = 0; j < dim_y; j++) 
		{
			if(Y_data[i]->at(j)) 
			{
				label = j;
			}
		}
		Y_int_comp.push_back(label);
		assert(Y_int_comp[i] == argmax(*(Y_data[i])));
	}
	
	// Read yint
	vector<int> Y_int;
	readDatayintHDF(hdfFilename, Y_int);
		
	// Compare values
	for (int i = 0; i < num_samples; i++)
	{
		assert(Y_int[i] == Y_int_comp[i]);
	}
	
	// Clean up
	for (int i = 0; i < num_samples; i++)
	{
		delete Y_data[i];
	}
}

/*---------------------------------
---------- learning ---------------
-----------------------------------*/

// Note - you need to change the directories for anno.json in unitTest/ for these to work correctly

// Test extractFeatures
TEST(Learning, extractFeatures)
{
	//void extractFeatures(TrData& trData, std::vector<std::vector<prec_type>* >& F, const int hier, const Json::Value &anno, const Json::Value &config, const cv::Size imsz, std::vector<int>& annoids, const double scale, const int split);
}

// analyzeTrainingData
TEST(Learning, analyzeTrainingData)
{
	//void analyzeTrainingData(TrData& trData, const Json::Value &config);
}

// computePositiveFeatures
TEST(Learning, computePositiveFeatures)
{
	// Delete all hdf files
	vector<string> validHDFExtensions;
	validHDFExtensions.push_back("h5");
	vector<string> hdffiles;
	getFilesInDirectory(string(configLocation + "/positiveImgs/"), hdffiles, validHDFExtensions);
	for (int i = 0; i < hdffiles.size(); i++)
	{
		char hdffile[1024];
		strcpy(hdffile, hdffiles[i].c_str());
		remove(hdffile);
	}

	// Get config and annotation files 
	Json::Value config;
  Json::Value anno;
  char configFilepath[1024];
  char annoFilepath[1024];
  strcpy(configFilepath, string(configLocation + "/config.json").c_str());
  strcpy(annoFilepath, string(configLocation + "/annoTest.json").c_str());
  parseJSONFile(configFilepath, config);
  parseJSONFile(annoFilepath, anno);
  
  // Set folder locations for debug
  char posFilePath[1024];
	strcpy(posFilePath, string(configLocation + "/positiveImgs/").c_str());
	char negFilePath[1024];
	strcpy(negFilePath, string(configLocation + "negativeImgs/").c_str());
  config["trainfolder"] = posFilePath;
  config["negfolder"] = negFilePath;
  config["out_directory"] = configLocation;
  
	// Get some useful values
	static int num_stages = config.get("stages", 1).asInt();
	static int num_splits = config.get("splits", 0).asInt();
	static int numHiers = config.get("numHiers",0).asInt();
	static int classifierType = config.get("classifier",0).asInt();
	
	// Initialize poseMachine
	PoseMachine poseMachine(num_stages, num_splits, numHiers, classifierType);
	
	// Get the positive file names
	vector<string> posFileNames;
	vector<string> validExtensions;
	validExtensions.push_back("jpg");
	validExtensions.push_back("png");
	getFilesInDirectory(string(posFilePath), posFileNames, validExtensions);
	
	//for (int stage = 0; stage < num_stages; stage++)
	int stage = 0;
	{
		// Create Data
		TrData trData(config, anno, stage);
		
		createTrainingData(trData,
			config,
			anno,
			string(negFilePath),
			poseMachine,
			stage);
		
		// Compute Positive Features
		//computePositiveFeatures(trData, posFileNames, config, anno,poseMachine, stage);
	}
		
	//void computePositiveFeatures(TrData& trData, std::vector<std::string>& posFileNames, const Json::Value &config, const Json::Value &anno, const PoseMachine& poseMachine, int stage);
}

// computeNegativeFeatures	
TEST(Learning, computeNegativeFeatures)
{
	//void computeNegativeFeatures(TrData& trData, std::vector<std::string>& negFileNames, const Json::Value &config, const Json::Value &anno, const PoseMachine& poseMachine, int stage);
}

// hardNegativeMining
TEST(Learning, hardNegativeMining)
{
	// Delete all hdf files
	vector<string> validHDFExtensions;
	validHDFExtensions.push_back("h5");
	vector<string> hdffiles;
	getFilesInDirectory(string(configLocation), hdffiles, validHDFExtensions);
	for (int i = 0; i < hdffiles.size(); i++)
	{
		char hdffile[1024];
		strcpy(hdffile, hdffiles[i].c_str());
		remove(hdffile);
	}

	// Get config and annotation files
	Json::Value config;
  Json::Value anno;
  char configFilepath[1024];
  char annoFilepath[1024];
  strcpy(configFilepath, string(configLocation + "/config.json").c_str());
  strcpy(annoFilepath, string(configLocation + "/annoTest.json").c_str());
  parseJSONFile(configFilepath, config);
  parseJSONFile(annoFilepath, anno);
   
  // Set folder locations for debug
  char posFilePath[1024];
	strcpy(posFilePath, string(configLocation + "/positiveImgs/").c_str());
	char negFilePath[1024];
	strcpy(negFilePath, string(configLocation + "negativeImgs/").c_str());
  config["trainfolder"] = posFilePath;
  config["negfolder"] = negFilePath;
  config["out_directory"] = configLocation;
  
	// Get some useful values
	static int num_stages = config.get("stages", 1).asInt();
	static int num_splits = config.get("splits", 0).asInt();
	static int numHiers = config.get("numHiers",0).asInt();
	config["classifier"] = 1;
	static int classifierType = config.get("classifier",0).asInt();
	
	// Get the positive file names
	vector<string> posFileNames;
	vector<string> validExtensions;
	validExtensions.push_back("jpg");
	validExtensions.push_back("png");
	getFilesInDirectory(string(posFilePath), posFileNames, validExtensions);
	
	// Create Data with HDF
	int stage = 0;
	TrData trDataHDF(config, anno, stage);
	PoseMachine poseMachineHDF(1, num_splits, numHiers, classifierType);

	createTrainingData(trDataHDF,
		config,
		anno,
		string(negFilePath),
		poseMachineHDF,
		stage);
		
	hardNegativeMining(trDataHDF, poseMachineHDF, config, anno, stage, true);
	hardNegativeMining(trDataHDF, poseMachineHDF, config, anno, stage, false);
	
	// Now make one without HDF
	config["run_hdf"] = 0; 
	TrData trData(config, anno, stage);
	PoseMachine poseMachine(1, num_splits, numHiers, classifierType);
	
	createTrainingData(trData,
		config,
		anno,
		string(negFilePath),
		poseMachine,
		stage);
 
 	hardNegativeMining(trData, poseMachine, config, anno, stage, true);
	hardNegativeMining(trData, poseMachine, config, anno, stage, false);
	
	// Compare!	
	// Get Data from HDF
	vector<int> data_sizes = trDataHDF.data_sizes;
	vector<vector<const vector<prec_type>*> > X_trainHDF; 
	vector<vector<const vector<prec_type>*> > Y_trainHDF;
	vector<vector<double> > w_trainHDF;
	for (int hier = 0; hier < numHiers; hier++)
	{
		int data_size = data_sizes[hier];
		char* hdffile = trDataHDF.hdf_filenames[hier];
		vector<const vector<prec_type>*> X_train;
		vector<const vector<prec_type>*> Y_train;
		vector<double> w_train;
		readDataXHDF(hdffile, X_train, 0, data_size);
		readDataYHDF(hdffile, Y_train, 0, data_size);
		readDatawHDF(hdffile, w_train, 0, data_size); 
		X_trainHDF.push_back(X_train);
		Y_trainHDF.push_back(Y_train);
		w_trainHDF.push_back(w_train);
	}
	
	// Get data from regular
	vector<vector<const vector<prec_type>* > > X_traincomp = trData.X_train; 
	vector<vector<const vector<prec_type>* > > Y_traincomp = trData.Y_train;
	vector<vector<double> > w_traincomp = trData.w_train;
	
	// Note - These checks are commented out because they won't work without proper seeding in learning.cpp. If you want to check these, uncomment the checks and go to computeNegativeFeatures in learning.cpp and uncomment the srand call
	
	// Check sizes
	EXPECT_EQ(X_trainHDF.size(), numHiers);
	EXPECT_EQ(Y_trainHDF.size(), numHiers);
	EXPECT_EQ(w_trainHDF.size(), numHiers);
	EXPECT_EQ(X_traincomp.size(), numHiers);
	EXPECT_EQ(Y_traincomp.size(), numHiers);
	EXPECT_EQ(w_traincomp.size(), numHiers);
	for (int hier = 0; hier < numHiers; hier++)
	{
		/*EXPECT_EQ(X_trainHDF[hier].size(), data_sizes[hier]);
		EXPECT_EQ(Y_trainHDF[hier].size(), data_sizes[hier]);
		EXPECT_EQ(w_trainHDF[hier].size(), data_sizes[hier]);
		EXPECT_EQ(X_trainHDF[hier].size(), X_traincomp[hier].size());
		EXPECT_EQ(Y_trainHDF[hier].size(), Y_traincomp[hier].size());
		EXPECT_EQ(w_trainHDF[hier].size(), w_traincomp[hier].size());*/
	}
	
	// Compare values
	for (int i = 0; i < X_traincomp.size(); i++)
	{
		for (int j = 0; j < X_traincomp[i].size(); j++)
		{
			for (int k = 0; k < X_traincomp[i][j]->size(); k++)
			{
				//EXPECT_EQ((*X_traincomp[i][j])[k], (*X_trainHDF[i][j])[k]);
			}
			for (int k = 0; k < Y_traincomp[i][j]->size(); k++)
			{
				//EXPECT_EQ((*Y_traincomp[i][j])[k], (*Y_trainHDF[i][j])[k]);
			}
			//EXPECT_EQ(w_traincomp[i][j], w_trainHDF[i][j]);
		}
	}
	
	// Clean up
	for (int hier = 0; hier < numHiers; hier++)
	{
		for (int i = 0; i < X_trainHDF[hier].size(); i++)
		{	
			delete X_trainHDF[hier][i];
			delete Y_trainHDF[hier][i];
		}
	}
}

// createTrainingData
TEST(Learning, createTrainingData)
{
	// Delete all hdf files
	vector<string> validHDFExtensions;
	validHDFExtensions.push_back("h5");
	vector<string> hdffiles;
	getFilesInDirectory(string(configLocation), hdffiles, validHDFExtensions);
	for (int i = 0; i < hdffiles.size(); i++)
	{
		char hdffile[1024];
		strcpy(hdffile, hdffiles[i].c_str());
		remove(hdffile);
	}

	// Get config and annotation files
	Json::Value config;
  Json::Value anno;
  char configFilepath[1024];
  char annoFilepath[1024];
  strcpy(configFilepath, string(configLocation + "/config.json").c_str());
  strcpy(annoFilepath, string(configLocation + "/annoTest.json").c_str());
  parseJSONFile(configFilepath, config);
  parseJSONFile(annoFilepath, anno);
  
  // Set folder locations for debug
  char posFilePath[1024];
	strcpy(posFilePath, string(configLocation + "/positiveImgs/").c_str());
	char negFilePath[1024];
	strcpy(negFilePath, string(configLocation + "negativeImgs/").c_str());
  config["trainfolder"] = posFilePath;
  config["negfolder"] = negFilePath;
  config["out_directory"] = configLocation;
  
	// Get some useful values
	static int num_stages = config.get("stages", 1).asInt();
	static int num_splits = config.get("splits", 0).asInt();
	static int numHiers = config.get("numHiers",0).asInt();
	static int classifierType = config.get("classifier",0).asInt();
	
	// Initialize poseMachine
	PoseMachine poseMachine(num_stages, num_splits, numHiers, classifierType);
	
	// Get the positive file names
	vector<string> posFileNames;
	vector<string> validExtensions;
	validExtensions.push_back("jpg");
	validExtensions.push_back("png");
	getFilesInDirectory(string(posFilePath), posFileNames, validExtensions);
	
	// Create Data with HDF
	int stage = 0;
	TrData trDataHDF(config, anno, stage);

	createTrainingData(trDataHDF,
		config,
		anno,
		string(negFilePath),
		poseMachine,
		stage);
	
	// Now make one without HDF
	config["run_hdf"] = 0;
	TrData trData(config, anno, stage);
	
	createTrainingData(trData,
		config,
		anno,
		string(negFilePath),
		poseMachine,
		stage);
		
	// Compare!
	// Get Data from HDF
	vector<int> data_sizes = trDataHDF.data_sizes;
	vector<vector<const vector<prec_type>*> > X_trainHDF; 
	vector<vector<const vector<prec_type>*> > Y_trainHDF;
	vector<vector<double> > w_trainHDF;
	for (int hier = 0; hier < numHiers; hier++)
	{
		int data_size = data_sizes[hier];
		char* hdffile = trDataHDF.hdf_filenames[hier];
		vector<const vector<prec_type>*> X_train;
		vector<const vector<prec_type>*> Y_train;
		vector<double> w_train;
		readDataXHDF(hdffile, X_train, 0, data_size);
		readDataYHDF(hdffile, Y_train, 0, data_size);
		readDatawHDF(hdffile, w_train, 0, data_size); 
		X_trainHDF.push_back(X_train);
		Y_trainHDF.push_back(Y_train);
		w_trainHDF.push_back(w_train);
	}
	
	// Get data from regular
	vector<vector<const vector<prec_type>* > > X_traincomp = trData.X_train; 
	vector<vector<const vector<prec_type>* > > Y_traincomp = trData.Y_train;
	vector<vector<double> > w_traincomp = trData.w_train;
	
	// Note - These checks are commented out because they won't work without proper seeding in learning.cpp. If you want to check these, uncomment the checks and go to computeNegativeFeatures in learning.cpp and uncomment the srand call
	
	// Check sizes
	EXPECT_EQ(X_trainHDF.size(), numHiers);
	EXPECT_EQ(Y_trainHDF.size(), numHiers);
	EXPECT_EQ(w_trainHDF.size(), numHiers);
	EXPECT_EQ(X_traincomp.size(), numHiers);
	EXPECT_EQ(Y_traincomp.size(), numHiers);
	EXPECT_EQ(w_traincomp.size(), numHiers);
	for (int hier = 0; hier < numHiers; hier++)
	{
		/*EXPECT_EQ(X_trainHDF[hier].size(), data_sizes[hier]);
		EXPECT_EQ(Y_trainHDF[hier].size(), data_sizes[hier]);
		EXPECT_EQ(w_trainHDF[hier].size(), data_sizes[hier]);
		EXPECT_EQ(X_trainHDF[hier].size(), X_traincomp[hier].size());
		EXPECT_EQ(Y_trainHDF[hier].size(), Y_traincomp[hier].size());
		EXPECT_EQ(w_trainHDF[hier].size(), w_traincomp[hier].size());*/
	}
	
	// Compare values
	for (int i = 0; i < X_traincomp.size(); i++)
	{
		for (int j = 0; j < X_traincomp[i].size(); j++)
		{
			for (int k = 0; k < X_traincomp[i][j]->size(); k++)
			{
				//EXPECT_EQ((*X_traincomp[i][j])[k], (*X_trainHDF[i][j])[k]);
			}
			for (int k = 0; k < Y_traincomp[i][j]->size(); k++)
			{
				//EXPECT_EQ((*Y_traincomp[i][j])[k], (*Y_trainHDF[i][j])[k]);
			}
			//EXPECT_EQ(w_traincomp[i][j], w_trainHDF[i][j]);
		}
	}
	
	// Clean up
	for (int hier = 0; hier < numHiers; hier++)
	{
		for (int i = 0; i < X_trainHDF[hier].size(); i++)
		{	
			delete X_trainHDF[hier][i];
			delete Y_trainHDF[hier][i];
		}
	}
}

// getTestData
TEST(Learning, getTestData)
{
	//void getTestData(TestData& testData, const Json::Value& config, const Json::Value& anno, std::string testSamplesDir);
}

// createStackedTrainingData
TEST(Learning, createStackedTrainingData)
{
	// Delete all hdf files
	vector<string> validHDFExtensions;
	validHDFExtensions.push_back("h5");
	vector<string> hdffiles;
	getFilesInDirectory(string(configLocation), hdffiles, validHDFExtensions);
	for (int i = 0; i < hdffiles.size(); i++)
	{
		char hdffile[1024];
		strcpy(hdffile, hdffiles[i].c_str());
		remove(hdffile);
	}

	// Get config and annotation files
	Json::Value config;
  Json::Value anno;
  char configFilepath[1024];
  char annoFilepath[1024];
  strcpy(configFilepath, string(configLocation + "/config.json").c_str());
  strcpy(annoFilepath, string(configLocation + "/annoTest.json").c_str());
  parseJSONFile(configFilepath, config);
  parseJSONFile(annoFilepath, anno);
  
  // Set folder locations for debug
  char posFilePath[1024];
	strcpy(posFilePath, string(configLocation + "/positiveImgs/").c_str());
	char negFilePath[1024];
	strcpy(negFilePath, string(configLocation + "negativeImgs/").c_str());
  config["trainfolder"] = posFilePath;
  config["negfolder"] = negFilePath;
  config["out_directory"] = configLocation;
  config["splits"] = 10;
  
	// Get some useful values
	static int num_stages = config.get("stages", 1).asInt();
	static int num_splits = config.get("splits", 0).asInt();
	static int numHiers = config.get("numHiers",0).asInt();
	static int classifierType = config.get("classifier",0).asInt();
	
	// Initialize poseMachine
	PoseMachine poseMachine(num_stages, num_splits, numHiers, classifierType);
	
	// Get the positive file names
	vector<string> posFileNames;
	vector<string> validExtensions;
	validExtensions.push_back("jpg");
	validExtensions.push_back("png");
	getFilesInDirectory(string(posFilePath), posFileNames, validExtensions);
	
	// Create Data with HDF
	int stage = 0;
	TrData trData(config, anno, stage);

	createTrainingData(trData,
		config,
		anno,
		string(negFilePath),
		poseMachine,
		stage);
	
	// Create Stacked Data
	createStackedTrainingData(trData, config);
	
	// Now test that everything is right
	for (int hier = 0; hier < numHiers; hier++)
	{
		// Get X_train, Y_train, w_train and split (all of it)
		vector<const vector<prec_type>*> X;
		vector<const vector<prec_type>*> Y;
		vector<double> w;
		vector<int> split = trData.split[hier];
		readDataXHDF(trData.hdf_filenames[hier], X, 0, trData.data_sizes[hier]);
		readDataYHDF(trData.hdf_filenames[hier], Y, 0, trData.data_sizes[hier]);
		readDatawHDF(trData.hdf_filenames[hier], w, 0, trData.data_sizes[hier]);
		
		for (int s = 0; s < num_splits; s++)
		{
			// Get same for split train and test
			vector<const vector<prec_type>*> X_train;
			vector<const vector<prec_type>*> Y_train;
			vector<double> w_train;
			readDataXHDF(trData.hdf_split_filenames_train[hier][s], X_train, 0, trData.data_sizes_split_train[hier][s]);
			readDataYHDF(trData.hdf_split_filenames_train[hier][s], Y_train, 0, trData.data_sizes_split_train[hier][s]);
			readDatawHDF(trData.hdf_split_filenames_train[hier][s], w_train, 0, trData.data_sizes_split_train[hier][s]);
			vector<const vector<prec_type>*> X_test;
			vector<const vector<prec_type>*> Y_test;
			readDataXHDF(trData.hdf_split_filenames_test[hier][s], X_test, 0, trData.data_sizes_split_test[hier][s]);
			readDataYHDF(trData.hdf_split_filenames_test[hier][s], Y_test, 0, trData.data_sizes_split_test[hier][s]);
			
			// Check sizes
			EXPECT_EQ(X.size(), X_train.size() + X_test.size());
			EXPECT_EQ(X_train.size(), Y_train.size());
			EXPECT_EQ(X_train.size(), w_train.size());
			EXPECT_EQ(X_test.size(), Y_test.size());
			
			// Now check values
			int test_idx = 0;
			int train_idx = 0;
			for (int i = 0; i < X.size(); i++)
			{
				if (split[i] != s)
				{
					for (int j = 0; j < X[i]->size(); j++)
					{
						EXPECT_EQ(X[i]->at(j), X_train[train_idx]->at(j));
					}
					for (int j = 0; j < Y[i]->size(); j++)
					{
						EXPECT_EQ(Y[i]->at(j), Y_train[train_idx]->at(j));
					}
					EXPECT_EQ(w[i], w_train[train_idx]);
					train_idx++;
				}
				else
				{
					for (int j = 0; j < X[i]->size(); j++)
					{
						EXPECT_EQ(X[i]->at(j), X_test[test_idx]->at(j));
					}
					for (int j = 0; j < Y[i]->size(); j++)
					{
						EXPECT_EQ(Y[i]->at(j), Y_test[test_idx]->at(j));
					}
					test_idx++;
				}
			}
	
			// Clean up
			for (int i = 0; i < X_train.size(); i++)
			{
				delete X_train[i];
				delete Y_train[i];
			}
			for (int i = 0; i < X_test.size(); i++)
			{
				delete X_test[i];
				delete Y_test[i];
			}			
		}
	
		// Clean up
		for (int i = 0; i < X.size(); i++)
		{
			delete X[i];
			delete Y[i];
		}
	}
}

#if COMPILE_CUDA
/*---------------------------------
  ---------- GPU Functions ----------
  -----------------------------------*/

// GetLeftWeightedSums
TEST(CUDAUtil, GetLeftWeightedSums)
{
	// Test that it works
	int arraySize = 9000000;
	prec_type* LeftWeightedSum = new prec_type[arraySize];
	prec_type* LeftWeightedSum_c = new prec_type[arraySize];
	prec_type* LeftWeightedSumSq = new prec_type[arraySize];
	prec_type* LeftWeightedSumSq_c = new prec_type[arraySize];
	prec_type* Y = new prec_type[arraySize];
	prec_type* weights = new prec_type[arraySize];
	int* sizes = new int[arraySize + 1];
	prec_type randval;
	for (int i = 0; i < arraySize; i++)
	{
		Y[i] = (prec_type)rand() / RAND_MAX;
		weights[i] = (prec_type)rand() / RAND_MAX;
		sizes[i] = i;
	}
	sizes[arraySize] = arraySize;
	prec_type* d_LeftWeightedSum;
	prec_type* d_LeftWeightedSumSq;
	prec_type* d_Y;
	prec_type* d_weights;
	int* d_sizes;
	gpuErrchk(cudaMalloc((void **)&d_LeftWeightedSum, arraySize*sizeof(prec_type)));
	gpuErrchk(cudaMalloc((void **)&d_LeftWeightedSumSq, arraySize*sizeof(prec_type)));
	gpuErrchk(cudaMalloc((void **)&d_Y, arraySize*sizeof(prec_type)));
	gpuErrchk(cudaMalloc((void **)&d_weights, arraySize*sizeof(prec_type)));
	gpuErrchk(cudaMalloc((void **)&d_sizes, (arraySize+1)*sizeof(int)));
	gpuErrchk(cudaMemcpy(d_Y, Y, arraySize*sizeof(prec_type), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpy(d_weights, weights, arraySize*sizeof(prec_type), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpy(d_sizes, sizes, (arraySize+1)*sizeof(int), cudaMemcpyHostToDevice));
	double t = (double)getTickCount();
	GetLeftWeightedSums(d_LeftWeightedSum, d_LeftWeightedSumSq, d_Y, d_weights, d_sizes, arraySize);
	t = ((double)getTickCount() - t)/getTickFrequency();
	gpuErrchk(cudaMemcpy(LeftWeightedSum, d_LeftWeightedSum, arraySize*sizeof(prec_type), cudaMemcpyDeviceToHost));
	gpuErrchk(cudaMemcpy(LeftWeightedSumSq, d_LeftWeightedSumSq, arraySize*sizeof(prec_type), cudaMemcpyDeviceToHost));
	cout << "CUDA weighted sum time: " << t << endl;

	gpuErrchk(cudaFree(d_LeftWeightedSum));
	gpuErrchk(cudaFree(d_LeftWeightedSumSq));
	gpuErrchk(cudaFree(d_Y));
	gpuErrchk(cudaFree(d_weights));
	gpuErrchk(cudaFree(d_sizes));

	t = (double)getTickCount();
	prec_type left_sum = 0;
	prec_type left_sum_sq = 0;
	for (int i = 0; i < arraySize; i++)
	{
		left_sum += weights[i] * Y[i];
		LeftWeightedSum_c[i] = left_sum;
		left_sum_sq += weights[i] * Y[i] * Y[i];
		LeftWeightedSumSq_c[i] = left_sum_sq;
	}
	t = ((double)getTickCount() - t)/getTickFrequency();
	cout << "CPU weighted sum time: " << t << endl;

	// Test
	for (int i = 0; i < arraySize; i++)
	{
		//EXPECT_LT(abs((LeftWeightedSum[i] - LeftWeightedSum_c[i])/LeftWeightedSum_c[i]), 1e-1);
		//EXPECT_LT(abs((LeftWeightedSumSq[i] - LeftWeightedSumSq_c[i])/LeftWeightedSumSq_c[i]), 1e-1);
	}

	cout << "GPU last sum: " << LeftWeightedSum[arraySize-1] << endl;
	cout << "CPU last sum: " << LeftWeightedSum_c[arraySize-1] << endl;
	cout << "True value: " << 0.0001 * arraySize << endl;
}

// MultiplyAll
TEST(CUDAUtil, MultiplyAll)
{
	// Create data
	srand(0);
	int num_samples = 10000;
	int num_labels = 15;
	prec_type* X = new prec_type[num_samples*num_labels];
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < num_labels; j++)
		{
			X[i*num_labels + j] = (prec_type)rand() / RAND_MAX;
		}
	}
	double multVal = (double)rand() / RAND_MAX;

	// Copy to GPU
	prec_type* d_X;
	double* d_multVal;
	int* d_num_samples;
	int* d_num_labels;
	gpuErrchk(cudaMalloc((void **)&d_num_samples, sizeof(int)));
	gpuErrchk(cudaMalloc((void **)&d_num_labels, sizeof(int)));
	gpuErrchk(cudaMalloc((void **)&d_X, num_samples*num_labels*sizeof(prec_type)));
	gpuErrchk(cudaMalloc((void **)&d_multVal, sizeof(double)));
	gpuErrchk(cudaMemcpy(d_num_samples, &num_samples, sizeof(int), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpy(d_num_labels, &num_labels, sizeof(int), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpy(d_X, X, num_samples*num_labels*sizeof(prec_type), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpy(d_multVal, &multVal, sizeof(double), cudaMemcpyHostToDevice));

	// Do it on CPU
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < num_labels; j++)
		{
			X[i*num_labels + j] *= multVal;
		}
	}

	// Do it on GPU
	MultiplyAll(d_X, d_multVal, num_samples*num_labels, d_num_samples, d_num_labels);
	prec_type* comp_X = new prec_type[num_samples*num_labels];
	gpuErrchk(cudaMemcpy(comp_X, d_X, num_samples*num_labels*sizeof(prec_type), cudaMemcpyDeviceToHost));

	// Compare values
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < num_labels; j++)
		{
			EXPECT_LT(abs(comp_X[i*num_labels + j] - X[i*num_labels + j]), allowable_prec_error);
		}
	}

	gpuErrchk(cudaFree(d_X));
	gpuErrchk(cudaFree(d_multVal));
	gpuErrchk(cudaFree(d_num_samples));
	gpuErrchk(cudaFree(d_num_labels));
}

// DivideAll
TEST(CUDAUtil, DivideAll)
{
	// Create data
	srand(0);
	int num_samples = 10000;
	int num_labels = 15;
	prec_type* X = new prec_type[num_samples*num_labels];
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < num_labels; j++)
		{
			X[i*num_labels + j] = (prec_type)rand() / RAND_MAX;
		}
	}
	int divVal = 31;

	// Copy to GPU
	prec_type* d_X;
	int* d_divVal;
	int* d_num_samples;
	int* d_num_labels;
	gpuErrchk(cudaMalloc((void **)&d_num_samples, sizeof(int)));
	gpuErrchk(cudaMalloc((void **)&d_num_labels, sizeof(int)));
	gpuErrchk(cudaMalloc((void **)&d_X, num_samples*num_labels*sizeof(prec_type)));
	gpuErrchk(cudaMalloc((void **)&d_divVal, sizeof(int)));
	gpuErrchk(cudaMemcpy(d_num_samples, &num_samples, sizeof(int), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpy(d_num_labels, &num_labels, sizeof(int), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpy(d_X, X, num_samples*num_labels*sizeof(prec_type), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpy(d_divVal, &divVal, sizeof(int), cudaMemcpyHostToDevice));

	// Do it on CPU
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < num_labels; j++)
		{
			X[i*num_labels + j] /= (double)divVal;
		}
	}

	// Do it on GPU
	DivideAll(d_X, d_divVal, num_samples*num_labels,d_num_samples, d_num_labels);
	prec_type* comp_X = new prec_type[num_samples*num_labels];
	gpuErrchk(cudaMemcpy(comp_X, d_X, num_samples*num_labels*sizeof(prec_type), cudaMemcpyDeviceToHost));

	// Compare values
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < num_labels; j++)
		{
			EXPECT_LT(abs(comp_X[i*num_labels + j] - X[i*num_labels + j]), allowable_prec_error);
		}
	}

	gpuErrchk(cudaFree(d_X));
	gpuErrchk(cudaFree(d_divVal));
	gpuErrchk(cudaFree(d_num_samples));
	gpuErrchk(cudaFree(d_num_labels));
}

// SetAll
TEST(CUDAUtil, SetAll)
{
	// Create data
	srand(0);
	int num_samples = 10000;
	int num_labels = 15;
	prec_type* X = new prec_type[num_samples*num_labels];
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < num_labels; j++)
		{
			X[i*num_labels + j] = (prec_type)rand() / RAND_MAX;
		}
	}
	int divVal = 31;

	// Copy to GPU
	prec_type* d_X;
	prec_type* d_X_copy;
	int* d_num_samples;
	int* d_num_labels;
	gpuErrchk(cudaMalloc((void **)&d_num_samples, sizeof(int)));
	gpuErrchk(cudaMalloc((void **)&d_num_labels, sizeof(int)));
	gpuErrchk(cudaMalloc((void **)&d_X, num_samples*num_labels*sizeof(prec_type)));
	gpuErrchk(cudaMalloc((void **)&d_X_copy, num_samples*num_labels*sizeof(prec_type)));
	gpuErrchk(cudaMemcpy(d_num_samples, &num_samples, sizeof(int), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpy(d_num_labels, &num_labels, sizeof(int), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpy(d_X, X, num_samples*num_labels*sizeof(prec_type), cudaMemcpyHostToDevice));

	// Do it on GPU
	SetAll(d_X_copy, d_X, num_samples*num_labels, d_num_samples, d_num_labels);
	prec_type* comp_X = new prec_type[num_samples*num_labels];
	gpuErrchk(cudaMemcpy(comp_X, d_X_copy, num_samples*num_labels*sizeof(prec_type), cudaMemcpyDeviceToHost));

	// Compare values
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < num_labels; j++)
		{
			EXPECT_LT(abs(comp_X[i*num_labels + j] - X[i*num_labels + j]), allowable_prec_error);
		}
	}

	gpuErrchk(cudaFree(d_X));
	gpuErrchk(cudaFree(d_X_copy));
	gpuErrchk(cudaFree(d_num_samples));
	gpuErrchk(cudaFree(d_num_labels));
}

// AddAll
TEST(CUDAUtil, AddAll)
{
	// Create data
	srand(0);
	int num_samples = 10000;
	int num_labels = 15;
	prec_type* A = new prec_type[num_samples*num_labels];
	prec_type* B = new prec_type[num_samples*num_labels];
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < num_labels; j++)
		{
			A[i*num_labels + j] = (prec_type)rand() / RAND_MAX;
			B[i*num_labels + j] = (prec_type)rand() / RAND_MAX;
		}
	}

	// Copy to GPU
	prec_type* d_A;
	prec_type* d_B;
	int* d_num_samples;
	int* d_num_labels;
	gpuErrchk(cudaMalloc((void **)&d_num_samples, sizeof(int)));
	gpuErrchk(cudaMalloc((void **)&d_num_labels, sizeof(int)));
	gpuErrchk(cudaMalloc((void **)&d_A, num_samples*num_labels*sizeof(prec_type)));
	gpuErrchk(cudaMalloc((void **)&d_B, num_samples*num_labels*sizeof(prec_type)));
	gpuErrchk(cudaMemcpy(d_num_samples, &num_samples, sizeof(int), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpy(d_num_labels, &num_labels, sizeof(int), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpy(d_A, A, num_samples*num_labels*sizeof(prec_type), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpy(d_B, B, num_samples*num_labels*sizeof(prec_type), cudaMemcpyHostToDevice));

	// Do it on CPU
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < num_labels; j++)
		{
			A[i*num_labels + j] += B[i*num_labels + j];
		}
	}

	// Do it on GPU
	AddAll(d_A, d_B, num_samples*num_labels, d_num_samples, d_num_labels);
	prec_type* comp_A = new prec_type[num_samples*num_labels];
	gpuErrchk(cudaMemcpy(comp_A, d_A, num_samples*num_labels*sizeof(prec_type), cudaMemcpyDeviceToHost));

	// Compare values
	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < num_labels; j++)
		{
			EXPECT_LT(abs(comp_A[i*num_labels + j] - A[i*num_labels + j]), allowable_prec_error);
		}
	}

	gpuErrchk(cudaFree(d_A));
	gpuErrchk(cudaFree(d_B));
	gpuErrchk(cudaFree(d_num_samples));
	gpuErrchk(cudaFree(d_num_labels));
}


// // computeHOGFeatures
// TEST(GPUTest, computeHOGFeatures)
// {
// 	// Get data
// 	Json::Value config;
// 	char configFilepath[1024];
// 	strcpy(configFilepath, string(configLocation + "/config.json").c_str());
// 	parseJSONFile(configFilepath, config);
// 	Mat img = imread(configLocation + "im0001.jpg", CV_LOAD_IMAGE_COLOR);
// 	Size imsz = img.size();
// 	int hier = 0;
// 	int sbin = config["sbin"].get(hier, 0).asInt();
// 	int stride = config["stride"].get(hier, 0).asInt();
// 	int w = config["patch_size"].get(hier, 0).asInt();
// 	int numSamples = getNumSamples(config, imsz, hier);

// 	// Test Dalal
// 	config["hog_variant"] = 1;

// 	int numFeats = getFeatSize(config, 0, hier);
// 	cout << "numFeats = " << numFeats << endl;
// 	vector<vector<prec_type>* > Fv1 = computeImageFeatures(img, config, sbin, stride, w, 0);
// 	prec_type* F1 = new prec_type[numFeats*numSamples];
// 	computeImageFeaturesGPU(F1, img, config, sbin, stride, w, 0);
// 	EXPECT_EQ(numSamples, Fv1.size());
// 	EXPECT_EQ(numFeats, Fv1[0]->size());
// 	for (int i = 0; i < numSamples; i++)
// 	{
// 		for (int j = 0; j < numFeats; j++)
// 		{
// 			EXPECT_EQ(F1[i*numFeats + j], (*Fv1[i])[j]);
// 		}
// 	}

// 	// Test UOC
// 	config["hog_variant"] = 2;
// 	numFeats = getFeatSize(config, 0, hier);
// 	vector<vector<prec_type>* > Fv2 = computeHOGFeatures(img, config, sbin, w, 0);
// 	prec_type* F2 = new prec_type[numFeats*numSamples];
// 	computeHOGFeaturesGPU(F2, img, config, sbin, w, 0);
// 	EXPECT_EQ(numSamples, Fv2.size());
// 	EXPECT_EQ(numFeats, Fv2[0]->size());
// 	for (int i = 0; i < numSamples; i++)
// 	{
// 		for (int j = 0; j < numFeats; j++)
// 		{
// 			EXPECT_EQ(F2[i*numFeats + j], (*Fv2[i])[j]);
// 		}
// 	}

// 	delete F1; delete F2;
// 	for (int i = 0; i < numSamples; i++)
// 	{
// 		delete Fv1[i];
// 	}
// 	for (int i = 0; i < numSamples; i++)
// 	{
// 		delete Fv2[i];
// 	}
// }

TEST(GPUTest, computePiotrFeatures)
{
	// Get data
	Json::Value config;
	char configFilepath[1024];
	strcpy(configFilepath, string(configLocation + "/config.json").c_str());
	parseJSONFile(configFilepath, config);
	Mat img = imread(configLocation + "im0003.jpg", CV_LOAD_IMAGE_COLOR);
	img = cropToBinMultiple(img, config);
	Size imsz = img.size();
	int hier = 0;

	int sbin = config["sbin"].get(hier, 0).asInt();
	int stride = config["stride"].get(hier, 0).asInt();
	int w = config["patch_size"].get(hier, 0).asInt();
	int numSamples = getNumSamples(config, imsz, hier);

	// Test
	config["hog_variant"] = 1;
	config["feat_type"] = 2;

	int numFeats = getFeatSize(config, 0, hier);
	cout << "numFeats = " << numFeats << endl;
	cout << "Compute on CPU" << endl;
	vector<vector<prec_type>* > Fv1 = computeImageFeatures(img, config, sbin, stride, w, 0);
	prec_type* F1 = new prec_type[numFeats*numSamples];
	cout << "Compute on GPU" << endl;
	computeImageFeaturesGPU(F1, img, config, sbin, stride, w, 0);
	EXPECT_EQ(numSamples, Fv1.size());
	EXPECT_EQ(numFeats, Fv1[0]->size());
	cout << "Done." << endl;

	for (int i = 0; i < numSamples; i++)
	{
		for (int j = 0; j < numFeats; j++)
		{
			EXPECT_EQ(F1[i*numFeats + j], (*Fv1[i])[j]);
		}
	}
	delete F1;
	for (int i = 0; i < numSamples; i++)
	{
		delete Fv1[i];
	}
}

// extractPatchFeatures
TEST(GPUTest, extractPatchFeatures)
{
	// Get data
	Json::Value config;
	char configFilepath[1024];
	strcpy(configFilepath, string(configLocation + "/config.json").c_str());
	parseJSONFile(configFilepath, config);
	Mat img = imread(configLocation + "im0001.jpg", CV_LOAD_IMAGE_COLOR);
	Size imsz = img.size();
	resetDevice();
	gpuDat->CreateGPUData(config, imsz);
	int numHiers = config["numHiers"].asInt();
	Json::Value numLabels = config["numLabels"];
	vector<vector<vector<prec_type>* > > FCPU;
	vector<prec_type*> FGPU;
	for (int hier = 0; hier < numHiers; hier++)
	{
		int sbin = config["sbin"].get(hier, 0).asInt();
		int stride = config["stride"].get(hier, 0).asInt();
		int w = config["patch_size"].get(hier, 0).asInt();
		int numSamples = getNumSamples(config, imsz, hier);
		int numFeats = getFeatSize(config, 0, hier);
		vector<vector<prec_type>* > Fv = computeImageFeatures(img, config, sbin, stride, w, 0);
		cout << "Fv.size = " << Fv.size() << endl;
		cout << "Fv[0].size = "  << (*Fv[0]).size() << endl;
		cout << "numSamples = " << numSamples;
		cout << "numFeats = " << numFeats;
		prec_type* F_h = new prec_type[numFeats*numSamples];
		computeImageFeaturesGPU(F_h, img, config, sbin, stride, w, 0);


		for (int i = 0; i < numSamples; i++)
		{
			for (int j = 0; j < numFeats; j++)
			{
				EXPECT_EQ(F_h[i*numFeats+j], (*Fv[i])[j]);
			}
		}
		cout << "got here" << endl;
		prec_type* d_F_h = copy2DArrayToCuda(F_h, gpuDat->d_F[hier], numSamples, numFeats);
			cout << "got here" << endl;
		delete F_h;
			cout << "got here" << endl;
		FGPU.push_back(d_F_h);
		FCPU.push_back(Fv);
	}

	vector<vector<Mat*> > scoremaps;
	vector<prec_type*> Y_out;
	srand(100);
	float randval;
	for (int hier = 0; hier < numHiers; hier++)
	{
		Size sz = getHOGFeatureSize(config, imsz, hier);
		int sx = sz.width;
		int sy = sz.height;
		int numLabelsHier = numLabels.get(hier, 0).asInt();
		prec_type* Y_h = new prec_type[sx*sy*numLabelsHier];
		for (int x = 0; x < sx; x++)
		{
			for (int y = 0; y < sy; y++)
			{
				int idx = y*sx + x;
				for (int i = 0; i < numLabelsHier; i++)
				{
					randval = (prec_type)rand() / RAND_MAX;
					Y_h[idx*numLabelsHier + i] = randval;
				}
			}
		}
		vector<Mat*> scoremap = scores2scmap(Y_h, sx*sy, numLabelsHier, sz);
		for (int x = 0; x < sx; x++)
		{
			for (int y = 0; y < sy; y++)
			{
				int idx = y*sx + x;
				for (int i = 0; i < numLabelsHier; i++)
				{
					EXPECT_EQ(Y_h[idx*numLabelsHier + i], scoremap[i]->at<float>(y,x));
				}
			}
		}
		// Copy to GPU and push back
		prec_type* d_Y_h;
		gpuErrchk(cudaMalloc((void **)&d_Y_h, sx*sy*numLabelsHier*sizeof(prec_type)));
		d_Y_h = copy2DArrayToCuda(Y_h, d_Y_h, numLabelsHier, sx*sy);
		Y_out.push_back(d_Y_h);
		scoremaps.push_back(scoremap);
	}
	cout << "got here" << endl;

	// Run CPU version
	extractPatchFeatures(FCPU, scoremaps, config, imsz);
	extractPatchFeaturesGPU(FGPU, scoremaps, Y_out, config, imsz, 1);

	// Now compare
	for (int hier = 0; hier < numHiers; hier++)
	{
		int numSamples = getNumSamples(config, imsz, hier);
		int numFeats = getFeatSize(config, 1, hier);
		vector<vector<prec_type>* > Fv = FCPU[hier];
		prec_type* d_F_h = FGPU[hier];
		prec_type* F_h = copy2DArrayBackFromCuda(d_F_h, numSamples, numFeats);
		EXPECT_EQ(numSamples, Fv.size());
		EXPECT_EQ(numFeats, Fv[0]->size());
		for (int i = 0; i < numSamples; i++)
		{
			for (int j = 0; j < numFeats; j++)
			{
				EXPECT_EQ((*Fv[i])[j], F_h[i*numFeats + j]);
			}
		}
		delete F_h;
	}

	// Cleanup
	gpuDat->DeleteGPUData();
	for (int i = 0; i < Y_out.size(); i++)
	{
		 gpuErrchk(cudaFree(Y_out[i]));
	}
}

// maxPooling
TEST(GPUTest, maxPooling)
{
	for (int count = 0; count < 100; count++)
	{
		Size sz(100, 100);
		Mat mat(sz, CV_32FC1, Scalar(0));
		int numLabels = 5;
		int sx = sz.width;
		int sy = sz.width;
		prec_type* Y = new prec_type[sx*sy*numLabels];
		srand(count);
		float randval;
		for (int x = 0; x < sx; x++)
		{
			for (int y = 0; y < sy; y++)
			{
				randval = (float)rand() / RAND_MAX;
				mat.at<float>(y, x) = randval;
				int idx = y*sx + x;
				for (int i = 0; i < numLabels; i++)
				{
					Y[idx*numLabels + i] = randval;
				}
			}
		}
		int poolSize = 5;
		Mat out1 = maxPooling(mat, poolSize);
		prec_type* Y_pooled = maxPoolingGPU(Y, sx, sy, numLabels, poolSize);
		for (int x = 0; x < sx; x++)
		{
			for (int y = 0; y < sy; y++)
			{
				int idx = y*sx + x;
				for (int i = 0; i < numLabels; i++)
				{
					EXPECT_EQ(out1.at<float>(y, x), Y_pooled[idx*numLabels + i]);
				}
			}
		}
		delete Y; delete Y_pooled;
	}
}

// maxPooling
TEST(GPUTest, maxPoolingCUDA)
{
	// Get data
	Json::Value config;
	char configFilepath[1024];
	strcpy(configFilepath, string(configLocation + "/config.json").c_str());
	parseJSONFile(configFilepath, config);
	Size maxSize = Size(640, 480);
	gpuDat->CreateGPUData(config, maxSize);
	int hier = 0;
	for (int count = 0; count < 100; count++)
	{
		Size sz(100, 100);
		int numLabels = config["numLabels"].get(hier,0).asInt();;
		int sx = sz.width;
		int sy = sz.width;
		prec_type* Y1 = new prec_type[sx*sy*numLabels];
		prec_type* Y2 = new prec_type[sx*sy*numLabels];
		srand(count);
		float randval;
		for (int x = 0; x < sx; x++)
		{
			for (int y = 0; y < sy; y++)
			{
				int idx = y*sx + x;
				for (int i = 0; i < numLabels; i++)
				{
					randval = (float)rand() / RAND_MAX;
					Y1[idx*numLabels + i] = randval;
					Y2[idx*numLabels + i] = randval;
				}
			}
		}
		int poolSize = config.get("poolSize", 1).asInt();
		prec_type* Y_pooled1 = maxPoolingGPU(Y1, sx, sy, numLabels, poolSize);

		// Copy to and from GPU and call max pooling
		prec_type* d_Y2;
		gpuErrchk(cudaMalloc((void **)&d_Y2, sx*sy*numLabels*sizeof(prec_type)));
		gpuErrchk(cudaMemcpy(d_Y2, Y2, sx*sy*numLabels*sizeof(prec_type), cudaMemcpyHostToDevice));

		prec_type* d_Y_pooled2 = maxPoolingGPUOnCUDA(d_Y2, sx, sy, numLabels, hier, poolSize);
		prec_type* Y_pooled2 = new prec_type[sx*sy*numLabels];
		gpuErrchk(cudaMemcpy(Y_pooled2, d_Y_pooled2, sx*sy*numLabels*sizeof(prec_type), cudaMemcpyDeviceToHost));

		for (int x = 0; x < sx; x++)
		{
			for (int y = 0; y < sy; y++)
			{
				int idx = y*sx + x;
				for (int i = 0; i < numLabels; i++)
				{
					EXPECT_EQ(Y_pooled1[idx*numLabels + i], Y_pooled2[idx*numLabels + i]);
				}
			}
		}
		delete Y1; delete Y_pooled1; delete Y2; delete Y_pooled2;
	}
	gpuDat->DeleteGPUData();
}

// RandomForest
class RFTest : public ::testing::Test
{

	protected:
		RFTest()
		{
			// Get data
			Json::Value config;
			char configFilepath[1024];
			strcpy(configFilepath, string(configLocation + "/config.json").c_str());
			parseJSONFile(configFilepath, config);
			numSamples = 1000;
			int hier = 0;
			int stage = 0;
			testForests.push_back(new ml::RandomForest(configLocation + "unitTestModels/SampleM_RF_stage_0_hier0_regressor_0.random_forest"));
			numFeatsVec.push_back(getFeatSize(config, stage, hier));
			numLabelsVec.push_back(config["numLabels"].get(hier, 0).asInt());
			testForests[testForests.size()-1]->preAllocate(numSamples, numFeatsVec[numFeatsVec.size()-1], numLabelsVec[numLabelsVec.size()-1], configLocation + "unitTestModels/", "SampleM_RF_stage_0_hier0_regressor_0.rf_cuda");
			hier = 1;
			testForests.push_back(new ml::RandomForest(configLocation + "unitTestModels/SampleM_RF_stage_0_hier1_regressor_0.random_forest"));
			numFeatsVec.push_back(getFeatSize(config, stage, hier));
			numLabelsVec.push_back(config["numLabels"].get(hier, 0).asInt());
			testForests[testForests.size()-1]->preAllocate(numSamples, numFeatsVec[numFeatsVec.size()-1], numLabelsVec[numLabelsVec.size()-1], configLocation + "unitTestModels/", "SampleM_RF_stage_0_hier1_regressor_0.rf_cuda");
			stage = 1;
			hier = 0;
			testForests.push_back(new ml::RandomForest(configLocation + "unitTestModels/SampleM_RF_stage_1_hier0_regressor_3.random_forest"));
			numFeatsVec.push_back(getFeatSize(config, stage, hier));
			numLabelsVec.push_back(config["numLabels"].get(hier, 0).asInt());
			testForests[testForests.size()-1]->preAllocate(numSamples, numFeatsVec[numFeatsVec.size()-1], numLabelsVec[numLabelsVec.size()-1], configLocation + "unitTestModels/", "SampleM_RF_stage_1_hier0_regressor_3.rf_cuda");
			hier = 1;
			testForests.push_back(new ml::RandomForest(configLocation + "unitTestModels/SampleM_RF_stage_1_hier1_regressor_4.random_forest"));
			numFeatsVec.push_back(getFeatSize(config, stage, hier));
			numLabelsVec.push_back(config["numLabels"].get(hier, 0).asInt());
			testForests[testForests.size()-1]->preAllocate(numSamples, numFeatsVec[numFeatsVec.size()-1], numLabelsVec[numLabelsVec.size()-1], configLocation + "unitTestModels/", "SampleM_RF_stage_1_hier1_regressor_4.rf_cuda");
			stage = 2;
			hier = 0;
			testForests.push_back(new ml::RandomForest(configLocation + "unitTestModels/SampleM_RF_stage_2_hier0_regressor_5.random_forest"));
			numFeatsVec.push_back(getFeatSize(config, stage, hier));
			numLabelsVec.push_back(config["numLabels"].get(hier, 0).asInt());
			testForests[testForests.size()-1]->preAllocate(numSamples, numFeatsVec[numFeatsVec.size()-1], numLabelsVec[numLabelsVec.size()-1], configLocation + "unitTestModels/", "SampleM_RF_stage_2_hier0_regressor_5.rf_cuda");
			hier = 1;
			testForests.push_back(new ml::RandomForest(configLocation + "unitTestModels/SampleM_RF_stage_2_hier1_regressor_6.random_forest"));
			numFeatsVec.push_back(getFeatSize(config, stage, hier));
			numLabelsVec.push_back(config["numLabels"].get(hier, 0).asInt());
			testForests[testForests.size()-1]->preAllocate(numSamples, numFeatsVec[numFeatsVec.size()-1], numLabelsVec[numLabelsVec.size()-1], configLocation + "unitTestModels/", "SampleM_RF_stage_2_hier1_regressor_6.rf_cuda");
		}

		virtual ~RFTest()
		{
			for (int i = 0; i < testForests.size(); i++)
			{
				testForests[i]->deAllocate();
				delete testForests[i];
			}
		}

		// Random Forest to test
		vector<ml::RandomForest*> testForests;
		vector<int> numFeatsVec;
		vector<int> numLabelsVec;
		int numSamples;
};

TEST_F(RFTest, rfRandomTest)
{
	for (int count = 0; count < 10; count++)
	{
		srand(count);
		float randval;
		for (int forest = 0; forest < testForests.size(); forest++)
		{
			// Create inputs
			int numFeats = numFeatsVec[forest];
			int numLabels = numLabelsVec[forest];
			prec_type* XGPU = new prec_type[numFeats*numSamples];
			prec_type* YGPU = new prec_type[numLabels*numSamples];
			vector<vector<prec_type> > XCPU;
			vector<vector<prec_type> > YCPU;
			for (int i = 0; i < numSamples; i++)
			{
				vector<prec_type> x;
				for (int j = 0; j < numFeats; j++)
				{
					randval = (prec_type)rand() / RAND_MAX;
					x.push_back(randval);
					XGPU[i*numFeats + j] = randval;
				}
				XCPU.push_back(x);
			}
			prec_type* d_XGPU;
			gpuErrchk(cudaMalloc((void**)&d_XGPU, numSamples*numFeats*sizeof(prec_type)));
			gpuErrchk(cudaMemcpy(d_XGPU, XGPU, numSamples*numFeats*sizeof(prec_type), cudaMemcpyHostToDevice));

			// Do prediction
			ml::RandomForest* classifier = testForests[forest];
			classifier->setInputData(d_XGPU, numSamples, numLabels);
			prec_type* d_Y = classifier->predictCuda(numSamples, numFeats, numLabels);
			YGPU = copy2DArrayBackFromCuda(d_Y, numSamples, numLabels);
			//classifier->predictCuda(YGPU, numSamples, numFeats, numLabels);
			for (int i = 0; i < numSamples; i++)
			{
				vector<prec_type> y;
				classifier->predict(XCPU[i], y);
				YCPU.push_back(y);
			}

			// Check predictions
			EXPECT_EQ(YCPU.size(), numSamples);
			EXPECT_EQ(YCPU[0].size(), numLabels);
			for (int i = 0; i < numSamples; i++)
			{
				for (int j = 0; j < numLabels; j++)
				{
					EXPECT_LT(abs(YCPU[i][j] - YGPU[i*numLabels + j]), allowable_prec_error);
				}
			}

			delete XGPU;
			delete YGPU;
			gpuErrchk(cudaFree(d_XGPU));
		}
	}
}

// SVM
class SVMTest : public ::testing::Test
{

	protected:
		SVMTest()
		{
			// Get data
			Json::Value config;
			char configFilepath[1024];
			strcpy(configFilepath, string(configLocation + "/config.json").c_str());
			parseJSONFile(configFilepath, config);
			numSamples = 1000;
			int hier = 0;
			int stage = 0;
			testSVMs.push_back(new ml::BoostedSVM(configLocation + "unitTestModels/", "SampleM_RF_stage_0_hier0"));
			numFeatsVec.push_back(getFeatSize(config, stage, hier));
			numLabelsVec.push_back(config["numLabels"].get(hier, 0).asInt());
			testSVMs[testSVMs.size()-1]->preAllocate(numSamples, numFeatsVec[numFeatsVec.size()-1], numLabelsVec[numLabelsVec.size()-1], configLocation + "unitTestModels/", "SampleM_RF_stage_0_hier0");
			hier = 1;
			testSVMs.push_back(new ml::BoostedSVM(configLocation + "unitTestModels/", "SampleM_RF_stage_0_hier1"));
			numFeatsVec.push_back(getFeatSize(config, stage, hier));
			numLabelsVec.push_back(config["numLabels"].get(hier, 0).asInt());
			testSVMs[testSVMs.size()-1]->preAllocate(numSamples, numFeatsVec[numFeatsVec.size()-1], numLabelsVec[numLabelsVec.size()-1], configLocation + "unitTestModels/", "SampleM_RF_stage_0_hier1");
			stage = 1;
			hier = 0;
			testSVMs.push_back(new ml::BoostedSVM(configLocation + "unitTestModels/", "SampleM_RF_stage_1_hier0"));
			numFeatsVec.push_back(getFeatSize(config, stage, hier));
			numLabelsVec.push_back(config["numLabels"].get(hier, 0).asInt());
			testSVMs[testSVMs.size()-1]->preAllocate(numSamples, numFeatsVec[numFeatsVec.size()-1], numLabelsVec[numLabelsVec.size()-1], configLocation + "unitTestModels/", "SampleM_RF_stage_1_hier0");
			hier = 1;
			testSVMs.push_back(new ml::BoostedSVM(configLocation + "unitTestModels/", "SampleM_RF_stage_1_hier1"));
			numFeatsVec.push_back(getFeatSize(config, stage, hier));
			numLabelsVec.push_back(config["numLabels"].get(hier, 0).asInt());
			testSVMs[testSVMs.size()-1]->preAllocate(numSamples, numFeatsVec[numFeatsVec.size()-1], numLabelsVec[numLabelsVec.size()-1], configLocation + "unitTestModels/", "SampleM_RF_stage_1_hier1");
			stage = 2;
			hier = 0;
			testSVMs.push_back(new ml::BoostedSVM(configLocation + "unitTestModels/", "SampleM_RF_stage_2_hier0"));
			numFeatsVec.push_back(getFeatSize(config, stage, hier));
			numLabelsVec.push_back(config["numLabels"].get(hier, 0).asInt());
			testSVMs[testSVMs.size()-1]->preAllocate(numSamples, numFeatsVec[numFeatsVec.size()-1], numLabelsVec[numLabelsVec.size()-1], configLocation + "unitTestModels/", "SampleM_RF_stage_2_hier0");
			hier = 1;
			testSVMs.push_back(new ml::BoostedSVM(configLocation + "unitTestModels/", "SampleM_RF_stage_2_hier1"));
			numFeatsVec.push_back(getFeatSize(config, stage, hier));
			numLabelsVec.push_back(config["numLabels"].get(hier, 0).asInt());
			testSVMs[testSVMs.size()-1]->preAllocate(numSamples, numFeatsVec[numFeatsVec.size()-1], numLabelsVec[numLabelsVec.size()-1], configLocation + "unitTestModels/", "SampleM_RF_stage_2_hier1");
		}

		virtual ~SVMTest()
		{
			for (int i = 0; i < testSVMs.size(); i++)
			{
				testSVMs[i]->deAllocate();
				delete testSVMs[i];
			}
		}

		// SVMs to test
		vector<ml::BoostedSVM*> testSVMs;
		vector<int> numFeatsVec;
		vector<int> numLabelsVec;
		int numSamples;
};

TEST_F(SVMTest, svmRandomTest)
{
	for (int count = 0; count < 10; count++)
	{
		srand(count + 10);
		float randval;
		for (int svmind = 0; svmind < testSVMs.size(); svmind++)
		{
			// Create inputs
			int numFeats = numFeatsVec[svmind];
			int numLabels = numLabelsVec[svmind];
			prec_type* XGPU = new prec_type[numFeats*numSamples];
			prec_type* YGPU = new prec_type[numLabels*numSamples];
			vector<vector<prec_type> > XCPU;
			vector<vector<prec_type> > YCPU;
			for (int i = 0; i < numSamples; i++)
			{
				vector<prec_type> x;
				for (int j = 0; j < numFeats; j++)
				{
					randval = 2*(prec_type)rand() / RAND_MAX + 1;
					x.push_back(randval);
					XGPU[i*numFeats + j] = randval;
				}
				XCPU.push_back(x);
			}
			prec_type* d_XGPU;
			gpuErrchk(cudaMalloc((void**)&d_XGPU, numSamples*numFeats*sizeof(prec_type)));
			gpuErrchk(cudaMemcpy(d_XGPU, XGPU, numSamples*numFeats*sizeof(prec_type), cudaMemcpyHostToDevice));

			// Do prediction
			ml::BoostedSVM* classifier = testSVMs[svmind];
			classifier->setInputData(d_XGPU, numSamples, numLabels);
			prec_type* d_Y = classifier->predictCuda(numSamples, numFeats, numLabels);
			YGPU = copy2DArrayBackFromCuda(d_Y, numSamples, numLabels);
			//classifier->predictCuda(YGPU, numSamples, numFeats, numLabels);
			for (int i = 0; i < numSamples; i++)
			{
				vector<prec_type> y;
				classifier->predict(XCPU[i], y);
				YCPU.push_back(y);
			}

			// Check predictions
			EXPECT_EQ(YCPU.size(), numSamples);
			EXPECT_EQ(YCPU[0].size(), numLabels);
			for (int i = 0; i < numSamples; i++)
			{
				for (int j = 0; j < numLabels; j++)
				{
					EXPECT_LT(abs(YCPU[i][j] - YGPU[i*numLabels + j]), allowable_prec_error);
				}
			}

			delete XGPU;
			delete YGPU;
			gpuErrchk(cudaFree(d_XGPU));
		}
	}
}

// AppendContext
class predictOnImageTest : public ::testing::Test
{

	protected:
		predictOnImageTest()
		{
			// Get data
			char configFilepath[1024];
			strcpy(configFilepath, string(configLocation + "/config.json").c_str());
			parseJSONFile(configFilepath, config);
			config["viz_scmaps"] = 1;

			// Init PoseMachine
			maxSize = Size(640, 480);
			numStages = config.get("stages", 1).asInt();
			int numSplits = config.get("splits", 0).asInt();
			numHiers = config.get("numHiers", 0).asInt();
			int classifierType = config.get("classifier", 0).asInt();
			classifier = new PoseMachine(numStages, numSplits, numHiers, classifierType, configLocation + "unitTestModels/", "SampleM_RF");

			// Setup CUDA
			resetDevice();
			gpuDat->CreateGPUData(config, maxSize);
			classifier->allocateCudaClassifiers(config, maxSize, configLocation + "unitTestModels/", "SampleM_RF");
		}

		virtual ~predictOnImageTest()
		{
			classifier->deAllocateCudaClassifiers();
			gpuDat->DeleteGPUData();
			delete classifier;
		}

		// PoseMachine and other data
		Json::Value config;
		PoseMachine* classifier;
		Size maxSize;
		int numStages;
		int numHiers;
};

TEST_F(predictOnImageTest, predictTest)
{
	for (int i = 1; i < 51; i++)
	{
		// Read in image
		char filename[1024];
		char extension[1024];
		strncpy(extension, configLocation.c_str(), sizeof(extension));
		sprintf(filename, "%sframe_%d.jpg", extension, i);
		cout << "filename: " << filename << endl;
		Mat img = imread(filename, CV_LOAD_IMAGE_COLOR);
		Size imsz = img.size();
		Json::Value sbin = config["sbin"];

		// CPU
		vector<vector<vector<prec_type>* > > FCPU;
		vector<vector<vector<vector<prec_type> > > > Y_outCPU;
		vector<vector<vector<Mat*> > > scoremapsCPU;
		predictOnImage(Y_outCPU, scoremapsCPU, FCPU, img, *classifier, config, sbin, imsz, numStages - 1, -1);

		// GPU
		vector<prec_type*> FGPU;
		vector<vector<prec_type*> > Y_outGPU;
		vector<vector<vector<Mat*> > > scoremapsGPU;
		predictOnImageGPU(Y_outGPU, scoremapsGPU, FGPU, img, *classifier, config, sbin, imsz, numStages - 1, -1);

		// Check all
		EXPECT_EQ(numHiers, FCPU.size());
		EXPECT_EQ(numHiers, FGPU.size());
		EXPECT_EQ(numStages, Y_outCPU.size());
		EXPECT_EQ(numStages, Y_outGPU.size());
		EXPECT_EQ(numHiers, Y_outCPU[0].size());
		EXPECT_EQ(numHiers, Y_outGPU[0].size());
		EXPECT_EQ(numStages, scoremapsCPU.size());
		EXPECT_EQ(numStages, scoremapsGPU.size());
		EXPECT_EQ(numHiers, scoremapsCPU[0].size());
		EXPECT_EQ(numHiers, scoremapsGPU[0].size());

		int numDiff = 0;
		prec_type maxDiff = 0;
		prec_type sumDiff = 0;
		for (int stage = 0; stage < numStages; stage++)
		{
			for (int hier = 0; hier < numHiers; hier++)
			{
				for (int i = 0; i < Y_outCPU[stage][hier].size(); i++)
				{
					int jsize = Y_outCPU[stage][hier][i].size();
					for (int j = 0; j < jsize; j++)
					{
						prec_type diff = abs(Y_outCPU[stage][hier][i][j] - Y_outGPU[stage][hier][i*jsize + j]);
						EXPECT_LT(diff, 0.1);
						if (diff > 0)
						{
							numDiff++;
							sumDiff += diff;
							if (diff > maxDiff)
								maxDiff = diff;
							//cout << "stage: " << stage << " hier: " << hier << " i: " << i << " j: " << j << endl;
							//cout << "CPU: " << Y_outCPU[stage][hier][i][j] << endl;
							//cout << "GPU: " << Y_outGPU[stage][hier][i*jsize + j] << endl;
						}
					}
				}
			}
			//cout << "At stage " << stage << " numDiff: " << numDiff << " maxDiff: " << maxDiff << " avgDiff: " << sumDiff / numDiff << endl;
			numDiff = 0; maxDiff = 0; sumDiff = 0;
		}

		vector<prec_type*> FGPU_Old;
		for (int hier = 0; hier < numHiers; hier++)
		{
			int isize = FCPU[hier].size();
			int jsize = FCPU[hier][0]->size();
			prec_type* temp = copy2DArrayBackFromCuda(FGPU[hier], isize, jsize);
			FGPU_Old.push_back(FGPU[hier]);
			FGPU[hier] = temp;
		}

		for (int hier = 0; hier < numHiers; hier++)
		{
			for (int i = 0; i < FCPU[hier].size(); i++)
			{
				int jsize = FCPU[hier][i]->size();
				for (int j = 0; j < jsize; j++)
				{
					prec_type diff = abs((*FCPU[hier][i])[j] - FGPU[hier][i*jsize + j]);
					if (diff > allowable_prec_error)
					{
						//cout << "hier: " << " i: " << i << " j: " << j << endl;
						//cout << "CPU: " << (*FCPU[hier][i])[j] << endl;
						//cout << "GPU: " << FGPU[hier][i*jsize + j] << endl;
					}
					EXPECT_LT(diff, 0.1);

					// DEBUG - transfer
					FGPU[hier][i*jsize + j] = (*FCPU[hier][i])[j];
				}
			}
		}

		for (int hier = 0; hier < numHiers; hier++)
		{
			int isize = FCPU[hier].size();
			int jsize = FCPU[hier][0]->size();
			prec_type* temp = FGPU_Old[hier];
			gpuErrchk(cudaMemcpy(temp, FGPU[hier], isize*jsize*sizeof(prec_type), cudaMemcpyHostToDevice));
			delete FGPU[hier];
			FGPU[hier] = temp;
		}

		// Try append with same F's
		numDiff = 0; maxDiff = 0; sumDiff = 0;
		//cout << "Try keeping F" << endl;
		int s = 2;
		vector<vector<Mat*> > scoremaps_s;
		vector<prec_type*> Y_out_s;
		for (int hier = 0; hier < numHiers; hier++)
		{
			int numSamples = getNumSamples(config, imsz, hier);
			int numDimIn = getFeatSize(config, s, hier);
			int numLabelsInt = config["numLabels"].get(hier,0).asInt();
			prec_type* Y_out_h = new prec_type[numSamples*numLabelsInt];
			prec_type* d_Y = stagePredictGPU(FGPU[hier], (*classifier).classifiers[s][hier], config, sbin.get(hier, 0).asInt(), numSamples, numDimIn, numLabelsInt, imsz);
			Y_out_h = copy2DArrayBackFromCuda(d_Y, numSamples, numLabelsInt);
			cv::Size sz = getHOGFeatureSize(config, imsz, hier);
			vector<Mat*> scoremaps_h = scores2scmap(Y_out_h, numSamples, numLabelsInt, sz);
			scoremaps_s.push_back(scoremaps_h);
			Y_out_s.push_back(Y_out_h);
		}

		int total = 0;
		int iworst = 0, jworst = 0, hierworst = 0;
		for (int hier = 0; hier < numHiers; hier++)
		{
			for (int i = 0; i < Y_outCPU[2][hier].size(); i++)
			{
				int jsize = Y_outCPU[2][hier][i].size();
				for (int j = 0; j < jsize; j++)
				{
					total++;
					prec_type diff = abs(Y_outCPU[2][hier][i][j] - Y_out_s[hier][i*jsize + j]);
					EXPECT_LT(diff, allowable_prec_error);
					if (diff > allowable_prec_error)
					{
						numDiff++;
						sumDiff += diff;
						if (diff > maxDiff)
						{
							maxDiff = diff;
							iworst = i;
							jworst = j;
							hierworst = hier;
						}
					}
				}
			}
		}
		//cout << "Now the numDiff: " << numDiff << " maxDiff: " << maxDiff << " avgDiff: " << sumDiff / numDiff << endl;
	}
}
#endif

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);

	// Argument is filename
	if (argc < 2)
	{
		cout << "usage unit_test config_unitTest_Directory" << endl;
		return 1;
	}
	configLocation = string(argv[1]);

	return RUN_ALL_TESTS();
}
