#include "color.h"
#include "types.h"
#include <arm_neon.h>

#define MIN(a,b) (a < b) ? a : b
#define MAX(a,b) (a > b) ? a : b

// number of 32 bit integers that can fit in
// a single SIMD register
#define VECTOR_REG_WIDTH 4

extern color32_t *colorBuffer;
extern void drawPixelsVectorized(int i, int j, int32x4_t mask);

typedef struct {
	fixedPoint_t A;
	fixedPoint_t B;
	fixedPoint_t C;
} edgeFunctionParams_t;

static inline edgeFunctionParams_t findEdgeFunctionParams(fixedPoint2d_t p, fixedPoint2d_t q) {
	return (edgeFunctionParams_t) {
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
	max_y = MAX(max_y, r.y);

	edgeFunctionParams_t pq = findEdgeFunctionParams(p, q);
	edgeFunctionParams_t qr = findEdgeFunctionParams(q, r);
	edgeFunctionParams_t rp = findEdgeFunctionParams(r, p);

	fixedPoint2d_t minPoint = {min_x, min_y};

	fixedPoint_t rowStartBaryCoord_pq = edgeFunction(pq, minPoint);
	fixedPoint_t rowStartBaryCoord_qr = edgeFunction(qr, minPoint);
	fixedPoint_t rowStartBaryCoord_rp = edgeFunction(rp, minPoint);

// vectorizing the initial barycentric coordinates for groups of 4 pixels	
	constexpr int32_t zeroToThree[VECTOR_REG_WIDTH] = {0,1,2,3};

	// pq
	int32x4_t pqA_vectorized = vmulq_s32(
			vdupq_n_s32(pq.A), vld1q_s32(zeroToThree)
		);
	int32x4_t rowStartBaryCoord_pq_vectorized = 
		vaddq_s32(vdupq_n_s32(rowStartBaryCoord_pq), pqA_vectorized);

	// qr
	int32x4_t qrA_vectorized = vmulq_s32(
			vdupq_n_s32(qr.A), vld1q_s32(zeroToThree)
		);
	int32x4_t rowStartBaryCoord_qr_vectorized = 
		vaddq_s32(vdupq_n_s32(rowStartBaryCoord_qr), qrA_vectorized);

	// rp
	int32x4_t rpA_vectorized = vmulq_s32(
			vdupq_n_s32(rp.A), vld1q_s32(zeroToThree)
		);
	int32x4_t rowStartBaryCoord_rp_vectorized = 
		vaddq_s32(vdupq_n_s32(rowStartBaryCoord_rp), rpA_vectorized);

	for (fixedPoint_t y = min_y; y <= max_y; y++) {
		int32x4_t currBaryCoord_pq_vectorized = rowStartBaryCoord_pq_vectorized;
		int32x4_t currBaryCoord_qr_vectorized = rowStartBaryCoord_qr_vectorized;
		int32x4_t currBaryCoord_rp_vectorized = rowStartBaryCoord_rp_vectorized;

		for (fixedPoint_t x = min_x; x <= max_x; x+= VECTOR_REG_WIDTH) {
#ifndef NDEBUG
			if (x == 352 && y == 300) {
				[[maybe_unused]] int debug = 0;
			}
#endif
			int32x4_t mask = 
				vorrq_s32(
				vorrq_s32(
					currBaryCoord_pq_vectorized, currBaryCoord_qr_vectorized),
					currBaryCoord_rp_vectorized);
			mask = vcgeq_s32(mask, vdupq_n_s32(0));
			drawPixelsVectorized(y, x, mask);

			currBaryCoord_pq_vectorized =
				vaddq_s32(vdupq_n_s32(VECTOR_REG_WIDTH*pq.A), currBaryCoord_pq_vectorized);
			currBaryCoord_qr_vectorized =
				vaddq_s32(vdupq_n_s32(VECTOR_REG_WIDTH*qr.A), currBaryCoord_qr_vectorized);
			currBaryCoord_rp_vectorized =
				vaddq_s32(vdupq_n_s32(VECTOR_REG_WIDTH*rp.A), currBaryCoord_rp_vectorized);
		}

		rowStartBaryCoord_pq_vectorized = vaddq_s32(rowStartBaryCoord_pq_vectorized, vdupq_n_s32(pq.B));
		rowStartBaryCoord_qr_vectorized = vaddq_s32(rowStartBaryCoord_qr_vectorized, vdupq_n_s32(qr.B));
		rowStartBaryCoord_rp_vectorized = vaddq_s32(rowStartBaryCoord_rp_vectorized, vdupq_n_s32(rp.B));
	}
}
