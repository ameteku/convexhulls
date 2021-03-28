// You need to complete this program for your second project.
//MIchael Ameteku

// Standard libraries
#include <string>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <vector>

struct Point
{
    double x;
    double y;
};

Point findNextSmallest(const std::vector<Point> &, const Point &);

void grahamScan(std::vector<Point> &, std::ofstream &);

void javisMarch(std::vector<Point> &, std::ofstream &);

void quickHull(std::vector<Point> &, std::ofstream &);

// sub function for jarvis march
Point nextLargestAngle(const std::vector<Point> &points, std::vector<Point> &colinearPoints, const Point &currentPoint);

void quickHull();

void restOfHull(const std::vector<Point> &points, Point left, Point right, std::vector<Point> &boundary, bool);

int main(int argc, char *argv[])
{
    if (argc < 3)
        std::cout << "wrong format! should be \"a.exe algType dataFile\"";
    else
    {
        std::string algType = argv[1];
        std::string dataFilename = argv[2];
        std::ifstream readPoints;

        //read your data points from dataFile (see class example for the format)
        readPoints.open(dataFilename);

        // check for correct file opening
        if (readPoints.fail())
        {
            std::cout << "File does not exist";
            return -1;
        }

        std::vector<Point> allPoints;

        int count = 0;
        std::string coordinate;

        while (!readPoints.eof())
        {
            Point newPoint;
            readPoints >> newPoint.x;
            readPoints >> newPoint.y;
            allPoints.push_back(newPoint);
            std::cout << allPoints[count].x << " " << allPoints[count].y << std::endl;
            ++count;
        };

        readPoints.close();

        std::string outputFile = "";

        std::ofstream out;
        if (algType[0] == 'G')
        {
            std::cout << "Calling graham!!";

            //call your Graham Scan algorithm to solve the problem
            outputFile = "hull_G.txt";
            out.open(outputFile);
            grahamScan(allPoints, out);
            out.close();
        }
        else if (algType[0] == 'J')
        {
            //call your Javis March algorithm to solve the problem
            std::cout << "Calling Jarvis!!!\n";
            outputFile = "hull_J.txt";
            out.open(outputFile);
            javisMarch(allPoints, out);
            out.close();
        }
        else
        { //default
            //call your Quickhull algorithm to solve the problem
            outputFile = "hull_Q.txt";
            out.open(outputFile);
            quickHull(allPoints, out);
            out.close();
        }

        //write your convex hull to the outputFile (see class example for the format)
        //you should be able to visulize your convex hull using the "ConvexHull_GUI" program.
    }

    return 0;
}

bool isSmaller(const Point &a, const Point &b)
{
    // std::cout << "comparing points( " << a.x << "," << a.y << ") and  (" << b.x << ',' << b.y << ")\n";
    return ((a.x * b.y - b.x * a.y) >= 0);
}

