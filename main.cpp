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

void algorithm(cv::Mat &frame)
{
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::makePtr<cv::aruco::Dictionary>(cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100));
    std::vector<std::vector<cv::Point2f>> corners;
    std::vector<int> ids;
    cv::Mat grey, greyTh;
    cv::cvtColor(frame, grey, cv::COLOR_RGB2GRAY);
    cv::threshold(grey, greyTh, 50, 255, cv::THRESH_OTSU);


    cv::aruco::detectMarkers(greyTh, dictionary, corners, ids);
    cv::Mat greyWarped;
    if (ids.size()) {
        cv::Size winSize  = cv::Size( 10, 10 );
        cv::Size zeroZone = cv::Size( -1, -1 );
        cv::TermCriteria criteria = cv::TermCriteria( cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 5000, 0.0001 );
        // cv::Mat cornersMat;

        // for (const auto& corner : corners) {
        //     cornersMat.push_back(corner);
        // }

        cv::cornerSubPix(greyTh, corners[0], winSize, zeroZone, criteria);
        std::vector<cv::Point2f> targetCorners = {
            cv::Point2f(0, 0),
            cv::Point2f(100, 0),
            cv::Point2f(100, 100),
            cv::Point2f(0, 100)
        };

        // Get the perspective transform


        corners.clear();
        ids.clear();
        std::vector<cv::Point2f> orig_corners;

        cv::aruco::detectMarkers(greyTh, dictionary, corners, ids);
        if (ids.size())
        {
            cv::Mat M = cv::getPerspectiveTransform(corners[0], targetCorners);
            cv::warpPerspective(grey, greyWarped, M, cv::Size(100, 100));
            cv::aruco::detectMarkers(greyWarped, dictionary, corners, ids);
            std::cout << "---------\nid: " << ids.size() << std::endl;
            cv::perspectiveTransform(corners[0], orig_corners, M.inv());
            for (auto &x : orig_corners) std::cout << x << std::endl;
            std::cout << frame.size() << std::endl;
            // colorCorners(orig_corners, frame);

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
        }

    }
    else cv::imshow("new window", grey);
}

int main()
{
    cv::VideoCapture cap;
    cap.open(0);
    if (!cap.isOpened())
    {
        std::cout << "can not open camera";
        return 1;
    }
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

    frame = cv::imread("C:/Users/archLinux/Downloads/image.png");
    algorithm(frame);
    cv::waitKey(0);

    return 0;
}
