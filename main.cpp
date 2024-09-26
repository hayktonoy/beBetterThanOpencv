#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

void colorCorners(std::vector<cv::Point2f> &corners, cv::Mat &R)
{
    cv::Vec3b color(0,0,255);
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
    // cv::Ptr<cv::aruco::Dictionary> dictionary = cv::makePtr<cv::aruco::Dictionary>(cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100));
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::makePtr<cv::aruco::Dictionary>(cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_ORIGINAL));
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

        cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);
        winSize = cv::Size(7, 7);
        cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);
        winSize = cv::Size(5, 5);
        cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);
        winSize = cv::Size(3, 3);
        cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);
        winSize = cv::Size(2, 2);
        cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);
        // cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);
        // cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);
        // cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);
        // cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);
        // cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);
        // cv::cornerSubPix(grey, corners[0], winSize, zeroZone, criteria);
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


}


int main()
{
    // cv::VideoCapture cap;
    // cap.open(0);
    cv::Mat frame;
    cv::namedWindow("new window", cv::WINDOW_NORMAL);
    // while (true)
    // {
    //     cap.read(frame);
    //     algorithm(frame);
    //     // cv::imshow("new window", frame);
    //     char t = cv::waitKey(25);
    //     if (t == 27) break;
    // }
    cv::VideoCapture cap(0);
    cv::Mat cameraFrame;
    while (true)
    {
        cap >> cameraFrame;
        if (cameraFrame.empty())
        {
            return -1;
            break;
        }
        algorithm(cameraFrame);
        char t = cv::waitKey(25);
        if (t == 27)
            break;
    }
    // frame = cv::imread("C:/Users/archLinux/Downloads/image.png");
    // std::vector<cv::Point2f> found_corners = algorithm(frame);

    // cv::namedWindow("testWindow", cv::WINDOW_NORMAL);
    // testMaybeBetterMaybeNotIDontKnowProbablyNot(frame, found_corners);
    // cv::waitKey(0);

    return 0;
}
