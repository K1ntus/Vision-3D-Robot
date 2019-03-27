#include "imgcv.h"
using namespace cv;
using namespace cv::ximgproc;

ImgCv::ImgCv():
    Mat(),
    stereo(false)
{}

ImgCv::ImgCv(std::string filePath, bool is_stereo) :  Mat(), stereo(is_stereo)
{
    Mat img = imread(filePath);
    img.copyTo(*this);

}

ImgCv::ImgCv(const Mat &img, bool is_stereo) :Mat(img), stereo(is_stereo)
{

}

ImgCv::ImgCv(const Mat &imgL, const Mat &imgR, bool is_stereo) : Mat(), stereo(is_stereo)
{
    // put the left and right image side by side
    int leftWidth = imgL.size().width;
    int rightWidth = imgR.size().width;
    cv::Mat cvImg(imgL.size().height, leftWidth + rightWidth, imgL.type());
    cvImg.adjustROI(0, 0, 0, -rightWidth);
    imgL.copyTo(cvImg);

    cvImg.adjustROI(0, 0, -leftWidth, rightWidth);
    imgR.copyTo(cvImg);

    cvImg.adjustROI(0, 0, leftWidth, 0);
    cvImg.copyTo(*this);
}

ImgCv::~ImgCv()
{

}

/**
 * @brief ImgCv::contour_laplace :  Calculate the Laplacian filter to draw picture's countours
 * @param img :  the image loaded
 * @return the converted image
 */
Mat ImgCv::contour_laplace(Mat const&img)
{
    Mat gray_img, result, final;
    Mat img_read = img.clone();


    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT); // apply the gaussianBlur to smooth the img
    if(img_read.channels()!=1)
        cvtColor(img_read,gray_img,CV_BGR2GRAY);
    else
        gray_img=img_read.clone();

    Laplacian(gray_img,result,CV_16S,3,1,0,BORDER_DEFAULT);
    convertScaleAbs(result,final,1,0);                          //convert to a CV_8U image
    return final;
}

/**
* @brief Convert an image following the laplace algorithm
* @return The parameters converted with laplace algorithm
*/

Mat ImgCv::contour_laplace() const
{
    Mat img_read;
    if(this->empty()){
        std::cout << "img is empty" << std::endl;
        return img_read;
    }else
        img_read = this->clone();
    Mat gray_img, result, final;

    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT); // apply the gaussianBlur to smooth the img
    cvtColor(img_read,gray_img,CV_BGR2GRAY);

    Laplacian(gray_img,result,CV_16S,3,1,0,BORDER_DEFAULT);
    convertScaleAbs(result,final,1,0);                          //convert to a CV_8U image
    return final;
}

/**
 * @brief ImgCv::contour_sobel :  convert an image using the Sobel filter. Permits to detect the contours of an image.
 * @param img :  the loaded image
 * @return the converted image
 */
Mat ImgCv::contour_sobel(const Mat &img){
    Mat gray_img,final,gx,gy,gx_goodFormat, gy_goodFormat;
    Mat img_read=img.clone();

    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT);
    if(img_read.channels()!=1)
        cvtColor(img_read,gray_img,CV_BGR2GRAY);
    else
        gray_img=img_read.clone();

    Sobel(gray_img,gx,CV_16S,1,0,3,1,0,BORDER_DEFAULT);  // derivative in x
    Sobel(gray_img,gy,CV_16S,0,1,3,1,0,BORDER_DEFAULT);// derivative in y

    convertScaleAbs(gy,gy_goodFormat,1,0);
    convertScaleAbs(gx,gx_goodFormat,1,0);

    addWeighted(gx_goodFormat,0.5,gy_goodFormat,0.5,0,final,-1); // final gradient is the addition of the two gradients
    return final;

}
/**
* @brief Convert an image following the sobel algorithm
* @param Image to convert
*  @return The parameters converted with sobel algorithm
*/
Mat ImgCv::contour_sobel() const
{
    Mat img_read;
    if(this->empty()){
        std::cout << "img is empty" << std::endl;
        return img_read;

    }else
        img_read = this->clone();

    Mat gray_img,final,gx,gy,gx_goodFormat, gy_goodFormat;

    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT);
    cvtColor(img_read,gray_img,CV_BGR2GRAY);

    Sobel(gray_img,gx,CV_16S,1,0,3,1,0,BORDER_DEFAULT);  // derivative in x
    Sobel(gray_img,gy,CV_16S,0,1,3,1,0,BORDER_DEFAULT);// derivative in y

    convertScaleAbs(gy,gy_goodFormat,1,0);
    convertScaleAbs(gx,gx_goodFormat,1,0);

    addWeighted(gx_goodFormat,0.5,gy_goodFormat,0.5,0,final,-1); // final gradient is the addition of the two gradients
    return final;
}

