//light indicator function definition
void LEDController(ubyte LEDBitmask) {
	//B2 (0x04 or B2): IR detected output address (only use in autonomous)
	//B3 (0x08 or B3): block detected output address
	//B4 (0x0F or B4): robot lifting output address
	//B5 (0x20 or B5): stopper in output address
	HTSPBwriteIO(HTSPB, LEDBitmask);
}
