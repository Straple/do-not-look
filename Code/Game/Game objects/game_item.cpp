#define GAME_ITEM_DRAW_SIZE 0.3
#define GAME_ITEM_LOCATOR_RADIUS 10

struct game_item {
	dot pos;
	dot dp;

	point_t animation_time = 0;

	enum item_t {
		IT_HEART,
		IT_COIN,
	} type;

	game_item(){}
	game_item(dot pos, item_t type) {
		this->pos = pos;
		this->type = type;
	}

	void draw() const {
		point_t diff_y = 0;
		if (animation_time < 0.75) {
			diff_y += animation_time * 2;
		}
		else {
			diff_y += 0.75 * 2 - (animation_time - 0.75) * 2;
		}

		if (type == IT_HEART) {
			draw_sprite(pos + dot(0, diff_y) + dot(-6, 6) * GAME_ITEM_DRAW_SIZE, GAME_ITEM_DRAW_SIZE, SP_HEART);
		}
		else {
			draw_sprite(pos + dot(0, diff_y) + dot(-7.5, 7.5) * GAME_ITEM_DRAW_SIZE, GAME_ITEM_DRAW_SIZE, SP_COIN);
		}

		if (locator_vis_mode) {
			draw_circle(Circle(pos - camera.pos, GAME_ITEM_LOCATOR_RADIUS), Color(0xffffff, 100));

			draw_circle(Circle(pos - camera.pos, 0.1), RED);
		}
	}

	bool simulate(point_t delta_time) {
		// обновить левитацию
		animation_time += delta_time;
		if (animation_time > 1.5) {
			animation_time = 0;
		}

		// следуем за игроком
		if ((player.pos - pos).getLen() <= GAME_ITEM_LOCATOR_RADIUS) {
			move_to2d(pos, player.pos, dp, (player.pos - pos) * 50, delta_time);

			// игрок подобрал
			if ((player.pos - pos).getLen() < 1) {

				add_sound_player_pop_item();

				if (type == IT_HEART) {
					player.update_health(+1);
				}
				else {
					player.money++;
				}
				return true;
			}
		}
		return false;
	}
};

std::vector<game_item> Game_items;

void add_random_game_object(dot pos) {
	Game_items.push_back(game_item(pos, get_urnd() < 0.3 ? game_item::IT_HEART : game_item::IT_COIN));
}
