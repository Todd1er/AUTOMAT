#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/videoio.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>


using namespace std;
using namespace cv;

struct linee {
    int x;
    int y;
};

struct bebra {
    linee RB;
    linee RG;
};

class detector {
public:
    const float minTargetRadius = 800;
    const float PI = 3.14;
    const float length = 20;
    bool enableRadiusCulling = 0;
    int frame_counter = 0;
    float angle_sum = 0;
    float dist_sum = 0;
    float a = 0;
    float d = 0;
    VideoCapture cap;
    Mat rgb_frame;
    Mat hsv_frame;
    Mat threshold_frame_green;
    Mat threshold_frame_blue;
    Mat threshold_frame_red;
    Mat frame;
    Mat img_res;

    detector() {
        cap.open(0);
        if (!cap.isOpened()) {
            cout << "ERROR! Unable to open camera\n";
        }
    }
    
    Mat pict() {
        cap >> frame;
        return frame;
    }

    void upd() {
        img_res = pict();
        img_res = find_colors(img_res);
    }

    Mat find_colors(Mat rgb_frame) {   
        cvtColor(rgb_frame, hsv_frame, COLOR_BGR2HSV);
        Scalar   min(40, 40, 40);
        Scalar   max(70, 250, 250);
        inRange(hsv_frame, min, max, threshold_frame_green);
        Scalar   minb(90, 40, 40);
        Scalar   maxb(120, 250, 250);
        inRange(hsv_frame, minb, maxb, threshold_frame_blue);
        Scalar   minr(170, 40, 40);
        Scalar   maxr(180, 250, 250);
        inRange(hsv_frame, minr, maxr, threshold_frame_red);
        Mat str_el = getStructuringElement(MORPH_RECT, Size(25, 25));
        morphologyEx(threshold_frame_green, threshold_frame_green, MORPH_OPEN, str_el);
        morphologyEx(threshold_frame_green, threshold_frame_green, MORPH_CLOSE, str_el);
        morphologyEx(threshold_frame_blue, threshold_frame_blue, MORPH_OPEN, str_el);
        morphologyEx(threshold_frame_blue, threshold_frame_blue, MORPH_CLOSE, str_el);
        morphologyEx(threshold_frame_red, threshold_frame_red, MORPH_OPEN, str_el);
        morphologyEx(threshold_frame_red, threshold_frame_red, MORPH_CLOSE, str_el);
        imshow("Live_hsv", hsv_frame);
        imshow("Live_threshold_green", threshold_frame_green);
        imshow("Live_threshold_blue", threshold_frame_blue);
        imshow("Live_threshold_red", threshold_frame_red);
        imshow("Live_rgb", rgb_frame);
        calculate(threshold_frame_green, threshold_frame_blue, threshold_frame_red);
        return (rgb_frame);
    }

    void calculate(Mat threshold_frame_green, Mat threshold_frame_blue, Mat threshold_frmae_red) {
        vector<vector<Point> > contours_g;
        vector<Vec4i> heirarchy_g;
        vector<Point2i> center_g;
        vector<int> radius_g;

        findContours(threshold_frame_green.clone(), contours_g, heirarchy_g, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

        size_t count_g = contours_g.size();

        for (int i = 0; i < count_g; i++)
        {
            Point2f c;
            float r;
            minEnclosingCircle(contours_g[i], c, r);

            if (!enableRadiusCulling || r >= minTargetRadius)
            {
                center_g.push_back(c);
                radius_g.push_back(r);
            }
        }

        vector<vector<Point> > contours_b;
        vector<Vec4i> heirarchy_b;
        vector<Point2i> center_b;
        vector<int> radius_b;

        findContours(threshold_frame_blue.clone(), contours_b, heirarchy_b, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

        size_t count_b = contours_b.size();

        for (int i = 0; i < count_b; i++)
        {
            Point2f c;
            float r;
            minEnclosingCircle(contours_b[i], c, r);

            if (!enableRadiusCulling || r >= minTargetRadius)
            {
                center_b.push_back(c);
                radius_b.push_back(r);
            }
        }
        vector<vector<Point> > contours_r;
        vector<Vec4i> heirarchy_r;
        vector<Point2i> center_r;
        vector<int> radius_r;

        findContours(threshold_frame_red.clone(), contours_r, heirarchy_r, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

        size_t count_r = contours_r.size();

        for (int i = 0; i < count_r; i++)
        {
            Point2f c;
            float r;
            minEnclosingCircle(contours_r[i], c, r);

            if (!enableRadiusCulling || r >= minTargetRadius)
            {
                center_r.push_back(c);
                radius_r.push_back(r);
            }
        }

        Scalar red(255, 0, 0);
        Scalar blue(0, 255, 0);
        Scalar green(0, 0, 255);

        linee RB;
        linee RG;

        for (int i = 0; i < center_b.size(); i++)
        {
            circle(rgb_frame, center_b[i], radius_b[i], red, 3);
            for (int j = 0; j < center_g.size(); j++) {
                circle(rgb_frame, center_g[j], radius_g[j], blue, 3);


                for (int q = 0; q < center_r.size(); q++)
                {
                    circle(rgb_frame, center_r[q], radius_r[q], green, 3);
                    RB.x = center_b[0].x - center_r[0].x;
                    RB.y = center_b[0].y - center_r[0].y;
                    RG.x = center_g[0].x - center_r[0].x;
                    RG.y = center_g[0].y - center_r[0].y;
                }
            }
        }
        bebra answ;
        answ.RB = RB;
        answ.RG = RG;
        a = angle(answ);
        d = dist(answ);
        if (frame_counter % 24 == 0) {
            cout << a << " " << d << " " << frame_counter << endl;
            angle_sum = 0;
            dist_sum = 0;
        }
    }

    float angle(bebra answ) {
        frame_counter++;
        angle_sum += acos((answ.RB.x * answ.RG.x + answ.RB.y * answ.RG.y) / (sqrt(answ.RB.x * answ.RB.x + answ.RB.y * answ.RB.y) * sqrt(answ.RG.x * answ.RG.x + answ.RG.y * answ.RG.y))) * 180 / PI;
        if (frame_counter % 24 == 0)
        return (angle_sum/24);
        
    }

    float dist(bebra answ) {
        dist_sum += sqrt(answ.RG.x * answ.RG.x + answ.RG.y * answ.RG.y) / sqrt(answ.RB.x * answ.RB.x + answ.RB.y * answ.RB.y) * length;
        if (frame_counter % 24 == 0)
        return (dist_sum/24);
    }
    
    };


    int main()
    {
        detector det;
        for (;;) {
            det.upd();
            if (waitKey(5) >= 0)
                break;
            }
        return 0;
    }
