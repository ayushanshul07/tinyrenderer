#include <iostream>
#include "tgaimage.h"
#include "parser.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

const int width = 1024;
const int height = 1024;


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

void draw_triangle(	Vec2i A, Vec3f A1,
					Vec2i B, Vec3f B1,
					Vec2i C, Vec3f C1,
					Vec2f T1, Vec2f T2, Vec2f T3,
					float* zbuff, TGAImage& im, float intensity, TGAImage* diff_tex){

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
			Vec3f P(j,i,0.0);
			Vec2i P1(j,i);
			Vec3f coords = barycentric_cordinates(P1,A,B,C);
			P.z += A1.z * coords.raw[0];
			P.z += B1.z * coords.raw[1];
			P.z += C1.z * coords.raw[2];

			Vec3f tex_1(T1.x, T1.y, 0.0);
			Vec3f tex_2(T2.x, T2.y, 0.0);
			Vec3f tex_3(T3.x, T3.y, 0.0);

			Vec3f final_uv =  (tex_1 * coords.raw[0])
			 				+ (tex_2 * coords.raw[1])
							+ (tex_3 * coords.raw[2]) ;

			TGAColor col_from_texture = diff_tex->get(final_uv.x, final_uv.y);

			//std::cout << final_uv.x << " " << final_uv.y << " " << final_uv.z << "\n";
			
			if(coords.x < 0 ||  coords.y < 0 || coords.z < 0 ) continue;
			if(zbuff[(int)(P.x + P.y * width)] < P.z){
				zbuff[(int)(P.x + P.y * width)] = P.z;
				//im.set(P.x, P.y, TGAColor(final_uv.x, final_uv.y, final_uv.z, 255));
				im.set(P.x, P.y, col_from_texture);
			}
		}
	}


}

int main(int argc, char** argv) {

	TGAImage image(width, height, TGAImage::RGB);
	TGAImage* diff_tex = new TGAImage();
	diff_tex->read_tga_file("../external/obj/african_head_diffuse.tga");
	diff_tex->flip_vertically();
	std::cout << diff_tex->get_width() << " " << diff_tex->get_height() << "\n";
	Vec3f light_dir = Vec3f(0,0,-1.0);
	//draw_triangle(Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160), image, red); 

	ObjParser parser("");
	parser.parse();

	float* zbuffer = new float[width * height];
	int num_pix = width * height;
	while(--num_pix) zbuffer[num_pix] = -1000000.0; // large negative value

	for(int i = 0; i < parser.faces.size(); ++i){
		Vec3i tri = parser.faces[i];
		Vec3i tex = parser.texture_faces[i];

		Vec3f A1 = parser.triangles[tri.raw[0]];
		Vec3f B1 = parser.triangles[tri.raw[1]];
		Vec3f C1 = parser.triangles[tri.raw[2]];

		Vec2f Tt1 = parser.textures[tex.raw[0]];
		Vec2f Tt2 = parser.textures[tex.raw[1]];
		Vec2f Tt3 = parser.textures[tex.raw[2]];

		Vec2f T1( (Tt1.x)*width, (Tt1.y)*height);
		Vec2f T2( (Tt2.x)*width, (Tt2.y)*height);
		Vec2f T3( (Tt3.x)*width, (Tt3.y)*height);

		//std::cout << T1.x << " " << T1.y << "\n";
		//std::cout << T2.x << " " << T2.y << "\n";
		//std::cout << T3.x << " " << T3.y << "\n";
		
		Vec2i A( (A1.x+1)*width/2, (A1.y+1)*height/2);
		Vec2i B( (B1.x+1)*width/2, (B1.y+1)*height/2);
		Vec2i C( (C1.x+1)*width/2, (C1.y+1)*height/2);

		Vec3f n = (A1 - B1) ^ (C1 - A1);
		n.normalize();

		float intensity = n * light_dir; // dot product
		if(intensity > 0)
			draw_triangle(A, A1, B, B1, C, C1, T1, T2, T3, zbuffer, image, intensity, diff_tex);
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	delete diff_tex;
	return 0;
}
