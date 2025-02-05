#include <iostream>
#include <opencv2/opencv.hpp>
#include "EdgeExtract.h"
#include "Converter.h"
#include "Region.h"
#include "Drawlines.h"

using namespace cv;
using namespace std;
//
//Hough Transform 파라미터
float rho = 2; // distance resolution in pixels of the Hough grid
float theta = 1 * CV_PI / 180; // angular resolution in radians of the Hough grid
float hough_threshold = 15;	 // minimum number of votes(intersections in Hough grid cell)
float minLineLength = 10; //minimum number of pixels making up a line
float maxLineGap = 20;	//maximum gap in pixels between connectable line segments
//

int main()
{
	//도로 주행 영상
	VideoCapture DrivingVideo("data/challenge.mp4");

	Mat frame;

	if (!DrivingVideo.isOpened())
	{
		cout << "Data can not open" << endl;
		return -1;
	}

	int nWidth = frame.size().width;
	int nHeight = frame.size().height;
	int cnt = 0;

	while (1)
	{
		//원본 영상 읽어오기
		DrivingVideo >> frame;
		if (frame.empty())
			break;

		//2. 차선 후보 따로 저장
		//img_filtered = 차선 후보 영상
		Mat img_filtered;
		//후보영상에 따른 edge영상
		Mat img_edges;
		color_detect(frame, img_filtered);

		//3. 그레이스케일 영상으로 변환하여 에지 성분을 추출
		Apply_Cannyf(img_filtered, img_edges);
		//imshow("2번",img_edges);

		//4. 차선 검출할 영역을 제한함 (진행방향 바닥에 존재하는 차선으로 한정)
		img_edges = SetRegion(img_edges, img_filtered.rows, img_filtered.cols);

		// 4번까지 실행결과
		//imshow("Extract edge", img_edges);

		UMat uImage_edges;
		img_edges.copyTo(uImage_edges);

		//5. 직선 성분을 추출(각 직선의 시작좌표와 끝좌표를 계산함)
		vector<Vec4i> lines;
		HoughLinesP(uImage_edges, lines, rho, theta, hough_threshold, minLineLength, maxLineGap);
		//그레이 스케일 영상으로 변환하여 Edge 추출
		

		Mat img_line = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
		draw_line(img_line, lines);
		
		Mat img_annotated;
		//7. 원본 영상에 6번의 직선을 같이 보여줌 
		addWeighted(frame, 0.8, img_line, 1.0, 0.0, img_annotated);
		//imshow("img_annotated", img_annotated);
		
		//9. 결과를 화면에 보여줌 
		Mat img_result;
		resize(img_annotated, img_annotated, Size(frame.rows * 0.7, frame.cols * 0.7));
		resize(img_edges, img_edges, Size(frame.rows * 0.7, frame.cols * 0.7));
		cvtColor(img_edges, img_edges, COLOR_GRAY2BGR);
		hconcat(img_edges, img_annotated, img_result);
		imshow("합친거", img_result);
		

		if (waitKey(10) > 0)
			break;
	}

	//writer.release();
	return 0;
}
