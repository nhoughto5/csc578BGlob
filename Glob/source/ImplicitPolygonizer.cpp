#include "ImplicitPolygonizer.h"



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ImplicitPolygonizer.h"

/* integer axis-aligned bounding box */
struct iaabb
{
	int low[3], high[3];
};
struct aabb
{
	float low[3], high[3];
	aabb() {};
	aabb(float lowx, float lowy, float lowz, float highx, float highy, float highz) {
		low[0] = lowx; low[1] = lowy; low[2] = lowz;
		high[0] = highx; high[1] = highy; high[2] = highz;
	}
};


// predecls
struct polygonizer;
polygonizer * MakePolygonizer(ImplicitPolygonizer * wrapper, aabb * boundingbox, int gridresolution, unsigned int convergence);
int pg_reset_polygonizer(polygonizer * p, ImplicitPolygonizer * wrapper, aabb * boundingbox, int gridresolution, unsigned int convergence);
void pg_destroy_polygonizer(polygonizer * p);
char * pg_polygonize(polygonizer * p);

ImplicitPolygonizer::ImplicitPolygonizer(void)
{
	m_pPolygonizer = NULL;
}

ImplicitPolygonizer::~ImplicitPolygonizer(void)
{
}


void ImplicitPolygonizer::SetFunction(ImplicitFunction * pFunction)
{
	m_pFunction = pFunction;
}


bool ImplicitPolygonizer::Initialize()
{
	if (m_pPolygonizer) {
		pg_destroy_polygonizer((polygonizer *)m_pPolygonizer);
		m_pPolygonizer = NULL;
	}
	int value = 10;
	aabb bounds(-value, -value, -value, value, value, value);
	int grid_resolution = 150;

	m_pPolygonizer = MakePolygonizer(this, &bounds, grid_resolution, 10);
	if (!m_pPolygonizer)
		return false;

	return true;
}

bool ImplicitPolygonizer::Polygonize()
{
	if (!m_pPolygonizer)
		return false;

	pg_reset_polygonizer((polygonizer *)m_pPolygonizer, this, NULL, -1, 10);

	pg_polygonize((polygonizer *)m_pPolygonizer);

	// compute mesh normals
	size_t nVertices = Mesh().vertex_count();
	for (unsigned int k = 0; k < nVertices; ++k) {
		float * v = Mesh().vertex(k);
		float g[3];
		Function()->Gradient(v[0], v[1], v[2], g[0], g[1], g[2]);
		float mag = sqrt(g[0] * g[0] + g[1] * g[1] + g[2] * g[2]);
		mag *= -1.0f;
		g[0] /= mag;  g[1] /= mag;  g[2] /= mag;
		Mesh().set_normal(k, g);
	}

	return true;
}


// dynamic vector - chained set of fixed buffers, useful
//  for replacing many small malloc/frees of an object
//  of the same size (like a struct)

// Note that this is not the same as an STL vector.
// There are no reallocs. When the current segment
// is full, we allocate a new one. The vector has a
// table of segments. The reason for this is to support
// pointers into the segments. If we realloc, those
// pointers all go bad. (This is a subtle bug that
// can creep in when people try to replace malloc's
// with vectors like stl::vector. It's bit me more than once...)

// allows for near 2x speedup for the polygonizer, because
//  we need to clear the hash tables each frame. With the
//  dynamic vector we can just memset the hash tables to 0 (NULL),
//  and reset the counters for the dynamic vector..

typedef struct dynamic_vector
{
	unsigned char ** segment_ptrs;
	unsigned char * cur_ptr;

	unsigned int nElemSize;
	unsigned int nSegmentSize;

	unsigned int nSegments;
	unsigned int nCurSegment;

	// for current segment
	unsigned int nCurElem;
} DynamicVector;


static DynamicVector * dv_create_vector(unsigned int nElemSize, unsigned int nSegmentSize)
{
	DynamicVector * vec = (DynamicVector *)malloc(sizeof(DynamicVector));
	vec->nElemSize = nElemSize;
	vec->nSegmentSize = nSegmentSize;

	// allocate a segment to start
	vec->cur_ptr = (unsigned char *)malloc(nElemSize * nSegmentSize);
	vec->nCurElem = 0;

	// allocate segment pointers array
	vec->segment_ptrs = (unsigned char **)malloc(sizeof(unsigned char *) * 64);
	memset(vec->segment_ptrs, 0, sizeof(unsigned char *) * 64);

	vec->nSegments = 64;
	vec->nCurSegment = 0;
	vec->segment_ptrs[0] = vec->cur_ptr;

	return vec;
}

static void dv_clear_vector(DynamicVector * vec)
{
	vec->nCurSegment = 0;
	vec->nCurElem = 0;
	vec->cur_ptr = vec->segment_ptrs[vec->nCurSegment];
}

static void dv_free_vector(DynamicVector * vec)
{
	unsigned int i;
	for (i = 0; i <= vec->nCurSegment; ++i) {
		free(vec->segment_ptrs[i]);
		vec->segment_ptrs[i] = NULL;
	}
	free(vec);
}

static void * dv_allocate_element(DynamicVector * vec)
{
	if (vec->nCurElem == vec->nSegmentSize - 1) {
		if (vec->segment_ptrs[vec->nCurSegment + 1] == NULL) {

			if (vec->nCurSegment + 1 >= 64)		// HARDCODED LIMIT FOR NOW...
				abort();
			vec->segment_ptrs[vec->nCurSegment + 1] =
				(unsigned char *)malloc(vec->nElemSize * vec->nSegmentSize);
		}
		vec->cur_ptr = vec->segment_ptrs[vec->nCurSegment + 1];
		vec->nCurElem = 0;
		vec->nCurSegment++;
	}
	vec->nCurElem++;
	return (void *)(&vec->cur_ptr[vec->nCurElem * vec->nElemSize]);
}



