        CHARACTER*80 NAME
        CHARACTER*80 NAM1
	CHARACTER*3 AREA,TYP1
	CHARACTER*2 TYPE
        CHARACTER*1 TAB
        TAB=CHAR(9)
	AREA='99X'
	ICNT=0
	TYPE*, 'THIS PROGRAM CONVERTS WHIZATLAS FILES' 
	TYPE*, '  INTO PGAP FORMAT FILES'
        WRITE(6,*) 'ENTER THE NAME OF THE FILE TO BE CONVERTED'
        READ(5,2) NAME
	OPEN(UNIT=2,NAME=NAME,READONLY,ERR=99)
2       FORMAT(A80)
        TYPE*, 'ENTER NAME OF CONVERTED FILE'
        READ(5,2) NAM1
        OPEN(UNIT=3,NAME=NAM1)
4	CONTINUE
        READ(2,445,err=99,end=50)AREA,ICNT,ID,NCNT,T1,T2,TYPE,LTYP,ICOL
C        WRITE(3,445)IPTS,TAB,IPTS,TAB,NAM1
445	FORMAT(1X,1A3,1I4,1X,1i3,1x,i4,2(1x,f6.2),1X,1A2,1X,1i5,
     *1X,1I1)
       IF (TYPE.EQ.'BA') LTYP=-LTYP  
	write(3,445)AREA,ICNT,ID,NCNT,T1,T2,TYPE,LTYP,ICOL
41      CONTINUE
CCCCCC
CCCCCC  DATA INTERPRETED
CCCCCC
	READ(2,*) ALAT,ALONG,IPEN
	WRITE(3,1006)ALAT,ALONG,IPEN
	IF(ALAT.EQ.99.00) go to 4 	
	GO TO 41
43	CONTINUE
	GO TO 50
99      TYPE*, 'ERROR DURING READ'
50      TYPE*, 'PROGRAM ENDED'
1006	FORMAT(F9.4,1X,F9.4,1X,I2)
        STOP
         END

