Color scheme:
  * <font color='#E60000'>Extremely basic support, or absolutely non compliant</font>
  * <font color='#B5551D'>Basic support</font>
  * <font color='#FF7B32'>Almost fully compliant</font>
  * <font color='#00A200'>Fully compliant (well, in theory :))</font>

# Section 11 - Types #

## 11.1 - Floats ##
  * <font color='#00A200'>Supported</font>

## 11.2 - Colors ##
  * <font color='#FF7B32'>Supported</font> (only "rgb" support, and the string isn't even accepted)

## 11.3 - Points, Vectors and Normals ##
  * <font color='#FF7B32'>All supported</font> (but coordinate system specification is not yet supported)

## 11.4 - Transformation Matrices ##

## 11.5 - Strings ##

## 11.6 - Arrays ##

## 11.7 - Uniform and Varying Variables ##
  * <font color='#B5551D'>Basic support: distinction is made but no optimization are performed</font>

# Section 12 - Shader Execution Environment #

## 12.1 - Surface Shaders ##
  * <font color='#B5551D'>Basic support</font>; only the following predefined variables are available:
    1. Cs
    1. Os
    1. P
    1. N
    1. Ng
    1. s
    1. t
    1. I
    1. Ci
    1. Oi

## 12.2 - Light Source Shaders ##

## 12.3 - Volume Shaders ##

## 12.4 - Displacement Shaders ##

## 12.5 - Imager Shaders ##

# Section 13 - Language Constructs #

## 13.1 - Expressions ##
  * <font color='#B5551D'>Basic arithmetic support (no comparison, no ternary, no cross product operator etc...)</font>

## 13.2 - Standard Control Flow Constructs ##

## 13.3 - Illuminance and Illuminate Statements ##

# Section 14 - Shaders and Functions #

## 14.1 - Shaders ##

## 14.2 - Functions ##

# Section 15 - Built-In Functions #

## 15.1 - Mathematical Functions ##
  * <font color='#00A200'>smoothstep</font>

## 15.2 - Geometric Functions ##
  * <font color='#00A200'>normalize</font>
  * <font color='#00A200'>faceforward</font>
  * <font color='#00A200'>reflect</font>

## 15.3 - Color Functions ##
  * <font color='#00A200'>mix</font>

## 15.4 - Matrix Functions ##

## 15.5 - String Functions ##

## 15.6 - Shading and Lighting Functions ##
  * <font color='#00A200'>ambient</font>
  * <font color='#B5551D'>diffuse</font>
  * <font color='#B5551D'>specular</font>
  * <font color='#00A200'>trace</font>

## 15.7 - Texture Mapping Functions ##

#### 15.7.1 - Basic texture maps ####

#### 15.7.2 - Environment maps ####

#### 15.7.3 - Shadow depth maps ####

#### 15.7.4 - Getting Information About Texture Maps ####

## 15.8 - Message Passing and Information Functions ##

# Appendix A - Standard RenderMan Interface Shaders #

## A.1 - Null Shader ##

## A.2 - Surface Shaders ##

#### A.2.1 - Constant surface ####
  * <font color='#00A200'>Supported</font>

#### A.2.2 - Matte surface ####
  * <font color='#00A200'>Supported</font>

#### A.2.3 - Metal surface ####
  * <font color='#00A200'>Supported</font>

#### A.2.4 - Shiny metal surface ####

#### A.2.5 - Plastic surface ####
  * <font color='#00A200'>Supported</font>

#### A.2.6 - Painted plastic surface ####

## A.3 - Light Source Shaders ##

#### A.3.1 - Ambient light source ####

#### A.3.2 - Distant light source ####

#### A.3.3 - Point light source ####

#### A.3.4 - Spotlight source ####

## A.4 - Volume Shaders ##

#### A.4.1 - Depth cue shader ####

#### A.4.2 - Fog shader ####

## A.5 - Displacement Shaders ##

#### A.5.1 - Bumpy shader ####

## A.6 - Imager Shaders ##

#### A.6.1 - Background shader ####

# Appendix E - Standard Built-In Filters #

## E.1 - Box Filter ##

## E.2 - Triangle Filter ##

## E.3 - CatmullRom Filter ##

## E.4 - Gaussian Filter ##

## E.5 - Sinc Filter ##