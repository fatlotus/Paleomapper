CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCC
CCCCCC PROGRAM ATLASMAP
CCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCC
CCCCCC THIS PROGRAM DRAWS PRESENT AND PALEOGEOGRAPHIC MAPS INCORPORATING
CCCCCC DATA COMPILED BY THE PALEOGEOGRAPHIC ATLAS PROJECT, UNIVERSITY OF
CCCCCC CHICAGO.  QUESTIONS SHOULD BE DIRECTED TO David Rowley AT (773)702-8146
CCCCCC	
	PROGRAM WHIZPGEOGRECON
	DIMENSION ROT(999,5),IROT(999) 
        DIMENSION ALAT(4500),ALONG(4500),IPEN(4500)
        DIMENSION ANLAT(4500),ANLONG(4500)
        CHARACTER*80 NAME
        CHARACTER*80 NAM2
	CHARACTER*3 REFNUM
	CHARACTER*2 TYPE
	CHARACTER*1 ICH(3)
	CHARACTER*1 TCH(3)
	CHARACTER*1 FCHN
	CHARACTER*1 TAB
       COMMON/BLOCK2/ROTLA1,ROTLO1,ROTAN1,ROTLA2,ROTLO2,ROTAN2
        COMMON/BLOCK5/IDIFLO,IDIFLA
	COMMON/ROT/ROT,IROT,TIME
C        COMMON/BLOCK6/TIME
5       CONTINUE
	TAB=CHAR(9)
       ICNT=0
       INC=0
       IND=0
       NCN=0
       TSTR=0.0
       CHOICE=1.0
       ANG=0.0
       ROTLA1=0.0
       ROTLO1=0.0
       ROTAN1=0.0
       NEWCNT=0
       NAME='/usr/people/rowley/Paleomapper/data/pzgren.cst'
CCCCCC
        TYPE*, 'THIS PROGRAM RECONSTRUCTS PALEOGEOGRAPHIC ATLAS PROJECT'
        TYPE*,'NEW FORMAT LINEAR DATA AND WRITES IT IN WHIZATLAS FORMAT'
507	WRITE(*,*) 'ENTER NAME OF NEW WHIZATLAS FORMAT FILE'
	READ(5,2) NAM2
100      CONTINUE
CCCCCC
CCCCCC       IN THIS SECTION USER SPECIFIES TIME OF RECONSTRUCTION
CCCCCC       POLES AND ANGLES OF ROTATION ARE CALCULATED USING
CCCCCC       SUBROUTINE ROTFND
CCCCCC
138	CONTINUE
       CALL ROTFND
	OPEN (UNIT=2,FILE=NAM2)
          WRITE(2,1105) 'Y/X'
190       CONTINUE
CCCCCC
CCCCCC       THIS SECTION QUERIES THE USER ABOUT THE DATA FILE TO BE
CCCCCC       PLOTTED
CCCCCC
200       CONTINUE
        NCNT=1
215     CONTINUE
        WRITE(*,*) 'DO YOU WANT YOU USE THE THE DEFAULT COASTLINE FILE'
        WRITE(*,*) 'WHICH IS GREN.CST?  Y=YES OR N=NO'
	READ(5,1119)FCHN
	WRITE(6,1120)FCHN
        IF ((FCHN.NE.'Y').AND.(FCHN.NE.'N')) GO TO 215
        IF(FCHN.EQ.'Y') GO TO 626
1119	FORMAT(3A1)
1120	FORMAT(1X,3A1)
625	CONTINUE
506	WRITE(*,*) 'ENTER PGAP DATA FILE TO CONVERT TO WHIZATLAS FORMAT'
	READ(5,2) NAME
	WRITE(6,*) NAME
626	CONTINUE
216       TYPE*, 'DO YOU WANT DATA TESTED BY TIME'
       TYPE*, '(YES/NO)'
       READ(5,1001)TCH
	WRITE(6,11001)TCH
       IF ((TCH(1).NE.'Y').AND.(TCH(1).NE.'N')) GO TO 216
220       CONTINUE
330       CONTINUE
       OPEN(UNIT=4,NAME=NAME,TYPE='OLD',ERR=215,SHARED,READONLY)
