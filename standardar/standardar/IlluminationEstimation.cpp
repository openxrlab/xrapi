//
// Created on 2018/1/11.
//

#include "IlluminationEstimation.h"
static int target_size = 0;
static unsigned char* target_img;

bool Utils::approximate_equal(double a, double b) {
    return std::abs(a - b) < 1e-6;
}
float Utils::bgr_to_intensity(const float b, const float g, const float r) {
    return 0.2126*r + 0.7152*g + 0.0722*b;
}

std::vector<float> inverse_tone_mapping_temp(256);
std::vector<float> InverseToneMapping::inverse_tone_mapping(const std::vector<unsigned char>& img, const int img_width, const int img_height) {
    //const double gamma = 2.2;
    std::vector<float> result(img_width*img_height*3);
    for (int i = 0; i < result.size(); ++i) {
        result[i] = (img[i] / 255.0f);
    }
    return result;
    /*const double gamma = 2.2;
    static bool bInitialTemp = false;
    if (!bInitialTemp){
        for (int i = 0; i < 256; ++i) {
            inverse_tone_mapping_temp[i] = pow((i / 255.), gamma);
        }
        bInitialTemp = true;
    }
    std::vector<float> result(img_width*img_height*3);
    int i;
    for (i = 0; i <= result.size() - 8; i += 8) {
        result[i + 0] = inverse_tone_mapping_temp[img[i + 0]];
        result[i + 1] = inverse_tone_mapping_temp[img[i + 1]];
        result[i + 2] = inverse_tone_mapping_temp[img[i + 2]];
        result[i + 3] = inverse_tone_mapping_temp[img[i + 3]];
        result[i + 4] = inverse_tone_mapping_temp[img[i + 4]];
        result[i + 5] = inverse_tone_mapping_temp[img[i + 5]];
        result[i + 6] = inverse_tone_mapping_temp[img[i + 6]];
        result[i + 7] = inverse_tone_mapping_temp[img[i + 7]];
    }
    for (; i < result.size(); ++i) {
        result[i] = inverse_tone_mapping_temp[img[i]];
    }
    return result;*/
}

void YUV2BGR::convertYUVtoRGB(int y, int u, int v, unsigned char& r, unsigned char& g, unsigned char& b) {
    int int_r = y + (int)1.402f*u;
    int int_g = y - (int)(0.344f*v + 0.714f*u);
    int int_b = y + (int)1.772f*v;
    r = int_r>255 ? 255 : int_r<0 ? 0 : int_r;
    g = int_g>255 ? 255 : int_g<0 ? 0 : int_g;
    b = int_b>255 ? 255 : int_b<0 ? 0 : int_b;
}

std::vector<unsigned char> ImageUtils::extract_perspective_image(
        const std::vector<unsigned char> &original_image, const int original_image_width,
        const int original_image_height, const int standard_image_width,
        const int standard_image_height,
        const bool is_image_yuv) {
	int x_begin=0, y_begin=0;
    const double original_image_width_height_ratio = 1.0 * original_image_width / original_image_height;
    const double standard_image_width_height_ratio = 1.0 * standard_image_width / standard_image_height;
    std::vector<unsigned char> result;
    // 恰好比例相同
	
    if (Utils::approximate_equal(original_image_width_height_ratio, standard_image_width_height_ratio)) {
        result = resize(original_image, original_image_width, original_image_height, standard_image_width, standard_image_height, is_image_yuv);
    }
        // 原图矮胖
    else if (original_image_width_height_ratio > standard_image_width_height_ratio) {
        // 填满原图高度
        const double scale_factor = 1.0 * standard_image_height / original_image_height;
        const int scaled_image_width = scale_factor * original_image_width;
        std::vector<unsigned char> scaled_image = resize(original_image, original_image_width, original_image_height, scaled_image_width, standard_image_height, is_image_yuv);
        // 取横向中间部分
        x_begin = (scaled_image_width - standard_image_width) / 2.0;
        result = extract_bgr_region<unsigned char>(scaled_image, scaled_image_width,
                                                   standard_image_height, 0,
                                                   standard_image_height, x_begin,
                                                   x_begin + standard_image_width);
    }
        // 原图高瘦
    else {
        // 填满原图宽度
        const double scale_factor = 1.0 * standard_image_width / original_image_width;
        const int scaled_image_height = scale_factor * original_image_height;
        std::vector<unsigned char> scaled_image = resize(original_image, original_image_width, original_image_height, standard_image_width, scaled_image_height, is_image_yuv);
        // 取纵向中间部分
        y_begin = (scaled_image_height - standard_image_height) / 2.0;
        result = extract_bgr_region<unsigned char>(scaled_image, standard_image_width,
                                                   scaled_image_height, y_begin,
                                                   y_begin + standard_image_height, 0,
                                                   standard_image_width);
    }
    return result;
}

