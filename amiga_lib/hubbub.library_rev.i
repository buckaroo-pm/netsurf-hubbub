VERSION		EQU	2
REVISION	EQU	1

DATE	MACRO
		dc.b '3.7.2012'
		ENDM

VERS	MACRO
		dc.b 'hubbub.library 2.1'
		ENDM

VSTRING	MACRO
		dc.b 'hubbub.library 2.1 (3.7.2012)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: hubbub.library 2.1 (3.7.2012)',0
		ENDM