340       CONTINUE
       NEWCNT=NEWCNT+1
445	FORMAT(1X,1A3,1I4,1X,1i3,1x,i4,2(1x,f6.2),1X,1A2,1X,1i5,
     *1X,1I1)
       READ(4,445,ERR=370,END=390)REFNUM,NEWSEQ,ID,INC,
     *T1,T2,TYPE,LTYP,ICOL
	WRITE(6,445)REFNUM,NEWSEQ,ID,INC,
     *T1,T2,TYPE,LTYP,ICOL
	IF (TYPE.EQ.'BA'.and.LTYP.gt.0) LTYP=-LTYP
       JROT=IROT(ID)
	IF ((TIME.NE.0.).AND.(JROT.EQ.1000.)) GO TO 380
       ROTLA1=ROT(JROT,1)
       ROTLO1=ROT(JROT,2)
       ROTAN1=ROT(JROT,3)
       IF (TCH(1).EQ.'N') GO TO 345
       IF ((IT1.LT.TIME).OR.(TIME.LT.IT2)) GO TO 380
345       CONTINUE
        JCNT=0
	ICNT=0
17      CONTINUE
	ICNT=ICNT+1
	READ(4,*,err=511)ALAT(ICNT),ALONG(ICNT),IPEN(ICNT)
	IF(ALAT(ICNT).EQ.99.0) GOTO 11
       CALL ROTATE(1,ALAT(ICNT),ALONG(ICNT),ANLAT(ICNT),ANLONG(ICNT))
        IF(ICNT.EQ.1) GOTO 17
        IF(IPEN(ICNT).EQ.3) GOTO 18
CC        IF(ICNT.LE.4) THEN
CC              WRITE(6,*) ANLAT(ICNT), ANLONG(ICNT)
CC        ENDIF
350       CONTINUE
	GOTO 17
18      CONTINUE
        JCNT=ICNT-1
        WRITE(2,1114)JCNT,TAB,JCNT,TAB,ID,TAB,LTYP,TAB,TIME,NAME
        DO 20 I=1,JCNT
	WRITE(2,1003)ANLAT(I),TAB,ANLONG(I)
CC        IF(I.LE.3) THEN
CC        	WRITE(6,*)ANLAT(I),ANLONG(I)
CC        ENDIF
20      CONTINUE
        ANLAT(1)=ANLAT(ICNT)
        ANLONG(1)=ANLONG(ICNT)
        ICNT=1
        GOTO 17
11      CONTINUE
        JCNT=ICNT-1
        IF (ICNT.LE.2)GO TO 30
        WRITE(2,1114)JCNT,TAB,JCNT,TAB,ID,TAB,LTYP,TAB,TIME,NAME
CC        WRITE(2,1104)JCNT,JCNT,ID,IAID,ISTRING,TIME,IT1,TYPE,LTYP,ICOL
        DO 29 I=1,JCNT
	WRITE(2,1003)ANLAT(I),TAB,ANLONG(I)
CC        IF(I.LE.3) THEN
CC        	WRITE(6,*)ANLAT(I),ANLONG(I)
CC        ENDIF
29      CONTINUE       
30      CONTINUE       
        GOTO 340
360       CONTINUE
370       CONTINUE
       TYPE*, 'ERROR DURING READ'
	WRITE(6,10004)REFNUM,NEWSEQ,ID,INC,IT1,IT2
       GO TO 345
380       CONTINUE
        ICNT=0
381     CONTINUE
	ICNT=ICNT+1
C       IF(ICNT.EQ.1) THEN
C          WRITE(6,*)'TIME TEST SENT ME HERE'
C          WRITE(6,1114)JCNT,JCNT,ID,TIME,NAME
C        ENDIF
	READ(4,1005,err=511)ALAT(ICNT),ALONG(ICNT),IPEN(ICNT)
	IF(ALAT(ICNT).EQ.99.0) GOTO 340
       GO TO 381
390       CONTINUE
       CLOSE(UNIT=4)
