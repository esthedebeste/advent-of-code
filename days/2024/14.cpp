#ifndef NDEBUG
#define SHOULD_DRAW_BMP
#endif

#include <bitset>
#include <filesystem>
#include "shared.h"

struct Robot {
	pos_t<> position;
	pos_t<> velocity;
};

// modulo(-1, 4) = 3, modulo(7, 4) = 3
template <class T> T modulo(T a, T b) { return (a % b + b) % b; }

constexpr int width = 101;
constexpr int height = 103;
constexpr auto rowWidth = 304; /* 101 * 3 rounded to 4*/
#pragma pack(push, 1)
constexpr struct {
	uint8_t magic1 = 'B';
	uint8_t magic2 = 'M';
	uint32_t fileSize = 14 + 40 + rowWidth * 103;
	uint16_t reserved1 = 0;
	uint16_t reserved2 = 0;
	uint32_t offset = 0x36;
	uint32_t headerSize = 40;
	uint32_t width = 101;
	uint32_t height = 103;
	uint16_t planes = 1;
	uint16_t bitsPerPixel = 24;
	uint32_t compression = 0;
	uint32_t imageSize = rowWidth * 103;
	uint32_t horizontalResolution = 2835;
	uint32_t verticalResolution = 2835;
	uint32_t colorsUsed = 0;
	uint32_t colorsImportant = 0;
} header{};
#pragma pack(pop)

void draw_bmp(const int iteration, const std::bitset<width * height> &field) {
#ifndef SHOULD_DRAW_BMP
	return;
#endif
	std::array<uint8_t, header.imageSize> imageData{};
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++) {
			const bool curr = field.test(y * width + x);
			uint8_t *c = &imageData[(height - y - 1) * rowWidth + x * 3];
			c[0] = c[1] = c[2] = curr * 255;
		}
	std::filesystem::create_directories("./output/2024/14/");
	const auto path = "./output/2024/14/" + std::to_string(iteration) + ".bmp";
	std::ofstream out(path, std::ios::binary);
	out.write(reinterpret_cast<const char *>(&header), sizeof(header));
	out.write(reinterpret_cast<const char *>(imageData.data()), imageData.size());
	out.flush();
}

// recognizes a tree only by the fact that the top line of it is 31 pixels wide
bool recognize_tree(const int iteration, const std::vector<Robot> &robots) {
	std::bitset<width * height> field{};
	for (const auto &robot : robots) field.set(robot.position.to_1d(width), true);
	for (int y = 0; y < height; y++) {
		int curr_row = 0;
		for (int x = 0; x < width; x++)
			if (!field.test(y * width + x)) [[likely]]
				curr_row = 0;
			else if (++curr_row == 31) [[unlikely]] {
				draw_bmp(iteration, field);
				return true;
			}
	}
	return false;
}

int main() {
	day(
		[](std::istream &is) {
			std::vector<Robot> robots;
			is >> "p=";
			do {
				auto &[position, velocity] = robots.emplace_back();
				is >> position.x >> "," >> position.y >> " v=" >> velocity.x >> "," >> velocity.y;
			} while (check(is, "\np="));
			return robots;
		},
		[](const std::vector<Robot> &robots) {
			std::array<int, 4> quadrants{};
			for (const auto &[position, velocity] : robots) {
				const int x = modulo(position.x + velocity.x * 100, 101);
				const int y = modulo(position.y + velocity.y * 100, 103);

				if (x > 50 && y < 51)
					quadrants[0]++;
				else if (x > 50 && y > 51)
					quadrants[1]++;
				else if (x < 50 && y < 51)
					quadrants[2]++;
				else if (x < 50 && y > 51)
					quadrants[3]++;
			}
			return quadrants[0] * quadrants[1] * quadrants[2] * quadrants[3];
		},
		[](std::vector<Robot>& robots) {
			for (int i = 0;; i++) {
				if (recognize_tree(i, robots))
					return i;
				for (auto &[position, velocity] : robots) {
					position.x = modulo(position.x + velocity.x, 101);
					position.y = modulo(position.y + velocity.y, 103);
				}
			}
		});
}