std::vector<unsigned char> ImageUtils::extract_perspective_image_cstyle(
        const unsigned char *original_image, const int original_image_width,
        const int original_image_height, const int standard_image_width,
        const int standard_image_height,
        const bool is_image_yuv) {
	int x_begin=0, y_begin=0;
    const double original_image_width_height_ratio = 1.0 * original_image_width / original_image_height;
    const double standard_image_width_height_ratio = 1.0 * standard_image_width / standard_image_height;
    std::vector<unsigned char> result;
    // 恰好比例相同
	
    if (Utils::approximate_equal(original_image_width_height_ratio, standard_image_width_height_ratio)) {
        result = resize_cstyle(original_image, original_image_width, original_image_height, standard_image_width, standard_image_height, is_image_yuv);
    }
        // 原图矮胖
    else if (original_image_width_height_ratio > standard_image_width_height_ratio) {
        // 填满原图高度
        const double scale_factor = 1.0 * standard_image_height / original_image_height;
        const int scaled_image_width = scale_factor * original_image_width;
        std::vector<unsigned char> scaled_image = resize_cstyle(original_image, original_image_width, original_image_height, scaled_image_width, standard_image_height, is_image_yuv);
        // 取横向中间部分
        x_begin = (scaled_image_width - standard_image_width) / 2.0;
        result = extract_bgr_region<unsigned char>(scaled_image, scaled_image_width,
                                                   standard_image_height, 0,
                                                   standard_image_height, x_begin,
                                                   x_begin + standard_image_width);
    }
        // 原图高瘦
    else {
        // 填满原图宽度
        const double scale_factor = 1.0 * standard_image_width / original_image_width;
        const int scaled_image_height = scale_factor * original_image_height;
        std::vector<unsigned char> scaled_image = resize_cstyle(original_image, original_image_width, original_image_height, standard_image_width, scaled_image_height, is_image_yuv);
        // 取纵向中间部分
        y_begin = (scaled_image_height - standard_image_height) / 2.0;
        result = extract_bgr_region<unsigned char>(scaled_image, standard_image_width,
                                                   scaled_image_height, y_begin,
                                                   y_begin + standard_image_height, 0,
                                                   standard_image_width);
    }
    return result;
}

template<typename T>
std::vector<T> ImageUtils::extract_bgr_region(const std::vector<T> &original_image,
                                         const int original_image_width,
                                         const int original_image_height, const int row_start,
                                         const int row_end_exclusive, const int col_start,
                                         const int col_end_exclusive){
    /*std::vector<T> result((row_end_exclusive - row_start) * (col_end_exclusive - col_start) * 3);
    const int result_image_width = col_end_exclusive - col_start;
    for (int original_row = row_start; original_row < row_end_exclusive; ++original_row){
        for (int original_col = col_start; original_col < col_end_exclusive; ++original_col){
            int original_index = (original_row*original_image_width+original_col)*3;
            int result_index = ((original_row-row_start)*result_image_width+(original_col-col_start))*3;
            result[result_index] = original_image[original_index];
            result[result_index+1] = original_image[original_index+1];
            result[result_index+2] = original_image[original_index+2];
        }
    }
    return result;*/
    std::vector<T> result((row_end_exclusive - row_start) * (col_end_exclusive - col_start) * 3);
    T* result_pointer = result.data();
    const T* image_pointer = original_image.data();

    const int result_image_width = (col_end_exclusive - col_start) * 3;
    for (int original_row = row_start; original_row < row_end_exclusive; ++original_row){
        int original_index = (original_row * original_image_width) * 3;
        int result_index = (original_row - row_start) * result_image_width;
        memcpy(result_pointer + result_index, image_pointer + original_index,
               result_image_width * sizeof(T));
    }
    return result;
}

template<typename T>
void ImageUtils::average_intensity(const std::vector<T>& bgr_image, const int image_width, const int image_height, float& vec1, float& vec2, float& vec3){
    vec1 = 0;
    vec2 = 0;
    vec3 = 0;
    for (int row = 0; row < image_height; ++row) {
        for (int col = 0; col < image_width; ++col) {
            int index = (row * image_width + col) * 3;
            vec1 += bgr_image[index];
            vec2 += bgr_image[index+1];
            vec3 += bgr_image[index+2];
        }
    }
    vec1 /= image_height * image_width;
    vec2 /= image_height * image_width;
    vec3 /= image_height * image_width;
}