CCCCCC       
CCCCCC       THE USER IS GIVEN THE OPTION OF PLOTTING ANOTHER 
CCCCCC       LINEAR DATA FILE
CCCCCC
        CFILE=1.0
395       TYPE*, 'DO YOU WISH TO PLOT ADDITIONAL LINEAR DATA'
       TYPE*, '(YES/NO)'
       READ(5,1001)ICH
	WRITE(6,11001)ICH
       IF ((ICH(1).NE.'Y').AND.(ICH(1).NE.'N')) GO TO 395
       IF (ICH(1).EQ.'Y')THEN
                 NCNT=NCNT+1          
                 GO TO 215
        ENDIF
        GO TO 12
511	CONTINUE
	WRITE(6,*) 'ERROR DURING READ IN LINE ',NEWCNT, ICNT
	Write(6,1005)ALAT(ICNT),ALONG(ICNT),IPEN(ICNT)
12	CONTINUE
        WRITE(2,*) '0  0'
	CLOSE(UNIT=2)
         WRITE(6,*) 'PROGRAM IS ENDED'
CCCCCC
CCCCCC	END ADDITION
CCCCCC
2	FORMAT(A)
1001       FORMAT(3A1)
11001       FORMAT(3A1)
1002       FORMAT(1A40)
1003	FORMAT(F7.2,A1,F7.2)
1004       FORMAT(1X,3A1,1I4,4(1X,1I3),1X,2A1,1X,3A1,1X,1I1)
10004       FORMAT(1X,3A1,1I4,4(1X,1I3))
1005	FORMAT(2F7.2,1I3)
1020       FORMAT(1I3)
1105    FORMAT(1X,1A3)
1114	FORMAT(1X,I4,A1,I4,A1,I3,1A,I5,A1,F5.1,1A40)
CCCCCC
       STOP
       END

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCC
CCCCCC                   SUBROUTINE ROTFND
CCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCC
CCCCCC       IN THIS SUBROUTINE, ROTATION PARAMETERS FOR RECONSTRUCTIONS
CCCCCC       ARE CALCULATED
CCCCCC
 	SUBROUTINE ROTFND
	DIMENSION TMAX(999),TMIN(999)
	DIMENSION ROT(999,5),IROT(999) 
	INTEGER REFCHC,REFID
	CHARACTER*80 FILE
	CHARACTER*1 FCH(3)
	COMMON/ROT/ROT,IROT,TIME
c when this is exported this line will need to be changed
	FILE='/usr/people/rowley/Paleomapper/rot/newworld.rot'
c initialize some variables
	TMAX0=1000.
c initialize the id array to 1000, and the tmin array to 1000
	DO 10 I=1,999
	IROT(I)=1000
	TMIN(I)=1000.
10	CONTINUE
c input rotation file name
20	CONTINUE
	WRITE(6,1018) 
	READ(5,1019)FCH
	WRITE(6,1020)FCH
	IF (FCH(1).EQ.'Y') GO TO 26
25	CONTINUE
	WRITE(6,1003)
        READ(5,1001) FILE
	WRITE(6,1002) FILE
c open rotation file
26	CONTINUE
	OPEN(UNIT=2,NAME=FILE,TYPE='OLD',ERR=20,SHARED,READONLY)
       OPEN(UNIT=3,NAME='ROT1.TMP')
c input time of reconstruction
70	WRITE(6,1005)
	WRITE(6,*) 'AGES BETWEEN 290 AND 0 MA'
	READ(5,*) TIME
	WRITE(6,1007) TIME
C	IF ((TIME.GT.TMAX0).OR.(TIME.LT.TMIN0)) THEN
C		WRITE(6,1008)		
C		GO TO 70
C	ENDIF
80	WRITE(6,1009)
	WRITE(6,1010)
	WRITE(6,1011)
	READ(5,*)REFCHC
	WRITE(6,1012)REFCHC
	IF ((REFCHC.NE.1).AND.(REFCHC.NE.2)) GO TO 80
	IF (REFCHC.EQ.1) THEN
		WRITE(6,1013)
		READ(5,*) REFID
		WRITE(6,1014)REFID
	END IF
