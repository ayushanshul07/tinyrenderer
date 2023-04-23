#include <iostream>
#include "tgaimage.h"
#include "parser.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

const int width = 800;
const int height = 800;


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

Vec3f barycentric_cordinates(Vec2i P, Vec2i A, Vec2i B, Vec2i C){

	// Cross Prroduct of [ABx, ACx, PAx] X [ABy, ACy, PAy] 

	Vec2i d1 = B - A;
	Vec2i d2 = C - A;
	Vec2i d3 = A - P;

	Vec3f u = Vec3f(d1.x, d2.x, d3.x)^Vec3f(d1.y, d2.y, d3.y);

	// this could have been u.z = 0, but u is float accounting for epsilon
	if(std::abs(u.z) < 1){
		//degenrate triangle
		return Vec3f(-1.0, 1.0, 1.0);
	}
	else{
		return Vec3f(1.0 - (u.x+u.y)/u.z, u.y/u.z, u.x/u.z); // ??
	}

}

void draw_triangle(Vec2i A, Vec2i B, Vec2i C, TGAImage& im, TGAColor col){

	const int widlt = im.get_width()-1; // width limit
	const int heilt = im.get_height()-1; // height limit

	Vec2i bboxmax(0,0);
	Vec2i bboxmin(widlt, heilt);

	bboxmin.x = std::max(0, std::min(bboxmin.x, A.x));
	bboxmin.y = std::max(0, std::min(bboxmin.y, A.y));

	bboxmax.x = std::min(widlt, std::max(bboxmax.x, A.x));
	bboxmax.y = std::min(heilt, std::max(bboxmax.y, A.y));

	bboxmin.x = std::max(0, std::min(bboxmin.x, B.x));
	bboxmin.y = std::max(0, std::min(bboxmin.y, B.y));

	bboxmax.x = std::min(widlt, std::max(bboxmax.x, B.x));
	bboxmax.y = std::min(heilt, std::max(bboxmax.y, B.y));

	bboxmin.x = std::max(0, std::min(bboxmin.x, C.x));
	bboxmin.y = std::max(0, std::min(bboxmin.y, C.y));

	bboxmax.x = std::min(widlt, std::max(bboxmax.x, C.x));
	bboxmax.y = std::min(heilt, std::max(bboxmax.y, C.y));

	for(int i = bboxmin.y; i <= bboxmax.y; ++i){
		for(int j = bboxmin.x; j <= bboxmax.x; ++j){
			Vec2i P(j,i);
			Vec3f coords = barycentric_cordinates(P,A,B,C);
			if(coords.x < 0 ||  coords.y < 0 || coords.z < 0 ) continue;
			im.set(j, i, col);
		}
	}


}

int main(int argc, char** argv) {

	TGAImage image(width, height, TGAImage::RGB);
	Vec3f light_dir = Vec3f(0,0,-1.0);
	//draw_triangle(Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160), image, red); 

	ObjParser parser("");
	parser.parse();

	for(int i = 0; i < parser.faces.size(); ++i){
		Vec3i tri = parser.faces[i];
		Vec3f A1 = parser.triangles[tri.raw[0]];
		Vec3f B1 = parser.triangles[tri.raw[1]];
		Vec3f C1 = parser.triangles[tri.raw[2]];
		
		Vec2i A( (A1.x+1)*width/2, (A1.y+1)*height/2);
		Vec2i B( (B1.x+1)*width/2, (B1.y+1)*height/2);
		Vec2i C( (C1.x+1)*width/2, (C1.y+1)*height/2);

		Vec3f n = (A1 - B1) ^ (C1 - A1);
		n.normalize();

		float intensity = n * light_dir; // dot product
		if(intensity > 0)
			draw_triangle(A, B, C, image, TGAColor(255*intensity, 255*intensity, 255*intensity, 255));
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}
