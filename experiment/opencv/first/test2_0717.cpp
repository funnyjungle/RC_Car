#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

using namespace cv;
using namespace std;

void help()
{
 cout << "\nThis program demonstrates line finding with the Hough transform.\n"
         "Usage:\n"
         "./houghlines <image_name>, Default is pic1.jpg\n" << endl;
}

int main()
{
  VideoCapture cap("driving4.mp4");
//  VideoCapture cap(0);
  Mat frame, dst, smoothed, cdst, res;
  if(!cap.isOpened())
  {
    cout << "Could not open the input video.." << endl ;
    return -1;
  }

  while(1)
  {
    cap.read(frame);
    if(frame.empty())
    {
	cerr << "ERROR! blank frame grabbed\n" ;
	break;
    }

// 그레이스케일로 변환
    cvtColor(frame, dst, COLOR_BGR2GRAY);

// 가우시안 블러링 적용 - 커널사이즈 5,5
    GaussianBlur(dst, smoothed, Size(5, 5), 0);

// Canny 엣지 적용
    Canny(smoothed, cdst, 75, 175);

// 빈 Mat 만듬 (8비트 1채널)
  Mat dstImage(cdst.rows, cdst.cols, CV_8UC1, Scalar(0,0,0));

//pts1[4] 꼭지점4개짜리 다각형 만들기 {왼쪽바닥점, 왼쪽윗점, 오른쪽윗점, 오른쪽바닥점}
  Point pts1[4] = {Point(20, cdst.rows), Point(cdst.cols/2-60 , cdst.rows/2-100), Point(cdst.cols/2+60, cdst.rows/2-100), Point(cdst.cols-20, cdst.rows)};
//pts2[3] 꼭지점3개짜리 더미 다각형(가짜)
  Point pts2[3] = {Point(100,100), Point(101, 101), Point(99, 99)};

//fillPoly 를 사용하기 위한 상수 배열 선언
  const Point *polygon[2] = {pts1, pts2};

//fillPoly 를 사용하기 위한 꼭지점갯수 배열 선언
  int npts[2] = {4,3};

//fillPoly(입력영상, 마스크영상, 꼭지점갯수, n개의 다각형 만들어라, 스칼라값 )
  fillPoly(dstImage, polygon, npts, 2, Scalar(255, 255, 255));

//bitwise_and : 두 영상 비교하여 1일경우만 거르기 - (미리만들어놓은 Mat) res 에 저장하기
  bitwise_and(cdst, dstImage, res);

// 넌제로 화소로 외곽선을 표현하므로, 흑백값을 반전한다
//   threshold(cdst, tdst, 0, 125, THRESH_BINARY_INV);
//    adaptiveThreshold(cdst, tdst, 125, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 5, 0);


#if 0
  vector<Vec2f> lines;
  HoughLines(dst, lines, 1, CV_PI/180, 200, 0, 0 );

  for( size_t i = 0; i < lines.size(); i++ )
  {
     float rho = lines[i][0], theta = lines[i][1];
     Point pt1, pt2;
     double a = cos(theta), b = sin(theta);
     double x0 = a*rho, y0 = b*rho;
     pt1.x = cvRound(x0 + 1000*(-b));
     pt1.y = cvRound(y0 + 1000*(a));
     pt2.x = cvRound(x0 - 1000*(-b));
     pt2.y = cvRound(y0 - 1000*(a));
     line( cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
  }

#else
  vector<Vec4i> lines;
  HoughLinesP(res, lines, 1, CV_PI/180, 85, 20, 20 );
  for( size_t i = 0; i < lines.size(); i++ )
  {
    Vec4i l = lines[i];
    line( res, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,255,255), 3, CV_AA);
  }

#endif


//   imshow("source", dst);
//   imshow("smoothing", smoothed);
//   imshow("canny edge", cdst);
   imshow("res", res);



   if(waitKey(10)>= 0) break;
   }

   cap.release();
   destroyAllWindows();

   return 0;
}

