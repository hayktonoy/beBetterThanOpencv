#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

#include <arucodetector.h>

cv::Mat colorCorners(cv::Mat &frame, std::vector<std::vector<cv::Point2f>> corners)
{
    cv::Mat colored;
    frame.copyTo(colored);
    for (auto &x : corners)
    {
        for (auto &y : x)
        {
            colored.at<cv::Vec3b>(y) = cv::Vec3b(0, 0, 255);
        }
    }
    return colored;
}

void refineCorners(const cv::Mat& frame, std::vector<cv::Point2f>& corners) {
    // Parameters for Shi-Tomasi corner detection
    int maxCorners = 1;
    double qualityLevel = 0.01;
    double minDistance = 1;
    int blockSize = 3;
    bool useHarrisDetector = false;
    double k = 0.04;

    cv::Mat image;
    cv::cvtColor(frame, image, cv::COLOR_BGR2GRAY);

    cv::Size winSize = cv::Size(5, 5);
    cv::Size zeroZone = cv::Size(-1, -1);
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 0.001);

    for (int i = 0; i < corners.size(); ++i) {
        int patchSize = 15;
        cv::Rect roi(corners[i].x - patchSize / 2, corners[i].y - patchSize / 2, patchSize, patchSize);

        roi &= cv::Rect(0, 0, image.cols, image.rows);

        cv::Mat cornerPatch = image(roi);

        std::vector<cv::Point2f> detectedCorners;
        goodFeaturesToTrack(cornerPatch, detectedCorners, maxCorners, qualityLevel, minDistance, cv::Mat(), blockSize, useHarrisDetector, k);

        if (detectedCorners.size() > 0) {
            cv::Point2f refinedCorner = detectedCorners[0] + cv::Point2f(roi.x, roi.y);

            cornerSubPix(image, std::vector<cv::Point2f>{refinedCorner}, winSize, zeroZone, criteria);

            corners[i] = refinedCorner;
        }
    }
}

int main()
{
    // TesterToliInch tester;
    // tester.startCamera();
    ArucoDetector detector((ArucoDetector::DetectorParams(cv::aruco::DICT_ARUCO_ORIGINAL)));
    cv::VideoCapture cap;
    cap.open(0);
    cv::Mat frame;

    while (true)
    {
        cap >> frame;
        // imshow("main window", frame);
        char t = cv::waitKey(25);
        detector.processImg(frame);
        detector.processImgMultipleSubPixXelaci();
        std::vector<std::vector<cv::Point2f>> corners = detector.getMarkerCorners();
        for(size_t i = 0; i < corners.size(); ++i) refineCorners(frame, corners[i]);
        imshow("new window", colorCorners(frame, corners));
        if (t == 27)
        {
            break;
        }
    }

    return 0;
}
