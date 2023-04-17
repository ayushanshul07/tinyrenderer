#include "tgaimage.h"

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
	/*for(int x = x1; x <= x2; ++x){
		if(steep){
			im.set(y, x, col);
		}
		else{
			im.set(x, y, col);
		}
		error += slope;
		if(error >= 1){
			y += (y2 > y1) ? 1 : -1;
			error -= 1;
		}
	}*/
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

int main(int argc, char** argv) {
	TGAImage image(100, 100, TGAImage::RGB);
	//image.set(52, 41, red);
	for(int i = 0; i < 1000000; ++i){
		draw_line(13, 20, 80, 40, image, white); 
		draw_line(20, 13, 40, 80, image, red); 
		draw_line(80, 40, 13, 20, image, red);
	}
	//image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	//image.write_tga_file("output.tga");
	return 0;
}
