String note_to_its_name(uint8_t note) {
	String pit_name("");
	switch (note % 12) {
		case 0:
		pit_name = "C";
		break;
		case 1:
		pit_name = "CS";
		break;
		case 2:
		pit_name = "D";
		break;
		case 3:
		pit_name = "DS";
		break;
		case 4:
		pit_name = "E";
		break;
		case 5:
		pit_name = "F";
		break;
		case 6:
		pit_name = "FS";
		break;
		case 7:
		pit_name = "G";
		break;
		case 8:
		pit_name = "GS";
		break;
		case 9:
		pit_name = "A";
		break;
		case 10:
		pit_name = "AS";
		break;
		case 11:
		pit_name = "B";
		break;
	}
	pit_name += String(note / 12);
	return pit_name;
}