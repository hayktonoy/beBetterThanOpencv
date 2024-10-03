#include "WarppingTwoImagesWithFeatureMatcher.h"

#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/aruco.hpp"
#include "ArucoDetector.h"
#include "Utils.h"
#include <iostream>


WarppingTwoImagesWithFeatureMatcher::WarppingTwoImagesWithFeatureMatcher(const Options &opts)
    : m_opts(opts)
{

}

void WarppingTwoImagesWithFeatureMatcher::warpWithArucoDetectionLineIntesectionPoints(const cv::Mat &img1, const cv::Mat &img2, int id, ArucoDetector::DetectorParams detectorParams, bool imshowResult)
{
    m_img1 = img1;
    m_img2 = img2;

    cv::Mat gray1, gray2;
    cv::cvtColor(img1, gray1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(img2, gray2, cv::COLOR_BGR2GRAY);

    detectorParams.detectorParams.cornerRefinementMethod = cv::aruco::CORNER_REFINE_CONTOUR;
    ArucoDetector ar1(detectorParams);

    ar1.processImg(img1);
    std::vector<cv::Point2f> corners1 = ar1.getMarkerById(id);

    if(corners1.empty())return;
    cv::Rect bounding1 = cv::boundingRect(corners1);

    ar1.processImg(img2);
    std::vector<cv::Point2f> corners2 = ar1.getMarkerById(id);

    if(corners2.empty())return;


    cv::Rect bounding2 = cv::boundingRect(corners2);

    Utils::extendRect(bounding1, img1.size(), 200);
    Utils::extendRect(bounding2, img2.size(), 200);

    cv::Mat cropped1 = img1(bounding1).clone();
    cv::Mat cropped2 = img2(bounding2).clone();

    m_homographyMatrix = cv::findHomography(corners1, corners2, cv::RANSAC);

    cv::warpPerspective(img1, m_img1WarpedToImg2, m_homographyMatrix      , img2.size());
    cv::warpPerspective(img2, m_img2WarpedToImg1, m_homographyMatrix.inv(), img1.size());


    cv::Mat gray1Warped;
    cv::cvtColor(m_img1WarpedToImg2, gray1Warped, cv::COLOR_BGR2GRAY);

    cv::Mat th1, th2;
    cv::threshold(gray2(bounding2), th2, 0, 255, cv::THRESH_OTSU | cv::THRESH_BINARY);
    cv::threshold(gray1Warped(bounding2), th1, 0, 255, cv::THRESH_OTSU | cv::THRESH_BINARY);
    if(imshowResult)
    {
        cv::namedWindow("cropped1", cv::WINDOW_NORMAL);
        cv::namedWindow("cropped2", cv::WINDOW_NORMAL);
        cv::namedWindow("croppedW", cv::WINDOW_NORMAL);
        cv::imshow("cropped1", cropped1);
        cv::imshow("cropped2", cropped2);
        cv::imshow("croppedW", m_img1WarpedToImg2(bounding2));

        cv::namedWindow("img1", cv::WINDOW_NORMAL);
        cv::namedWindow("img2", cv::WINDOW_NORMAL);
        cv::imshow("img1", img1);
        cv::imshow("img2", img2);

        cv::namedWindow("m_img1WarpedToImg2", cv::WINDOW_NORMAL);
        cv::namedWindow("m_img2WarpedToImg1", cv::WINDOW_NORMAL);
        cv::imshow("m_img1WarpedToImg2", m_img1WarpedToImg2);
        cv::imshow("m_img2WarpedToImg1", m_img2WarpedToImg1);

        cv::namedWindow("xor", cv::WINDOW_NORMAL);
        cv::imshow("xor", (th1 ^ th2));

        cv::namedWindow("th1", cv::WINDOW_NORMAL);
        cv::namedWindow("th2", cv::WINDOW_NORMAL);
        cv::imshow("th1", th1);
        cv::imshow("th2", th2);
        cv::waitKey();
    }

    //    {

    //        cv::aruco::DetectorParameters params;
    //        params.cornerRefinementMethod = cv::aruco::CORNER_REFINE_NONE;
    //        ArucoDetector ar((ArucoDetector::DetectorParams(cv::aruco::DICT_4X4_50, params)));

    //        ar.processImg(img1);
    //        std::vector<cv::Point2f> corners1 = ar.getMarkerById(id);
    //        cv::cornerSubPix(gray1, corners1, winSize, zeroZone, criteria);
    //        if(corners1.empty())return;

    //        std::vector<cv::Point2f> corners2(corners1.size());
    //        cv::perspectiveTransform(corners1, corners2, m);

    //        m_resPointsInImg1 = corners1;
    //        m_resPointsInImg2 = corners2;

    //        cv::namedWindow("img2", cv::WINDOW_NORMAL);
    //        auto r = cv::boundingRect(corners2);
    //        Utils::extendRect(r, img2.size(), 200);

    //        cv::imshow("img2", img2(r));
    //        cv::waitKey(0);

    //        cv::namedWindow("_img1", cv::WINDOW_NORMAL);
    //        cv::namedWindow("_img2", cv::WINDOW_NORMAL);
    //        cv::imshow("_img1", img1);
    //        cv::imshow("_img2", img2);

    //    }
}


cv::Mat WarppingTwoImagesWithFeatureMatcher::getImg1WarpedOnImg2() const
{
    return m_img1WarpedToImg2;
}

cv::Mat WarppingTwoImagesWithFeatureMatcher::getImg2WarpedOnImg1() const
{
    return m_img2WarpedToImg1;
}

cv::Mat WarppingTwoImagesWithFeatureMatcher::get1To2HomographyMatrix() const
{
    return m_homographyMatrix;
}
