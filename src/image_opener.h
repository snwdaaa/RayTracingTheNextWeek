#ifndef IMAGE_OPENER
#define IMAGE_OPENER

// 이미지 생성 프로그램이 끝나면 자동으로 ppm 이미지 파일을 실행
#include <string>
#include <cstdlib>

void openImage(const std::string& filename) {
    std::string command;

#ifdef _WIN32 // Windows 환경일 경우
    command = "start " + filename;
#elif __APPLE__ // macOS 환경일 경우
    command = "open " + filename;
#else // Linux 및 기타 Unix 계열 환경일 경우
    command = "xdg-open " + filename;
#endif

    std::cout << "이미지 생성 완료. 결과 파일을 엽니다: " << filename << std::endl;
    system(command.c_str());
}

#endif

