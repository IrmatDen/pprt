# Polygons & PointsPolygons #

Convex polygons are currently NOT decomposed into triangles. Instead, generic barycentric coordinates are computed as described in:
> "Generalized Barycentric Coordinates on Irregular Polygons" (Meyer et al. (2002))<br />
> Published in JGT Vol. 7, Nr 1, 2002

[Link to PDF published on Meyer's page](http://www.cs.caltech.edu/~mmeyer/Publications/barycentric.pdf)

Not sure if it'll work everywhere, but since smoothness is guaranteed, in theory it should, right? :)

Aqsis (1.4 & 1.6 tested only) have a different rendering, but I'm not sure it's a proof of correctness (I mean compared to the real Renderman). Anyway, it's not everyday you render such polygons.
  * Aqsis:
![http://dbulant.free.fr/pprt_images/aqsis_polygon_generalizedbarcoords.png](http://dbulant.free.fr/pprt_images/aqsis_polygon_generalizedbarcoords.png)
  * pprt:
![http://dbulant.free.fr/pprt_images/pprt_polygon_generalizedbarcoords.png](http://dbulant.free.fr/pprt_images/pprt_polygon_generalizedbarcoords.png)

Sample rib to reproduce:
```
Option "searchpath" "shader" ["&"]

Display "irregular_convex_polygon.png" "file" "rgba"
# This line is only usable with Aqsis as pprt doesn't support multiple display output yet
# Display "+irregular_convex_polygon.png" "framebuffer" "rgba"

Projection "perspective" "fov" 45
# Those lines are specified for Aqsis since pprt doesn't yet support those functions
# PixelFilter "box" 1 1
# Quantize "rgba" 255 0 255 0

Translate 0 1 6

WorldBegin
    Surface "constant"
    Polygon "P" [-12 -2.5 16   -12  0 16   -8 5 16   8 5 16   12 0 16   12 -2.5 16]
            "Cs" [1 0 0    0 1 0   0 0 1   1 0 1   1 1 0   0 1 1]
WorldEnd
```
