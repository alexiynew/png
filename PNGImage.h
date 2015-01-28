#ifndef PNGIMAGE_H
#define PNGIMAGE_H

#include <string>
#include <memory>
#include <exception>

namespace png {

class PNGImage {
public:
    PNGImage() noexcept;
    
    PNGImage(const PNGImage& other);
    PNGImage(PNGImage&& other) noexcept;
    
    PNGImage& operator= (const PNGImage& other);
    PNGImage& operator= (PNGImage&& other) noexcept;
    
    ~PNGImage();
    
    bool open (const std::string& file_name);
    bool create (size_t width, size_t height);
    bool save_as (const std::string& file_name);
    
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

struct NotImplemented : std::exception {
  const char* what() const noexcept {return "Function Not Implemented!\n";}
};


} // namespace png

#endif