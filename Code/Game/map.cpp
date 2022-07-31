const int map_h = 5;
const int map_w = 10;

std::vector<text_t> map = {
".#.......#",
".#..####.#",
"....####.#",
"###.####.#",
"....####..",
};

void render_map() {
	for (int i = 0; i < map.size(); i++) {
		text_t text = map[i];
		int j = 0;
		while (*text) {
			if (*text == '.') {
				draw_sprite(dot(j, -i) * 32, 0.5, SP_FLOOR);
			}
			j += 1;
			text++;
		}
	}
}
