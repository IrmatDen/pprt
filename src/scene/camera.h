#ifndef PPRT_CAMERA_H
#define PPRT_CAMERA_H

#include "ray.h"

class Camera
{
public:
	enum CameraModel
	{
		CM_Orthographic,
		CM_Perspective
	};

public:
	Camera()
	{
	}

	void	finalize(CameraModel model,	const Matrix4 &camToWorld, float aspectRatio, float fov,
						int width, int height, float hither, float yon,
						const float screenExtents[4]);
	void	project(float x, float y, Ray &r) const;

public:
	CameraModel	camModel;
	Matrix4		CamToWorld, WorldToCam;
	Matrix4		CamToScreen, WorldToScreen, RasterToCam;
	Matrix4		ScreenToRaster, RasterToScreen;
	float		nearClip, farClip;
};

#endif
