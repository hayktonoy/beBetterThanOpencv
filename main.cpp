#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

// #include <arucodetector.h>
#include "warppingtwoimageswithfeaturematcher.h"
#include "utils.h"

void from_json(const nlohmann::json &json, cv::Mat &mat)
{
    mat = cv::Mat();
    try
    {
        auto  cols     = json.find("cols");
        auto  rows     = json.find("rows");
        auto  step     = json.find("step");
        auto  dataType = json.find("dataType");
        auto  binData  = json.find("binData");

        if(cols     == json.end() ||
            rows     == json.end() ||
            step     == json.end() ||
            dataType == json.end()||
            binData == json.end())
        {
            return;
        }
        int int_cols     = *cols    ;
        int int_rows     = *rows    ;
        int int_step     = *step    ;
        int int_dataType = *dataType;

        if(int_cols <= 0||
            int_rows <= 0||
            int_step <= 0)
        {
            return;
        }

        std::vector<int> intDataArr = (*binData);
        std::vector<uchar> binDataArr(intDataArr.begin(),intDataArr.end());
        mat = cv::Mat(int_rows,
                      int_cols,
                      int_dataType,
                      binDataArr.data(),
                      int_step).clone();

        // if(int_dataType == CV_8UC1 ||
        //    int_dataType == CV_8UC3 ||
        //    int_dataType == CV_8UC4)
        // {
        //     cv::imshow("",mat);
        //     cv::waitKey(0);
        // }
    }
    catch(std::exception excpt)
    {
        std::cerr<<__FUNCSIG__<<" "<<excpt.what()<<std::endl;
        mat = cv::Mat();
    }
    catch(...)
    {
        mat = cv::Mat();
    }

}


std::vector<cv::Point3d> createGeometryByArucoIndices(int gridSize, const std::vector<cv::Point> &cornerIds, double arucoLength)
{
    std::vector<cv::Point3d> res;

    for(int i = 0; i < cornerIds.size(); ++i)
    {
        int i_ = cornerIds[i].x;
        int j_ = cornerIds[i].y;
        double cellLength = arucoLength / (1. * gridSize);
        res.push_back(cv::Point3d(cellLength * i_ - 0.5*arucoLength, cellLength * j_ - 0.5*arucoLength, 0));
    }

    return res;
}

cv::Mat RT, RT1, R, T, intrinsic1, intrinsic2, distortion1, distortion2;
std::vector<cv::Point3d> truangulate(int id, std::vector<cv::Point> &geometryByIndex, cv::Mat fujiImg1, cv::Mat fujiImg2)
{
    WarppingTwoImagesWithFeatureMatcher w;

    bool imshowWarpProcess = false;
    w.warpWithArucoDetectionLineIntesectionPoints(fujiImg1, fujiImg2, id, ArucoDetector::DetectorParams(cv::aruco::DICT_4X4_100), imshowWarpProcess);
    std::vector<cv::Point2f> points1;
    std::vector<cv::Point2f> points2;

    {
        const auto &img1 = fujiImg1;
        const auto &img2 = fujiImg2;
        const auto &m    = w.get1To2HomographyMatrix();


        cv::Size winSize  = cv::Size( 5, 5 );
        cv::Size zeroZone = cv::Size( -1, -1 );
        cv::TermCriteria criteria = cv::TermCriteria( cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10000, 0.00001 );

        cv::aruco::DetectorParameters params;
        params.cornerRefinementMethod = cv::aruco::CORNER_REFINE_NONE;
        ArucoDetector ar((ArucoDetector::DetectorParams(cv::aruco::DICT_4X4_50, params)));

        ar.processImg(img1);
        ar.processImgMultipleSubPix();

        ar.getArucoInsideCornersAndGeometryById(id, points1, geometryByIndex, 6);

        if(points1.empty())return {};
        cv::Mat gray1;
        cv::cvtColor(fujiImg1, gray1, cv::COLOR_BGR2GRAY);
        cv::cornerSubPix(gray1, points1, winSize, zeroZone, criteria);

        points2 = std::vector<cv::Point2f>(points1.size());
        cv::perspectiveTransform(points1, points2, m);

        auto r = cv::boundingRect(points2);
        Utils::extendRect(r, img2.size(), 200);

        //                cv::namedWindow("img2", cv::WINDOW_NORMAL);
        //                cv::imshow("img2", img2(r));

        //                cv::namedWindow("_img1", cv::WINDOW_NORMAL);
        //                cv::namedWindow("_img2", cv::WINDOW_NORMAL);
        //                cv::imshow("_img1", img1);
        //                cv::imshow("_img2", img2);


    }

    if(points1.size() != points2.size())return {};


    cv::Mat ProjectionLeftMat = intrinsic1 * RT1;
    cv::Mat ProjectionRightMat = intrinsic2 * RT, Q;

    cv::Mat pnts3D;
    cv::triangulatePoints(ProjectionLeftMat, ProjectionRightMat, points1, points2, pnts3D);

    std::vector<cv::Point3d> points;
    for(int i = 0; i < points1.size(); ++i)
    {
        float divider = pnts3D.at<float>(3, i);
        points.push_back(cv::Point3d(pnts3D.at<float>(0, i),
                                     pnts3D.at<float>(1, i),
                                     pnts3D.at<float>(2, i)) / divider);
    }
    return points;
};

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