c start calculating rotations
100	CONTINUE 
	ID1=ID2 
	T1=T2 
	RLAT1=RLAT2 
	RLON1=RLON2 
	RANG1=RANG2 
	REF1=REF2 
c first, read until you reach the first pole older than your
c reconstruction time 
	READ(2,*,END=400,ERR=810)ID2,T2,RLAT2,RLON2,RANG2,REF2
	IF(TIME.LE.T2) GO TO 200
	GO TO 100 
c the reconstruction time should be bracketed by t1 and t2 and
c the pole is bracketed by rlat1,rlon1,rang1 and rlat2,rlon2,rang2
200	CONTINUE 
c if there are two different references flee
       IF(REF1.NE.REF2)  GO TO 900
c otherwise you have to calculate the rotation to get you from the
c rlat1,rlon1,rang1 at time t1 to the rotation for time
c first, calculates what fraction of the rotation from t1 to t2
c that you need to get to time
	DELTA=(T2-TIME)/(T2-T1) 
c then you subtract rlat1,rlon1,rang1 from rlat2,rlon2,rang2
c to get the rotation to take you from t1 to t2
c namely, rlat,rlon,rang
	CALL ADDER(RLAT2,RLON2,-RANG2,RLAT1,RLON1,RANG1,RLAT,RLON,RANG) 
c then you multiply the angle by the fraction delta to get
c the partial rotation to get to time
	RANG=RANG*DELTA 
c then you add rlat1,rlon1,rang1 (the rotation to get to t1)
c and the fraction of the rotation to get from t1 to t2
c to yield the rotation to get to time
C the rotation to get from the present to time referred to ref2
c is stored in the array rot(i,j)
	INC=INC+1 
	CALL ADDER(RLAT2,RLON2,RANG2,RLAT,RLON,RANG,
     * 	ROT(INC,1),ROT(INC,2),ROT(INC,3))
	ROT(INC,4)=REF1
	ROT(INC,5)=ID1
	IROT(ID1)=INC 
c then we keep reading the rotation file until we come to the
c next plate, when we start over
300	CONTINUE 
	READ(2,*,END=400,ERR=810) ID2,T2,RLAT2,RLON2,RANG2,REF2 
	IF(ID2.NE.ID1) GO TO 100 
	GO TO 300 
c when we have rotations for all of the plates
c we close the file
400	CONTINUE 
	CLOSE(UNIT=2)
c then we have to convert the rotations for all of the plates to
c a common reference frame, the graticule
c so we go through the rotation array an element at a time
c and check to see if rot(i,4), the reference plate, is 0
c ie if each plates reference is the graticule
        WRITE(3,442) ROT(1,5),TIME,ROT(1,1),ROT(1,2),ROT(1,3) 
	DO 700 I=1,INC 
	J=INT(ROT(I,4))
c if it is zero then proceed
	IF(J.EQ.0) GO TO 700 
c if it's not zero then we find the rotation for the reference plate
c and add it to the original plates rotation
	RLAT1=ROT(I,1)
	RLON1=ROT(I,2)
	RANG1=ROT(I,3)
500	CONTINUE 
	K=IROT(J) 
	CALL ADDER(RLAT1,RLON1,RANG1,ROT(K,1),ROT(K,2),ROT(K,3),
     *	RLAT,RLON,RANG) 
c then we check to see if the reference plate had a 0 reference
	J=INT(ROT(K,4))
c if it did then we proceed
	IF(J.EQ.0.0)  GO TO 600
c if not then we add the rotation of the next reference to the modified
c rotation
	RLAT1=RLAT 
	RLON1=RLON 
	RANG1=RANG 
c and we loop through this until the reference is zero (the graticule)
	GO TO 500
600	CONTINUE 
c if the reference is zero then we put the rotation into the array
c and proceed
	ROT(I,1)=RLAT 
	ROT(I,2)=RLON 
	ROT(I,3)=RANG 
	ROT(I,4)=0.0 
c then we loop up and do the next array element
        WRITE(3,442) ROT(I,5),TIME,RLAT,RLON,RANG 
