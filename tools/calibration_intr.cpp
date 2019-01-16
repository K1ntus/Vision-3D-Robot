#include "calibration_intr.h"

using namespace std;
using namespace cv;

Calibration_intr::Calibration_intr(std::vector<cv::Mat> &imgs, int nLines, int nCols) :
    currentImg(0)
{
    this->imgs = new vector<cv::Mat>(imgs);
    board_size = Size(nLines, nCols);

    gray_image = new Mat;
    object_points = new vector<vector<Point3f>>;
    image_points = new vector<vector<Point2f>>;

    intrParam = new IntrinsicParameters();

    rvecs = new std::vector<cv::Mat>;
    tvecs = new std::vector<cv::Mat>;
}

Calibration_intr::~Calibration_intr()
{
    delete gray_image;

    delete object_points;
    delete image_points;

    delete intrParam;

    delete rvecs;
    delete tvecs;
}

void Calibration_intr::newImageSet(const std::vector<Mat> &images)
{
    object_points->clear();
    image_points->clear();
    rvecs->clear();
    tvecs->clear();
    currentImg = 0;
    imgs->clear();
    for (size_t i = 0; i < images.size(); i++) {
        imgs->push_back(images[i]);
    }
}

size_t Calibration_intr::getCurrentImgIndex() const
{
    return currentImg;
}

void Calibration_intr::setNextImgIndex(size_t const& newIndex)
{
    if(newIndex < imgs->size()){
        currentImg = newIndex;
        if(image_points->size() != 0)
            find_chessboard_corners(image_points->at(currentImg));
        else{
            vector<Point2f> corners;
            find_chessboard_corners(corners);
        }

    }
}

Mat& Calibration_intr::get_image_origin() const{
    return imgs->at(currentImg);
}

Mat& Calibration_intr::get_gray_image() const{
    return *gray_image;
}

IntrinsicParameters& Calibration_intr::getIntrinsicParameters() const{
    return *this->intrParam;
}

void Calibration_intr::setIntrinsincParameters(IntrinsicParameters &intrinsicParam)
{
    this->intrParam->setCameraMatrix(intrinsicParam.getCameraMatrix());
    this->intrParam->setDistCoeffsMatrix(intrinsicParam.getDistCoeffs());

}

vector<Mat>& Calibration_intr::get_rvecs() const{
    return *rvecs;
}

vector<Mat>& Calibration_intr::get_tvecs() const{
    return *tvecs;
}



bool Calibration_intr::find_chessboard_corners(std::vector<cv::Point2f>& corners){
    Mat &img = imgs->at(currentImg);
    cvtColor(img, *gray_image, CV_BGR2GRAY);
    bool found = findChessboardCorners(img, board_size, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);

    if(found){
        cornerSubPix(*gray_image, corners, Size(11,11), Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
        drawChessboardCorners(*gray_image, board_size, corners, found);
    }

    return found;
}

void Calibration_intr::calibrate(){


    int &nb_lines(board_size.height), &nb_columns(board_size.width),
            squareSize = 2;

    // 3D coordinates of chessboard points

    vector<Point3f> obj;

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
            object_points->push_back(obj);

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

    calibrateCamera(*object_points, *image_points, img.size(), *camera_matrix, *dist_coeffs, *rvecs, *tvecs);// , CV_CALIB_USE_INTRINSIC_GUESS);

    intrParam->setCameraMatrix(*camera_matrix);
    intrParam->setDistCoeffsMatrix(*dist_coeffs);
}

cv::Mat Calibration_intr::undistorted_image() const{
    Mat image_undistorted;
    if(imgs->empty() || intrParam == nullptr || intrParam->empty())
        return image_undistorted;
    else {
        Mat &img = imgs->at(currentImg);
        Mat &dist_coeffs = intrParam->getDistCoeffs();
        Mat &camera_matrix = intrParam->getCameraMatrix();

        undistort(img, image_undistorted, camera_matrix, dist_coeffs);
    }
}
