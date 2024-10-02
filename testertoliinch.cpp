#include "testertoliinch.h"
#include "arucodetector.h"
TesterToliInch::TesterToliInch()
{
    m_detector = new ArucoDetector();
    m_lastCornerNormal = std::vector<std::vector<cv::Point2f>>(51);
    m_lastCornerOrig = std::vector<std::vector<cv::Point2f>>(51);
}

void TesterToliInch::startCamera()
{
    cv::VideoCapture cap;
    cap.open(0);
    cv::Mat frame;
    while (true)
    {
        cap >> frame;
        m_detector->processImg(frame);
        auto corners1 = origFunction(frame);
        auto corners2 = normalFunction(frame);
        // for (auto x : corners1)
        // {
        //     for (auto y : x) std::cout << y << " ";
        //     std::cout << std::endl;
        // }
        auto ids = m_detector->getMarkerIds();
        if (!m_firstFrame)
        {
            for (size_t i = 0; i < ids.size(); ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    std::cout << cv::norm(corners1[i][j] - m_lastCornerOrig[ids[i]][j]) << " ";
                    std::cout << cv::norm(corners2[i][j] - m_lastCornerNormal[ids[i]][j]) << std::endl;
                }
            }
        }

        for (size_t i = 0; i < ids.size(); ++i)
        {
            // std::cout << ids[i] << std::endl;
            m_lastCornerNormal[ids[i]] = corners2[i];
            m_lastCornerOrig[ids[i]] = corners1[i];
        }
        m_firstFrame = false;
        char t = cv::waitKey(300);
        if (t == 'q')
        {
            break;
        }
    }
}

std::vector<std::vector<cv::Point2f> > TesterToliInch::origFunction(cv::Mat &img)
{
    m_detector->processImg(img);
    auto corners = m_detector->getMarkerCorners();

    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 5000, 0.01);
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    for (auto &corner : corners)cv::cornerSubPix(gray, corner, cv::Size(5, 5), cv::Size(-1, -1), criteria);
    return corners;
}

std::vector<std::vector<cv::Point2f> > TesterToliInch::normalFunction(cv::Mat &img)
{
    m_detector->processImg(img);
    m_detector->processImgMultipleSubPix();
    return m_detector->getMarkerCorners();
}

