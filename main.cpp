#include "tgaimage.h"
#include "parser.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

void draw_line(int x1, int y1, int x2, int y2, TGAImage& im, TGAColor col){
	bool steep = false;
	if(std::abs(x1-x2) < std::abs(y1-y2)){
		steep = true;
		std::swap(x1,y1);
		std::swap(x2,y2);
	}
	if(x1 > x2) {
		std::swap(x1,x2);
		std::swap(y1,y2);
	}
	
	float slope = std::abs((y2-y1)/(float)(x2-x1));
	float error = 0.0;
	int y = y1;

	//No coditional branching inside for loop
	if(steep){
		for(int x = x1; x <= x2; ++x){
			im.set(y, x, col);
			error += slope;
			if(error >= 1){
				y += (y2 > y1) ? 1 : -1;
				error -= 1;
			}
		}
	}
	else{
		for(int x = x1; x <= x2; ++x){
			im.set(x, y, col);
			error += slope;
			if(error >= 1){
				y += (y2 > y1) ? 1 : -1;
				error -= 1;
			}
		}
	}
}

const int width = 800;
const int height = 800;

int main(int argc, char** argv) {

	TGAImage image(width, height, TGAImage::RGB);

	ObjParser parser("");
	parser.parse();

	for(int i = 0; i < parser.faces.size(); ++i){
		Vec3i tri = parser.faces[i];
		for(int j = 0; j < 3; ++j){
			Vec3f v1 = parser.triangles[tri.raw[j]];
			Vec3f v2 = parser.triangles[tri.raw[(j+1)%3]];
			int x1 = ((v1.x+1)*width)/2;
			int y1 = ((v1.y+1)*height)/2;
			int x2 = ((v2.x+1)*width)/2;
			int y2 = ((v2.y+1)*height)/2;

			draw_line(x1, y1, x2, y2, image, white);
		}
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}