/**
 * @brief ImgCv::disparity_map_SGBM : compute a disparity map, using SGBM (a block matching method)\n
 * @param IO_minDisparity : the minimal disparity\n
 * @param IO_numberOfDisparities
 * @param IO_SADWindowSize
 * @param IO_P1
 * @param IO_P2
 * @param IO_disp12MaxDif
 * @param IO_preFilterCap
 * @param IO_uniquenessRatio
 * @param IO_speckleWindowSize
 * @param IO_speckleRange
 * @param IO_full_scale\n
 * @return the disparity map computed.
 */
Mat ImgCv::disparity_map_SGBM(const size_t &IO_minDisparity, const size_t &IO_numberOfDisparities, const size_t &IO_SADWindowSize,
                              const size_t &IO_P1, const size_t &IO_P2, const int &IO_disp12MaxDif,
                              const size_t &IO_preFilterCap, const size_t &IO_uniquenessRatio, const size_t &IO_speckleWindowSize,
                              const size_t &IO_speckleRange, const int &IO_full_scale) const
{
    Mat img_right_gray, img_left_gray;
    Mat disp;

    //Convert the two stereo image in gray
    cvtColor(getImgL(), img_left_gray, CV_BGR2GRAY);
    cvtColor(getImgR(),img_right_gray, CV_BGR2GRAY);

    Ptr<StereoSGBM> sgbm = StereoSGBM::create(
                IO_minDisparity,
                IO_numberOfDisparities,
                IO_SADWindowSize,
                IO_P1,
                IO_P2,
                IO_disp12MaxDif,
                IO_preFilterCap,
                IO_uniquenessRatio,
                IO_speckleWindowSize,
                IO_speckleRange,
                IO_full_scale
                );
    sgbm->compute(img_left_gray, img_right_gray, disp);    //Generate the disparity map
    normalize(disp, disp, 0, 255, NORM_MINMAX, CV_8U);
    return disp;
}
/**
 * @brief ImgCv::sbm : Compute the disparity map using the SBM method (a block matching method)
 * @param IO_numberOfDisparities : the number of disparities
 * @param IO_SADWindowSize :  the size of the block
 * @return the disparity map
 */
Mat ImgCv::sbm(const size_t &IO_numberOfDisparities, const size_t &IO_SADWindowSize) const
{
    Mat imgL, imgR;
    Mat dst = Mat(this->size(), CV_8U);
    cvtColor(getImgL(), imgL,CV_BGR2GRAY);
    cvtColor(getImgR(),imgR,CV_BGR2GRAY);

    Ptr<StereoBM> matcher= StereoBM::create(IO_numberOfDisparities,IO_SADWindowSize);
    matcher->compute(imgL,imgR,dst);
    normalize(dst, dst, 0, 255, CV_MINMAX, CV_8U);

    return dst;
}
Mat ImgCv::sbm(const size_t &IO_minDisparity, const size_t &IO_numberOfDisparities, const size_t &IO_SADWindowSize,
               const int &IO_disp12MaxDif,const size_t &IO_preFilterCap, const size_t &IO_uniquenessRatio,
               const size_t &IO_speckleWindowSize,const size_t &IO_speckleRange, const size_t & IO_textureTreshold, const size_t &IO_tresholdFilter) const
{
    Mat imgL, imgR;
    Mat dst;
    cvtColor(getImgL(), imgL,CV_BGR2GRAY);
    cvtColor(getImgR(),imgR,CV_BGR2GRAY);

    Ptr<StereoBM> matcher= StereoBM::create(IO_numberOfDisparities,IO_SADWindowSize);

    matcher->setPreFilterCap(IO_preFilterCap);
    matcher->setMinDisparity(IO_minDisparity);
    matcher->setDisp12MaxDiff(IO_disp12MaxDif);
    matcher->setUniquenessRatio(IO_uniquenessRatio);
    matcher->setSpeckleWindowSize(IO_speckleWindowSize);
    matcher->setSpeckleRange(IO_speckleRange);
    matcher->setTextureThreshold(IO_textureTreshold);
    matcher-> setNumDisparities(IO_numberOfDisparities);

    matcher->compute(imgL,imgR,dst);
    normalize(dst, dst, 0, 255, NORM_MINMAX, CV_8U);
    threshold(dst,dst,IO_tresholdFilter,255,THRESH_TOZERO);

    return dst;
}


