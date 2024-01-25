//
// Created on 2018/1/11.
//

#ifndef STANDARD_AR_ILLUMINATIONESTIMATIONINONEFILE_C_API_H_
#define STANDARD_AR_ILLUMINATIONESTIMATIONINONEFILE_C_API_H_

#define _USE_MATH_DEFINES

#include <cmath>
#include <cstring>
#include <vector>

namespace Configuration {
    // 监测亮度的图片宽度
    const int ESTIMATE_INTENSITY_IMAGE_WIDTH = 200;
    // 监测亮度的图片高度
    const int ESTIMATE_INTENSITY_IMAGE_HEIGHT = 150;
}

class Utils {
public:
    /*
    两个数近似相等
     Args:
         a, b: 两个实数
    Returns:
         是否近似相等
    */
    static bool approximate_equal(double a, double b);
    /*
     * bgr到亮度
     */
    static float bgr_to_intensity(const float b, const float g, const float r);
};

class InverseToneMapping {
public:
    /*
    将输入图片逆向Tone Mapping
     Args:
         img, img_width, img_height: 输入图片
    */
    static std::vector<float> inverse_tone_mapping(const std::vector<unsigned char>& img, const int img_width, const int img_height);
};

class YUV2BGR {
public:
    /*
     * 将yuv转换为rgb
     * Args:
     * 		y,u,v: yuv
     * Returns:
     * 		r,g,b: 红绿蓝
     */
    static void convertYUVtoRGB(int y, int u, int v, unsigned char& r, unsigned char& g, unsigned char& b);
};

class ImageUtils {
public:
    /*
    从图片中截取标准大小的透视投影图片
    Args:
         original_image, original_image_width, original_image_height: 原始图片
         standard_image_width, standard_image_height: 标准图片的宽和高
         is_image_yuv: 图片是否为yuv nv21
    */
    static std::vector<unsigned char> extract_perspective_image(
            const std::vector<unsigned char> &original_image, const int original_image_width,
            const int original_image_height, const int standard_image_width,
            const int standard_image_height,
            const bool is_image_yuv);
    static std::vector<unsigned char> extract_perspective_image_cstyle(
            const unsigned char* original_image, const int original_image_width,
            const int original_image_height, const int standard_image_width,
            const int standard_image_height,
            const bool is_image_yuv);

    /*
     * 提取图片区域
     * Args:
     *		original_image, original_image_width, original_image_height: 原始图片
     *		row_start, row_end_exclusive: 行的起止区间
     *		col_start, col_end_exclusive: 列的起止区间
     */
    template<typename T>
    static std::vector<T> extract_bgr_region(const std::vector<T> &original_image,
                                             const int original_image_width,
                                             const int original_image_height, const int row_start,
                                             const int row_end_exclusive, const int col_start,
                                             const int col_end_exclusive);

    /*
     * 求图片平均亮度
     * Args:
     * 		image, image_width, image_height: 图片
     * Returns:
     * 		vec1, vec2, vec3: 平均亮度
     */
    template<typename T>
    static void average_intensity(const std::vector<T> &bgr_image, const int image_width,
                                  const int image_height, float &vec1, float &vec2, float &vec3);

	template<typename T>
	static void color_extract(const std::vector<T> &bgr_image, const int image_width,
		const int image_height, float &out_color_b, float &out_color_g, float &out_color_r);

public:
    /*
     * 缩放图片
     * Args:
     * 		original_image, original_image_width, original_image_height: 原始图片
     * 		target_image_width, target_image_height: 目标图片宽高
     * 		is_image_yuv: 图片是否为yuv nv21格式
     */
    static std::vector<unsigned char>
    resize(const std::vector<unsigned char> &original_image, const int original_image_width,
           const int original_image_height, const int target_image_width,
           const int target_image_height, const bool is_image_yuv);
    static std::vector<unsigned char>
    resize_cstyle(const unsigned char *original_image, const int original_image_width,
                 const int original_image_height, const int target_image_width,
                 const int target_image_height, const bool is_image_yuv);

    static unsigned char* resize_cstyle1(const unsigned char *original_image, const int original_image_width,
                  const int original_image_height, const int target_image_width,
                  const int target_image_height, const bool is_image_yuv);
};

/*
光照恢复算法模块接口
*/
class IlluminationEstimationAPI {
private:
    //InverseToneMapping inverseToneMapping;
public:
    /*
     * 估计环境光亮度
     */
    static float* estimate_intensity(
            const float exposure,
            const unsigned char* input_image,
            const int input_image_width,
            const int input_image_height,
            const int input_image_array_size,
            const bool is_input_image_yuv);
};

#endif //STANDARD_AR_ILLUMINATIONESTIMATIONINONEFILE_C_API_H_
