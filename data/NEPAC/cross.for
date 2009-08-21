	CHARACTER*40 NAME
	DIMENSION ALA(6),ALO(6),II(6)
	D=1/57.29578
	ALA(1)=.05
	ALO(1)=.05
	II(1)=3
	ALA(2)=0.0
	ALO(2)=0.0
	II(2)=2
	ALA(3)=-.05
	ALO(3)=-.05
	II(3)=2
	ALA(4)=-.05
	ALO(4)=.05
	II(4)=3
	ALA(5)=0.0
	ALO(5)=0.0
	II(5)=2
	ALA(6)=.05
	ALO(6)=-.05
	II(6)=2
	TYPE*, 'ENTER NAME OF OLD FILE TO BE ALTERED'
	READ(5,1)NAME
1	FORMAT(1A40)
	OPEN(UNIT=4,NAME=NAME,TYPE='OLD')
	TYPE*, 'ENTER NAME OF NEW FILE'
	READ(5,1)NAME
	OPEN(UNIT=3,NAME=NAME,TYPE='NEW')
10	CONTINUE
        ICNT=ICNT+1
29      FORMAT(A,1X,A)
	READ(4,2001,END=20) IREFNUM,NEWSEQ,ID,NCN,T1,T2,TYPE,LTYP,ICOL
	WRITE(6,2001) IREFNUM,NEWSEQ,ID,NCN,T1,T2,TYPE,LTYP,ICOL
	WRITE(3,2001) IREFNUM,NEWSEQ,ID,NCN,T1,T2,TYPE,LTYP,ICOL
C        READ(4,1004,ERR=17,END=20)REFNUM,NEWSEQ,ID,INC,
C     *IT1,IT2,TYPE,LTYP,ICOL
C1004    FORMAT(1X,1A3,1I4,1X,1I3,3(1X,1I3),1X,1A2,1X,1A3,
C     *1X,1I1)
C	WRITE(3,1004)REFNUM,NEWSEQ,ID,INC,
C     *IT1,IT2,TYPE,LTYP,ICOL
15      CONTINUE
        READ(4,2000,END=20)ALAT,ALONG,IPEN
        WRITE(3,2000)ALAT,ALONG,IPEN
C1003    FORMAT(2F7.2,1I3)
        IF(ALAT.EQ.99.0) GOTO 16
	DO 21 I=1,6
        RLAT=ALAT+ALA(I)
        RLON=ALONG+ALO(I)
        NPEN=II(I)
	WRITE(3,2000)RLAT,RLON,NPEN
21	CONTINUE
        GOTO 15
17	CONTINUE
	WRITE(6,*) 'ERROR DURING READ'
	GO TO 15
20      CONTINUE
        ALAT=99.0000
        ALONG=99.0000
        IPEN=3
16	WRITE(3,2000)ALAT,ALONG,IPEN
        CLOSE(UNIT=4)
	CLOSE(UNIT=3)
2000	FORMAT(F9.4,1X,F9.4,1I3)
2001	FORMAT(I2,1X,I3,1X,I3,1X,I3,1X,F6.1,1X,F6.1,1X,A2,1X,I3,1X,I2)
	TYPE*, 'END OF PROGRAM'
	STOP
	END
