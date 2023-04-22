#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "geometry.h"


class ObjParser{

    private:
        std::string filePath;

        ObjParser(const ObjParser&) = delete;
        ObjParser(ObjParser&&) = delete;
        ObjParser& operator=(const ObjParser&) = delete;
        ObjParser& operator=(ObjParser&&) = delete;
    public:
        std::vector<Vec3f> triangles;
        std::vector<Vec3i> faces;
        ObjParser(const std::string&);
        void parse();
};