void grahamScan(std::vector<Point> &points, std::ofstream &writeToFile)
{

    //find lowest y coordinate()
    Point lowestPoint = points[0];
    // storing vector of each point
    std::vector<Point> relativeVectors;

    // sort by angles
    std::vector<Point> sortedPoints;

    for (int i = 0; i < points.size(); i++)
    {
        std::cout << " current point is" << lowestPoint.y << std::endl;
        if (points[i].y < lowestPoint.y)
            lowestPoint = points[i];
    }
    std::cout << "lowest point is" << lowestPoint.y;

    for (int i = 0; i < points.size(); i++)
    {
        Point tempPoint;
        //check if duplicate of lowest point
        if (points[i].x != lowestPoint.x && points[i].y != lowestPoint.y)
        {
            tempPoint.x = points[i].x - lowestPoint.x;
            tempPoint.y = points[i].y - lowestPoint.y;
            relativeVectors.push_back(tempPoint);
        }
    }

    std::sort(relativeVectors.begin(), relativeVectors.end(), isSmaller);

    for (int i = 0; i < relativeVectors.size(); i++)
    {
        Point temp;
        temp.x = relativeVectors[i].x + lowestPoint.x;
        temp.y = relativeVectors[i].y + lowestPoint.y;
        sortedPoints.push_back(temp);

        std::cout << "     " << temp.x << " " << temp.y << std::endl;
    }
    std::cout << "successfully sorted";

    std::vector<Point> boundary;
    std::vector<Point> usedPoints;

    //start by pushing
    boundary.push_back(lowestPoint);
    boundary.push_back(sortedPoints[0]);

    // Main sort section
    // Store the lowest point on file, move to the next point,
    // compare all points using the angle their vectors make and push the smallest angle point unto stck
    // repeat this until the last point is the original lowest point

    Point currentTerminalPoint = sortedPoints[0];
    std::cout << "THe next outer bound is (" << currentTerminalPoint.x << ", " << currentTerminalPoint.y << std::endl;

    while (currentTerminalPoint.x != lowestPoint.x && currentTerminalPoint.y != lowestPoint.y)
    {
        std::cout << "Current boundary point: (" << currentTerminalPoint.x << ", " << currentTerminalPoint.y << ")\n";
        Point nextSmallest = findNextSmallest(points, currentTerminalPoint);
        boundary.push_back(nextSmallest);
        usedPoints.push_back(nextSmallest);
        currentTerminalPoint = nextSmallest;
    }

    //writing boundary points to file
    for (int i = 0; i < boundary.size(); i++)
    {
        writeToFile << boundary[i].x << " " << boundary[i].y << std::endl;
    }
}

// finds the point with the smallest angle it relative vector makes with the point stated
Point findNextSmallest(const std::vector<Point> &points, const Point &lowestPoint)
{
    std::vector<Point> relativeVectors;
    for (int i = 0; i < points.size(); i++)
    {
        Point tempPoint = points[i];
        //check if duplicate of lowest point

        tempPoint.x = points[i].x - lowestPoint.x;
        tempPoint.y = points[i].y - lowestPoint.y;
        if (tempPoint.x != 0 && tempPoint.y != 0)
            relativeVectors.push_back(tempPoint);
    }

    std::sort(relativeVectors.begin(), relativeVectors.end(), isSmaller);
    relativeVectors[0].x = relativeVectors[0].x + lowestPoint.x;
    relativeVectors[0].y = relativeVectors[0].y + lowestPoint.y;
    return relativeVectors[0];
}

// section for Jarvis March
// find leftmost point,
// find compare all values and find the point with the largest vector angle anticlockwise
// during the comparisons, if the more than one point has the same angle, mark as colinear and store both,
// if they end being the largest angle keep both of them , if not discard and get the largest point

void javisMarch(std::vector<Point> &points, std::ofstream &writeToFile)
{
    Point leftMostPoint;
    std::vector<Point> colinearPoints;
    std::vector<Point> boundary;

    std::cout << "finding leftmost point\n";
    //finding leftmost point
    std::sort(points.begin(), points.end(), [](Point a, Point b) {
        // std::cout << "A.x:" << a.x << "and B.x: " << b.x << std::endl;
        return a.x <= b.x;
    });

    leftMostPoint = points[0];
    std::cout << "Leftmost point is: (" << leftMostPoint.x << ", " << leftMostPoint.y << ")\n";

    boundary.push_back(leftMostPoint);

    do
    {
        std::cout << "Current point is(" << leftMostPoint.x << " " << leftMostPoint.y << ")\n";
        leftMostPoint = nextLargestAngle(points, colinearPoints, leftMostPoint);

        if (!colinearPoints.empty())
        {
            colinearPoints.push_back(leftMostPoint);
            std::sort(colinearPoints.begin(), colinearPoints.end(), [boundary](Point a, Point b) {
                Point reference = boundary[boundary.size() - 1];
                double distanceA = (a.x - reference.x) * (a.x - reference.x) + (a.y - reference.y) * (a.y - reference.y);
                double distanceB = (b.x - reference.x) * (b.x - reference.x) + (b.y - reference.y) * (b.y - reference.y);
                return distanceA >= distanceB;
            });

            while (!colinearPoints.empty())
            {
                boundary.push_back(colinearPoints[colinearPoints.size() - 1]);
                colinearPoints.pop_back();
            }
            leftMostPoint = boundary[boundary.size() - 1];
            std::cout << "New leftmost point : " << leftMostPoint.x << " , " << leftMostPoint.y << std::endl;
        }
        else
            boundary.push_back(leftMostPoint);

    } while (leftMostPoint.x != boundary[0].x && leftMostPoint.y != boundary[0].y);

    for (int i = 0; i < boundary.size(); i++)
    {
        writeToFile << boundary[i].x << " " << boundary[i].y << std::endl;
    }
}

