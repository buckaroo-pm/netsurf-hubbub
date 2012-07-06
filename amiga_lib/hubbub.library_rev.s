VERSION = 2
REVISION = 1

.macro DATE
.ascii "3.7.2012"
.endm

.macro VERS
.ascii "hubbub.library 2.1"
.endm

.macro VSTRING
.ascii "hubbub.library 2.1 (3.7.2012)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: hubbub.library 2.1 (3.7.2012)"
.byte 0
.endm
