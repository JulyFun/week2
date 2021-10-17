#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
using namespace std;
using namespace cv;


int main() {
    const int bw = 9, bh = 6;
    const int bn = bw * bh;
    // 棋盘大小
    Size bsize(bw, bh);
    Mat gray, draw;
    vector< Point2f > pbuf;
    vector< vector<Point2f> > p;
    int found = 0, suc = 0;
    Size imgsize;
    int cnt = 0;
    int k = 0, n = 0;
    for (int i = 0; i < 24; ++i) {
        cout << "For pic " << i << ":" << endl;
        Mat src = imread(string("../src/") + __cxx11::to_string(i).append("_orig.jpg"));
        // Mat src = imread(__cxx11::to_string(i).append(".jpg"));
        // 读入的时候是BGR
        
        if (src.data == NULL) {
            cout << "imread FAILED." << endl;
            return 0;
            //
        }
        cvtColor(src, src, COLOR_BGR2GRAY);
        imshow("win", src);
      //  waitKey(300);
        float alp = 4.0;
        float beta = 15;
        Mat dst = Mat::zeros(src.size(), src.type());
        Mat m1;
        src.convertTo(m1, CV_32F);

        int h = src.rows;
        int w = src.cols;

        for (int row = 0; row < h; ++row)
            for (int col = 0; col < w; ++col) {
                if (src.channels() == 3) {
                    float b = m1.at<Vec3f>(row, col)[0];
                    float g = m1.at<Vec3f>(row, col)[1];
                    float r = m1.at<Vec3f>(row, col)[2];
                    dst.at<Vec3b>(row, col)[0] = saturate_cast<uchar>(b * alp + beta);
                    dst.at<Vec3b>(row, col)[1] = saturate_cast<uchar>(g * alp + beta);
                    dst.at<Vec3b>(row, col)[2] = saturate_cast<uchar>(r * alp + beta);
                }
                else if (src.channels() == 1) {
                    float v = src.at<uchar>(row, col);
                    dst.at<uchar>(row, col) = saturate_cast<uchar>(v * alp + beta);
                }
            }
        imshow("win", dst);



        // waitKey(300);
        // 读入图片
        if (cnt == 0) {
            imgsize.width = src.cols;
            imgsize.height = src.rows;
        }
        found = findChessboardCorners(dst, bsize, pbuf);

        cout << found << ' ' << pbuf.size() << endl;
        if (found and pbuf.size() == bn) {
            ++suc;
            find4QuadCornerSubpix(dst, pbuf, Size(5, 5));
            // 寻找边界，反正要用
            p.push_back(pbuf);
            draw = src.clone();
            drawChessboardCorners(draw, bsize, pbuf, found);
            // 画上去
            imshow("win", draw);
            imwrite(string("../output/res_" + __cxx11::to_string(i) + ".jpg"), draw);

            // waitKey(300);
        }
        else {
            cout << "\t404" << endl;

        }
        pbuf.clear();
        ++cnt;
    }
    cout << suc << "usedful b;" << endl;
    // 成功的图片
    Size sqsiz(10, 10);
    vector<vector<Point3f> > gp;
    vector<Point3f> gpbuf;
    vector<int> pcount;
    Mat cam(3, 3, CV_32FC1, Scalar::all(0));
    Mat dis(1, 5, CV_32FC1, Scalar::all(0));
    vector<Mat> rs;
    vector<Mat> ts;
    for (int i = 0; i < suc; ++i) {
        for (int j = 0; j < bh; ++j)
            for (int k = 0; k < bw; ++k) {
                Point3f pt;
                pt.x = k * sqsiz.width;
                pt.y = j * sqsiz.height;
                pt.z = 0;
                gpbuf.push_back(pt);
                // gpbuf 存一个空间
            }
        gp.push_back(gpbuf);
        gpbuf.clear();
        pcount.push_back(bh * bw);
    }
    cout << calibrateCamera(gp, p, imgsize, cam, dis, rs, ts);
    cout << cam << endl
        << dis << endl;

    return 0;
}