Point nextLargestAngle(const std::vector<Point> &points, std::vector<Point> &colinearPoints, const Point &currentPoint)
{
    std::vector<Point> colinearVectors;
    std::vector<Point> relativeVectors;

    //compare relative vectors of each point against each other for the largest value
    for (int i = 0; i < points.size(); i++)
    {
        Point tempPoint;
        if (points[i].x != currentPoint.x && points[i].y != currentPoint.y)
        {
            tempPoint.x = points[i].x - currentPoint.x;
            tempPoint.y = points[i].y - currentPoint.y;
            relativeVectors.push_back(tempPoint);
        }
    }

    Point largestAnglePoint = relativeVectors[0];
    for (int i = 0; i < relativeVectors.size(); i++)
    {
        if (relativeVectors[i].x != largestAnglePoint.x && relativeVectors[i].y != largestAnglePoint.y)
        {
            int value = (largestAnglePoint.x * relativeVectors[i].y) - (largestAnglePoint.y * relativeVectors[i].x);
            if (value < 0)
            {
                largestAnglePoint.x = relativeVectors[i].x;
                largestAnglePoint.y = relativeVectors[i].y;
            }
        }
    }

    if (!colinearVectors.empty())
    {
        Point tempPoint;
        if (colinearVectors.size() == 1)
        {
            tempPoint.x = colinearVectors[0].x + currentPoint.x;
            tempPoint.y = colinearVectors[0].y + currentPoint.y;
            colinearPoints.push_back(tempPoint);
        }
        else
        {
            for (int a = 0; a < colinearVectors.size(); a++)
            {
                tempPoint.x = colinearVectors[0].x + currentPoint.x;
                tempPoint.y = colinearVectors[0].y + currentPoint.y;
                colinearPoints.push_back(tempPoint);
            }
        }
    }

    for (int i = 0; i < colinearPoints.size(); i++)
    {
        std::cout << "Colin pts: (" << colinearPoints[i].x << ", " << colinearPoints[i].y << std::endl;
    }

    largestAnglePoint.x = largestAnglePoint.x + currentPoint.x;
    largestAnglePoint.y = largestAnglePoint.y + currentPoint.y;
    return largestAnglePoint;
}

void quickHull(std::vector<Point> &points, std::ofstream &out)
{
    Point leftMostPoint;
    Point rightMostPoint;
    std::vector<Point> colinearPoints;
    std::vector<Point> boundary;

    std::cout << "finding leftmost point and rightMost\n";
    //finding leftmost point
    std::sort(points.begin(), points.end(), [](Point a, Point b) {
        // std::cout << "A.x:" << a.x << "and B.x: " << b.x << std::endl;
        return a.x <= b.x;
    });

    leftMostPoint = points[0];
    rightMostPoint = points[points.size() - 1];
    boundary.push_back(leftMostPoint);

    std::cout << "Leftmost point is: (" << leftMostPoint.x << ", " << leftMostPoint.y << ")\n";

    std::vector<Point> topSide;
    std::vector<Point> bottomSide;

    // sort to both sides
    double gradient;
    double mainGradient = (leftMostPoint.y - rightMostPoint.y) / static_cast<double>(leftMostPoint.x - rightMostPoint.x);
    for (int i = 0; i < points.size(); i++)
    {
        if (leftMostPoint.x != points[i].x && leftMostPoint.y != points[i].y)
        {
            gradient = (leftMostPoint.y - points[i].y) / static_cast<double>(leftMostPoint.x - points[i].x);
            if (gradient > mainGradient)
            {
                topSide.push_back(points[i]);
            }
            else if (gradient < mainGradient)
            {
                bottomSide.push_back(points[i]);
            }
        }
    }

    //call on first section
    restOfHull(topSide, leftMostPoint, rightMostPoint, boundary, true);

    boundary.push_back(rightMostPoint);

    // call on second
     restOfHull(bottomSide, leftMostPoint,rightMostPoint, boundary, false );
    boundary.push_back(leftMostPoint);
    for (int a = 0; a < boundary.size(); a++)
    {
        out << boundary[a].x << " " << boundary[a].y << std::endl;
    }

    // out << leftMostPoint.x << " " << leftMostPoint.y<< std::endl;
    // out << rightMostPoint.x << " " << rightMostPoint.y<< std::endl;
    // out << leftMostPoint.x << " " << leftMostPoint.y<< std::endl;
}

