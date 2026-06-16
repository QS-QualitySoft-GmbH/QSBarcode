#include <qsbarcode/qsbarcode.hpp>

int main() {
    qsbarcode::bounds bounds{};
    qsbarcode::barcode_result result{};
    qsbarcode::rendered_image image{};
    result.region = bounds;
    image.gray8 = true;
    image.stride = 1;
    return result.region.width == 0 && image.gray8 && sizeof(qsbarcode::reader) > 0 ? 0 : 1;
}
