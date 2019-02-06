#include "intrinsicparameters.h"
using namespace std;
using namespace cv;

IntrinsicParameters::IntrinsicParameters() :
    camera_matrix(nullptr),
    dist_coeffs(nullptr)
{
    camera_matrix = new Mat(3, 3, CV_32FC1);
    camera_matrix->ptr<float>(0)[0] = 1;
    camera_matrix->ptr<float>(1)[1] = 1;
    dist_coeffs = new Mat;
}

IntrinsicParameters::IntrinsicParameters(cv::Mat& new_camera_matrix, cv::Mat& dist_coeffs_mat) :
    camera_matrix(nullptr),
    dist_coeffs(nullptr)
{
    camera_matrix = new Mat;
    dist_coeffs = new Mat;
    new_camera_matrix.copyTo(*camera_matrix);
    dist_coeffs_mat.copyTo(*dist_coeffs);

}

IntrinsicParameters::~IntrinsicParameters()
{
    delete camera_matrix;
    delete dist_coeffs;
}


Mat& IntrinsicParameters::getCameraMatrix() const{
    return *camera_matrix;
}

Mat& IntrinsicParameters::getDistCoeffs() const{
    return *dist_coeffs;
}

void IntrinsicParameters::setCameraMatrix(Mat &camera_matrix)
{
    camera_matrix.copyTo(*this->camera_matrix);
}

void IntrinsicParameters::setDistCoeffsMatrix(Mat &dist_coeffs_mat)
{
    dist_coeffs_mat.copyTo(*this->dist_coeffs);
}

bool IntrinsicParameters::empty()
{
    return dist_coeffs  == nullptr || dist_coeffs->empty() || camera_matrix == nullptr;
}

bool IntrinsicParameters::readIntrCalibration(std::string const &filename, IntrinsicParameters &outputIntrinsicParam)
{
    FileStorage in(filename, FileStorage::READ);
    if(in.isOpened()){

        Mat &cameraMat = outputIntrinsicParam.getCameraMatrix(),
                distCoeffs;

        in["cameraMatrix"] >> cameraMat;
        in["distCoefficientsMatrix"] >> distCoeffs;

        in.release();
        outputIntrinsicParam.setCameraMatrix(cameraMat);
        outputIntrinsicParam.setDistCoeffsMatrix(distCoeffs);
        return true;

    }else
        cout << "error while opening file stream" << endl;
    return false;

}

bool IntrinsicParameters::readIntrStereoCalibration(string const &filename, IntrinsicParameters &outputIntrinsicParamLeft, IntrinsicParameters &outputIntrinsicParamRight)
{
    FileStorage in(filename, FileStorage::READ);
    if(in.isOpened()){

        Mat cameraMat, distCoeffs;

        in["cameraMatrixLeft"] >> cameraMat;
        in["distCoefficientsMatrixRight"] >> distCoeffs;

        outputIntrinsicParamLeft.setCameraMatrix(cameraMat);
        outputIntrinsicParamLeft.setDistCoeffsMatrix(distCoeffs);


        in["cameraMatrixRight"] >> cameraMat;
        in["distCoefficientsMatrixRight"] >> distCoeffs;

        outputIntrinsicParamRight.setCameraMatrix(cameraMat);
        outputIntrinsicParamRight.setDistCoeffsMatrix(distCoeffs);
        in.release();

        return true;

    }else
        cout << "error while opening file stream" << endl;
    return false;
}

bool IntrinsicParameters::readIntrCalibration(const string &filename, Mat &cameraMat, Mat &distCoeffs)
{
    FileStorage in(filename, FileStorage::READ);
    if(in.isOpened()){

        in["cameraMatrixLeft"] >> cameraMat;
        in["distCoefficientsMatrixRight"] >> distCoeffs;

        in["cameraMatrixRight"] >> cameraMat;
        in["distCoefficientsMatrixRight"] >> distCoeffs;

         in.release();

        return true;

    }else
        cout << "error while opening file stream" << endl;
    return false;
}

bool IntrinsicParameters::readIntrStereoCalibration(const string &filename, Mat &cameraMatL, Mat &distCoeffsL, Mat &cameraMatR, Mat &distCoeffsR)
{
    FileStorage in(filename, FileStorage::READ);
    if(in.isOpened()){

        Mat cameraMat, distCoeffs;

        in["cameraMatrixLeft"] >> cameraMatL;
        in["distCoefficientsMatrixRight"] >> distCoeffsL;


        in["cameraMatrixRight"] >> cameraMatR;
        in["distCoefficientsMatrixRight"] >> distCoeffsR;

       in.release();

        return true;

    }else
        cout << "error while opening file stream" << endl;
    return false;
}

bool IntrinsicParameters::printIntrCalibration(string const &filename, IntrinsicParameters const &intrinsicParamToPrint)
{
    return printIntrCalibration(filename, intrinsicParamToPrint.getCameraMatrix(), intrinsicParamToPrint.getDistCoeffs());
}

bool IntrinsicParameters::printIntrStereoCalibration(string const &filename, IntrinsicParameters const &intrinsicParamLeftToPrint, IntrinsicParameters const &intrinsicParamRightToPrint)
{
    return printIntrStereoCalibration(filename, intrinsicParamLeftToPrint.getCameraMatrix(), intrinsicParamLeftToPrint.getDistCoeffs(),
                                      intrinsicParamRightToPrint.getCameraMatrix(), intrinsicParamRightToPrint.getDistCoeffs());

}

bool IntrinsicParameters::printIntrCalibration(const string &filename, Mat &cameraMat, Mat &distCoeffs)
{
    FileStorage out(filename, FileStorage::WRITE);
    if(out.isOpened()){

        out << "cameraMatrix" << cameraMat;
        out << "distCoefficientsMatrix" << distCoeffs;

        out.release();
        return true;

    }
    else
        cout << "error while opening file stream" << endl;
    return false;
}

bool IntrinsicParameters::printIntrStereoCalibration(const string &filename, Mat &cameraMatL, Mat &distCoeffsL, Mat &cameraMatR, Mat &distCoeffsR)
{
    FileStorage out(filename, FileStorage::WRITE);
    if(out.isOpened()){

        out << "cameraMatrixLeft" << cameraMatL;
        out << "distCoefficientsMatrixLeft" << distCoeffsL;


        out << "cameraMatrixRight" << cameraMatR;
        out << "distCoefficientsMatrixRight" << distCoeffsR;

        out.release();
        return true;

    }
    else
        cout << "error while opening file stream" << endl;
    return false;
}
