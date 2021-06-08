/*
Ahmet Yaylalioglu
10.02.2017


#include <stdio.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp> //blur efekti için
#include <opencv2/video/background_segm.hpp>
#include <opencv2/imgproc/types_c.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;


int main(int argc, const char** argv)
{


    VideoCapture cam(0);
    if (!cam.isOpened()) {
        cout << "ERROR not opened " << endl;
        return -1;
    }
    Mat img;
    Mat img_threshold;
    Mat img_gray;
    Mat img_roi;
    Mat img_YCbCr;
    namedWindow("Original_image", WINDOW_AUTOSIZE);
    namedWindow("Gray_image", WINDOW_AUTOSIZE);
    namedWindow("Thresholded_image", WINDOW_AUTOSIZE);
    namedWindow("ROI", WINDOW_AUTOSIZE);
    namedWindow("YCbCr", WINDOW_AUTOSIZE);
    char a[40];
    int count = 0;

    CascadeClassifier face_classifier;

    face_classifier.load("{YOUR_OPENCV_PATH}/data/haarcascades/haarcascade_frontalface_default.xml");

    Mat frame;
    while (1) {
        bool b = cam.read(img);
        if (!b) {
            cout << "ERROR : cannot read" << endl;
            return -1;
        }
        Rect roi(340, 100, 270, 270);//식별칸 만들기
        img_roi = img(roi);
        cvtColor(img_roi, img_YCbCr, COLOR_BGR2YCrCb);//color detedction
        Scalar upper(255, 173, 127);
        Scalar lower(0, 133, 77);
        inRange(img_YCbCr, lower, upper, img_YCbCr);


        cvtColor(img_roi, img_gray, CV_RGB2GRAY);//흑백


        GaussianBlur(img_gray, img_gray, Size(19, 19), 0.0, 0);//color detection+흑백+가우시안 블러
        //threshold(img_gray, img_threshold, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);//color detection+흑백+가우시안블러

        vector<vector<Point> >contours;
        vector<Vec4i>hierarchy;
        //findContours(원본이미지,외곽선백터(배열),outputarray,윤광 검색모드 설정, 윤광 근사방법, 오프셋)
        findContours(img_YCbCr, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point());
        if (contours.size() > 0) {
            size_t indexOfBiggestContour = -1;
            size_t sizeOfBiggestContour = 0;


            for (size_t i = 0; i < contours.size(); i++) {
                if (contours[i].size() > sizeOfBiggestContour) {
                    sizeOfBiggestContour = contours[i].size();
                    indexOfBiggestContour = i;
                }
            }
            vector<vector<int> >hull(contours.size());
            vector<vector<Point> >hullPoint(contours.size());
            vector<vector<Vec4i> >defects(contours.size());
            vector<vector<Point> >defectPoint(contours.size());
            vector<vector<Point> >contours_poly(contours.size());
            Point2f rect_point[4];
            vector<RotatedRect>minRect(contours.size());
            vector<Rect> boundRect(contours.size());

            for (size_t i = 0; i < contours.size(); i++) {
                if (contourArea(contours[i]) > 5000) {
                    convexHull(contours[i], hull[i], true);
                    convexityDefects(contours[i], hull[i], defects[i]);
                    if (indexOfBiggestContour == i) {
                        minRect[i] = minAreaRect(contours[i]);
                        for (size_t k = 0; k < hull[i].size(); k++) {
                            int ind = hull[i][k];
                            hullPoint[i].push_back(contours[i][ind]);
                        }
                        count = 0;

                        for (size_t k = 0; k < defects[i].size(); k++) {
                            if (defects[i][k][3] > 13 * 256) {

                                int p_end = defects[i][k][1];
                                int p_far = defects[i][k][2];
                                defectPoint[i].push_back(contours[i][p_far]);
                                circle(img_roi, contours[i][p_end], 3, Scalar(0, 255, 0), 2);
                                count++;
                            }

                        }

                        if (count == 1)
                            strcpy(a, "1 it is ");
                        else if (count == 2)
                            strcpy(a, "2 it is ");
                        else if (count == 3)
                            strcpy(a, "3 it is ");
                        else if (count == 4)
                            strcpy(a, "4 it is");
                        else if (count == 5)
                            strcpy(a, "5 it is");
                        else
                            strcpy(a, "Welcome !!");

                        putText(img, a, Point(70, 70), FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 0, 0), 2, 8, false);
                        drawContours(img_YCbCr, contours, i, Scalar(255, 255, 0), 2, 8, vector<Vec4i>(), 0, Point());
                        drawContours(img_YCbCr, hullPoint, i, Scalar(255, 255, 0), 1, 8, vector<Vec4i>(), 0, Point());
                        drawContours(img_roi, hullPoint, i, Scalar(0, 0, 255), 2, 8, vector<Vec4i>(), 0, Point());
                        approxPolyDP(contours[i], contours_poly[i], 3, false);
                        boundRect[i] = boundingRect(contours_poly[i]);
                        rectangle(img_roi, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 2, 8, 0);
                        minRect[i].points(rect_point);
                        for (size_t k = 0; k < 4; k++) {
                            line(img_roi, rect_point[k], rect_point[(k + 1) % 4], Scalar(0, 255, 0), 2, 8);
                        }

                    }
                }

            }
            imshow("Original_image", img);
            imshow("Gray_image", img_gray);
           // imshow("Thresholded_image", img_threshold);
            imshow("ROI", img_roi);
            imshow("YCbCr", img_YCbCr);
            if (waitKey(30) == 27) {
                return -1;
            }

        }

    }

    return 0;
}*/