/**
 * @brief ImgCv::disparity_post_filtering : Filter the disparity map
 * @return the filtered disparity map
 */
Mat ImgCv::disparity_post_filtering(const size_t &IO_minDisparity, const size_t &IO_numberOfDisparities, const size_t &IO_SADWindowSize, const int &IO_disp12MaxDif, const size_t &IO_preFilterCap, const size_t &IO_uniquenessRatio, const size_t &IO_speckleWindowSize, const size_t &IO_speckleRange, const size_t &IO_textureTreshold, const size_t &IO_tresholdFilter) {
    Mat left_disparity, right_disparity, filtered, left_for_matching, right_for_matching;
    Mat final_disparity_map;
    left_for_matching= getImgL().clone();
    right_for_matching = getImgR().clone();

    Ptr<StereoBM> matcher_left = StereoBM::create(IO_numberOfDisparities, IO_SADWindowSize); // use StereoBM to create a matcher
    matcher_left->setPreFilterCap(IO_preFilterCap);
    matcher_left->setMinDisparity(IO_minDisparity);
    matcher_left->setDisp12MaxDiff(IO_disp12MaxDif);
    matcher_left->setUniquenessRatio(IO_uniquenessRatio);
    matcher_left->setSpeckleWindowSize(IO_speckleWindowSize);
    matcher_left->setSpeckleRange(IO_speckleRange);
    matcher_left->setTextureThreshold(IO_textureTreshold);
    matcher_left-> setNumDisparities(IO_numberOfDisparities);

    Ptr<DisparityWLSFilter> filter = cv::ximgproc::createDisparityWLSFilter(matcher_left); // creation of the filter
    Ptr<StereoMatcher> matcher_right= createRightMatcher(matcher_left);// creation of the right matcher

    cv::cvtColor(left_for_matching,left_for_matching,COLOR_BGR2GRAY);
    cv::cvtColor(right_for_matching, right_for_matching, COLOR_BGR2GRAY);
    matcher_left->compute(left_for_matching,right_for_matching,left_disparity);    // compute the left disparity map
    matcher_right->compute(right_for_matching,left_for_matching, right_disparity); // compute the right disparity map

    filter->setLambda(8000);// lambda defining regularization of the filter.  With a high value, the edge of the disparity map will "more" match with the source image
    filter->setSigmaColor(0.8); // sigma represents the sensitivity of the filter

    filter->filter(left_disparity,getImgL(),filtered,right_disparity); // apply the filter

    cv::ximgproc::getDisparityVis(filtered,final_disparity_map, 10.0);// permits to visualize the disparity map
    normalize(final_disparity_map, final_disparity_map,0,255,CV_MINMAX, CV_8U);
    return final_disparity_map;

}
/**
 * @brief ImgCv::disparity_post_filtering: applies a filter on a disparity map compute with SGBM
 * @param IO_numberOfDisparities
 * @param IO_SADWindowSize
 * @param IO_preFilterCap
 * @param IO_P1
 * @param IO_P2
 * @return the disparity map post_filtered
 */
