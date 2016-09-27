#include <Event.h>
#include <Timer.h>
#include <vector>
#include <map>
#include <string>
#include <StandardCplusplus.h>
#include <Arduino.h>
#include <iostream>


using namespace std;


struct hNote {
	uint8_t pitch;
	uint8_t duration = 1;
	bool	on_off = 1;
	hNote(uint8_t p) : pitch(p) {}
	hNote() {pitch = 0;}
};
struct nNote {
	uint8_t pitch;
	nNote() {pitch = 0;}
	nNote(uint8_t p) : pitch(p) {}
};
// fixed size because of small memory of arduino
// it could be solved by PROGMEM macro but it's tricy to use.
// only predefined type can be used.
class nMeasure {
private:
	vector<nNote> measure;
	vector<nNote>::iterator note_tracker;
public:
	explicit nMeasure() {
		measure.resize(16);
		note_tracker = measure.begin();
		for(auto p : measure) {
			p = nNote(0);
		}
	}
public:
	nMeasure& operator=(nMeasure nM) {
		measure = nM.measure;
		note_tracker = nM.note_tracker;
		return *this;
	}
	friend void measure_performer();
	// following two function will accord with measure performer
	vector<nNote>::iterator get_current_tracker() {return note_tracker;}
	inline void init() {
		for(auto p : measure) {
			p = nNote(0);
		}
	}
	inline void inc_note_tracker() {
		++note_tracker; 
	}
	inline bool is_ending_note() {
		if(note_tracker == measure.end()) return true;
		else return false;
	}
	inline void move_first() {
		note_tracker = measure.begin();
	}
	inline void dec_note_tracker() {
		if(note_tracker == measure.begin()) {
			note_tracker = measure.end();
			note_tracker--;
		} else --note_tracker;
	}
	// note from key just pressed, pos from current led position.
	// assuming one note at a time.
	inline void set_note_at_pos(uint8_t pitch, uint8_t pos) {
		measure[pos] = nNote(pitch);
	}
	// when pressed remove button, this function will be called.
	inline void remove_note_at_pos_n(uint8_t pos) {
		measure[pos] = nNote(0);
	}
};

class nMelody {
private:
	list<nMeasure> melody;
	list<nMeasure>::iterator measure_tracker;
	uint8_t mea_siz;
public:
	explicit nMelody() {
		// fixed size because of small memory of arduino
		// melody.resize(16);
		mea_siz = 0;
	}
	explicit nMelody(list<nMeasure> n) {
		melody = n;
	}
	inline void inc_mea_tracker() {
		++measure_tracker;
		if(measure_tracker == melody.end()) --measure_tracker;
	}
	inline void dec_mea_tracker() {
		if(measure_tracker != melody.begin())
			--measure_tracker;
	}
	inline list<nMeasure>::iterator get_current_measure() {return measure_tracker;}
	// inline void add_mea(nMeasure nM) {melody}
	inline uint8_t get_siz() {return mea_siz;}
};
