void constructNormCubeFace(float begini, float endi, float beginj, float endj, float begink, float endk) {
} // end constructNormCubeFace()

void saveVector(Vec3D &v, unsigned char* buf) {
	// Pack into one byte and save
	v.Clamp01();
	buf[0] = (unsigned char)(v.x*255);
	buf[1] = (unsigned char)(v.y*255);
	buf[2] = (unsigned char)(v.z*255);
} // end saveVector()