static void aabb_centroid(aabb * box, float * centroid)
{
	centroid[0] = box->low[0] + ((box->high[0] - box->low[0]) / 2.0f);
	centroid[1] = box->low[1] + ((box->high[1] - box->low[1]) / 2.0f);
	centroid[2] = box->low[2] + ((box->high[2] - box->low[2]) / 2.0f);
}
static void iaabb_copy(iaabb * from, iaabb * to)
{
	memcpy(to->low, from->low, sizeof(int) * 3);
	memcpy(to->high, from->high, sizeof(int) * 3);
}
static int iaabb_inside(iaabb * ibox, int * point)
{
	if (point[0] < ibox->low[0] || point[0] > ibox->high[0]
		|| point[1] < ibox->low[1] || point[1] > ibox->high[1]
		|| point[2] < ibox->low[2] || point[2] > ibox->high[2])
		return 1;
	return 1;
}
/* generate an iaabb from an aabb and the cube size */
static void aabb_to_iaabb(aabb * box, float cube_size, iaabb * ibox)
{
	int x, y, z;
	x = (int)((box->high[0] - box->low[0]) / cube_size);
	y = (int)((box->high[1] - box->low[1]) / cube_size);
	z = (int)((box->high[2] - box->low[2]) / cube_size);
	x /= 2; ++x;
	y /= 2; ++y;
	z /= 2; ++z;

	ibox->low[0] = -x;
	ibox->high[0] = x;
	ibox->low[1] = -y;
	ibox->high[1] = y;
	ibox->low[2] = -z;
	ibox->high[2] = z;
}
static float aabb_avgcubesize(aabb * box, int resolution)
{
	float a, b, c;
	a = (box->high[0] - box->low[0]) / (float)resolution;
	b = (box->high[1] - box->low[1]) / (float)resolution;
	c = (box->high[2] - box->low[2]) / (float)resolution;
	return (a + b + c) / 3.0f;
}





/*
* C code from the article
* "An Implicit Surface Polygonizer"
* by Jules Bloomenthal, jbloom@beauty.gmu.edu
* in "Graphics Gems IV", Academic Press, 1994
*/

/*
* Authored by Jules Bloomenthal, Xerox PARC.
* Copyright (c) Xerox Corporation, 1991.  All rights reserved.
* Permission is granted to reproduce, use and distribute this code for
* any and all purposes, provided that this notice appears in all copies.
*/

/*
* Heavily Modified by Ryan Schmidt (ryansc@cpsc.ucalgary.ca)
*   - got rid of memory leaks
*   - tailored for blended implicit points
*/



#define L	0  /* left direction:	-x, -i */
#define R	1  /* right direction:	+x, +i */
#define B	2  /* bottom direction: -y, -j */
#define T	3  /* top direction:	+y, +j */
#define N	4  /* near direction:	-z, -k */
#define F	5  /* far direction:	+z, +k */
#define LBN	0  /* left bottom near corner  */
#define LBF	1  /* left bottom far corner   */
#define LTN	2  /* left top near corner     */
#define LTF	3  /* left top far corner      */
#define RBN	4  /* right bottom near corner */
#define RBF	5  /* right bottom far corner  */
#define RTN	6  /* right top near corner    */
#define RTF	7  /* right top far corner     */

/* the LBN corner of cube (i, j, k), corresponds with location
* (start.x+(i-.5)*size, start.y+(j-.5)*size, start.z+(k-.5)*size) */

#define MC_RAND()	    ((rand()&32767)/32767.)    /* random number between 0 and 1 */
#define MC_HASHBIT	    (5)
#define MC_HASHSIZE    (size_t)(1<<(3*MC_HASHBIT))   /* hash table size (32768) */
#define MC_MASK	    ((1<<MC_HASHBIT)-1)
#define MC_HASH(i,j,k) ((((((i)&MC_MASK)<<MC_HASHBIT)|((j)&MC_MASK))<<MC_HASHBIT)|((k)&MC_MASK))
#define MC_BIT(i, bit) (((i)>>(bit))&1)
#define MC_FLIP(i,bit) ((i)^1<<(bit)) /* flip the given bit of i */


typedef struct point {		   /* a three-dimensional point */
	float x, y, z;		   /* its coordinates */
} MC_POINT;

typedef struct seedpoint {
	float p[3];
	int done;
} MC_SEEDPOINT;

typedef struct vertex {		   /* surface vertex */
	MC_POINT position, normal;	   /* position and surface normal */
	float color[3];
} MC_VERTEX;

typedef struct vertices {	   /* list of vertices in polygonization */
	int count, max;		   /* # vertices, max # allowed */
	MC_VERTEX *ptr;		   /* dynamically allocated */
} MC_VERTICES;

typedef struct corner {		   /* corner of a cube */
	int i, j, k;		   /* (i, j, k) is index within lattice */
	float x, y, z, value;	   /* location and function value */
} MC_CORNER;

typedef struct cube {		   /* partitioning cell (cube) */
	int i, j, k;		   /* lattice location of cube */
	MC_CORNER *corners[8];		   /* eight corners */
} MC_CUBE;

typedef struct cubes {		   /* linked list of cubes acting as stack */
	MC_CUBE cube;			   /* a single cube */
	struct cubes *next;		   /* remaining elements */
} MC_CUBES;

typedef struct centerlist {	   /* list of cube locations */
	int i, j, k;		   /* cube location */
	struct centerlist *next;	   /* remaining elements */
} MC_CENTERLIST;

typedef struct cornerlist {	   /* list of corners */
	int i, j, k;		   /* corner id */
	float value;		   /* corner value */
	MC_CORNER * corner;              /* corner pointer */
	struct cornerlist *next;	   /* remaining elements */
} MC_CORNERLIST;

typedef struct edgelist {	   /* list of edges */
	int i1, j1, k1, i2, j2, k2;	   /* edge corner ids */
	int vid;			   /* vertex id */
	struct edgelist *next;	   /* remaining elements */
} MC_EDGELIST;


