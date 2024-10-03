#include "arucodetector.h"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc.hpp"

ArucoDetector::ArucoDetector(const DetectorParams &params) : m_params(params)
{
    m_dictionary = cv::makePtr<cv::aruco::Dictionary>(cv::aruco::getPredefinedDictionary(m_params.arucoType));
    m_detectorParams= new cv::aruco::DetectorParameters(m_params.detectorParams);
}

void ArucoDetector::setNewParams(const DetectorParams &params)
{
    m_params = params;
    m_dictionary = cv::makePtr<cv::aruco::Dictionary>(cv::aruco::getPredefinedDictionary(m_params.arucoType));
}

void ArucoDetector::processImg(const cv::Mat &img)
{
    m_img = img;
    m_markerCorners.clear();
    m_markerIds.clear();
    cv::aruco::detectMarkers(img, m_dictionary, m_markerCorners, m_markerIds, m_detectorParams);
}

void ArucoDetector::processImg(const cv::Mat &img, std::vector<int> &markerIds, std::vector<std::vector<cv::Point2f> > &markerCorners)
{
    m_img = img;
    m_markerCorners.clear();
    m_markerIds.clear();
    cv::aruco::detectMarkers(img, m_dictionary, m_markerCorners, m_markerIds, m_detectorParams);
    markerCorners = m_markerCorners;
    markerIds = m_markerIds;
}

void ArucoDetector::processImgMultipleSubPix()
{
    cv::Mat gray;
    cv::cvtColor(m_img, gray, cv::COLOR_BGR2GRAY);
    // cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);
    cv::Size zeroZone = cv::Size(-1, -1);
    cv::Size winSize = cv::Size(15, 15);
    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 5000, 0.001); // Adjusted criteria

    for (size_t i = 0; i < m_markerCorners.size(); ++i) {
        for (int j = 15; j >= 3; j--) {
            cv::cornerSubPix(gray, m_markerCorners[i], cv::Size(j, j), zeroZone, criteria);
        }
    }
}

void ArucoDetector::processImgMultipleSubPixXelaci()
{
    cv::Mat gray, blurred, threshed;
    cv::cvtColor(m_img, gray, cv::COLOR_BGR2GRAY);
    cv::bilateralFilter(gray, blurred, 9, 75, 75);
    // cv::adaptiveThreshold(blurred, threshed, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 2);
    cv::threshold(blurred, threshed, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    cv::Size zeroZone = cv::Size(-1, -1);
    cv::Size winSize = cv::Size(5, 5);
    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 0.001); // Adjusted criteria

    for (size_t i = 0; i < m_markerCorners.size(); ++i) {
        cv::cornerSubPix(threshed, m_markerCorners[i], winSize, zeroZone, criteria);
    }

    int maxCorners = 1;
    double qualityLevel = 0.001;
    double minDistance = 1;
    int blockSize = 3;
    bool useHarrisDetector = false;
    double k = 0.04;

    cv::Mat image = blurred;

    // cv::aruco::detectMarkers(image, m_dictionary, m_markerCorners, m_markerIds, m_detectorParams);

    for (int j = 0; j < m_markerCorners.size(); ++j)
    {
        for (int i = 0; i < m_markerCorners[j].size(); ++i) {
            int patchSize = 15;
            cv::Rect roi(m_markerCorners[j][i].x - patchSize / 2, m_markerCorners[j][i].y - patchSize / 2, patchSize, patchSize);

            roi &= cv::Rect(0, 0, image.cols, image.rows);

            cv::Mat cornerPatch = image(roi);

            std::vector<cv::Point2f> detectedCorners;
            goodFeaturesToTrack(cornerPatch, detectedCorners, maxCorners, qualityLevel, minDistance, cv::Mat(), blockSize, useHarrisDetector, k);

            if (detectedCorners.size() > 0) {
                cv::Point2f refinedCorner = detectedCorners[0] + cv::Point2f(roi.x, roi.y);

                cornerSubPix(image, std::vector<cv::Point2f>{refinedCorner}, winSize, zeroZone, criteria);

                m_markerCorners[j][i] = refinedCorner;
            }
        }
    }
}