Mat ImgCv::disparity_post_filtering(const size_t &IO_minDisparity, const size_t &IO_numberOfDisparities, const size_t &IO_SADWindowSize, const size_t &IO_P1, const size_t &IO_P2, const int &IO_disp12MaxDif, const size_t &IO_preFilterCap, const size_t &IO_uniquenessRatio, const size_t &IO_speckleWindowSize, const size_t &IO_speckleRange, const int &IO_full_scale){
    Mat left_disparity, right_disparity, filtered, left_for_matching, right_for_matching;
    Mat final_disparity_map;
    left_for_matching= getImgL();
    right_for_matching = getImgR();
    Ptr <StereoSGBM> matcher_left = StereoSGBM::create(
                IO_minDisparity,
                IO_numberOfDisparities,
                IO_SADWindowSize,
                IO_P1,
                IO_P2,
                IO_disp12MaxDif,
                IO_preFilterCap,
                IO_uniquenessRatio,
                IO_speckleWindowSize,
                IO_speckleRange,
                IO_full_scale
                ); // use SGBM to create a matcher

    //    matcher_left->setPreFilterCap(IO_preFilterCap);
    //    matcher_left->setP1(IO_P1); // smoothness of the disparity map
    //    matcher_left->setP2(IO_P2);

    Ptr<DisparityWLSFilter> filter = cv::ximgproc::createDisparityWLSFilter(matcher_left); // creates the WLSfilter
    Ptr<StereoMatcher> matcher_right= createRightMatcher(matcher_left);


    matcher_left->compute(left_for_matching,right_for_matching,left_disparity); // compute the left disparity map
    matcher_right->compute(right_for_matching,left_for_matching, right_disparity); // compute the right disparity map

    filter->setLambda(12000);
    filter->setSigmaColor(1.5);

    filter->filter(left_disparity,getImgL(),filtered,right_disparity);
    cv::ximgproc::getDisparityVis(filtered,final_disparity_map, 10.0);
    normalize(final_disparity_map, final_disparity_map,0,255,CV_MINMAX, CV_8U);
    // bitwise_not(final_disparity_map,final_disparity_map);
    return final_disparity_map;

}

ImgCv ImgCv::rectifiedImage(ImgCv &distortedImage, const IntrinsicParameters &paramL, const IntrinsicParameters &paramR, const Mat &R, const Mat &T)
{
    return rectifiedImage(distortedImage, paramL.getDistCoeffs(), paramL.getCameraMatrix(),
                          paramR.getDistCoeffs(), paramR.getCameraMatrix(), R, T);
}

/**
 * @brief ImgCv::rectifiedImage
 * @param distortedImage the image to undistort
 * @param dist_coeffsL left parameters of the camera to undistort the pict
 * @param camera_matrixL left parameters of the camera to undistort the pict
 * @param dist_coeffsR right parameters of the camera to undistort the pict
 * @param camera_matrixR right parameters of the camera to undistort the pict
 * @param R rotation parameters of the camera to undistort the pict
 * @param T translation parameters of the camera to undistort the pict
 * @return the rectified image
 */
ImgCv ImgCv::rectifiedImage(ImgCv &distortedImage, cv::Mat const&dist_coeffsL, cv::Mat const&camera_matrixL,
                            cv::Mat const&dist_coeffsR, cv::Mat const&camera_matrixR,
                            cv::Mat const&R, cv::Mat const&T)
{
    if(!distortedImage.isStereo())
        std::cout << "Error: the image is not a stereo file. \nin: Imgcv::rectifiedImage" << std::endl;

    ImgCv rectifiedImg;
    cv::Mat imgLrectified, imgRrectified;
    undistort(distortedImage.getImgL(), imgLrectified, camera_matrixL, dist_coeffsL);
    undistort(distortedImage.getImgR(), imgRrectified, camera_matrixR, dist_coeffsR);
    rectifiedImg.setImg(imgLrectified, imgRrectified);

//        cv::Mat imgLrectified, imgRrectified,
//                rotL, rotR, projL, projR, dispToDepthMat;
//        cv::Mat const &imgL = distortedImage.getImgL();
//        cv::Mat const &imgR = distortedImage.getImgR();

//        cv::Rect roi1, roi2;
//        cv::Size imgSize(imgL.size());
//        cv::stereoRectify(camera_matrixL, dist_coeffsL, camera_matrixR, dist_coeffsR,
//                          imgSize, R, T, rotL, rotR, projL, projR, dispToDepthMat,
//                          cv::CALIB_ZERO_DISPARITY, -1, imgSize, &roi1, &roi2);


//        cv::Mat map11, map12, map21, map22;

//        cv::initUndistortRectifyMap(camera_matrixL, dist_coeffsL, rotL, projL, imgSize, CV_16SC2, map11, map12);
//        cv::initUndistortRectifyMap(camera_matrixR, dist_coeffsR, rotR, projR, imgSize, CV_16SC2, map21, map22);

//        cv::remap(imgL, imgLrectified, map11, map12, cv::INTER_LINEAR);
//        cv::remap(imgR, imgRrectified, map21, map22, cv::INTER_LINEAR);


//        cv::Rect vroi1(cvRound(roi1.x), cvRound(roi1.y),
//                       cvRound(roi1.width), cvRound(roi1.height));



//        cv::Rect vroi2(cvRound(roi2.x), cvRound(roi2.y),
//                       cvRound(roi2.width), cvRound(roi2.height));

//        rectifiedImg.setImg(imgLrectified(vroi1), imgRrectified(vroi2));

    return rectifiedImg;
}

