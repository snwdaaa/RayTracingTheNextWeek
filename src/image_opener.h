#ifndef IMAGE_OPENER
#define IMAGE_OPENER

// �̹��� ���� ���α׷��� ������ �ڵ����� ppm �̹��� ������ ����
#include <string>
#include <cstdlib>

void openImage(const std::string& filename) {
    std::string command;

#ifdef _WIN32 // Windows ȯ���� ���
    command = "start " + filename;
#elif __APPLE__ // macOS ȯ���� ���
    command = "open " + filename;
#else // Linux �� ��Ÿ Unix �迭 ȯ���� ���
    command = "xdg-open " + filename;
#endif

    std::cout << "�̹��� ���� �Ϸ�. ��� ������ ���ϴ�: " << filename << std::endl;
    system(command.c_str());
}

#endif