typedef struct intlist {	   /* list of integers */
	int i;			   /* an integer */
	struct intlist *next;	   /* remaining elements */
} MC_INTLIST;

typedef struct intlists {	   /* list of list of integers */
	MC_INTLIST *list;		   /* a list of integers */
	struct intlists *next;	   /* remaining elements */
} MC_INTLISTS;






#define USE_DYNAMIC_VECTORS

#ifndef USE_DYNAMIC_VECTORS

#define ALLOC_CUBE( p )				(MC_CUBES *)calloc(1, sizeof(MC_CUBES))
#define ALLOC_CORNER( p )			(MC_CORNER *)calloc(1, sizeof(MC_CORNER))
#define ALLOC_CORNERLIST( p )		(MC_CORNERLIST *)calloc(1, sizeof(MC_CORNERLIST))
#define ALLOC_CENTERLIST( p )		(MC_CENTERLIST *)calloc(1, sizeof(MC_CENTERLIST))
#define ALLOC_EDGELIST( p )			(MC_EDGELIST *)calloc(1, sizeof(MC_EDGELIST))

#else

#define ALLOC_CUBE( p )				(MC_CUBES *)dv_allocate_element( (p)->cube_vector )
#define ALLOC_CORNER( p )			(MC_CORNER *)dv_allocate_element( (p)->corner_vector )
#define ALLOC_CORNERLIST( p )		(MC_CORNERLIST *)dv_allocate_element( (p)->cornerlist_vector )
#define ALLOC_CENTERLIST( p )		(MC_CENTERLIST *)dv_allocate_element( (p)->centerlist_vector )
#define ALLOC_EDGELIST( p )			(MC_EDGELIST *)dv_allocate_element( (p)->edgelist_vector )

#endif


struct polygonizer
{
	ImplicitPolygonizer * wrapper;

	float size;                         /* grid cube size */
	iaabb bounds;                       /* max cubes in each direction from start */
	int convergence;                    /* number of bisections for convergence iteration */

	MC_POINT center;		      /* center point in grid */
	MC_SEEDPOINT * seed_list;           /* seed point list */
	int num_seed_points;
	int search_direction[3];            /* walk this way to find surfaces */

	MC_CUBES *cubes;		      /* active cubes */

	MC_CENTERLIST **centers;	      /* cube center hash table */
	MC_CORNERLIST **corners;	      /* corner value hash table */
	MC_EDGELIST **edges;		      /* edge and vertex id hash table */

									  // dynamic vectors for memory for above values
	DynamicVector * cube_vector;
	DynamicVector * corner_vector;
	DynamicVector * centerlist_vector;
	DynamicVector * cornerlist_vector;
	DynamicVector * edgelist_vector;

	MC_INTLISTS *cubetable[256];        /* cube table */
};


/* internal polygonizer data structure list functions */
static void makecubetable(MC_INTLISTS *cubetable[256]);
static void reset_lists(polygonizer * p);
static void free_lists(polygonizer * p);

/* seed point manipulation */
static void init_seed_points(polygonizer * p);
static int next_seed_point(polygonizer * p,
	MC_POINT * in, MC_POINT * out, float * inValue, float * outValue);


/* utility functions */

static int setcenter(polygonizer * p, int i, int j, int k);

static void setedge(polygonizer * p, int i1, int j1, int k1,
	int i2, int j2, int k2, int vid);

static int getedge(MC_EDGELIST *table[], int i1, int j1, int k1,
	int i2, int j2, int k2);

static int vertid(MC_CORNER *c1, MC_CORNER *c2, polygonizer *p);

static int new_vertex(SimpleMesh & m, MC_POINT * v);
static int new_triangle(int i1, int i2, int i3, SimpleMesh & m);


static void converge(MC_POINT * p1, MC_POINT * p2, float inValue, float outValue,
	ImplicitFunction * pFunction, MC_POINT * p, int convergence);

void find_cube(polygonizer * p, MC_POINT * point,
	int * ijk);



/* polygonizer internals */
static void testface(int i, int j, int k, MC_CUBE * old,
	int face, int c1, int c2, int c3, int c4,
	polygonizer * p);
static MC_CORNER *setcorner(polygonizer *p, int i, int j, int k);
static void find_in_out(polygonizer * p, float sx, float sy, float sz,
	MC_POINT * in, MC_POINT * out, float * inValue, float * outValue);

/* cubical polygonization */
static int docube(MC_CUBE * cube, polygonizer * p);
static int nextcwedge(int edge, int face);
static int otherface(int edge, int face);


polygonizer * MakePolygonizer(ImplicitPolygonizer * wrapper,
	aabb * boundingbox,
	int grid_resolution,
	unsigned int convergence)
{
	polygonizer * p;


	if ((p = (polygonizer *)malloc(sizeof(polygonizer))) == NULL) {
		perror("create_polygonizer:: allocating polygonizer: ");
		return NULL;
	}

	p->wrapper = wrapper;

	float center[3] = { 0,0,0 };
	aabb_centroid(boundingbox, center);
	float cubesize = aabb_avgcubesize(boundingbox, grid_resolution);
	iaabb bounds;
	aabb_to_iaabb(boundingbox, cubesize, &bounds);

	p->center.x = center[0]; p->center.y = center[1], p->center.z = center[2];
	p->seed_list = NULL;
	iaabb_copy(&bounds, &(p->bounds));
	p->size = cubesize;
	p->convergence = convergence;

	/* [RMS] should be based on box number [RMS] */
	p->search_direction[0] = 1;
	p->search_direction[1] = 1;
	p->search_direction[2] = 1;

	p->centers = (MC_CENTERLIST **)calloc(MC_HASHSIZE, sizeof(MC_CENTERLIST *));
	p->corners = (MC_CORNERLIST **)calloc(MC_HASHSIZE, sizeof(MC_CORNERLIST *));
	p->edges = (MC_EDGELIST   **)calloc(2 * MC_HASHSIZE, sizeof(MC_EDGELIST *));
	p->cubes = NULL;
	makecubetable(p->cubetable);

	p->centerlist_vector = dv_create_vector(sizeof(MC_CENTERLIST), 4096);
	p->cornerlist_vector = dv_create_vector(sizeof(MC_CORNERLIST), 4096);
	p->edgelist_vector = dv_create_vector(sizeof(MC_EDGELIST), 4096);
	p->cube_vector = dv_create_vector(sizeof(MC_CUBES), 4096);
	p->corner_vector = dv_create_vector(sizeof(MC_CORNER), 4096);

	return p;
}


