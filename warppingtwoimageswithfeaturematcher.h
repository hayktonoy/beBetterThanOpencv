#pragma once
#include "opencv2/core/mat.hpp"
#include "ArucoDetector.h"

class WarppingTwoImagesWithFeatureMatcher
{
public:
    struct Options
    {
        int warpImageIndex;
        Options(int _warpImageIndex = 2) : warpImageIndex(_warpImageIndex)
        {

        }
    };
    struct Result
    {
        cv::Mat img1;
        cv::Mat img2;
        cv::Mat warppedImg1;
        cv::Mat warppedImg2;
        cv::Mat xorOfWarppedImgs;


    };

public:
    WarppingTwoImagesWithFeatureMatcher(const Options &opts = Options());

    void warpWithArucoDetectionLineIntesectionPoints(const cv::Mat &img1, const cv::Mat &img2, int id, ArucoDetector::DetectorParams detectorParams, bool imshowResult = false);

    cv::Mat getImg1WarpedOnImg2() const;
    cv::Mat getImg2WarpedOnImg1() const;
    cv::Mat get1To2HomographyMatrix() const; // returns matrix that projects img1 points to img2

private:
    Options m_opts;

    cv::Mat m_img1, m_img2;
    cv::Mat m_img1WarpedToImg2;
    cv::Mat m_img2WarpedToImg1;

    cv::Mat m_homographyMatrix;
};