442	FORMAT(2X,1F8.2,2X,1F8.2,2X,1F8.2,2X,1F8.2,2X,1F8.2)
700	CONTINUE 
c if we want one plate held fixed then we must subtract its
c rotation from every rotation in the array
	IF (REFCHC.EQ.1) THEN
	J=IROT(REFID)
	ROTLAT=ROT(J,1)
	ROTLON=ROT(J,2)
	ROTANG=-ROT(J,3)
	DO 800 I=1,INC
	CALL ADDER(ROT(I,1),ROT(I,2),ROT(I,3),ROTLAT,ROTLON,ROTANG,
     *	RLAT,RLON,RANG)
	ROT(I,1)=RLAT
	ROT(I,2)=RLON
	ROT(I,3)=RANG
800	CONTINUE
	END IF
c when the entire array is done we return to the main program
	RETURN 
810	WRITE(6,1004)
	WRITE(6,1015)ID2,T2,RLAT2,RLON2,RANG2,REF2 
	STOP
900	WRITE(6,1016)  
	WRITE(6,1017)REF1,REF2
	STOP
1001	FORMAT(1A80)
1002	FORMAT(1X,1A80)
1003	FORMAT(1X,'ENTER NAME OF ROTATION FILE')
1004	FORMAT(1X,'ERROR DURING READ')
1005	FORMAT(1X,'ENTER TIME OF RECONSTRUCTION')
1006	FORMAT(1X,'BETWEEN 290MA  AND PRESENT')
1007    FORMAT(1X,1F6.2)
1008	FORMAT(1X,'INVALID TIME')
1009	FORMAT(1X,'CHOOSE REFERENCE FRAME')
1010	FORMAT(1X,'1) HOLD A PLATE FIXED')
1011	FORMAT(1X,'2) PALEOMAGNETIC REFERENCE FRAME')
1012	FORMAT(1X,1I1)
1013	FORMAT(1X,'ENTER PLATE-ID NUMBER OF FIXED PLATE')
1014	FORMAT(1X,1I3)
1015	FORMAT(1X,1I3,1F6.2,3F7.2,1F4.0)
1016	FORMAT(1X,'ERROR:  REF1  NOT EQUAL TO REF2')
1017	FORMAT(1X,2F4.0)
1018	FORMAT(1X,'DO YOU WISH TO USE DEFAULT ROTATION FILE (Y/N)')
1019	FORMAT(3A1)
1020	FORMAT(1X,3A1)
	END
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCC
CCCCCC                  SUBROUTINE ADDER
CCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCC
CCCCCC       THIS SUBROUTINE ADDS ROTATION PARAMETERS (LAT,LONG, AND ANGLE OF
CCCCCC       ROTATION) TO EACH OTHER
CCCCCC
       SUBROUTINE ADDER (EN,EO,EP,EQ,ER,ES,FT,FU,FM) 
	IF ((EN.EQ.EQ).AND.(EO.EQ.ER)) GO TO 998
       IF (EP.EQ.-ES) GO TO 999
       IF ((EP+ES.LE..01).AND.(EP+ES.GE.-.01)) GO TO 999
       R=57.29578 
       FN=EN/R 
       FO=EO/R 
       FP=EP/R 
       FR=ER/R 
       FS=ES/R 
       FQ=EQ/R 
       FA=COS(FP/2) 
       FB=SIN(FP/2)*SIN((90/R)-FN)*COS(FO) 
       FC=SIN(FP/2)*SIN((90/R)-FN)*SIN(FO) 
       FD=SIN(FP/2)*COS((90/R)-FN) 
       FE=COS(FS/2) 
       FF=SIN(FS/2)*SIN((90/R)-FQ)*COS(FR) 
       FG=SIN(FS/2)*SIN((90/R)-FQ)*SIN(FR) 
       FH=SIN(FS/2)*COS((90/R)-FQ) 
       FI=FA*FE-FB*FF-FC*FG-FD*FH 
       FJ=FA*FF+FB*FE-FC*FH+FD*FG 
       FK=FA*FG+FB*FH+FC*FE-FD*FF 
       FL=FA*FH-FB*FG+FC*FF+FD*FE 
       IF (ABS(FI).GE.1.0) FM=0.0
       IF (ABS(FI).GE.1.0) GOTO 10
       ASIN=ATAN(FI/SQRT(1.0-FI*FI)) 
       ACOS=((3.14159/2.0-ASIN)*R) 
       FM=ACOS*2 
       IF(FM.GT.180.0) FM=FM-360.0 