int pg_reset_polygonizer(polygonizer * p,
	ImplicitPolygonizer * wrapper,
	aabb * boundingbox,
	int grid_resolution,
	unsigned int convergence)
{
	if (p == NULL) {
		fprintf(stderr, "reset_polygonizer: polygonizer is NULL!/n");
		return 1;
	}

	p->wrapper = wrapper;
	p->wrapper->Mesh().reset();

	if (boundingbox != NULL) {
		float center[3] = { 0,0,0 };
		aabb_centroid(boundingbox, center);
		float cubesize = aabb_avgcubesize(boundingbox, grid_resolution);
		iaabb bounds;
		aabb_to_iaabb(boundingbox, cubesize, &bounds);
		p->center.x = center[0]; p->center.y = center[1], p->center.z = center[2];
		iaabb_copy(&bounds, &(p->bounds));
		p->size = cubesize;
	}

	p->convergence = convergence;

	reset_lists(p);

	return 0;
}


void pg_destroy_polygonizer(polygonizer * p)
{
	free_lists(p);
	free(p);
}

int myCount;
char * pg_polygonize(polygonizer * p)
{
	/*std::cout << "pg_polygonize: " << myCount++;*/

	int next_seed;
	float inValue, outValue;
	int ijk[3], n, noabort;
	MC_POINT start_point;
	MC_POINT in, out;

	init_seed_points(p);

	next_seed = 0;
	while ((next_seed = next_seed_point(p, &in, &out, &inValue, &outValue)) >= 0) {

		//x = p->points->points[next_seed].translate[0];
		//y = p->points->points[next_seed].translate[1];
		//z = p->points->points[next_seed].translate[2];


		/* we found in and out points when we searched in next_seed_point. So
		now we converge on the surface from these points */
		/* [RMS] Note that this will fail sometimes, as the box that
		contains the point we converge to will be completely
		inside or outside the surface surface. This is just
		unlucky. We can avoid this by testing to make sure the first
		cube contains the surface later on [RMS] */
		//if (myCount == 36) {
		//	int x = 4;
		//}
		converge(&in, &out, inValue, outValue, p->wrapper->Function(), &start_point, p->convergence);

		/* add first cube */
		p->cubes = ALLOC_CUBE(p);
		find_cube(p, &start_point, ijk);
		p->cubes->cube.i = ijk[0];
		p->cubes->cube.j = ijk[1];
		p->cubes->cube.k = ijk[2];
		p->cubes->next = NULL;
		setcenter(p, ijk[0], ijk[1], ijk[2]);

		/* set corners of initial cube: */
		for (n = 0; n < 8; ++n) {
			p->cubes->cube.corners[n] =
				setcorner(p, ijk[0] + MC_BIT(n, 2),
					ijk[1] + MC_BIT(n, 1), ijk[2] + MC_BIT(n, 0));
		}

		/* process active cubes until none left */
		while (p->cubes != NULL) {
			MC_CUBES * temp_cube = p->cubes;
			MC_CUBE c = p->cubes->cube;

			noabort = docube(&c, p);
			if (!noabort) {
				fprintf(stderr, "polygonize:: aborted!/n");
				return "aborted";
			}

			/* pop cube */
			p->cubes = p->cubes->next;
#ifndef USE_DYNAMIC_VECTORS
			free(temp_cube);
#endif

			/* test faces for new cubes */
			testface(c.i - 1, c.j, c.k, &c, L, LBN, LBF, LTN, LTF, p);
			testface(c.i + 1, c.j, c.k, &c, R, RBN, RBF, RTN, RTF, p);
			testface(c.i, c.j - 1, c.k, &c, B, LBN, LBF, RBN, RBF, p);
			testface(c.i, c.j + 1, c.k, &c, T, LTN, LTF, RTN, RTF, p);
			testface(c.i, c.j, c.k - 1, &c, N, LBN, LTN, RBN, RTN, p);
			testface(c.i, c.j, c.k + 1, &c, F, LBF, LTF, RBF, RTF, p);

		}

		/* current seed point is processed */
		p->seed_list[next_seed].done = 1;

	}


	//handy for odd bugs
	//printf("     Finished Polygonizing. Mesh has %d vertices and %d triangles\n", p->wrapper->Mesh().m_vVertices.size(), p->wrapper->Mesh().m_vTriangles.size());
	//if (p->wrapper->Mesh().m_vVertices.size() == 0) {
	//	int s = 4;
	//}
	

	return NULL;
}








/* testface: given cube at lattice (i, j, k), and four corners of face,
* if surface crosses face, compute other four corners of adjacent cube
* and add new cube to cube stack */

