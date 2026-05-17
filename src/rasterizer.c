#include "color.h"
#include "types.h"

#define MIN(a,b) (a < b) ? a : b
#define MAX(a,b) (a > b) ? a : b

extern color32_t *colorBuffer;
extern void drawPixel(int i, int j);

typedef struct {
	fixedPoint_t A;
	fixedPoint_t B;
	fixedPoint_t C;
} edgeFunctionParams_t;

static inline edgeFunctionParams_t findEdgeFunctionParams(fixedPoint2d_t p, fixedPoint2d_t q) {
	return (edgeFunctionParams_t){
		.A = p.y - q.y,
		.B = -p.x + q.x,
		.C = p.x * q.y - q.x * p.y
	};
}

static inline fixedPoint_t edgeFunction(edgeFunctionParams_t pq, fixedPoint2d_t z) {
	return pq.A * z.x + pq.B * z.y + pq.C;
}

void rasterizeTriangle(fixedPoint2d_t p, fixedPoint2d_t q, fixedPoint2d_t r) {
	fixedPoint_t min_x = MIN(p.x, q.x);
	min_x = MIN(min_x, r.x);
	fixedPoint_t max_x = MAX(p.x, q.x);
	max_x = MAX(max_x, r.x);

	fixedPoint_t min_y = MIN(p.y, q.y);
	min_y = MIN(min_y, r.y);
	fixedPoint_t max_y = MAX(p.y, q.y);
	max_y = MAX(max_y, r.x);

	//what is wrong with max????

	edgeFunctionParams_t pq = findEdgeFunctionParams(p, q);
	edgeFunctionParams_t qr = findEdgeFunctionParams(q, r);
	edgeFunctionParams_t rp = findEdgeFunctionParams(r, p);

	fixedPoint2d_t minPoint = {min_x, min_y};

	fixedPoint_t rowStartBaryCoord_pq = edgeFunction(pq, minPoint);
	fixedPoint_t rowStartBaryCoord_qr = edgeFunction(qr, minPoint);
	fixedPoint_t rowStartBaryCoord_rp = edgeFunction(rp, minPoint);
	for (fixedPoint_t y = min_y; y <= max_y; y++) {
		fixedPoint_t currBaryCoord_pq = rowStartBaryCoord_pq;
		fixedPoint_t currBaryCoord_qr = rowStartBaryCoord_qr;
		fixedPoint_t currBaryCoord_rp = rowStartBaryCoord_rp;

		for (fixedPoint_t x = min_x; x <= max_x; x++) {
			if ((currBaryCoord_pq | currBaryCoord_qr | currBaryCoord_rp) >= 0) {
				drawPixel(y, x);
			}
			
			currBaryCoord_pq += pq.A;
			currBaryCoord_qr += qr.A;
			currBaryCoord_rp += rp.A;
		}

		rowStartBaryCoord_pq += pq.B;
		rowStartBaryCoord_qr += qr.B;
		rowStartBaryCoord_rp += rp.B;
	}
}
