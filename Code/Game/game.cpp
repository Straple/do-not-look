#include "map.cpp"

#include "game_utils.cpp"

#include "UI Objects/ui_objects.cpp"

#include "Game objects/game_objects.cpp"

// game objects

std::vector<Slime> Slimes;

std::vector<Bat> Bats;

#include "game_collision.cpp"

void build_world() {
	for (int i = 0; i < map_h; i++) {
		for (int j = 0; j < map_w; j++) {
			// в первой клетке без мобов
			if (i + j != 0 && map[i][j] == '.') { 
				if (get_urnd() < 0.5) {
					Bats.push_back(Bat(dot(j, -i) * 32 + dot(16, -16)));
				}
				else {
					Slimes.push_back(Slime(dot(j, -i) * 32 + dot(16, -16)));
				}
			}
		}
	}
}

// UI objects

Mouse mouse(SP_CURSOR, SP_FOCUS_CURSOR, 0.09);

void simulate_player(const Input& input, point_t delta_time) {

	// накопление вектора движения
	auto accum_ddp = [&input](button_t left, button_t right, button_t top, button_t bottom) -> dot {
		return dot(is_down(right) - is_down(left), is_down(top) - is_down(bottom));
	};

	player.simulate(delta_time, accum_ddp(BUTTON_A, BUTTON_D, BUTTON_W, BUTTON_S), is_down(BUTTON_J), pressed(BUTTON_SPACE));

	// player attack
	{
		if (player.simulate_attack(Slimes) |
			player.simulate_attack(Bats)) {

			player.now_is_attached = false;
		}
	}
}

void simulate_physics(point_t delta_time) {

	// simulate player
	{
		simulate_game_collision(player);
	}

	// simulate slimes
	{
		for (auto& slime : Slimes) {
			slime.simulate(delta_time);
		}

		simulate_game_collision(Slimes);
	}

	// simulate bats
	{
		for (auto& bat : Bats) {
			bat.simulate(delta_time);
		}

		simulate_game_collision(Bats);
	}

	// simulate game items
	{
		for(int i = 0; i < Game_items.size(); i++){
			if (Game_items[i].simulate(delta_time)) {
				Game_items.erase(Game_items.begin() + i);
				i--;
			}
		}
	}

	// simulate effects
	{
		for (u32 i = 0; i < Effects.size(); i++) {
			if (Effects[i].simulate(delta_time)) {
				Effects.erase(Effects.begin() + i);
				i--;
			}
		}
	}
}