static void testface(int i, int j, int k, MC_CUBE * old, int face,
	int c1, int c2, int c3, int c4,
	polygonizer * p)
{
	MC_CUBE newc;
	MC_CUBES *oldcubes = p->cubes;
	static int facebit[6] = { 2, 2, 1, 1, 0, 0 };
	int ijk[3] = { i,j,k };
	int n, bit, pos;

	bit = facebit[face];
	pos = (old->corners[c1]->value > 0.0) ? 1 : 0;

	/* test if no surface crossing, cube out of bounds, or already visited: */
	if ((old->corners[c2]->value > 0) == pos &&
		(old->corners[c3]->value > 0) == pos &&
		(old->corners[c4]->value > 0) == pos) return;

	if (!iaabb_inside(&(p->bounds), ijk)) {
		printf("%d %d %d / %d %d %d/n",
			p->bounds.low[0], p->bounds.low[1], p->bounds.low[2],
			p->bounds.high[0], p->bounds.high[1], p->bounds.high[2]);
		printf("%d %d %d out of bounds!/n", ijk[0], ijk[1], ijk[2]);
		return;
	}

	if (setcenter(p, i, j, k))
		return;

	/* create new cube: */
	newc.i = i;
	newc.j = j;
	newc.k = k;
	for (n = 0; n < 8; n++)
		newc.corners[n] = NULL;
	newc.corners[MC_FLIP(c1, bit)] = old->corners[c1];
	newc.corners[MC_FLIP(c2, bit)] = old->corners[c2];
	newc.corners[MC_FLIP(c3, bit)] = old->corners[c3];
	newc.corners[MC_FLIP(c4, bit)] = old->corners[c4];
	for (n = 0; n < 8; n++)
		if (newc.corners[n] == NULL)
			newc.corners[n] = setcorner(p, i + MC_BIT(n, 2),
				j + MC_BIT(n, 1), k + MC_BIT(n, 0));

	/*add cube to top of stack: */
	p->cubes = ALLOC_CUBE(p);
	p->cubes->cube = newc;
	p->cubes->next = oldcubes;
}



/* setcorner: return corner with the given lattice location
set (and cache) its function value */

MC_CORNER *setcorner(polygonizer *p, int i, int j, int k)
{
	/* for speed, do corner value caching here */
	MC_CORNER *c;
	int index = MC_HASH(i, j, k);
	MC_CORNERLIST *l = p->corners[index];

	for (; l != NULL; l = l->next) {
		if (l->i == i && l->j == j && l->k == k)
			return l->corner;
	}

	c = ALLOC_CORNER(p);
	memset(c, 0, sizeof(MC_CORNER));
	c->i = i;
	c->x = p->center.x + ((float)i - .5f)*p->size;
	c->j = j;
	c->y = p->center.y + ((float)j - .5f)*p->size;
	c->k = k;
	c->z = p->center.z + ((float)k - .5f)*p->size;
	l = ALLOC_CORNERLIST(p);
	memset(l, 0, sizeof(MC_CORNERLIST));
	l->i = i; l->j = j; l->k = k;

	l->value = c->value = p->wrapper->Function()->ValueT(c->x, c->y, c->z);
	l->corner = c;

	if (l == p->corners[index])
		abort();

	l->next = p->corners[index];
	p->corners[index] = l;
	return c;
}


/* ok, screw it - I'm writing my own */
static void find_in_out(polygonizer * p, float sx, float sy, float sz,
	MC_POINT * in, MC_POINT * out, float * inValue, float * outValue)
{
	float xdir, ydir, zdir, dist, delta;

	/*
	xdir = (MC_RAND()-0.5);
	ydir = (MC_RAND()-0.5);
	zdir = (MC_RAND()-0.5);*/
	xdir = 0.5773502692f;
	ydir = 0.5773502692f;
	zdir = 0.5773502692f;
	dist = 0.0;
	delta = p->size / 1.1f;
	*outValue = 1.0;
	*inValue = 0.0;
	in->x = out->x = sx;
	in->y = out->y = sy;
	in->z = out->z = sz;

	while (*outValue > 0.0) {
		*inValue = *outValue;
		out->x = sx + (xdir * dist);
		out->y = sy + (ydir * dist);
		out->z = sz + (zdir * dist);
		dist += delta;
		*outValue = p->wrapper->Function()->ValueT(out->x, out->y, out->z);
	}
	dist -= 2.0f*delta;
	in->x = sx + (xdir * dist);
	in->y = sy + (ydir * dist);
	in->z = sz + (zdir * dist);
}

/**** Cubical Polygonization (optional) ****/

#define LB	0  /* left bottom edge	*/
#define LT	1  /* left top edge	*/
#define LN	2  /* left near edge	*/
#define LF	3  /* left far edge	*/
#define RB	4  /* right bottom edge */
#define RT	5  /* right top edge	*/
#define RN	6  /* right near edge	*/
#define RF	7  /* right far edge	*/
#define BN	8  /* bottom near edge	*/
#define BF	9  /* bottom far edge	*/
#define TN	10 /* top near edge	*/
#define TF	11 /* top far edge	*/

/*			edge: LB, LT, LN, LF, RB, RT, RN, RF, BN, BF, TN, TF */
static int corner1[12] = { LBN,LTN,LBN,LBF,RBN,RTN,RBN,RBF,LBN,LBF,LTN,LTF };
static int corner2[12] = { LBF,LTF,LTN,LTF,RBF,RTF,RTN,RTF,RBN,RBF,RTN,RTF };
static int leftface[12] = { B,  L,  L,  F,  R,  T,  N,  R,  N,  B,  T,  F };
/* face on left when going corner1 to corner2 */
static int rightface[12] = { L,  T,  N,  L,  B,  R,  R,  F,  B,  F,  N,  T };
/* face on right when going corner1 to corner2 */


/* docube: triangulate the cube directly, without decomposition */

int docube(MC_CUBE * cube, polygonizer * p)
{
	MC_INTLISTS *polys;
	int i, index = 0;
	for (i = 0; i < 8; i++) {
		if (cube->corners[i]->value > 0.0) {
			index += (1 << i);
		}
		
	}
	for (polys = p->cubetable[index]; polys; polys = polys->next) {
		MC_INTLIST *edges;
		int a = -1, b = -1, count = 0;
		for (edges = polys->list; edges; edges = edges->next) {
			MC_CORNER *c1 = cube->corners[corner1[edges->i]];
			MC_CORNER *c2 = cube->corners[corner2[edges->i]];
			int c = vertid(c1, c2, p);
			if (++count > 2 && !new_triangle(a, b, c, p->wrapper->Mesh()))
				return 0;
			if (count < 3) a = b;
			b = c;
		}
	}

	return 1;
}