template<typename T>
void ImageUtils::color_extract(const std::vector<T>& bgr_image, const int image_width, const int image_height, float& out_color_b, float& out_color_g, float& out_color_r) {
	
	//int b, g, r;
	const int delta = 12;
	int half_delta = delta / 2;
	const int box_num = (255 + half_delta) / delta + 1;
	int* bottle = new int[box_num*box_num*box_num];
	//int bottle[10 * 10 * box_num];
	memset(bottle, 0, box_num*box_num*box_num * 4);

	for (int row = 0; row < image_height; ++row) {
		for (int col = 0; col < image_width; ++col) {
			int index = (row * image_width + col) * 3;

			int b = (int)(bgr_image[index] * 255.0f);
			int g = (int)(bgr_image[index + 1] * 255.0f);
			int r = (int)(bgr_image[index + 2] * 255.0f);

			b = (b + half_delta) / delta;
			g = (g + half_delta) / delta;
			r = (r + half_delta) / delta;

			int index2 = b * box_num * box_num + g * box_num + r;
			bottle[index2]++;
		}
	}

	int max_num = 0;
	int max_index = 0;
	for (int i = 0; i < box_num * box_num * box_num; i++) {

		if (bottle[i] > max_num) {
			max_num = bottle[i];
			max_index = i;
		}
	}

	int b_color = max_index / (box_num * box_num);
	int g_color = (max_index - b_color * (box_num * box_num)) / box_num;
	int r_color = (max_index - b_color * (box_num * box_num) - box_num * g_color);

	b_color = b_color*delta > 255 ? 255 : b_color*delta;
	g_color = g_color*delta > 255 ? 255 : g_color*delta;
	r_color = r_color*delta > 255 ? 255 : r_color*delta;

	
	out_color_b = b_color / 255.0f;
	out_color_g = g_color / 255.0f;
	out_color_r = r_color / 255.0f;
}


std::vector<unsigned char> ImageUtils::resize(const std::vector<unsigned char>& original_image,
                                              const int original_image_width,
                                              const int original_image_height,
                                              const int target_image_width,
                                              const int target_image_height,
                                              const bool is_image_yuv){
    std::vector<unsigned char> target_image(target_image_height*target_image_width*3);
    // 用最简单的近邻
    for (int target_row = 0; target_row < target_image_height; ++target_row){
        const int original_row = original_image_height * target_row / target_image_height;
        for (int target_col = 0; target_col < target_image_width; ++target_col){
            const int original_col = original_image_width * target_col / target_image_width;
            const int original_index = (original_row * original_image_width + original_col)*3;
            const int target_index = (target_row * target_image_width + target_col)*3;
            if (is_image_yuv){
                const int y_index = original_row * original_image_width + original_col;
                const int u_index = original_image_width*original_image_height + y_index/(2*original_image_width)*original_image_width + (y_index%original_image_width)/2*2;
                const int v_index = u_index + 1;
                const int y = original_image[y_index] & 0xff;
                const int u = (original_image[u_index] & 0xff) - 128;
                const int v = (original_image[v_index] & 0xff) - 128;
                unsigned char r,g,b;
                YUV2BGR::convertYUVtoRGB(y, u, v, r, g, b);
                target_image[target_index] = b;
                target_image[target_index+1] = g;
                target_image[target_index+2] = r;
            }else{
                target_image[target_index] = original_image[original_index];
                target_image[target_index+1] = original_image[original_index+1];
                target_image[target_index+2] = original_image[original_index+2];
            }
        }
    }
    return target_image;
}

unsigned char* ImageUtils::resize_cstyle1(const unsigned char *original_image,
                                 const int original_image_width, const int original_image_height,
                                 const int target_image_width, const int target_image_height,
                                 const bool is_image_yuv)
{
    if (target_size < target_image_height * target_image_width) {
        target_size = target_image_height * target_image_width;
        target_img = new unsigned char[target_size];
    }

    for (int target_row = 0; target_row < target_image_height; ++target_row){
        const int original_row = original_image_height * target_row / target_image_height;
        for (int target_col = 0; target_col < target_image_width; ++target_col){
            const int original_col = original_image_width * target_col / target_image_width;
            const int original_index = (original_row * original_image_width + original_col);
            const int target_index = (target_row * target_image_width + target_col);

            target_img[target_index] = original_image[original_index];
        }
    }
    return target_img;
}

