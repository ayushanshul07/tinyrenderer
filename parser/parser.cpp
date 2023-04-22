#include "parser.h"

ObjParser::ObjParser(const std::string& path): filePath(std::string("../external/obj/african_head.obj")){}

void ObjParser::parse(){

    std::ifstream obj_file(filePath);

    if(obj_file.is_open()){
        std::string line;
        while(std::getline(obj_file, line)){
            std::istringstream iss(line);
            if(!line.compare(0, 2, "v ")){
                char trash;
                iss >> trash;
                Vec3f v;
                for(int i = 0; i < 3; ++i) iss >> v.raw[i];
                triangles.push_back(v);
            }
            else if(!line.compare(0, 2, "f ")){
                char trash; int itrash;
                iss >> trash;
                int idx;
                Vec3i v;
                int counter = 0;
                while(iss >> idx >> trash >> itrash >> trash >> itrash){
                    --idx;
                    v.raw[counter] = idx;
                    ++counter;
                }
                faces.push_back(v);
            }
        }

        obj_file.close();
    }
    else return;
}