/**
 * @brief ImgCv::rectifiedImage
 * @param distortedImage the image to undistort
 * @param outFile the output file with the parameters to undistort
 * @return the rectified image
 */
ImgCv ImgCv::rectifiedImage(ImgCv &distortedImage, const std::string &outFile)
{
    ImgCv rectifiedImg;

    if(!distortedImage.isStereo())
        std::cout << "Error: the image is not a stereo file. \nin: Imgcv::rectifiedImage" << std::endl;
    FileStorage fs(outFile, FileStorage::READ);
    if(fs.isOpened()){
        cv::Mat imgLrectified, imgRrectified,
                dist_coeffsL, camera_matrixL,
                dist_coeffsR, camera_matrixR,
                R, T, rotL, rotR, projL, projR, dispToDepthMat;

        fs["cameraMatrixLeft"] >> camera_matrixL;
        fs["distCoefficientsMatrixLeft"] >> dist_coeffsL;
        fs["cameraMatrixRight"] >> camera_matrixR;
        fs["distCoefficientsMatrixRight"] >> dist_coeffsR;
        fs["rotationMatrix"] >> R;
        fs["translationMatrix"] >> T;

        fs.release();

        rectifiedImg = rectifiedImage(distortedImage, dist_coeffsL, camera_matrixL,
                                      dist_coeffsR, camera_matrixR,
                                      R, T);
    }else
        std::cout << "Error: Couldn't open file. \nin: Imgcv::rectifiedImage" << std::endl;

    return rectifiedImg;

}

Mat ImgCv::getDispToDepthMat(const std::string &outFile)
{
    cv::Mat dispToDepthMat;
    FileStorage fs(outFile, FileStorage::READ);
    if(fs.isOpened()){

        fs["dispToDepthMatrix"] >> dispToDepthMat;

        fs.release();

    }else
        std::cout << "Error: Couldn't open file. \nin: Imgcv::rectifiedImage" << std::endl;
    return dispToDepthMat;
}

void ImgCv::trackOrb(const ImgCv &image, ImgCv &displayer)
{
    std::vector<cv::Point2f> cornersL, cornersR;
    Mat grayImage, dispL(displayer.getImgL()), dispR(displayer.getImgR());
    cvtColor(image.getImgL(), grayImage, CV_BGR2GRAY);

    double qualityLevel(0.01), minDist(10), k(0.04);
    int blockSize(3), maxCorners(200);
    bool useHarrisDetector(true);

    goodFeaturesToTrack(grayImage, cornersL, maxCorners,
                        qualityLevel, minDist, Mat(),
                        blockSize, useHarrisDetector, k);

    cvtColor(image.getImgR(), grayImage, CV_BGR2GRAY);
    goodFeaturesToTrack(grayImage, cornersR, maxCorners,
                        qualityLevel, minDist, Mat(),
                        blockSize, useHarrisDetector, k);
    std::cout << "Number of corners detected in trackSomething: " << cornersL.size() + cornersR.size() << std::endl;


    int radius(4);
    for(size_t i(0); i < cornersL.size(); i++)
        circle(dispL, cornersL[i], radius, Scalar(RNG(12345).uniform(0, 255),RNG(12345).uniform(0, 255), RNG(12345).uniform(0, 255)) );

    for(size_t i(0); i < cornersR.size(); i++)
        circle(dispR, cornersR[i], radius, Scalar(RNG(12345).uniform(0, 255),RNG(12345).uniform(0, 255), RNG(12345).uniform(0, 255)) );

    std::vector<cv::DMatch> matches;
    std::vector<KeyPoint> descriptorL, descriptorR;
    KeyPoint::convert(cornersL, descriptorL);
    KeyPoint::convert(cornersR, descriptorR);
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
    matcher->match(descriptorL, descriptorR, matches);
    drawMatches(dispL, descriptorL, dispR, descriptorR, matches, displayer);

}

