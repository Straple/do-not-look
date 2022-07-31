// visibility
#define BAT_DRAW_SIZE 0.4
#define BAT_DELTA_DRAW_POS dot(-8, 26) * BAT_DRAW_SIZE
#define BAT_FRAME_DURATION 1.0 / 7

struct Bat {

	inline static const enemy_state enemy_state = enemy_state(1, 3.5, 40, 50, 8, 300, 0.3, 5, 3, 4.5);

	// pos and move
	dot pos;
	dot dp;
	dot ddp;

	dot walk_to;

	// cooldowns
	point_t attack_cooldown_accum = enemy_state.attack_cooldown;
	point_t paralyzed_cooldown_accum = enemy_state.paralyzed_cooldown;
	point_t walk_accum = 0;

	// settings
	s16 hp = 2;
	bool have_target = false;

	// animation
	animation anim = animation(SP_BAT, 0, 5, BAT_FRAME_DURATION, 16);

	Bat(){}
	Bat(const dot& p) {
		walk_to = pos = p;
		std::uniform_int_distribution<s64> random_range(1, 1000);
		walk_accum = (random_range(rnd) / 1000.0) * enemy_state.walk_sum_time;
	}

	collision_circle get_collision() const {
		return Circle(pos, enemy_state.collision_radius);
	}

	void simulate(point_t delta_time) {

		attack_cooldown_accum += delta_time;
		paralyzed_cooldown_accum += delta_time;

		if (paralyzed_cooldown_accum < enemy_state.paralyzed_cooldown) {
			simulate_move2d(pos, dp, ddp, delta_time);
		}
		else {

			auto bad_target = [&](point_t radius) -> bool {
				return (player.pos - pos).getLen() > radius;
			};

			ddp = dot();

			anim.frame_update(delta_time);

			if (dp.x < 0) {
				anim.sprite_sheet = SP_BAT_INVERSE;
			}
			else {
				anim.sprite_sheet = SP_BAT;
			}
			
			// цель потеряна
			if (have_target && bad_target(enemy_state.persec_radius)) {
				have_target = false;
				walk_to = pos;
				walk_accum = enemy_state.walk_time;
			}

			if (!have_target) { // цели нет

				// поищем

				if (!bad_target(enemy_state.locator_radius)) {
					have_target = true;
				}
			}

			if (have_target) {

				auto& player = ::player;

				move_to2d(pos, player.pos, dp, (player.pos - pos).normalize() * enemy_state.ddp_speed, delta_time);

				// мы близко к игроку и кулдаун атаки прошел
				if ((player.pos - pos).getLen() <= enemy_state.jump_radius &&
					attack_cooldown_accum >= enemy_state.attack_cooldown) {

					if (!player.is_eyes_closed) {

						// hit

						attack_cooldown_accum = 0;

						pos = player.pos; // прыгаем на игрока

						player.update_health(-1);
						add_hit_effect(player.pos + dot(-8, 16) * PLAYER_DRAW_SIZE);
						add_sound_player_hurt();
					}
				}
			}
			else {
				// цели нет. Можем погулять

				walk_accum += delta_time;

				if (pos == walk_to) {
					walk_to = Circle(pos, 20).get_random_dot();
				}

				if (walk_accum > enemy_state.walk_sum_time) {
					walk_accum = 0;
					walk_to = Circle(pos, 20).get_random_dot();
				}

				if (walk_accum < enemy_state.walk_time) {
					move_to2d(pos, walk_to, dp, (walk_to - pos).normalize() * enemy_state.ddp_speed, delta_time);
				}
			}
		}
	}

	void draw() const {

		draw_sprite(pos + dot(-5, 0) * BAT_DRAW_SIZE, BAT_DRAW_SIZE, SP_SMALL_SHADOW);

		anim.draw(pos + BAT_DELTA_DRAW_POS, BAT_DRAW_SIZE, [&](const Color& color) {
			return paralyzed_cooldown_accum < enemy_state.paralyzed_cooldown ?
				WHITE :
				color;
		});

		draw_collision_obj(*this);

		draw_hp(*this);

		if (locator_vis_mode) {
			dot p = pos;
			static_pos_update(p);

			draw_circle(Circle(p, enemy_state.persec_radius), Color(0xf000f0, 64));

			draw_circle(Circle(p, enemy_state.locator_radius), Color(0xfff000, 32));

			draw_circle(Circle(p, enemy_state.jump_radius), Color(0xff0000, 16));

			draw_object(walk_accum, p, 0.4, 0xffffffff);

			p = walk_to;
			static_pos_update(p);

			draw_rect(p, dot(0.3, 0.3), 0xffff0000);
		}
	}

	bool simulate_hit(const Player& player) {
		add_hit_effect(pos + BAT_DELTA_DRAW_POS);

		hp -= player.damage;

		if (hp <= 0) {
			add_sound_bat_death();
			add_death_effect(pos + dot(-16, 22) * BAT_DRAW_SIZE);
			add_random_game_object(pos);
			return true;
		}
		else {
			add_sound_bat_hurt();
			ddp += player.get_dir() * enemy_state.ddp_speed * 1.5;
			paralyzed_cooldown_accum = 0;
			return false;
		}
	}
};
