# pprt
Automatically exported from code.google.com/p/pprt

Toy raytracer, using Pixar's RenderMan Interface 3.2 as inspiration.
Refer to the wiki to see current rib & rsl compliancy

### Dependancies

* SSE2
* boost (specified as a VC++ 10 property sheet): using spirit (for RIB parsing), memory pools, variant, shared_ptr
* FreeImage (included in ./dependancy)
* Intel TBB 3 (included in ./dependancy): using parallel_for, atomic, enumerable_thread_specific
* cpp-gpengine, used to parse GOLD grammars for RMSL (included in ./dependancy)
* SFML2 (for framebuffer display type, commit 17971ca198b484a52bad, specified as a VC++ 10 property sheet)
* libply, used in the ply2rib tool (source directly used within ply2rib since some changes were needed for it to work with VC++ 2010)