void ImgCv::trackSomething(const Mat &image, Mat &displayer)
{
    std::vector<cv::Point2f> corners;
    Mat grayImage;
    cvtColor(image, grayImage, CV_BGR2GRAY);

    double qualityLevel(0.01), minDist(10), k(0.04);
    int blockSize(3), maxCorners(200);
    bool useHarrisDetector(true);

    goodFeaturesToTrack(grayImage, corners, maxCorners,
                        qualityLevel, minDist, Mat(),
                        blockSize, useHarrisDetector, k);

    std::cout << "Number of corners detected in trackSomething: " << corners.size() << std::endl;


    int radius(4);
    for(size_t i(0); i < corners.size(); i++)
        circle(displayer, corners[i], radius, Scalar(RNG(12345).uniform(0, 255),RNG(12345).uniform(0, 255), RNG(12345).uniform(0, 255)) );

}

void ImgCv::trackCamShift(const Mat &image,Rect &trackWindow)
{

    int hsize(16), vmin(0), vmax(180), smin(180), channels[] = {0, 0};
    float hranges[] = {0, 180};
    const float* phranges = hranges;
    Mat hsv, hue, mask, hist, histimg(Mat::zeros(200,320, CV_8UC3)), backproj;
    cvtColor(image, hsv, COLOR_BGR2HSV);
    inRange(hsv, Scalar(0, smin, vmin), Scalar(180, 256, vmax), mask);
    hue.create(hsv.size(), hsv.depth());
    mixChannels(&hsv, 1, &hue, 1, channels, 1);


    Mat roi(hue, trackWindow), maskroi( mask, trackWindow);
    calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
    normalize(hist, hist, 0, 255, NORM_MINMAX);
    histimg = Scalar::all(0);
    int binW(histimg.cols / hsize);
    Mat buf(1, hsize, CV_8UC3);
    for(size_t i(0); i < hsize; i++){
        buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180./hsize), 255, 255);
    }
    cvtColor(buf, buf, COLOR_HSV2BGR);
    for(size_t i(0); i < hsize; i++){
        int val(saturate_cast<int>(hist.at<float>(i)*histimg.rows/255));
        rectangle(histimg, Point(i*binW, histimg.rows), Point((i+1)*binW, histimg.rows -val),
                  Scalar(buf.at<Vec3b>(i)), -1, 8);
    }

    calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
    backproj &= mask;
    CamShift(backproj, trackWindow, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));



}

/**
 * @brief ImgCv::depthMap :  Compute the depth map using the disparity and the camera parameters\n
 * @param disparityMap Tje dosparity map
 * @param dispToDepthMatrix The matrix containing the intrinsec and extrinsec parameters of the camera
 * @return the depth map
 */
Mat ImgCv::depthMap(const Mat &disparityMap, Mat &dispToDepthMatrix)
{
    Mat depthMapImage;
    cv::Vec3f floatPoint;
    reprojectImageTo3D(disparityMap, depthMapImage, dispToDepthMatrix, true);


    Mat depthMap(depthMapImage.rows, depthMapImage.cols, CV_32F);
    for(size_t i(0); i < depthMapImage.rows; i++){
        for(size_t j(0); j < depthMapImage.cols; j++){
            floatPoint = depthMapImage.at<Vec3f>(i,j);
            depthMap.at<float>(i, j) = floatPoint[2];
        }
    }

    return depthMap;

}