std::vector<cv::Point2f> ArucoDetector::getMarkerById(int id) const
{
    for(int i = 0; i < m_markerIds.size(); ++i)
    {
        if(m_markerIds[i] == id)
        {
            return m_markerCorners.at(i);
        }
    }
    return std::vector<cv::Point2f>();
}

std::vector<std::vector<cv::Point2f> > ArucoDetector::getMarkerCorners() const
{
    return m_markerCorners;
}

std::vector<int> ArucoDetector::getMarkerIds() const
{
    return m_markerIds;
}

bool ArucoDetector::getArucoInsideCornersAndGeometryById(int arucoId, std::vector<cv::Point2f> &allCorners, std::vector<cv::Point> &cornersByIndexOnArucoGrid, int arucoGridSize)
{
    const auto corners = getMarkerById(arucoId);
    if(corners.empty())return false;

    cornersByIndexOnArucoGrid.clear();
    allCorners.clear();

    cv::Rect t = cv::boundingRect(corners);
    int mx = std::max(t.height, t.width);
    mx += arucoGridSize - (mx % arucoGridSize);

    cv::Mat imgWarped;

    cv::Size resSize(mx, mx);

    std::vector<cv::Point2f> pnts {cv::Point2f(0, 0),
                                  cv::Point2f(0, resSize.height - 1),
                                  cv::Point2f(resSize.width - 1, resSize.height - 1),
                                  cv::Point2f(resSize.width - 1, 0)};

    cv::Mat m = cv::findHomography(corners, pnts);
    cv::warpPerspective(m_img, imgWarped, m, resSize);

    cv::Mat warpedGray;
    cv::cvtColor(imgWarped, warpedGray, cv::COLOR_BGR2GRAY);

    cv::Mat thresh;
    cv::threshold(warpedGray, thresh, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    cv::Size gridRectSize(imgWarped.size() / arucoGridSize);

    std::vector<std::vector<int>> meanValues(arucoGridSize, std::vector<int>(arucoGridSize));
    for(int i = 0; i < arucoGridSize; ++i)
    {
        for(int j = 0; j < arucoGridSize; ++j)
        {
            int cur = cv::mean(thresh(cv::Rect(i * gridRectSize.width, j * gridRectSize.height, gridRectSize.width, gridRectSize.height)))[0];
            if(cur > 125)cur = 255;
            else cur = 0;
            meanValues[i][j] = cur;
        }
    }

    std::vector<cv::Point2f> insideCorners;
    std::vector<cv::Point> insideCornersGridPos;

    for(int i = 1; i < arucoGridSize; ++i)
    {
        for(int j = 1; j < arucoGridSize; ++j)
        {
            if(checkForCorner(meanValues[i - 1][j - 1], meanValues[i - 1][j],
                               meanValues[i][j], meanValues[i][j - 1]))
            {
                insideCorners.push_back(cv::Point2f(i * gridRectSize.width, j * gridRectSize.height));
                insideCornersGridPos.push_back({i, j});
            }
        }
    }

    for(int i = 0; i <= arucoGridSize; i += arucoGridSize)
    {
        for(int j = 0; j <= arucoGridSize; j += arucoGridSize)
        {
            insideCorners.push_back(cv::Point2f(i * gridRectSize.width, j * gridRectSize.height));
            insideCornersGridPos.push_back({i, j});
        }
    }

    cv::perspectiveTransform(insideCorners, insideCorners, m.inv());
    allCorners = insideCorners;
    cornersByIndexOnArucoGrid = insideCornersGridPos;
    return true;
}

bool ArucoDetector::checkForCorner(int m00, int m01, int m11, int m10)
{
    int cnt = 0;
    cnt += m00 / 255 + m01 / 255 + m11 / 255 + m10 / 255;
    if(cnt == 1 || cnt == 3)return true;
    if(m00 == m11 && m01 != m00)return true;
    return false;
}
