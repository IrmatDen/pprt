Color scheme:
  * <font color='#E60000'>Extremely basic support, or absolutely non compliant</font>
  * <font color='#B5551D'>Basic support</font>
  * <font color='#FF7B32'>Almost fully compliant</font>
  * <font color='#00A200'>Fully compliant (well, in theory :))</font>

Global:
  * No support yet for other color spaces than "rgb"

# Section 4 - Graphics State #

## 4.1 - Options ##

#### 4.1.1 - Camera ####
  * <font color='#00A200'>Format</font>
  * <font color='#E60000'>Clipping</font> (read, but unused for actual clipping)
  * <font color='#00A200'>Projection</font>

#### 4.1.2 - Displays ####
  * <font color='#00A200'>PixelSamples</font> (only uniform sampling ATM)
  * <font color='#B5551D'>Display</font> (mode & parameters still unused: only rgba output at origin (0,0), and only one display available)

#### 4.1.3 - Additional options ####

#### 4.1.4 - Implementation-specific options ####
  * <font color='#00A200'>Option "searchpath" "shader"</font>
  * <font color='#00A200'>Option "usethreads"</font> (non-standard)
  * <font color='#00A200'>Option "maxtracedepth"</font> (non-standard)

## 4.2 - Attributes ##
  * <font color='#00A200'>AttributeBegin</font>
  * <font color='#00A200'>AttributeEnd</font>

#### 4.2.1 - Color and opacity ####
  * <font color='#B5551D'>Color</font> (not in its array form)
  * <font color='#B5551D'>Opacity</font> (not in its array form)

#### 4.2.2 - Texture coordinates ####

#### 4.2.3 - Light sources ####
  * <font color='#E60000'>Ambient light</font> (hardcoded, not part of graphic state)
  * <font color='#E60000'>Point lights</font> (position & color embedded, no shaders, not part of graphic state)

#### 4.2.4 - Surface shading ####
  * <font color='#00A200'>Surface</font>

#### 4.2.5 - Displacement shading ####

#### 4.2.6 - Volume shading ####

#### 4.2.7 - Shading rate ####

#### 4.2.8 - Shading interpolation ####

#### 4.2.9 - Matte objects ####

#### 4.2.10 - Bound ####

#### 4.2.11 - Detail ####

#### 4.2.12 - Geometric approximation ####

#### 4.2.13 - Orientation and sides ####

## 4.3 - Transformations ##
  * <font color='#00A200'>Identity</font>
  * <font color='#00A200'>Translate</font>
  * <font color='#00A200'>Rotate</font>

#### 4.3.1 - Named coordinate systems ####

#### 4.3.2 - Transformation stack ####
  * <font color='#00A200'>TransformBegin</font>
  * <font color='#00A200'>TransformEnd</font>

## 4.4 - Implementation-specific Attributes ##

# Section 5 - Geometric Primitives #

## 5.1 - Polygons ##
  * <font color='#B5551D'>Polygon</font> (missing "Pz", "Pw", "s", "t" & "st")
  * <font color='#B5551D'>PointsPolygons</font> (missing "Pz", "Pw", "s", "t" & "st")

## 5.2 - Patches ##

## 5.3 - Subdivision Surfaces ##

## 5.4 - Quadrics ##
  * <font color='#B5551D'>Sphere</font> (only radius supported)

## 5.5 - Point and Curve Primitives ##

## 5.6 - Blobby Implicit Surfaces ##

## 5.7 - Procedural Primitives ##

## 5.8 - Implementation-specific Geometric Primitives ##

## 5.9 - Solids and Spatial Set Operations ##

## 5.10 - Retained Geometry ##

# Section 6 - Motion #

# Section 7 - External Resources #

## 7.1 - Texture Map Utilities ##

#### 7.1.1 - Making texture maps ####

#### 7.1.2 - Making environment maps ####

#### 7.1.3 - Making shadow maps ####

## 7.2 - Errors ##

## 7.3 - Archive Files ##

# Appendix F - Standard Basis Matrices #