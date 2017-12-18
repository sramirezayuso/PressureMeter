void WriteEEPROM(unsigned int uiAddress, unsigned char ucData) {
	/* Wait for completion of previous write */
	while (EECR & (1<<EEPE));

	/* Set up address and Data Registers */
	EEAR = uiAddress;
	EEDR = ucData;
	/* Write logical one to EEMPE */
	EECR |= (1<<EEMPE);
	/* Start eeprom write by setting EEPE */
	EECR |= (1<<EEPE);
}

unsigned char ReadEEPROM(unsigned int uiAddress) {
	/* Wait for completion of previous write */
	while (EECR & (1<<EEPE));

	/* Set up address register */
	EEAR = uiAddress;
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from Data Register */
	return EEDR;
}

void WriteEEPROMString(unsigned int uiAddress, unsigned char * ucData, int size) {
	int i;
	for (i = 0; i < size; i++) {
		WriteEEPROM(uiAddress+i, ucData[i]);
	}
}

void ReadEEPROMString(unsigned int uiAddress, unsigned char * ucData, int size) {
	int i;
	unsigned int auxAddress = uiAddress;
	for (i = 0; i < size; i++) {
		ucData[i] = ReadEEPROM(auxAddress);
		auxAddress ++;
	}
}

void WriteIntEEPROM(unsigned int uiAddress, int data) {
	char dataString[4];
	int i;
	itoa(data, dataString, 10);
	for (i = 0; i < 4; i++) {
		WriteEEPROM(uiAddress+i,dataString[i]);
	}
}

int ReadIntEEPROM(unsigned int uiAddress) {
	char dataString[4];
	int i;
	for( i = 0; i < 4; i++) {
		dataString[i] = ReadEEPROM(uiAddress+i);
	}
	return atoi(dataString);
}