double executeTriangulation(cv::Mat left, cv::Mat right)
{
    std::vector<cv::Point> geomIds;
    auto points = truangulate(4,geomIds, left, right);

    auto geom = createGeometryByArucoIndices(6, geomIds, 25);

    if(geom.size() != points.size())
    {
        // std::cerr << "geom size is not equal to triangulated points size id : " << id
                  // << "\t\t" << __FUNCSIG__ << " line : " << __LINE__ << std::endl;

        // return false;
        return -1;
    }

    double maxDist = 0;
    //            for(int i = 0; i < geom.size(); ++i)
    //            {
    //                for(int j = 0; j < geom.size(); ++j)
    //                {
    //                    double distInGeom = Utils::dist(geom[i], geom[j]);
    //                    double distInRes  = Utils::dist(points.at(i), points.at(j));
    //                    maxDist = std::max(maxDist, std::abs(distInRes - distInGeom));
    //                }
    //            }
    std::vector<cv::Point3d> cornerPoints(4);
    for(int i = 0; i < points.size(); ++i)
    {
        if(geomIds.at(i) == cv::Point(0, 0))cornerPoints[0] = points.at(i);
        if(geomIds.at(i) == cv::Point(0, 6))cornerPoints[1] = points.at(i);
        if(geomIds.at(i) == cv::Point(6, 6))cornerPoints[2] = points.at(i);
        if(geomIds.at(i) == cv::Point(6, 0))cornerPoints[3] = points.at(i);
    }
    for(int i = 0; i < cornerPoints.size(); ++i)
    {
        double distInRes  = Utils::dist(cornerPoints.at(i), cornerPoints.at((i + 1) % 4));
        maxDist = std::max(maxDist, std::abs(distInRes - 25));
    }
    return maxDist;
}


int main()
{
    // TesterToliInch tester;
    // tester.startCamera();
    // ArucoDetector detector((ArucoDetector::DetectorParams(cv::aruco::DICT_4X4_50)));
    // // ArucoDetector detector;
    // cv::VideoCapture cap;
    // cap.open(0);
    // cv::Mat frame = cv::imread("C:/Users/archLinux/Downloads/right4_2.767748.png");
    // detector.processImg(frame);
    // detector.processImgMultipleSubPixXelaci();
    // std::vector<std::vector<cv::Point2f>> corners = detector.getMarkerCorners();
    // for (auto &x : corners)
    // {
    //     for (auto &y : x)
    //     {
    //         std::cout << y << std::endl;
    //     }
    // }
    // cv::imwrite("cool_image.png", colorCorners(frame, corners));
    // cv::waitKey();
    // while (true)
    // {
    //     cap >> frame;
    //     // imshow("main window", frame);
    //     char t = cv::waitKey(25);
    //     detector.processImg(frame);
    //     detector.processImgMultipleSubPixXelaci();
    //     std::vector<std::vector<cv::Point2f>> corners = detector.getMarkerCorners();
    //     // for(size_t i = 0; i < corners.size(); ++i) refineCorners(frame, corners[i]);
    //     imshow("new window", colorCorners(frame, corners));
    //     if (t == 27)
    //     {
    //         break;
    //     }
    // }








    nlohmann::json js;
    std::ifstream i("data.json");
    if(!i.is_open())
    {
        std::cerr << "Failed to load data.json" << std::endl;
        return -1;
    }
    i >> js;
    i.close();
    from_json(js["intrinsic1"], intrinsic1 );
    from_json(js["intrinsic2"], intrinsic2 );
    from_json(js["dist1"], distortion1 );
    from_json(js["dist2"], distortion2 );
    from_json(js["R"], R );
    from_json(js["T"], T );

    std::string directoryPath = "C:/Users/archLinux/Downloads/badImages/badImages";
    for (const auto& entry : fs::directory_iterator(directoryPath))
    {
        if (entry.is_regular_file())
        {
            std::string fileName = entry.path().filename().string();
            std::regex fileNameRegex(R"((right)(\d+)_(\w+\.png))");
            std::smatch match;
            if (std::regex_match(fileName, match, fileNameRegex))
            {
                std::string id = match[2];
                std::string error = match[3];

                std::string leftImageName = "left" + id + "_" + error + ".png";
                fs::path leftImagePath = entry.path().parent_path() / leftImageName;

                std::cout << id << " " << fileName << " " << leftImageName << std::endl;

            }
        }
    }




    // std::vector<int> ids = detector.getMarkerIds();



    return 0;
}