double distance(const Point &left, const Point &right, const Point &p)
{
    double dist = (p.y - left.y) * (right.x - left.x) - (right.y - left.y) * (p.x - left.x);

    if (dist < 0)
        return -(dist);
    return dist;
}

void restOfHull(const std::vector<Point> &points, Point left, Point right, std::vector<Point> &boundary, bool isTop)
{
    std::vector<Point> leftMaxSide;
    std::vector<Point> rightMaxSide;

    // fin point with max distance from the line
    Point max = points[0];
    for (int a = 1; a < points.size(); a++)
    {
        if (distance(left, right, max) < distance(left, right, points[a]))
            max = points[a];
    }

    // sort to both sides
    double gradient;
    double leftSideGradient = (left.y - max.y) / static_cast<double>(left.x - max.x);
    double rightSideGradient = (right.y - max.y) / static_cast<double>(right.x - max.x);

    //sorting points to the left and right side of the max above the line
    for (int i = 0; i < points.size(); i++)
    {
        if (points[i].x <= max.x)
        {
            if (left.x != points[i].x && left.y != points[i].y)
            {
                gradient = (left.y - points[i].y) / static_cast<double>(left.x - points[i].x);
                if (isTop)
                {
                    if (gradient > leftSideGradient)
                    {
                        leftMaxSide.push_back(points[i]);
                    }
                }
                else
                {
                    if (gradient < leftSideGradient)
                    {
                        leftMaxSide.push_back(points[i]);
                    }
                }
            }
        }

        else
        {
            if (max.x != points[i].x && max.y != points[i].y)
            {
                gradient = (max.y - points[i].y) / static_cast<double>(max.x - points[i].x);
                if(isTop) {
                if (gradient > rightSideGradient)
                {
                    rightMaxSide.push_back(points[i]);
                }
                }else {
                   if (gradient < rightSideGradient)
                {
                    rightMaxSide.push_back(points[i]);
                } 
                }
            }
        }
    }

    // using to get the points in order
    if(isTop) {
    if (!leftMaxSide.empty())
    {
        restOfHull(leftMaxSide, left, max, boundary, isTop);
    }
    
    boundary.push_back(max);

    if (!rightMaxSide.empty())
    {
        restOfHull(rightMaxSide, max, right, boundary, isTop);
    }
    }
    else {
        if (!rightMaxSide.empty())
    {
        restOfHull(rightMaxSide, max, right, boundary, isTop);
    }

    boundary.push_back(max);

       if (!leftMaxSide.empty())
    {
        restOfHull(leftMaxSide, left, max, boundary, isTop);
    }
     
    }

    return;
}

// void findHull(const std::vector<Point> &section, Point left, Point right, std::vector<Point> &boundary, bool topSide)
// {
//     //divide according to gradient,
//     std::vector<Point> topSide;
//     std::vector<Point> bottomSide;

//     // sort to both sides
//     double gradient;
//     double mainGradient = (left.y - right.y) / static_cast<double>(left.x - right.x);
//     for (int i = 0; i < section.size(); i++)
//     {
//         if (left.x != section[i].x && left.y != section[i].y)
//         {
//             gradient = (left.y - section[i].y) / static_cast<double>(left.x - section[i].x);
//             if (gradient > mainGradient)
//             {
//                 topSide.push_back(section[i]);
//             }
//             else if (gradient < mainGradient)
//             {
//                 bottomSide.push_back(section[i]);
//             }
//         }
//     }

//     //find max part of top side
//     findHull(topSide, )
// }