void render_game() {

	clear_screen(BLACK);

	if (player.is_eyes_closed) {
		player.draw();
	}
	else {

		render_map();

		// draw players, slimes, bats, 
		// bushes, trees, fireplaces,
		// logs
		{
			// top sort

			struct top_sort_object {

				enum type_object {
					TO_PLAYER,
					TO_SLIME,
					TO_BAT,
					TO_GAME_ITEM,

					TO_UNDEFIND,
				};

				type_object type;
				const void* ptr;

				top_sort_object() {
					type = TO_UNDEFIND;
					ptr = nullptr;
				}

				top_sort_object(const Player& player) {
					type = TO_PLAYER;
					ptr = reinterpret_cast<const void*>(&player);
				}
				top_sort_object(const Bat& player) {
					type = TO_BAT;
					ptr = reinterpret_cast<const void*>(&player);
				}
				top_sort_object(const Slime& slime) {
					type = TO_SLIME;
					ptr = reinterpret_cast<const void*>(&slime);
				}
				top_sort_object(const game_item& gitem) {
					type = TO_GAME_ITEM;
					ptr = reinterpret_cast<const void*>(&gitem);
				}

				point_t get_y() const {
					switch (type) {
					case TO_PLAYER: {
						return reinterpret_cast<const Player*>(ptr)->pos.y;
					}break;
					case TO_SLIME: {
						return reinterpret_cast<const Slime*>(ptr)->pos.y;
					}break;
					case TO_BAT: {
						return reinterpret_cast<const Bat*>(ptr)->pos.y;
					}break;
					case TO_GAME_ITEM: {
						return reinterpret_cast<const game_item*>(ptr)->pos.y;
					}break;
					}

					ASSERT(false, "undefind object type");
				}

				void draw() {
					switch (type) {
					case TO_PLAYER: {
						reinterpret_cast<const Player*>(ptr)->draw();
					}break;
					case TO_SLIME: {
						reinterpret_cast<const Slime*>(ptr)->draw();
					}break;
					case TO_BAT: {
						reinterpret_cast<const Bat*>(ptr)->draw();
					}break;
					case TO_GAME_ITEM: {
						reinterpret_cast<const game_item*>(ptr)->draw();
					}break;
					case TO_UNDEFIND: {
						ASSERT(false, "undefind object type");
					}break;
					}
				}

				bool operator < (const top_sort_object& Rhs) const {
					return get_y() > Rhs.get_y();
				}
			};

			std::vector<top_sort_object> Objects;
			Objects.push_back(player);
			for (auto& slime : Slimes) {
				Objects.push_back(slime);
			}
			for (auto& bat : Bats) {
				Objects.push_back(bat);
			}
			for (auto& gitem : Game_items) {
				Objects.push_back(gitem);
			}

			std::stable_sort(Objects.begin(), Objects.end());

			for (auto& obj : Objects) {
				obj.draw();
			}
		}
	}
	
	// draw effects
	{
		for (u32 i = 0; i < Effects.size(); i++) {
			Effects[i].draw();
		}
	}
	
	
	/*if (eng_state.show_ui()) {
		// hp
		ui_state(dot(5 - arena_half_size.x, arena_half_size.y - 5), dot(25 - arena_half_size.x, arena_half_size.y - 7.5))
			.draw(Players[player_id].hp, Players[player_id].max_hp, GREY, RED);

		// exp
		ui_state(dot(5 - arena_half_size.x, arena_half_size.y - 10), dot(25 - arena_half_size.x, arena_half_size.y - 12.5))
			.draw(Players[player_id].exp, s16(10), GREY, YELLOW);

		// damage
		draw_object(Players[player_id].damage, dot(5 - arena_half_size.x, arena_half_size.y - 15), 0.6, BLUE);

		draw_object(Players[player_id].lvl, dot(5 - arena_half_size.x, arena_half_size.y - 20), 0.6, PURPLE);

		mouse.draw();
	}*/

	mouse.draw();
}

template<typename func_t>
void simulate_input(const Input& input, func_t&& window_mode_callback) {
	if (pressed(BUTTON_ESC)) {
		running = false;
	}

	if (!player.is_paralyzed) {
		player.is_eyes_closed = is_down(BUTTON_SHIFT);
	}
	else {
		player.is_eyes_closed = false;
	}

	if (pressed(BUTTON_ENTER)) {
		window_mode_callback();
	}

	if (pressed(BUTTON_TAB)) {
		debug_mode = !debug_mode;
	}

	if (pressed(BUTTON_K)) {
		locator_vis_mode = !locator_vis_mode;
	}
	
	if (pressed(BUTTON_F)) {
		show_fps = !show_fps;
	}

	// update render_scale
	{
		if (is_down(BUTTON_UP)) {

			point_t pt_x = (mouse.pos.x + arena_half_size.x) * scale_factor;
			point_t pt_y = (mouse.pos.y + arena_half_size.y) * scale_factor;


			render_scale *= 0.95;

			// relax scaling
			scale_factor = render_state.height * render_scale;

			// relax arena
			arena_half_size = dot(static_cast<point_t>(render_state.width) / scale_factor, static_cast<point_t>(1) / render_scale) * 0.5;


			mouse.pos = dot(pt_x, pt_y)
				/ scale_factor - arena_half_size;
		}

		if (is_down(BUTTON_DOWN)) {
			point_t pt_x = (mouse.pos.x + arena_half_size.x) * scale_factor;
			point_t pt_y = (mouse.pos.y + arena_half_size.y) * scale_factor;


			render_scale /= 0.95;

			// relax scaling
			scale_factor = render_state.height * render_scale;

			// relax arena
			arena_half_size = dot(static_cast<point_t>(render_state.width) / scale_factor, static_cast<point_t>(1) / render_scale) * 0.5;


			mouse.pos = dot(pt_x, pt_y)
				/ scale_factor - arena_half_size;
		}
	}

	mouse.simulate(input);
}

template<typename func_t>
void simulate_game(const Input& input, point_t delta_time, func_t&& window_mode_callback) {

	simulate_input(input, window_mode_callback);

	camera.simulate(player.pos, delta_time);

	// simulate players
	{
		simulate_player(input, delta_time);

		//simulate_game_collision(Players);
	}

	simulate_physics(delta_time);
	mouse.simulate(input);

	render_game();
}
