#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <opencv2/opencv.hpp>

#include <iostream>

void normHistogram(cv::Mat &hist)
{
    float sum1 = 0.0f;

    for (int i = 0; i < hist.cols; ++i)
    {
        sum1 += hist.at<float>(i);
    }

    if ((sum1 - 1.0f) < 0.01f) return;

    for (int i = 0; i < hist.cols; ++i)
    {
        hist.at<float>(i) = hist.at<float>(i)/sum1;
    }
}

inline float MAE(const cv::Mat &h1,const cv::Mat &h2)
{
    float error = 0.0;

    for (int i = 0; i < h1.cols; ++i)
    {
        error += std::fabs(h1.at<float>(i) - h2.at<float>(i));
    }

    return error/h1.cols;
}

inline float MAPE(const cv::Mat &h1,const cv::Mat &h2)
{
    float error = 0.0;

    for (int i = 0; i < h1.cols; ++i)
    {
        if (h1.at<float>(i) == 0.0f && h2.at<float>(i) == 0.0f)
        {
            continue;
        }

        float h1h2 = std::numeric_limits<float>::max();
        float h2h1 = std::numeric_limits<float>::max();

        if(h1.at<float>(i) != 0.0f)
        {
            h1h2 = (h1.at<float>(i) - h2.at<float>(i))/h1.at<float>(i);
        }

        if(h2.at<float>(i) != 0.0f)
        {
            h2h1 = (h2.at<float>(i) - h1.at<float>(i))/h2.at<float>(i);
        }

        error += std::min(std::abs(h1h2),std::abs(h2h1));
    }

    return 100.0f * error/h1.cols;
}

inline float MAPE2(const cv::Mat &h1,const cv::Mat &h2)
{
    float error1 = 0.0;
    float error2 = 0.0;

    for (int i = 0; i < h1.cols; ++i)
    {
        if(h1.at<float>(i) != 0.0f)
        {
            error1 += std::abs((h1.at<float>(i) - h2.at<float>(i))/h1.at<float>(i));
        }

        if(h2.at<float>(i) != 0.0f)
        {
            error2 += std::abs((h2.at<float>(i) - h1.at<float>(i))/h2.at<float>(i));
        }
    }

    return 100.0f * std::min(error1,error2)/h1.cols;
}

inline float RMSE(const cv::Mat &h1,const cv::Mat &h2)
{
    float error = 0.0;

    for (int i = 0; i < h1.cols; ++i)
    {
        error += std::pow(h1.at<float>(i) - h2.at<float>(i),2);
    }

    return sqrtf(error/h1.cols);
}


#endif // HELPERFUNCTIONS_H
