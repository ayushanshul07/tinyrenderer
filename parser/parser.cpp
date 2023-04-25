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
            else if(!line.compare(0, 3, "vt ")){
                char trash;
                iss >> trash >> trash;
                Vec2f v;
                for(int i = 0; i < 2; ++i) iss >> v.raw[i];
                textures.push_back(v);
            }
            else if(!line.compare(0, 2, "f ")){
                char trash; int itrash1; int itrash2;
                iss >> trash;
                int idx;
                Vec3i v;
                Vec3i t;
                int counter = 0;
                while(iss >> idx >> trash >> itrash1 >> trash >> itrash2){
                    --idx;
                    --itrash1;
                    v.raw[counter] = idx;
                    t.raw[counter] = itrash1;
                    ++counter;
                }
                faces.push_back(v);
                texture_faces.push_back(t);
            }
        }

        obj_file.close();
    }
    else return;
}