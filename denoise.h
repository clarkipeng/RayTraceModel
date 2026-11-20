#ifndef DENOISE_H
#define DENOISE_H

#include <OpenImageDenoise/oidn.hpp>
#include <iostream>
#include <vector>

// Post-process: run Intel Open Image Denoise over a noisy linear-light float RGB buffer, in place.
// `rgb` holds nx*ny*3 floats (any row order). Returns false (buffer untouched) if OIDN reports an error.
inline bool denoise_rgb(std::vector<float> &rgb, int nx, int ny, std::vector<float> *albedo = nullptr, std::vector<float> *normal = nullptr) {
    oidn::DeviceRef device = oidn::newDevice();
    device.commit();
    std::vector<float> out(rgb.size());
    oidn::FilterRef filter = device.newFilter("RT");
    filter.setImage("color", rgb.data(), oidn::Format::Float3, nx, ny);
    filter.setImage("output", out.data(), oidn::Format::Float3, nx, ny);
    if (albedo) filter.setImage("albedo", albedo->data(), oidn::Format::Float3, nx, ny);
    if (albedo && normal) filter.setImage("normal", normal->data(), oidn::Format::Float3, nx, ny);
    filter.set("hdr", true);
    filter.commit();
    filter.execute();
    const char *msg;
    if (device.getError(msg) != oidn::Error::None) { std::cerr << "OIDN: " << msg << std::endl; return false; }
    rgb.swap(out);
    return true;
}

#endif