/**
 * @brief depthMapInterpreter : detect the presence of a personne from the image and compute his distance from
 * the source.
 * @param image the left image of the stereo image which allow the detection of a person.
 *  if no human is found the function return [0, 0, 0]
 * @param disparityMap the coresponding disparity of the stereo image.
 * @param depthMap the coresponding depth map of the stereo image.
 * @return a 1 * 4 matrix with the position of the human in the image and is distance from the source:
 *      [vx: position x of the human,
 *       vy: position y of the human,
 *      theta: the rotation angle of the position of vx, vy and the orthonormal basis,
 *      d: distance of the human from the source]
 */
Mat ImgCv::depthMapInterpreter(Mat image, const Mat &disparityMap, Mat &depthMap)
{
    Mat computedResult(1, 4, CV_32F);
    cv::Rect2f roi;

    if(roi.area() <= 0)
        return computedResult;
    for(size_t i(roi.x); i < disparityMap.rows - (size_t)(roi.x + roi.width); i++){
        for(size_t j(roi.y); j < disparityMap.cols - (size_t)(roi.y + roi.height); j++){
            float d = depthMap.at<float>(i, j);
        }
    }
    return computedResult;
}
/**
 * @brief ImgCv::isStereo : Check if
 * @return the stereo boolean value
 */
bool ImgCv::isStereo() const
{
    return stereo;
}

/**
 * @brief ImgCv::getImg : Clone an image and recover it.
 * @return a clone of the image loaded
 */
Mat ImgCv::getImg() const
{
    return this->clone();
}

/**
 * @brief ImgCv::getImgL : Get the left side of a stereo image
 * @return  the left image
 */
Mat ImgCv::getImgL() const
{
    Range rows(0, this->rows), columns(0, this->cols >> 1);
    return this->operator()(rows, columns);
}

/**
 * @brief ImgCv::getImgR : Get the right side of an image
 * @return the right image
 */
Mat ImgCv::getImgR() const
{
    Range rows(0, this->rows), columns((this->cols & 1) ? (this->cols >> 1) + this->cols % 2 : this->cols >> 1, this->cols);
    return this->operator()(rows, columns);
}

/**
 * @brief ImgCv::getDisparityMap
 * @return  the disparity map
 */
Mat ImgCv::getDisparityMap(const std::string &calibFile, cv::Mat param ){
    FileStorage fs(calibFile, FileStorage::READ);
    if(fs.isOpened()){

        fs["DisparityParameter"] >> param;
        cv::Mat res;
        switch(param.at<int>(0)){
        case 0:
            std::cout << "sbm entries" << std::endl;

            res = sbm(
                        param.at<int>(1), param.at<int>(2), param.at<int>(3),
                        param.at<int>(4), param.at<int>(5), param.at<int>(6),
                        param.at<int>(7), param.at<int>(8), param.at<int>(9), param.at<int>(10)
                        );

            std::cout << "sbm done" << std::endl;

            break;
        case 1:
            std::cout << "sbm + PS entries" << std::endl;

            res = disparity_post_filtering(
                        param.at<int>(1), param.at<int>(2), param.at<int>(3),
                        param.at<int>(4), param.at<int>(5), param.at<int>(6),
                        param.at<int>(7), param.at<int>(8), param.at<int>(9), param.at<int>(10)
                        );
            std::cout << "sbm  +PS done" << std::endl;
            break;
        case 2:
            std::cout << "SGBM entries" << std::endl;

            res = disparity_map_SGBM(
                        param.at<int>(1), param.at<int>(2), param.at<int>(3),
                        param.at<int>(4), param.at<int>(5), param.at<int>(6),
                        param.at<int>(7), param.at<int>(8), param.at<int>(9),
                        param.at<int>(10),  param.at<int>(11)
                        );
            std::cout << "SGBM done" << std::endl;
            break;
        case 3:
            std::cout << "SGBM + PS entries" << std::endl;

            res = disparity_post_filtering(
                        param.at<int>(1), param.at<int>(2), param.at<int>(3),
                        param.at<int>(4), param.at<int>(5), param.at<int>(6),
                        param.at<int>(7), param.at<int>(8), param.at<int>(9),
                        param.at<int>(10),  param.at<int>(11)
                        );
            std::cout << "SGBM + PS done" << std::endl;

            break;
        default:
            std::cout << "[ERROR] can't match to any case" << std::endl;
            break;

        }
        fs.release();
        return res;
    }else
        return sbm(((this->size().width >> 3) + 15) & -16 , 15);
}