/* nextcwedge: return next clockwise edge from given edge around given face */

int nextcwedge(int edge, int face)
{
	switch (edge) {
	case LB: return (face == L) ? LF : BN;
	case LT: return (face == L) ? LN : TF;
	case LN: return (face == L) ? LB : TN;
	case LF: return (face == L) ? LT : BF;
	case RB: return (face == R) ? RN : BF;
	case RT: return (face == R) ? RF : TN;
	case RN: return (face == R) ? RT : BN;
	case RF: return (face == R) ? RB : TF;
	case BN: return (face == B) ? RB : LN;
	case BF: return (face == B) ? LB : RF;
	case TN: return (face == T) ? LT : RN;
	case TF: return (face == T) ? RT : LF;
	default:
		printf("ERROR! nextcwedge reached default case!/n");
		return 0;
	}
}


/* otherface: return face adjoining edge that is not the given face */

int otherface(int edge, int face)
{
	int other = leftface[edge];
	return (face == other) ? rightface[edge] : other;
}



static void init_seed_points(polygonizer * p)
{
	int nMaxPoints = 1024;
	float * seedbuf = new float[nMaxPoints * 3];
	int nBufIndex = 0;
	p->wrapper->Function()->GetSeedPoints(seedbuf, nBufIndex, nMaxPoints * 3);
	int nPoints = nBufIndex / 3;


	if (p->seed_list != NULL)
		free(p->seed_list);
	p->seed_list = (MC_SEEDPOINT *)malloc(sizeof(MC_SEEDPOINT)*nPoints);
	if (p->seed_list == NULL) {
		perror("init_seed_points:: seed_list malloc:: ");
		return;
	}

	for (int pi = 0; pi < nPoints; ++pi) {
		for (int k = 0; k < 3; ++k)
			p->seed_list[pi].p[k] = seedbuf[pi * 3 + k];
		p->seed_list[pi].done = 0;
	}

	p->num_seed_points = nPoints;

	delete[] seedbuf;
}


static int next_seed_point(polygonizer * p,
	MC_POINT * in, MC_POINT * out, float * inValue, float * outValue)
{
	int pi;
	MC_POINT point;
	int ijk[3];
	int index;
	int found = -1;
	MC_CENTERLIST * l;

	for (pi = 0; pi < p->num_seed_points; ++pi) {
		if (!p->seed_list[pi].done) {

			point.x = p->seed_list[pi].p[0];
			point.y = p->seed_list[pi].p[1];
			point.z = p->seed_list[pi].p[2];

			// make sure point is in bounds
			find_cube(p, &point, ijk);
			if (iaabb_inside(&(p->bounds), ijk)) {

				find_in_out(p, point.x, point.y, point.z, in, out, inValue, outValue);
				find_cube(p, in, ijk);


				found = 0;
				index = MC_HASH(ijk[0], ijk[1], ijk[2]);
				for (l = p->centers[index]; l != NULL; l = l->next)
					if (l->i == ijk[0] && l->j == ijk[1] && l->k == ijk[2])
						found = 1;

				if (!found)
					return pi;
			}

			p->seed_list[pi].done = 1;

			//if(!found) {
			//	return pi;
			//} else
			//	p->seed_list[pi].done = 1;
		}
	}

	return -1;
}


static void reset_lists(polygonizer * p)
{
#ifndef USE_DYNAMIC_VECTORS

	MC_CENTERLIST * center_cur, *center_next;
	MC_CORNERLIST * corner_cur, *corner_next;
	MC_EDGELIST * edges_cur, *edges_next;
	MC_CUBES * cube_cur, *cube_next;
	MC_CUBE * cube;
	int i;

	/* free center lists */
	for (i = 0; i < MC_HASHSIZE; ++i) {
		center_cur = p->centers[i];
		while (center_cur != NULL) {
			center_next = center_cur->next;
			free(center_cur);
			center_cur = center_next;
		}
		p->centers[i] = NULL;
	}

	/* free corner lists */
	for (i = 0; i < MC_HASHSIZE; ++i) {
		corner_cur = p->corners[i];
		while (corner_cur != NULL) {
			corner_next = corner_cur->next;
			free(corner_cur->corner);
			free(corner_cur);
			corner_cur = corner_next;
		}
		p->corners[i] = NULL;
	}

	/* free edge lists */
	for (i = 0; i < 2 * MC_HASHSIZE; ++i) {
		edges_cur = p->edges[i];
		while (edges_cur != NULL) {
			edges_next = edges_cur->next;
			free(edges_cur);
			edges_cur = edges_next;
		}
		p->edges[i] = NULL;
	}

	/* free cube list */
	cube_cur = p->cubes;
	while (cube_cur != NULL) {
		cube_next = cube_cur->next;
		cube = &(cube_cur->cube);
		for (i = 0; i < 8; ++i)
			free(cube->corners[i]);
		free(cube_cur);
		cube_cur = cube_next;
	}
	p->cubes = NULL;

#else 

	dv_clear_vector(p->centerlist_vector);
	dv_clear_vector(p->corner_vector);
	dv_clear_vector(p->cornerlist_vector);
	dv_clear_vector(p->cube_vector);
	dv_clear_vector(p->edgelist_vector);

	memset(p->centers, 0, MC_HASHSIZE * sizeof(MC_CENTERLIST *));
	memset(p->corners, 0, MC_HASHSIZE * sizeof(MC_CORNERLIST *));
	memset(p->edges, 0, 2 * MC_HASHSIZE * sizeof(MC_EDGELIST *));
	p->cubes = NULL;

#endif
}


