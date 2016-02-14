# TerrainTessellation

- Terrain mesh is generated from a Digital Elevation Model (DEM) in the format TIF FLOAT. Currently i'm using Freeimage to load it.
- The DEM is loaded as texture and sent to GPU, where the displacement is made.
- Tessellation is made to reduce the original (in this example) 13m per edge to ~30cm.
- Tessellation occurs around the camera only, in order to create a good local LOD.
- TODO
	- For now we have a flat tessellation, which means that newly created triangles are coplanar to it's original triangle. There are
	several methods to actually use the new triangles, among them:
		- Bicubic interpolation
		- Fractals
	- Improve lighting

HOW TO TEST:
- Run on vs2015
- commands:
	- WASD move camera
	- Q/E/SHIFT speedup camera
	- F go wireframe (so you can see the tessellation)
