#include "Common.h"

// 렌더 준비 & 렌더 루프 실행
__global__ void Render(int width, int height) { // TODO: 데이터 정보 배열 포인터
#pragma omp parallel for schedule(dynamic)
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			Color pixelColor(0, 0, 0);
			for (int sample = 0; sample < samplesPerPixel; sample++) {
				Ray r = GetRay(i, j); // 픽셀 정사각형 내에서 랜덤 샘플링
				pixelColor += GetRayColor(r, maxDepth, world);
			}
			pixelColor *= pixelSamplesScale; // 평균 구하기
			images[j * width + i] = pixelColor;
		}
	}
}