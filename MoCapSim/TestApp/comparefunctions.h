#ifndef COMPAREFUNCTIONS_H
#define COMPAREFUNCTIONS_H

#endif // COMPAREFUNCTIONS_H

#include "helperfunctions.h"

#include <cv.hpp>

namespace CompareHist
{

enum class CompareHistogram {
    CORREL = CV_COMP_CORREL,
    CHISQUARE = CV_COMP_CHISQR,
    INTERSECTION = CV_COMP_INTERSECT,
    BHATTACHARYYA = CV_COMP_BHATTACHARYYA,
    CHISQUARE_NORM = 4,
    MAE = 5,
    MAPE = 6,
    MAPE2 = 7,
    SMAPE = 8,
    RMSE = 9
};

double compareHistogramFunction(cv::Mat h1, cv::Mat h2,CompareHistogram method)
{
    //norm if needed
    switch (method) {
    case CompareHistogram::BHATTACHARYYA :
    case CompareHistogram::INTERSECTION:
    case CompareHistogram::CHISQUARE_NORM:
        normHistogram(h1);
        normHistogram(h2);
        break;
    default:
        break;
    }

    double distance = 0.0;

    //compare
    switch (method) {
    case CompareHistogram::CORREL:
    case CompareHistogram::INTERSECTION:
    case CompareHistogram::BHATTACHARYYA:
        distance = cv::compareHist(h1,h2,static_cast<std::underlying_type<CompareHistogram>::type>(method));
        break;
    case CompareHistogram::CHISQUARE:
        distance = std::min(cv::compareHist(h1,h2,static_cast<std::underlying_type<CompareHistogram>::type>(method)),
                            cv::compareHist(h2,h1,static_cast<std::underlying_type<CompareHistogram>::type>(method)));
        break;
    case CompareHistogram::CHISQUARE_NORM:
        distance = std::min(cv::compareHist(h1,h2,CV_COMP_CHISQR),
                            cv::compareHist(h2,h1,CV_COMP_CHISQR));
        break;
    case CompareHistogram::MAE:
        distance = static_cast<double>(MAE(h1,h2));
        break;
    case CompareHistogram::MAPE:
        distance = static_cast<double>(MAPE(h1,h2));
        break;
    case CompareHistogram::MAPE2:
        distance = static_cast<double>(MAPE2(h1,h2));
        break;
    case CompareHistogram::SMAPE:
        distance = static_cast<double>(SMAPE(h1,h2));
        break;
    case CompareHistogram::RMSE:
        distance = static_cast<double>(RMSE(h1,h2));
        break;
    }

    //transform
    switch (method) {
    case CompareHistogram::CORREL:
        distance *= -1.0;
        distance += 1.0;
        break;
    case CompareHistogram::INTERSECTION:
        distance = 1.0 - distance;
        break;
    default:
        break;
    }

    return distance;
}


} //comparehist


