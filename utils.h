#pragma once
#include "opencv2/core/types.hpp"
#include "opencv2/core/mat.hpp"

namespace Utils
{

inline bool isNan(const cv::Point3f& p){return std::isnan(p.x ) || std::isnan(p.y ) || std::isnan(p.z );}
inline bool isNan(const cv::Vec3f  & p){return std::isnan(p[0]) || std::isnan(p[1]) || std::isnan(p[2]);}

inline float dist(const cv::Point3f& a, const cv::Point3f& b){return std::sqrt((a.x - b.x)*(a.x - b.x)+(a.y - b.y)*(a.y - b.y)+(a.z - b.z)*(a.z - b.z));}
inline double dist(const cv::Point3d& a, const cv::Point3d& b){return std::sqrt((a.x - b.x)*(a.x - b.x)+(a.y - b.y)*(a.y - b.y)+(a.z - b.z)*(a.z - b.z));}

inline float  dist(const cv::Point2f& a, const cv::Point2f& b){return std::sqrt((a.x - b.x)*(a.x - b.x)+(a.y - b.y)*(a.y - b.y));}
inline double dist(const cv::Point2d& a, const cv::Point2d& b){return std::sqrt((a.x - b.x)*(a.x - b.x)+(a.y - b.y)*(a.y - b.y));}

inline void normalize(cv::Point3f& v){v /= std::sqrt(v.ddot(v));}
inline void normalize(cv::Vec3f&   v){v /= std::sqrt(v.ddot(v));}

inline void normalize(cv::Point3d& v){v /= std::sqrt(v.ddot(v));}
inline void normalize(cv::Vec3d&   v){v /= std::sqrt(v.ddot(v));}

inline double length(const cv::Point3d &v) { return std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z); }

inline cv::Point3f normalized(const cv::Point3f& v){return v / std::sqrt(v.ddot(v));}
inline cv::Vec3f normalized(const cv::Vec3f&   v){return v / std::sqrt(v.ddot(v));}

inline double angleBetweenInRad(const cv::Point3d& a, const cv::Point3d& b) { return std::acos(a.ddot(b) / (length(a) * length(b))); }

inline double radToDeg(double a){ return a / CV_PI * 180.; }

inline double angleBetweenInDeg(const cv::Point3d& a, const cv::Point3d& b) { return radToDeg(angleBetweenInRad(a, b)); }

inline void extendRect(cv::Rect &r, const cv::Size &imgSize = cv::Size(-1, -1), int rectDilationInPx = 200)
{
    r.x = std::max(0, r.x - rectDilationInPx);
    r.y = std::max(0, r.y - rectDilationInPx);

    r.height = imgSize == cv::Size(-1, -1)?r.height + rectDilationInPx * 2:std::min(r.height + rectDilationInPx * 2, imgSize.height - 1 - r.y);
    r.width  = imgSize == cv::Size(-1, -1)?r.width  + rectDilationInPx * 2:std::min(r.width  + rectDilationInPx * 2, imgSize.width  - 1 - r.x);
}

cv::Vec3d CalculateMean(const cv::Mat_<cv::Vec3d> &points);

cv::Mat_<double> FindRigidTransform(const cv::Mat_<cv::Vec3d> &points1, const cv::Mat_<cv::Vec3d> &points2);

}
