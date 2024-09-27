#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
#include "opencv2/aruco.hpp"

class ArucoDetector
{
public:
    struct DetectorParams
    {
        cv::aruco::DetectorParameters detectorParams;
        cv::aruco::PredefinedDictionaryType arucoType;
        DetectorParams(cv::aruco::PredefinedDictionaryType _arucoType = cv::aruco::DICT_ARUCO_ORIGINAL,
                       cv::aruco::DetectorParameters _detectorParams = cv::aruco::DetectorParameters())
            : detectorParams(_detectorParams)
            , arucoType(_arucoType)
        {

        }
    };
public:
    ArucoDetector(const DetectorParams& params = DetectorParams());

    void setNewParams(const DetectorParams& params);

    void processImg(const cv::Mat &img);

    void processImg(const cv::Mat &img,
                    std::vector<int>& markerIds,
                    std::vector<std::vector<cv::Point2f>>& markerCorners);

    void processImgMultipleSubPix();

    void improveDetectionResult();

    void improveDetectionResultById(int id);

    std::vector<cv::Point2f> getMarkerById(int id) const;

    std::vector<std::vector<cv::Point2f>> getMarkerCorners() const;
    std::vector<int> getMarkerIds() const;

    bool getArucoInsideCornersAndGeometryById(int arucoId, std::vector<cv::Point2f> & allCorners,
                                              std::vector<cv::Point> &cornersByIndexOnArucoGrid, int arucoGridSize);

private:

    bool checkForCorner(int m00, int m01, int m11, int m10); // values are 0 or 255

private:
    DetectorParams m_params;

    // one iteration data
    cv::Mat m_img;

    std::vector<int> m_markerIds;
    std::vector<std::vector<cv::Point2f>> m_markerCorners;
    std::vector<std::vector<cv::Point2f>> m_rejectedCandidates;
    cv::Ptr<cv::aruco::Dictionary> m_dictionary;
    cv::Ptr<cv::aruco::DetectorParameters> m_detectorParams;


};