std::vector<unsigned char> ImageUtils::resize_cstyle(const unsigned char *original_image,
                                                     const int original_image_width,
                                                     const int original_image_height,
                                                     const int target_image_width,
                                                     const int target_image_height,
                                                     const bool is_image_yuv){
    std::vector<unsigned char> target_image(target_image_height * target_image_width * 3);

    std::vector<int> x_ofs(target_image_width);
    double rh = original_image_height * 1.0 / target_image_height;
    double rw = original_image_width * 1.0 / target_image_width;
    for (int target_col = 0; target_col < target_image_width; ++target_col){
        x_ofs[target_col] = (int)(target_col * rw);
    }

    if (is_image_yuv){
        for (int target_row = 0; target_row < target_image_height; ++target_row){
            const int original_row = (int)(target_row * rh);
            int target_index = target_row * target_image_width * 3;
            for (int target_col = 0; target_col < target_image_width; ++target_col){
                const int original_col = x_ofs[target_col];

                const int y_index = original_row * original_image_width + original_col;
                const int u_index = original_image_width*original_image_height + y_index/(2*original_image_width)*original_image_width + (y_index%original_image_width)/2*2;
                const int v_index = u_index + 1;
                const int y = original_image[y_index] & 0xff;
                const int u = (original_image[u_index] & 0xff) - 128;
                const int v = (original_image[v_index] & 0xff) - 128;
                unsigned char r,g,b;
                YUV2BGR::convertYUVtoRGB(y, u, v, r, g, b);
                target_image[target_index] = b;
                target_image[target_index+1] = g;
                target_image[target_index+2] = r;
                target_index += 3;
            }
        }
    } else{
        for (int target_row = 0; target_row < target_image_height; ++target_row){
            const int original_row = (int)(target_row * rh);
            int target_index = target_row * target_image_width * 3;
            for (int target_col = 0; target_col < target_image_width; ++target_col){
                const int original_col = x_ofs[target_col];
                const int original_index = (original_row * original_image_width + original_col) * 3;
                target_image[target_index] = original_image[original_index];
                target_image[target_index+1] = original_image[original_index+1];
                target_image[target_index+2] = original_image[original_index+2];
                target_index += 3;
            }
        }
    }
    return target_image;
}



float* IlluminationEstimationAPI::estimate_intensity(
        const float exposure,
        const unsigned char* input_image,
        const int input_image_width,
        const int input_image_height,
        const int input_image_array_size,
        const bool is_input_image_yuv){
    std::vector<unsigned char> estimate_intensity_size_image;
    //estimate_intensity_size_image.assign(input_image, input_image + input_image_array_size);
    //estimate_intensity_size_image = ImageUtils::extract_perspective_image(estimate_intensity_size_image, input_image_width, input_image_height, Configuration::ESTIMATE_INTENSITY_IMAGE_WIDTH, Configuration::ESTIMATE_INTENSITY_IMAGE_HEIGHT, is_input_image_yuv);
    estimate_intensity_size_image = ImageUtils::extract_perspective_image_cstyle(input_image, input_image_width, input_image_height, Configuration::ESTIMATE_INTENSITY_IMAGE_WIDTH, Configuration::ESTIMATE_INTENSITY_IMAGE_HEIGHT, is_input_image_yuv);

    std::vector<float> hdr_bgr_input = InverseToneMapping::inverse_tone_mapping(estimate_intensity_size_image, Configuration::ESTIMATE_INTENSITY_IMAGE_WIDTH, Configuration::ESTIMATE_INTENSITY_IMAGE_HEIGHT);
    float average_b = 0;
    float average_g = 0;
    float average_r = 0;
    ImageUtils::average_intensity<float>(hdr_bgr_input, Configuration::ESTIMATE_INTENSITY_IMAGE_WIDTH, Configuration::ESTIMATE_INTENSITY_IMAGE_HEIGHT, average_b, average_g, average_r);
    
	float extract_b = 0;
	float extract_g = 0;
	float extract_r = 0;
	ImageUtils::color_extract<float>(hdr_bgr_input, Configuration::ESTIMATE_INTENSITY_IMAGE_WIDTH, Configuration::ESTIMATE_INTENSITY_IMAGE_HEIGHT, extract_b, extract_g, extract_r);
	float intensity = Utils::bgr_to_intensity(average_b, average_g, average_r) / exposure;
	float* result = new float[4];
	result[0] = extract_r;
	result[1] = extract_g;
	result[2] = extract_b;
	result[3] = intensity;
	return result;
}
