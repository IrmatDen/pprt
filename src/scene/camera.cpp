#include "camera.h"

void Camera::finalize(CameraModel model, const Matrix4 &worldToCam, float aspectRatio, float fov,
						int width, int height, float hither, float yon,
						const float screenExtents[4])
{
	camModel	= model;

	nearClip	= hither;
	farClip		= yon;

	xRes	= width;
	yRes	= height;

	WorldToCam	= worldToCam;
	CamToWorld	= inverse(WorldToCam);

	switch (camModel)
	{
	case CM_Orthographic:
		// Near/far clipping are enforced through ray boundaries defined in Camera::project
		CamToScreen = Matrix4::orthographic(screenExtents[0], screenExtents[1], screenExtents[2], screenExtents[3], 0.f, 1.f);
		break;

	case CM_Perspective:
		// Ignore aspect ratio setting here as we apply it below through the screenScale matrix
		CamToScreen = Matrix4::perspective(fov, 1.f, nearClip, farClip);
		break;
	}

	WorldToScreen = CamToScreen * WorldToCam;

	const Matrix4 resScale		(Matrix4::scale(Vector3(static_cast<float>(xRes),
														static_cast<float>(yRes),
														1.f)));
	const Matrix4 screenScale	(Matrix4::scale(Vector3(1.f / (screenExtents[1] - screenExtents[0]),
														1.f / (screenExtents[2] - screenExtents[3]),
														1.f)));
	const Matrix4 trans(Matrix4::translation(Vector3(-screenExtents[0], -screenExtents[3], 0.f)));
	ScreenToRaster = resScale * screenScale * trans;
	RasterToScreen = inverse(ScreenToRaster);

	RasterToCam = inverse(CamToScreen) * RasterToScreen;
	// Revert Z axis in order to look toward +Z
	RasterToCam.setElem(3, 2, -RasterToCam.getElem(3, 2));

    WorldToCamN = WorldToCam;
    WorldToCamN.setElem(3, 0, 0.f);
    WorldToCamN.setElem(3, 1, 0.f);
    WorldToCamN.setElem(3, 2, 0.f);
    WorldToCamN.setElem(0, 3, 0.f);
    WorldToCamN.setElem(1, 3, 0.f);
    WorldToCamN.setElem(2, 3, 0.f);
    WorldToCamN.setElem(3, 3, 1.f);
    WorldToCamN = transpose(inverse(WorldToCamN));
    CamToWorldN = inverse(WorldToCamN);
}

void Camera::project(float x, float y, Ray &r) const
{
	const Point3 rasterPt(x, y, 0);
	const Point3 camPt((RasterToCam * rasterPt).getXYZ());

	Point3	rayOrigin;
	Vector3	rayDirection;
	switch (camModel)
	{
	case CM_Perspective:
		rayDirection = normalize(Vector3(camPt));
		rayOrigin = Point3(0.f);
		break;

	case CM_Orthographic:
		rayOrigin = camPt;
		rayDirection = Vector3::zAxis();
		break;
	}

	r.origin = Point3((CamToWorld * rayOrigin).get128());
	r.setDirection(Vector3((CamToWorld * rayDirection).get128()));
	r.minT = nearClip;
	r.maxT = farClip;
}
