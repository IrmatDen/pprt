Toy raytracer, using Pixar's [RenderMan Interface 3.2](https://renderman.pixar.com/products/rispec/index.htm) as inspiration.

Refer to the wiki to see current rib & rsl compliancy

---

## Dependancies ##
  * SSE2
  * boost (specified as a VC++ 10 property sheet): using spirit (for RIB parsing), memory pools, variant, shared\_ptr
  * FreeImage (included in ./dependancy)
  * Intel TBB 3 (included in ./dependancy): using parallel\_for, atomic, enumerable\_thread\_specific
  * [cpp-gpengine](http://cpp-gpengine.sourceforge.net/), used to parse GOLD grammars for RMSL (included in ./dependancy)
  * SFML2 (for framebuffer display type, commit 17971ca198b484a52bad, specified as a VC++ 10 property sheet)
  * [libply](http://people.cs.kuleuven.be/~ares.lagae/libply/index.html), used in the ply2rib tool (source directly used within ply2rib since some changes were needed for it to work with VC++ 2010)