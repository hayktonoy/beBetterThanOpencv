#pragma once
#include <opencv2/opencv.hpp>
class ArucoDetector;
// namespace std{
// class vector;
// }

#include <vector>
class TesterToliInch
{
public:
    TesterToliInch();
    void startCamera();
private:
    std::vector<std::vector<cv::Point2f>> origFunction(cv::Mat &img);
    std::vector<std::vector<cv::Point2f>> normalFunction(cv::Mat &img);

private:
    ArucoDetector *m_detector = nullptr;
    std::vector<std::vector<cv::Point2f>> m_corners;
    bool m_firstFrame = true;
    std::vector<std::vector<cv::Point2f>> m_lastCornerOrig;
    std::vector<std::vector<cv::Point2f>> m_lastCornerNormal;
};