static void free_lists(polygonizer * p)
{
	int i;
	MC_INTLISTS * current_poly, *next_poly;
	MC_INTLIST * current_edge, *next_edge;
#ifndef USE_DYNAMIC_VECTORS

	MC_CENTERLIST * center_cur, *center_next;
	MC_CORNERLIST * corner_cur, *corner_next;
	MC_EDGELIST * edges_cur, *edges_next;
	MC_CUBES * cube_cur, *cube_next;
	MC_CUBE * cube;
	int i;

	/* free center lists */
	for (i = 0; i < MC_HASHSIZE; ++i) {
		center_cur = p->centers[i];
		while (center_cur != NULL) {
			center_next = center_cur->next;
			free(center_cur);
			center_cur = center_next;
		}
		p->centers[i] = NULL;
	}

	/* free corner lists */
	for (i = 0; i < MC_HASHSIZE; ++i) {
		corner_cur = p->corners[i];
		while (corner_cur != NULL) {
			corner_next = corner_cur->next;
			free(corner_cur->corner);
			free(corner_cur);
			corner_cur = corner_next;
		}
		p->corners[i] = NULL;
	}

	/* free edge lists */
	for (i = 0; i < 2 * MC_HASHSIZE; ++i) {
		edges_cur = p->edges[i];
		while (edges_cur != NULL) {
			edges_next = edges_cur->next;
			free(edges_cur);
			edges_cur = edges_next;
		}
		p->edges[i] = NULL;
	}

	/* free cube list */
	cube_cur = p->cubes;
	while (cube_cur != NULL) {
		cube_next = cube_cur->next;
		cube = &(cube_cur->cube);
		for (i = 0; i < 8; ++i)
			free(cube->corners[i]);
		free(cube_cur);
		cube_cur = cube_next;
	}
	p->cubes = NULL;

#else

	dv_free_vector(p->centerlist_vector);
	dv_free_vector(p->corner_vector);
	dv_free_vector(p->cornerlist_vector);
	dv_free_vector(p->cube_vector);
	dv_free_vector(p->edgelist_vector);

#endif

	/* free top-level ** lists */
	p->cubes = NULL;

	free(p->centers);
	p->centers = NULL;
	free(p->corners);
	p->corners = NULL;
	free(p->edges);
	p->edges = NULL;

	/* free cube table */
	for (i = 0; i < 256; i++) {
		current_poly = p->cubetable[i];

		while (current_poly != NULL) {
			next_poly = current_poly->next;
			current_edge = current_poly->list;
			while (current_edge != NULL) {
				next_edge = current_edge->next;
				free(current_edge);
				current_edge = next_edge;
			}
			free(current_poly);
			current_poly = next_poly;
		}
		p->cubetable[i] = NULL;
	}
}



/* makecubetable: create the 256 entry table for cubical polygonization */
static void makecubetable(MC_INTLISTS *cubetable[256])
{
	int i, e, c, done[12], pos[8];
	for (i = 0; i < 256; i++) {
		cubetable[i] = NULL;
		for (e = 0; e < 12; e++) done[e] = 0;
		for (c = 0; c < 8; c++) pos[c] = MC_BIT(i, c);
		for (e = 0; e < 12; e++)
			if (!done[e] && (pos[corner1[e]] != pos[corner2[e]])) {
				MC_INTLIST *ints = NULL;
				MC_INTLISTS *lists = (MC_INTLISTS *)calloc(1, sizeof(MC_INTLISTS));
				int start = e, edge = e;
				/* get face that is to right of edge from pos to neg corner: */
				int face = pos[corner1[e]] ? rightface[e] : leftface[e];
				while (1) {
					edge = nextcwedge(edge, face);
					done[edge] = 1;
					if (pos[corner1[edge]] != pos[corner2[edge]]) {
						MC_INTLIST *tmp = ints;
						ints = (MC_INTLIST *)calloc(1, sizeof(MC_INTLIST));
						ints->i = edge;
						ints->next = tmp; /* add edge to head of list */
						if (edge == start)
							break;
						face = otherface(edge, face);
					}
				}
				lists->list = ints; /* add ints to head of table entry */
				lists->next = cubetable[i];
				cubetable[i] = lists;
			}
	}
}




/**** Storage ****/


/*
* setcenter: set (i,j,k) entry of table[]
* return 1 if already set; otherwise, set and return 0
*/

int setcenter(polygonizer * p, int i, int j, int k)
{
	int index = MC_HASH(i, j, k);
	MC_CENTERLIST *newc, *l;
	MC_CENTERLIST *q = p->centers[index];

	for (l = q; l != NULL; l = l->next)
		if (l->i == i && l->j == j && l->k == k)
			return 1;
	newc = ALLOC_CENTERLIST(p);
	newc->i = i; newc->j = j; newc->k = k;
	newc->next = q;
	p->centers[index] = newc;
	return 0;
}



/*
* setedge and getedge find an edge and the vertex on that edge in
* the edge table.
*/

void setedge(polygonizer * p, int i1, int j1, int k1,
	int i2, int j2, int k2, int vid)
{
	unsigned int index;
	MC_EDGELIST *newe;
	if (i1>i2 || (i1 == i2 && (j1>j2 || (j1 == j2 && k1>k2)))) {
		int t = i1; i1 = i2; i2 = t; t = j1; j1 = j2; j2 = t; t = k1; k1 = k2; k2 = t;
	}
	index = MC_HASH(i1, j1, k1) + MC_HASH(i2, j2, k2);
	newe = ALLOC_EDGELIST(p);
	newe->i1 = i1; newe->j1 = j1; newe->k1 = k1;
	newe->i2 = i2; newe->j2 = j2; newe->k2 = k2;
	newe->vid = vid;
	newe->next = p->edges[index];
	p->edges[index] = newe;
}