/**
 * @brief ImgCv::getDepthMap
 * @param TProjectionMat The matrix containing the intrinsec and extrinsec parameters of the camera
 * @return the depth map
 */
Mat ImgCv::getDepthMap(Mat &TProjectionMat){
    return depthMap(getDisparityMap(), TProjectionMat);
}

/**
 * @brief ImgCv::setImg set the image, is the stereo parameters is enable, automatically split them
 *
 * @param newImg image to set
 * @param isStereo check if the image is stereo
 */
void ImgCv::setImg(const Mat &img, bool isStereo){
    img.copyTo(*this);
    stereo = isStereo;
}

void ImgCv::setImg(const Mat &imgL, const Mat &imgR)
{
    if(imgL.empty() || imgR.empty()){
        std::cout << "one of the image is empty in ImgCv::setImg\n"<< std::endl;
        return;
    }
    hconcat(imgL, imgR, *this);
    //    const int& leftWidth = imgL.cols;
    //    const int& rightWidth = imgR.cols;
    //    Mat res(imgL.rows, leftWidth + rightWidth,imgL.type());
    //    res.adjustROI(0, 0, 0, -rightWidth);
    //    imgL.copyTo(res);

    //    res.adjustROI(0, 0, -leftWidth, rightWidth);
    //    imgR.copyTo(res);

    //    res.adjustROI(0, 0, leftWidth, 0);
    //    res.copyTo(*this);
    stereo = true;
}

/**
 * @brief ImgCv::split Split the first parameters into two image of the same width and put them into the two others parameters
 *
 * @param img image to split
 * @param img_left left splitted image
 * @param img_right right splitted image
 */
void ImgCv::split(Mat &img, Mat &imgL, Mat &imgR){
    int width= img.cols >> 1 ;
    int x_right= width +img.cols%2; //First width position for the right image
    std::cout<<x_right<<std::endl;
    // check if the ptr is already in use
    img.adjustROI(0,0, 0, -x_right);
    img.copyTo(imgL);

    img.adjustROI(0,0, -x_right, x_right);
    img.copyTo(imgR);

    img.adjustROI(0,0, x_right, 0);
}


Mat ImgCv::orbDetection(const Mat &firstImage, const Mat &secondImage){
    Mat descriptorL, descriptorR, dst;
    Ptr<AKAZE> detector = AKAZE::create();
    //Ptr<ORB> detector =ORB::create();
    std::vector<KeyPoint> keypointL, keypointR;
    std::vector<DMatch> matches, best_matches;
    detector->detectAndCompute(firstImage,noArray(),keypointL,descriptorL);
    detector->detectAndCompute(secondImage, noArray(),keypointR,descriptorR);
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
    matcher->match(descriptorL, descriptorR, matches);
    float d_max = 0, d_min = 125;

    for(int i = 0; i<(int)matches.size(); i++){  //Find d_max && d_min
        if(matches[i].distance<d_min)
            d_min = matches[i].distance;
        if(matches[i].distance > d_max)
            d_max= matches[i].distance;
    }
    for (int i = 0; i<(int) matches.size(); i++){       //select only keypoint with low distance
        if(matches[i].distance <=4.0*d_min)
            best_matches.push_back(matches[i]);

    }
    cv::drawMatches(firstImage, keypointL, secondImage, keypointR,best_matches,dst);

    std::vector<Point2f> object, scene;
    for(unsigned int i = 0; i<best_matches.size();i++){
        object.push_back(keypointL[best_matches[i].queryIdx].pt);
        scene.push_back(keypointR[best_matches[i].trainIdx].pt);

    }
    return dst;
}
