#include "chessboardcalibration.h"

using namespace std;
using namespace cv;

ChessboardCalibration::ChessboardCalibration(std::vector<cv::Mat> &imgs, int nLines, int nCols): Calibration_intr(imgs, nLines, nCols)
{}

ChessboardCalibration::~ChessboardCalibration()
{
}

bool ChessboardCalibration::find_chessboard_corners(std::vector<cv::Point2f>& corners){
    Mat &img = imgs->at(currentImg);
    cvtColor(img, *gray_image, CV_BGR2GRAY);
    bool found = findChessboardCorners(img, board_size, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);

    if(found){
        cornerSubPix(*gray_image, corners, Size(11,11), Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
        drawChessboardCorners(*gray_image, board_size, corners, found);
    }

    return found;
}

void ChessboardCalibration::calibrate(){


    int &nb_lines(board_size.height), &nb_columns(board_size.width),
            squareSize = 2;

    // 3D coordinates of chessboard points

    vector<Point3f> obj;
    vector<vector<Point3f>> object_points;

    for (int i = 0; i < nb_lines; i++)
        for (int j = 0; j < nb_columns; j++)
            obj.push_back(Point3f((float)j * squareSize, (float)i * squareSize, 0));

    size_t nb_image = imgs->size();
    vector<Point2f> corners;

    size_t im = 0;
    size_t nb_success = 0;
    while(nb_success<nb_image){
        currentImg =  im%nb_image;

        if(find_chessboard_corners(corners)){
            image_points->push_back(corners);
            object_points.push_back(obj);

            nb_success++;

            if(nb_success >= nb_image)
                break;
        }
        else if(im > 3*nb_image)
            return;

        im++;

    }

    Mat &img = imgs->at(currentImg);
    Mat *dist_coeffs = new Mat;
    Mat *camera_matrix = new Mat(3, 3, CV_32FC1);
    camera_matrix->ptr<float>(0)[0] = 1;
    camera_matrix->ptr<float>(1)[1] = 1;

    calibrateCamera(object_points, *image_points, img.size(), *camera_matrix, *dist_coeffs, *rvecs, *tvecs);// , CV_CALIB_USE_INTRINSIC_GUESS);

    intrParam->setCameraMatrix(*camera_matrix);
    intrParam->setDistCoeffsMatrix(*dist_coeffs);
}