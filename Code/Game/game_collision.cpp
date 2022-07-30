

template<typename T1, typename T2>
bool verify_others_obj(const T1& Lhs, const T2& Rhs) {
	return reinterpret_cast<const void*>(&Lhs) != reinterpret_cast<const void*>(&Rhs);
}

template<typename collision>
void update_collision(Player& player, const collision& coll) {
	if (!player.is_paralyzed) {
		player.pos = coll.bubble(player.get_collision());
	}
}
template<typename collision>
void update_collision(Slime& slime, const collision& coll) {
	if (!slime.is_attack) {
		slime.pos = coll.bubble(slime.get_collision());
	}
}
template<typename collision>
void update_collision(Bat& bat, const collision& coll) {
	bat.pos = coll.bubble(bat.get_collision());
}

void simulate_game_collision(Player& player) {
	// оттолкнуть слаймов
	for (auto& slime : Slimes) {
		update_collision(slime, player.get_collision());
	}

	// map collision
	for (int i = 0; i < map.size(); i++) {
		text_t text = map[i];
		int j = 0;
		while (*text) {
			if (*text == '#') {
				collision_box box(dot(j, -i) * 32, dot(j + 1, -i - 1) * 32);
				player.pos = box.bubble(player.get_collision());
			}
			j += 1;
			text++;
		}
	}

	// map border collision
	player.pos = dot(
		clamp<point_t>(PLAYER_COLLISION_RADIUS, player.pos.x, map_w * 32 - PLAYER_COLLISION_RADIUS),
		clamp<point_t>(-map_h * 32 + PLAYER_COLLISION_RADIUS, player.pos.y, -PLAYER_COLLISION_RADIUS)
	);
}

// массив объектов с коллизией выталкивет другие объекты
template<typename container_t>
void simulate_game_collision(container_t& Objects) {

	if (Objects.empty()) {
		return;
	}

	if (typeid(Objects.back()) == typeid(Bat)) {

		// bat2 толкает bat1
		for (auto& bat1 : Bats) {
			for (auto& bat2 : Bats) {
				if (verify_others_obj(bat1, bat2)) {
					update_collision(bat2, bat1.get_collision());
				}
			}
		}
	}
	else { // obj is slime

		// slimes
		for (auto& obj : Objects) {
			for (auto& slime : Slimes) {
				if (verify_others_obj(slime, obj)) {

					update_collision(slime, obj.get_collision());
				}
			}
		}
	}

	// map collision
	{
		for (int i = 0; i < map.size(); i++) {
			text_t text = map[i];
			int j = 0;
			while (*text) {
				if (*text == '#') {
					collision_box box(dot(j, -i) * 32, dot(j + 1, -i - 1) * 32);
					for (auto& obj : Objects) {
						obj.pos = box.bubble(obj.get_collision());
					}
				}
				j += 1;
				text++;
			}
		}

		point_t collision_radius = Objects[0].get_collision().circle.radius;

		// map border collision
		for (auto& obj : Objects) {
			obj.pos = dot(
				clamp<point_t>(collision_radius, obj.pos.x, map_w * 32 - collision_radius),
				clamp<point_t>(-map_h * 32 + collision_radius, obj.pos.y, -collision_radius)
			);
		}
	}
}