10     CONTINUE
	IF (FM.EQ.0.0) GO TO 999
       FV=FL/SIN(FM/(2*R)) 
       IF (ABS(FV).GE.1.0) FT=90.0
       IF (ABS(FV).GE.1.0) GO TO 11
       ASIN=ATAN(FV/SQRT(1.0-FV*FV)) 
       ACOS=((3.14159/2.0-ASIN)*R) 
       FT=90.0-ACOS 
11       CONTINUE
       IF(FM.LT.0.0) FT=-FT 
	IF ((FK.EQ.0.0).AND.(FJ.EQ.0.0)) FU=0.0
	IF ((FK.EQ.0.0).AND.(FJ.EQ.0.0)) GO TO 20
       FU=ATAN2(FK,FJ)*R 
       IF(FU.GT.180.00) FU=FU-360.0 
C       TYPE *,FT,FU,FM 
       GO TO 20
998	CONTINUE
	FT=EN
	FU=EO
	FM=EP+ES
	GO TO 20
999       CONTINUE
C       TYPE*, EP, ES
       FM=0.0
       FU=0.0
       FT=0.0
20       CONTINUE
       RETURN 
       END 
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCC
CCCCCC                   SUBROUTINE ROTATE
CCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCC
CCCCCC       THIS SUBROUTINE CALCULATES THE IMAGE OF A PARTICULAR LATITUDE,
CCCCCC       LONGITUDE POINT UNDER ROTATION
CCCCCC
       SUBROUTINE ROTATE(ITST,ALAT,ALONG,ANLAT,ANLONG) 
       COMMON/BLOCK2/ROTLA1,ROTLO1,ROTAN1,ROTLA2,ROTLO2,ROTAN2
       ROTLAT=ROTLA1
       ROTLO=ROTLO1
       ROTAN=ROTAN1
       IF(ALAT.EQ.90.0) ALAT=89.9 
       IF(ALAT.EQ.-90.0) ALAT=-89.9 
       D=3.14159/180.0 
10       CONTINUE 
       A1=90.0*D-ALAT*D 
       SINA1=SIN(A1) 
       A2=ALONG*D 
       PX=SINA1*COS(A2) 
       PY=SINA1*SIN(A2) 
       PZ=COS(A1) 
       A3=90.0*D-ROTLAT*D 
       SINA3=SIN(A3) 
       A4=ROTAN*D 
       COSA4=COS(A4) 
       SINA4=SIN(A4) 
       A5=ROTLO*D 
       SINA5=SIN(A5)
       COSA5=COS(A5) 
       GX=SINA3*COSA5 
       GY=SINA3*SINA5 
       GZ=COS(A3) 
       VCT=(PX*GX)+(PY*GY)+(PZ*GZ) 
       RX=COSA4*PX+(1.0-COSA4)*VCT*GX+ 
     1SINA4*(GY*PZ-GZ*PY) 
       RY=COSA4*PY+(1.0-COSA4)*VCT*GY+ 
     1SINA4*(GZ*PX-GX*PZ) 
       RZ=COSA4*PZ+(1.0-COSA4)*VCT*GZ+ 
     1SINA4*(GX*PY-GY*PX) 
C       IF(RZ.EQ.1.0) GO TO 20 
       IF(SQRT(1.0-RZ*RZ).EQ.0.0) GOTO 20 
       ASIN=ATAN(RZ/SQRT(1.0-RZ*RZ)) 
       ACOS=((3.14159/2.0-ASIN)*57.29578) 
       GO TO 30 
20       CONTINUE
       ACOS=0.0 
30       ANLAT=90.0-ACOS
       ANLONG=90.0-(ATAN2(RX,RY)*57.29578)
       IF(ANLONG.GT.180.0) ANLONG=ANLONG-360.0 
999       CONTINUE 
C  999       TYPE 500 
       RETURN 
       END 
