#include <opencv2/opencv.hpp>

#include <map>
#include <sstream>

#include <QVector>

#include "mocapanimation.h"

using cv::Mat;
using cv::Point2d;

template <class T>
inline std::string to_string (const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

class DbScan
{
public:
    std::map<int, int> labels;
    QVector<MocapAnimation*>& data;
    int C;
    float eps;
    int mnpts;
    cv::Mat dp;

    DbScan(QVector<MocapAnimation*>& _data,float _eps,int _mnpts):data(_data)
    {
        C=-1;
        for(int i=0; i<data.size(); ++i)
        {
            labels[i]=-99;
        }

        eps=_eps;
        mnpts=_mnpts;
    }

    void setDistanceMatrix(cv::Mat mat)
    {
        dp = mat;
    }

    void run()
    {
        if (dp.empty())
        {
            std::cout << "dp empty";
            dp = cv::Mat::eye(data.size(),data.size(), CV_32FC1) -1.0f;
        }

        /*
        for(size_t i = 0;i<data.size();i++)
        {
            dp[i][i] = 0.0;
        }
        */

        for(int i=0;i<data.size();i++)
        {
            if(!isVisited(i))
            {
                std::vector<int> neighbours = regionQuery(i);
                if(neighbours.size()<mnpts)
                {
                    labels[i]=-1;//noise
                }else
                {
                    C++;
                    expandCluster(i,neighbours);
                }
            }
        }
    }

    void expandCluster(int p,std::vector<int> neighbours)
    {
        labels[p]=C;
        for(int i=0;i<neighbours.size();i++)
        {
            if(!isVisited(neighbours[i]))
            {
                labels[neighbours[i]]=C;
                std::vector<int> neighbours_p = regionQuery(neighbours[i]);
                if (neighbours_p.size() >= mnpts)
                {
                    expandCluster(neighbours[i],neighbours_p);
                }
            }
        }
    }

    bool isVisited(int i)
    {
        return labels[i]!=-99;
    }

    std::vector<int> regionQuery(int p)
    {
        std::vector<int> res;
        for(size_t i=0;i<data.size();i++)
        {
            float dist = distanceFunc(p,i);

            if(dist<=eps)
            {
                //std::cout << "distance " << p << " " << i << " " << dist << " eps " << eps << std::endl;
                res.push_back(i);
            }
        }
        return res;
    }

    double dist2d(Point2d a,Point2d b)
    {
        return sqrt(pow(a.x-b.x,2) + pow(a.y-b.y,2));
    }

    float distanceFunc(int ai,int bi)
    {
        return dp.at<float>(ai,bi);
        /*
        if(dp.at<float>(ai,bi) + 1.0f < 0.0001f)
        {
            return dp.at<float>(ai,bi);
        }

        MocapAnimation* a = data[ai];
        MocapAnimation* b = data[bi];

        double dist = 0.0;// b->getError(*a, pointDist);

        dp.at<float>(ai,bi) = dp.at<float>(bi,ai) = dist;
        return dist;
*/
    }

    std::vector<std::vector<MocapAnimation*> > getGroups()
    {
        std::vector<std::vector<MocapAnimation*> > ret;
        for(int i=0;i<=C;i++)
        {
            ret.push_back(std::vector<MocapAnimation*>());
            for(size_t j=0;j<data.size();j++)
            {
                if(labels[j]==i)
                {
                    ret[ret.size()-1].push_back(data[j]);
                }
            }
        }
        return ret;
    }
};

/*
int main(int argc,char** argv )
{
    Mat im = imread("c:/data/football.png",0);
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    findContours(im.clone(), contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    vector<MocapAnimation*> boxes;
    for(size_t i = 0; i < contours.size(); i++)
    {
        MocapAnimation* r = boundingMocapAnimation*(contours[i]);
        boxes.push_back(r);
    }
    DbScan dbscan(boxes,20,2);
    dbscan.run();
    //done, perform display

    Mat grouped = Mat::zeros(im.size(),CV_8UC3);
    vector<Scalar> colors;
    RNG rng(3);
    for(int i=0;i<=dbscan.C;i++)
    {
        colors.push_back(HSVtoRGBcvScalar(rng(255),255,255));
    }
    for(int i=0;i<dbscan.data.size();i++)
    {
        Scalar color;
        if(dbscan.labels[i]==-1)
        {
            color=Scalar(128,128,128);
        }else
        {
            int label=dbscan.labels[i];
            color=colors[label];
        }
        putText(grouped,to_string(dbscan.labels[i]),dbscan.data[i].tl(),    FONT_HERSHEY_COMPLEX,.5,color,1);
        drawContours(grouped,contours,i,color,-1);
    }

    imshow("grouped",grouped);
    imwrite("c:/data/grouped.jpg",grouped);
    waitKey(0);
}
*/
