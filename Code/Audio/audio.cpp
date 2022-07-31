
enum audio_t {

	// PLAYER
	AUD_PLAYER_HURT1,
	AUD_PLAYER_HURT2,

	AUD_SWORD1,
	AUD_SWORD2,

	AUD_STEP1,
	AUD_STEP2,
	AUD_STEP3,
	AUD_STEP4,
	AUD_STEP5,
	AUD_STEP6,

	AUD_POP1,
	AUD_POP2,

	// BAT

	AUD_BAT_DEATH,
	AUD_BAT_HURT1,
	AUD_BAT_HURT2,
	AUD_BAT_HURT3,
	AUD_BAT_HURT4,

	// SLIME

	AUD_SLIME_BALLOON,
	AUD_SLIME_HURT,

	AUD_COUNT,
};

struct sound {
	OutputStreamPtr* sound_ptr = nullptr;
	size_t size;

	void play() {
		for (size_t i = 0; i < size; i++) {
			if (!sound_ptr[i]->isPlaying()) {
				sound_ptr[i]->setVolume(1);
				sound_ptr[i]->play();
				return;
			}
		}
		ASSERT(false, "not found free sound");
	}
	void stop() {
		for (size_t i = 0; i < size; i++) {
			sound_ptr[i]->stop();
		}
	}

	bool is_play() {
		size_t i;
		for (i = 0; i < size && sound_ptr[i]->isPlaying() == false; i++) {
		}
		return i < size;
	}

	void OpenSound(AudioDevicePtr& device, const char* path, bool streaming, size_t _size) {
		size = _size;
		sound_ptr = new OutputStreamPtr[_size];
		for (size_t i = 0; i < _size; i++) {
			sound_ptr[i] = audiere::OpenSound(device, path, streaming);
		}
	}
};

sound Sounds[AUD_COUNT];

void init_sound(AudioDevicePtr& device, audio_t type, std::string file_name) {
	Sounds[type].OpenSound(device, ("Audio\\" + file_name).c_str(), true, 10);
}

void init_audio(AudioDevicePtr& device) {

	// PLAYER
	{
		init_sound(device, AUD_PLAYER_HURT1, "Player\\playerhurt1.mp3");
		init_sound(device, AUD_PLAYER_HURT2, "Player\\playerhurt2.mp3");

		init_sound(device, AUD_SWORD1, "Player\\sword1.mp3");
		init_sound(device, AUD_SWORD2, "Player\\sword2.mp3");

		init_sound(device, AUD_STEP1, "Player\\step1.mp3");
		init_sound(device, AUD_STEP2, "Player\\step2.mp3");
		init_sound(device, AUD_STEP3, "Player\\step3.mp3");
		init_sound(device, AUD_STEP4, "Player\\step4.mp3");
		init_sound(device, AUD_STEP5, "Player\\step5.mp3");
		init_sound(device, AUD_STEP6, "Player\\step6.mp3");

		init_sound(device, AUD_POP1, "Player\\pop1.mp3");
		init_sound(device, AUD_POP2, "Player\\pop2.mp3");
	}

	// BAT
	{
		init_sound(device, AUD_BAT_DEATH, "Bat\\batdeath.mp3");
		init_sound(device, AUD_BAT_HURT1, "Bat\\bathurt1.mp3");
		init_sound(device, AUD_BAT_HURT2, "Bat\\bathurt2.mp3");
		init_sound(device, AUD_BAT_HURT3, "Bat\\bathurt3.mp3");
		init_sound(device, AUD_BAT_HURT4, "Bat\\bathurt4.mp3");
	}

	// SLIME
	{
		init_sound(device, AUD_SLIME_BALLOON, "Slime\\slimeballoon.mp3");
		init_sound(device, AUD_SLIME_HURT, "Slime\\slimehurt.mp3");
	}
	
}

/*
* PLAYER
*/

void add_sound_player_hurt() {
	if (get_urnd() < 0.5) {
		Sounds[AUD_PLAYER_HURT1].play();
	}
	else {
		Sounds[AUD_PLAYER_HURT1].play();
	}
}

void add_sound_player_pop_item() {
	if (get_urnd() < 0.5) {
		Sounds[AUD_POP1].play();
	}
	else {
		Sounds[AUD_POP2].play();
	}
}

/*
* BAT
*/

void add_sound_bat_hurt() {
	Sounds[AUD_BAT_HURT1 + get_rnd_range_int(0, 3)].play();
}

void add_sound_bat_death() {
	Sounds[AUD_BAT_DEATH].play();
}

/*
* SLIME
*/

void add_sound_slime_balloon() {
	Sounds[AUD_SLIME_BALLOON].play();
}

void add_sound_slime_hurt() {
	Sounds[AUD_SLIME_HURT].play();
}