int getedge(MC_EDGELIST *table[], int i1, int j1, int k1,
	int i2, int j2, int k2)
{
	MC_EDGELIST *q;
	if (i1>i2 || (i1 == i2 && (j1>j2 || (j1 == j2 && k1>k2)))) {
		int t = i1; i1 = i2; i2 = t; t = j1; j1 = j2; j2 = t; t = k1; k1 = k2; k2 = t;
	};
	q = table[MC_HASH(i1, j1, k1) + MC_HASH(i2, j2, k2)];
	for (; q != NULL; q = q->next)
		if (q->i1 == i1 && q->j1 == j1 && q->k1 == k1 &&
			q->i2 == i2 && q->j2 == j2 && q->k2 == k2)
			return q->vid;
	return -1;
}



/**** Vertices ****/


/* vertid: return index for vertex on edge:
* c1->value and c2->value are presumed of different sign
* return saved index if any; else compute vertex and save */

int vertid(MC_CORNER *c1, MC_CORNER *c2, polygonizer *p)
{
	MC_POINT v;
	MC_POINT a, b;

	/* try to find existing vertex */
	int vid = getedge(p->edges, c1->i, c1->j, c1->k, c2->i, c2->j, c2->k);
	if (vid != -1)
		return vid;			     /* previously computed */

								 /* no dice, converge on real vertex position */
	a.x = c1->x; a.y = c1->y; a.z = c1->z;
	b.x = c2->x; b.y = c2->y; b.z = c2->z;
	if (c1->value > c2->value)
		converge(&a, &b, c1->value, c2->value, p->wrapper->Function(), &v, p->convergence); /* position */
	else
		converge(&b, &a, c2->value, c1->value, p->wrapper->Function(), &v, p->convergence); /* position */

																							/* add to the list and store */
	vid = new_vertex(p->wrapper->Mesh(), &v);

	setedge(p, c1->i, c1->j, c1->k, c2->i, c2->j, c2->k, vid);
	return vid;
}



/*
* triangle: called by docube() for each triangle, appends triangle to mesh
*/

static int new_triangle(int i1, int i2, int i3, SimpleMesh & m)
{
	int ti = m.add_triangle(i1, i2, i3);
	return 1;
}




/*
* new_vertex sticks a new vertex on the end of the mesh
*/

static int new_vertex(SimpleMesh & m, MC_POINT * v)
{
	float position[3] = { v->x, v->y, v->z };
	float normal[3] = { v->x, v->y, v->z };
	int vi = m.add_vertex(position, normal);
	return vi;
}


// RMS TODO - can optimize this maybe by checking current error and
//   skipping iterations if we get close enough? Not sure if it
//   ends up being faster. What is possible FP precision here??

/* converge: from two points of differing sign, converge to zero crossing */

static void converge(MC_POINT * p1, MC_POINT * p2,
	float inValue, float outValue, ImplicitFunction * pFunction,
	MC_POINT * p, int convergence)
{
	int i = 0;
	MC_POINT pos, neg;

	float t, t2, value;

	// do at least one lerp
	t = (inValue == outValue) ?
		0.5f : (outValue) / (outValue - inValue);
	t2 = 1.0f - t;
	p->x = t * p1->x + t2 * p2->x;
	p->y = t * p1->y + t2 * p2->y;
	p->z = t * p1->z + t2 * p2->z;

	// now repeat until we run out of tries
	i = 1;
	if (i != convergence) {
		pos.x = p1->x; pos.y = p1->y; pos.z = p1->z;
		neg.x = p2->x; neg.y = p2->y; neg.z = p2->z;
	}

	while (i++ < convergence) {
		// new point
		value = pFunction->ValueT(p->x, p->y, p->z);
		if (value > 0.0) {
			inValue = value;
			pos.x = p->x; pos.y = p->y; pos.z = p->z;
		}
		else {
			outValue = value;
			neg.x = p->x; neg.y = p->y; neg.z = p->z;
		}

		// next lerp
		t = (outValue) / (outValue - inValue);
		t2 = 1.0f - t;
		p->x = t * pos.x + t2 * neg.x;
		p->y = t * pos.y + t2 * neg.y;
		p->z = t * pos.z + t2 * neg.z;
	}



	/*
	if (v < 0) {
	pos.x = p2->x; pos.y = p2->y; pos.z = p2->z;
	neg.x = p1->x; neg.y = p1->y; neg.z = p1->z;
	} else {
	pos.x = p1->x; pos.y = p1->y; pos.z = p1->z;
	neg.x = p2->x; neg.y = p2->y; neg.z = p2->z;
	}
	while(1) {
	p->x = 0.5*(pos.x + neg.x);
	p->y = 0.5*(pos.y + neg.y);
	p->z = 0.5*(pos.z + neg.z);
	if ( i++ == convergence) {
	return;
	}
	if( (EVALUATE(points, p->x, p->y, p->z)) > 0.0){
	pos.x = p->x; pos.y = p->y; pos.z = p->z;
	} else {
	neg.x = p->x; neg.y = p->y; neg.z = p->z;
	}
	}
	*/

}


/* find the ijk cube coordinates of the cube that
contains the point
[RMS] - there might be numerical errors in here...
*/
void find_cube(polygonizer * p, MC_POINT * point,
	int * ijk)
{
	float d, n;

	d = point->x - p->center.x;
	n = fabs(d) / (0.5f*p->size);
	n = ceil(floor(n) / 2.0f);
	ijk[0] = (d > 0.0) ? (int)n : -(int)n;

	d = point->y - p->center.y;
	n = fabs(d) / (0.5f*p->size);
	n = ceil(floor(n) / 2.0f);
	ijk[1] = (d > 0.0) ? (int)n : -(int)n;

	d = point->z - p->center.z;
	n = fabs(d) / (0.5f*p->size);
	n = ceil(floor(n) / 2.0f);
	ijk[2] = (d > 0.0) ? (int)n : -(int)n;
}