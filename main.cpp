#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

#include <arucodetector.h>

void colorCorners(std::vector<cv::Point2f> &corners, cv::Mat &R, const cv::Vec3b &color = cv::Vec3b(0,0,255))
{

    for (size_t j = 0; j < corners.size(); ++j)
    {
        // std::cout << corners[j];
        R.at<cv::Vec3b>(corners[j]) = color;
    }
    // std::cout << std::endl;
    // std::cout << std::endl;
}

std::vector<cv::Point2f> algorithm(cv::Mat &frame)
{
    // double alpha = 1.5;  // Contrast control (1.0-3.0)
    // int beta = 50;       // Brightness control (0-100)

    // // Adjust the brightness and contrast
    // cv::Mat adjusted;
    // frame.convertTo(frame, -1, alpha, beta);

    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::makePtr<cv::aruco::Dictionary>(cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50));
    // cv::Ptr<cv::aruco::Dictionary> dictionary = cv::makePtr<cv::aruco::Dictionary>(cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_ORIGINAL));
    std::vector<std::vector<cv::Point2f>> corners;
    std::vector<int> ids;
    cv::Mat grey;
    // cv::Mat greyTh;
    cv::cvtColor(frame, grey, cv::COLOR_RGB2GRAY);
    // cv::threshold(grey, greyTh, 50, 255, cv::THRESH_OTSU);


    cv::aruco::detectMarkers(grey, dictionary, corners, ids);
    // cv::Mat greyWarped;
    if (ids.size()) {
        cv::Size winSize  = cv::Size( 10, 10 );
        cv::Size zeroZone = cv::Size( -1, -1 );
        cv::TermCriteria criteria = cv::TermCriteria( cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 50000, 0.00001 );
        // cv::Mat cornersMat;

        // for (const auto& corner : corners) {
        //     cornersMat.push_back(corner);
        // }

        // cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);
        // winSize = cv::Size(7, 7);
        // cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);
        // winSize = cv::Size(5, 5);
        // cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);
        // winSize = cv::Size(3, 3);
        // cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);
        // winSize = cv::Size(2, 2);
        // cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);

        for (int i = 20; i >= 1; i--) {
           std::cout << i << " " << i << std::endl;
           cv::cornerSubPix(grey, corners[0], cv::Size(i, i), zeroZone, criteria);
        }

        // std::vector<cv::Point2f> targetCorners = {
        //     cv::Point2f(0, 0),
        //     cv::Point2f(100, 0),
        //     cv::Point2f(100, 100),
        //     cv::Point2f(0, 100)
        // };

        // // Get the perspective transform


        // corners.clear();
        // ids.clear();
        // std::vector<cv::Point2f> orig_corners;

        // cv::aruco::detectMarkers(frame, dictionary, corners, ids);
        if (ids.size())
        {
            // cv::Mat M = cv::getPerspectiveTransform(corners[0], targetCorners);
            // cv::warpPerspective(grey, greyWarped, M, cv::Size(100, 100));
            // cv::aruco::detectMarkers(frame, dictionary, corners, ids);
            std::cout << "---------\nid: " << ids.size() << std::endl;
            // cv::perspectiveTransform(corners[0], orig_corners, M.inv());
            // for (auto &x : orig_corners) std::cout << x << std::endl;
            // std::cout << frame.size() << std::endl;
            colorCorners(corners[0], frame);

            // for (size_t i = 0; i < corners.size(); ++i)
            // {
            //     cv::Vec3b color(0,0,255);
            //     for (size_t j = 0; j < corners[i].size(); ++j)
            //     {
            //         frame.at<cv::Vec3b>(corners[i][j]) = color;
            //         std::cout << corners[i][j] << " ";
            //     }
            //     std::cout << std::endl;
            // }
            // cv::imshow("grey", greyWarped);

            cv::imshow("new window", frame);
            return corners[0];
        }

    }
    else cv::imshow("new window", frame);
    return {};
}

void testMaybeBetterMaybeNotIDontKnowProbablyNot(cv::Mat &frame, std::vector<cv::Point2f> &corners)
{
    if (!corners.size()) cv::imshow("testWindow", corners);
    std::vector<cv::Point> int_corners(corners.begin(), corners.end());
    std::cout << "hello world!" << std::endl;
    // Draw the rectangle using the 4 corners as a closed polygon
    const cv::Point* pts[1] = { int_corners.data() };
    int npts = int_corners.size();

    cv::Mat grey, greyTh, xored, blank;
    blank = cv::Mat::zeros(cv::Size(900, 900), CV_8UC1);
    cv::cvtColor(frame, grey, cv::COLOR_RGB2GRAY);
    cv::threshold(grey, greyTh, 0, 255, cv::THRESH_OTSU);
    // cv::rectangle(grey, cv::Rect(corners[0], corners[1] ));

    cv::fillPoly(blank, pts, &npts, 1, cv::Scalar(255));

    cv::imshow("testWindow", blank);
    cv::namedWindow("debugWindow");


}


int main()
{
    ArucoDetector detector(cv::aruco::DICT_4X4_50);
    cv::Mat img = cv::imread("C:/Users/archLinux/Downloads/img2.png");
    cv::Mat imgOrig, imgNormal;

    detector.processImg(img.clone());
    auto corners = detector.getMarkerCorners();

    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 5000, 0.01);
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    for (auto &corner : corners)cv::cornerSubPix(gray, corner, cv::Size(5, 5), cv::Size(-1, -1), criteria);
    img.copyTo(imgOrig);
    for (auto &x : corners) colorCorners(x, imgOrig, cv::Vec3b(255, 0, 0));
    cv::imshow("originalSubPix", imgOrig);

    corners=detector.getMarkerCorners();
    detector.processImgMultipleSubPix();
    corners=detector.getMarkerCorners();
    img.copyTo(imgNormal);
    for (auto &x : corners) colorCorners(x, imgNormal);
    cv::imshow("normal", imgNormal);
    // cv::namedWindow("testingWindow");
    // cv::imshow("testingWindow", img);
    cv::waitKey();
    return 0;
}
